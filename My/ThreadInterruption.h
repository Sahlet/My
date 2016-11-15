#pragma once
#include <thread>
#include <atomic>
#include <future>
#include <memory>

namespace My {
	struct interruption : public std::exception {
		interruption() : std::exception("tread interruption") {}
	};
	void interruption_point();

	class interruptible_thread {
		class interrupt_flag {
			std::weak_ptr< std::atomic< bool > > flag;
			interrupt_flag(std::weak_ptr< std::atomic< bool > > flag);
		public:
			interrupt_flag() = default;
			interrupt_flag(interrupt_flag&& obj) = default;
			interrupt_flag(const interrupt_flag& obj) = default;
			interrupt_flag& operator = (interrupt_flag&& obj) = default;
			interrupt_flag& operator = (const interrupt_flag& obj) = default;

			static interrupt_flag get_interrupt_flag();
			void set();
		} flag;
		std::thread t;
		static std::thread get_thread(std::function<void()> f);

	public:
		//template< class FunctionType >
		interruptible_thread(/*FunctionType*/std::function<void()> f) {
			std::promise< interrupt_flag > p;
			auto fres = p.get_future();

			t = get_thread([&p, f]() {
				p.set_value(interrupt_flag::get_interrupt_flag());
				f();
			});

			flag = std::move(fres.get());
		}

		inline void detach() {
			t.detach();
		}
		inline std::thread::id get_id() {
			t.get_id();
		}
		inline void join() {
			t.join();
		}
		inline bool joinable() {
			return t.joinable();
		}
		inline std::thread::native_handle_type native_handle() {
			return t.native_handle();
		}
		inline void interrupt() {
			flag.set();
		}
	};
}