#pragma once
#include "ThreadInterruption.h"
#include <vector>
#include <functional>
#include <boost/lockfree/queue.hpp>
#include <type_traits>
namespace My {

	template< class Tread >
	struct thread_joiner {
		std::vector< Tread >& working_threads;
		thread_joiner(std::vector< Tread >& working_threads) : working_threads(working_threads) {}
		~thread_joiner() {
			for (auto& thread : working_threads) {
				if (thread.joinable()) thread.join();
			}
		}
	};

	template< typename TContext = void >
	class thread_pool {
	public:
		using ThreadContext = TContext;
		typedef std::function< void(ThreadContext) > task;
	private:
		std::atomic< bool > done;
		boost::lockfree::queue< task* > task_queue;
		std::vector< interruptible_thread > working_threads;
		thread_joiner< interruptible_thread > joiner;

		template<typename Context>
		std::enable_if_t< std::is_void< Context >::value > thread_worker() {
			while (!done.load()) {
				try {
					thread_pool::task* task;
					if (task_queue.pop(task)) {
						std::unique_ptr< thread_pool::task > task_guard(task);
						(*task)();
					} else std::this_thread::yield();
				} catch (const interruption&) {}
			}
		}

		template<typename Context>
		std::enable_if_t< !std::is_void< Context >::value > thread_worker() {
			std::remove_reference< Context >::type context;
			while (!done.load()) {
				try {
					thread_pool::task* task;
					if (task_queue.pop(task)) {
						std::unique_ptr< thread_pool::task > task_guard(task);
						(*task)(context);
					} else std::this_thread::yield();
				}
				catch (const interruption&) {}
			}
		}

		void init(const size_t& thread_n) {
			working_threads.reserve(thread_n);
			try {
				for (size_t i = 0; i < thread_n; i++) {
					working_threads.push_back(interruptible_thread(std::bind(&thread_pool::thread_worker< ThreadContext >, this)));
				}
			} catch (...) {
				done.store(true);
			}
		}

		template <typename task_guard_maker>
		void _add_task(task_guard_maker task_wrap) {
			if (!done.load()) {
				std::unique_ptr< thread_pool::task > task_guard = task_wrap.ptr();

				while (!done.load()) {
					if (task_queue.push(task_guard.get())) {
						task_guard.release();
						break;
					} else std::this_thread::yield();
				}

				if (done.load() && !task_guard) {
					thread_pool::task* task;
					if (task_queue.pop(task)) { delete task; }
				}
			}
		}

	public:
		thread_pool() : done(false), task_queue(std::thread::hardware_concurrency() * 32), joiner(working_threads) {
			init(std::thread::hardware_concurrency());
		}
		thread_pool(const size_t& thread_n, const size_t& task_queue_size = 128) : done(false), task_queue(std::thread::hardware_concurrency() * 32), joiner(working_threads) {
			init(thread_n);
		}

		void queue_reserve(size_t n) {
			task_queue.reserve(n);
		}

		//(отдает void, принимает а аргумент void, приходит в параметр void), (void, !void, !void)
		template< typename _func_ >
		std::enable_if_t< std::is_void< std::result_of_t< _func_(ThreadContext) > >::value >
		add_task(_func_ f) {
			struct task_guard_maker {
				_func_ f;
				task_guard_maker(_func_ f) : f(std::move(f)) {}
				task_guard_maker(task_guard_maker&& obj) : f(std::move(obj.f)) {}
				std::unique_ptr< thread_pool::task > ptr() { return std::unique_ptr< thread_pool::task >(new thread_pool::task(std::move(f))); }
			};
			_add_task(task_guard_maker(std::move(f)));
		}

		//(void, void, !void)
		template<typename _func_>
		std::enable_if_t< std::is_void< std::result_of_t< _func_() > >::value && !std::is_void< ThreadContext >::value >
		add_task(_func_ f) {
			struct task_caller_guard_maker {
				_func_ f;
				task_caller_guard_maker(_func_ f) : f(std::move(f)) {}
				task_caller_guard_maker(const task_caller_guard_maker& obj) : f(std::move(obj.f)) {}
				std::unique_ptr< thread_pool::task > ptr() { return std::unique_ptr< thread_pool::task >(new thread_pool::task(task_caller_guard_maker(std::move(f)))); }
				void operator()(ThreadContext) { f(); }
			};
			_add_task(task_caller_guard_maker(std::move(f)));
		}
		
		//(!void, void, void), (!void, void, !void)
		template< typename _func_ >
		std::enable_if_t< !std::is_void< std::result_of_t< _func_() > >::value, std::future< std::result_of_t< _func_() > > >
		add_task(_func_ f) {
			typedef std::result_of_t< _func_() > res_type;

			std::promise < res_type > prom;
			auto res = prom.get_future();

			struct move_param_lambda {
				mutable _func_ f;
				mutable std::promise < res_type > prom;
				move_param_lambda(_func_ f, std::promise < res_type > prom) : f(std::move(f)), prom(std::move(prom)) {}
				move_param_lambda(const move_param_lambda& obj) : f(std::move(obj.f)), prom(std::move(obj.prom)) {}
				std::unique_ptr< thread_pool::task > ptr() { return std::unique_ptr< thread_pool::task >(new thread_pool::task(move_param_lambda(std::move(f), std::move(prom)))); }
				void operator ()(ThreadContext) { prom.set_value(f()); }
			};
			_add_task(move_param_lambda(std::move(f), std::move(prom)));
		
			return res;
		}

		//(!void, !void, !void)
		template< typename _func_ >
		std::enable_if_t< !std::is_void< std::result_of_t< _func_(ThreadContext) > >::value && !std::is_void< ThreadContext >::value, std::future< std::result_of_t< _func_(ThreadContext) > > >
		add_task(_func_ f) {
			typedef std::result_of_t< _func_(ThreadContext) > res_type;

			std::promise < res_type > prom;
			auto res = prom.get_future();

			struct move_param_lambda {
				mutable _func_ f;
				mutable std::promise < res_type > prom;
				move_param_lambda(_func_ f, std::promise < res_type > prom) : f(std::move(f)), prom(std::move(prom)) {}
				move_param_lambda(const move_param_lambda& obj) : f(std::move(obj.f)), prom(std::move(obj.prom)) {}
				std::unique_ptr< thread_pool::task > ptr() { return std::unique_ptr< thread_pool::task >(new thread_pool::task(move_param_lambda(std::move(f), std::move(prom)))); }
				void operator ()(ThreadContext&& context) { prom.set_value(f(std::forward< ThreadContext >(context))); }
			};
			_add_task(move_param_lambda(std::move(f), std::move(prom)));

			return res;
		}

		~thread_pool(void) {
			done.store(true);
			thread_pool::task* task;
			while (task_queue.pop(task)) { 
				delete task;
			}
			for (auto& _thread_ : working_threads) _thread_.interrupt();
		}
	};
}
