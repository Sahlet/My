//Properties.h

#ifndef PROPERTIES_H_
#define PROPERTIES_H_

#include <set>
#include <list>
#include <Log.h>
#include <memory>
//#include <algorithm>
//#include <typeinfo>

#include <existence_tester.h>

namespace My {
	template< typename FUNC >
	class func_manager;

	class prop_handler;

	class prop_helper {
		template< typename FUNC >
		friend class func_manager;

		existence_tester::__tester__ creator_existence_tester;
		existence_tester::__tester__ owner_existence_tester;
		std::function<void()> remove_func;
		prop_helper(existence_tester::__tester__ creator_existence_tester, existence_tester::__tester__ owner_existence_tester) :
			creator_existence_tester(creator_existence_tester), owner_existence_tester(owner_existence_tester) {}
	public:
		using ptr = std::shared_ptr< prop_helper >;
		using wptr = std::weak_ptr< prop_helper >;

		virtual ~prop_helper();
	};
	class prop_handler : public existence_tester {
		template< typename FUNC >
		friend class func_manager;

		std::list< prop_helper::ptr > helpers;
		bool in_prop_handler_destructor = false;

	public:
		virtual ~prop_handler() {
			in_prop_handler_destructor = true;
			helpers.clear();
			in_prop_handler_destructor = false;
		}
	};

	//using __FUNC__ = void();
	template< typename __FUNC__ >
	class func_manager : public prop_handler {
	public:
		typedef int priority;
		typedef std::function< __FUNC__ > FUNC;
		typedef std::shared_ptr< FUNC > FUNC_PTR;
	private:
		struct handler_data {
			FUNC_PTR func;
			prop_helper* helper_rawptr;
		};
		struct more {
			bool operator()(const priority& lhs, const priority& rhs) const { return lhs > rhs; }
		};

		std::map< priority, std::list< handler_data > , more > handlers;
		bool delete_lists_from_handlers = true;

		typedef std::function< void() > cleaner;
		std::list< cleaner > cleaners;

		struct remove_func_data {
			using ptr = std::shared_ptr< remove_func_data >;
			std::list< handler_data >* list_ptr = nullptr;
			typename std::list< handler_data >::iterator iter;
		};
		struct handler_data_to_add {
			FUNC_PTR func;
			prop_helper::ptr helper;
			priority p;

			typename remove_func_data::ptr additional_data;
		};
		std::list< handler_data_to_add > to_add;

		bool critical_section = false;
		void init_remove_func_data(handler_data_to_add& data) {
			if (data.helper.use_count() > 1) {
				auto& list = handlers[data.p];
				list.emplace_back(handler_data{ data.func, data.helper.get() });
				data.additional_data->list_ptr = &list;
				data.additional_data->iter = std::prev(list.end());
				if (default_state) default_state(*data.func);
			}
		}
	public:
		func_manager() {}
		func_manager(const func_manager &) = delete;
		func_manager &operator =(const func_manager &) = delete;

		std::function<void(FUNC f)> default_state;

		int get_handlers_number() const {
			int res = 0;
			for (auto& pare : handlers) {
				res += pare.second.size();
			}
			return res;
		}

		bool get_critical_section() { return critical_section; }

		void detach(prop_handler* owner) {
			if (!owner) return;
			auto tester = get_existence_tester();
			for (auto iter = owner->helpers.begin(), end = owner->helpers.end(); iter != end;) {
				auto prev = iter++;
				if (tester == (*prev)->creator_existence_tester) {
					(*prev)->remove_func();
				}
			}
		}

		struct handler_controler {
			void detach_handler()  {
				auto helper = get_helper();
				if (helper && helper->remove_func) helper->remove_func();
			}
			bool valid() const {
				auto helper = get_helper();
				return helper && helper->remove_func;
			}
		private:
			friend class func_manager;
			prop_helper::wptr whelper;

			handler_controler(prop_helper::wptr whelper = prop_helper::wptr()) : whelper(std::move(whelper)) {}
			inline prop_helper::ptr get_helper() const { return whelper.lock(); }
		};

		handler_controler register_handler(prop_handler* owner, FUNC_PTR f, priority p = 0) {
			if (!f || !*f) {
				LOGE("FIND ME in %s : if (!f || !*f)", __func__);
				return handler_controler();
			}

			if (!owner) owner = this;
			prop_helper::ptr helper(new prop_helper(this->get_existence_tester(), owner->get_existence_tester()));
			owner->helpers.push_back(helper);
			auto additional_data = std::make_shared<remove_func_data>();
			helper->remove_func = [this, h = helper.get(), f, owner, owner_iter = std::prev(owner->helpers.end()), additional_data, p]() {
				auto next_remove_func = [this, f, additional_data, p]() mutable {
					f.reset();
					if (!additional_data || !additional_data->list_ptr) return;
					additional_data->list_ptr->erase(additional_data->iter);
					if (delete_lists_from_handlers && additional_data->list_ptr->empty()) {
						auto map_iter = handlers.find(p);
						if (map_iter != handlers.end() && &map_iter->second == additional_data->list_ptr) {
							handlers.erase(map_iter);
						}
					}
					additional_data.reset();
				};

				if (additional_data->list_ptr) additional_data->iter->func.reset();

				if (critical_section) cleaners.emplace_back(next_remove_func);
				else next_remove_func();

				//makeing copies on stack because 'h->remove_func = nullptr;' deletes originals
				auto owner_copy = owner;
				auto owner_iter_copy = owner_iter;
				auto owner_existence_tester_copy = h->owner_existence_tester;
				h->remove_func = nullptr;
				if (owner_existence_tester_copy() && !owner_copy->in_prop_handler_destructor) {
					owner_copy->helpers.erase(owner_iter_copy);
				}
			};

			auto data_to_add = handler_data_to_add{ f, helper, p, additional_data };
			if (!critical_section) init_remove_func_data(data_to_add);
			else to_add.emplace_back(std::move(data_to_add));

			return handler_controler(helper);
		}
		inline handler_controler register_handler(prop_handler* owner, FUNC func, priority p = 0) {
			if (!func) {
				LOGE("FIND ME in %s : if (!func)", __func__);
				return handler_controler();
			}
			return register_handler(owner, std::make_shared< FUNC >(func), p);
		}

		template<class ...Args>
		void operator()(Args...args) {
			if (critical_section) {
				LOGW("FIND ME in %s : operator() was called when critical_section", __func__);
				return;
			}

			critical_section = true;
			for (auto& pair : handlers) {
				auto& list = pair.second;
				for (auto& data : list) {
					if (data.func) (*data.func)(args...);
				}
			}
			critical_section = false;
			for (auto& f : cleaners) f();
			cleaners.clear();

			for (auto& data_to_add : to_add) {
				init_remove_func_data(data_to_add);
			}
			to_add.clear();
		}

		~func_manager() {
			if (critical_section) {
				LOGE("FIND ME in %s : destructor was called when critical_section", __func__);
				std::terminate();
				return;
			}

			delete_lists_from_handlers = false;

			for (auto& pair : handlers) {
				auto& list = pair.second;
				for (auto iter = list.begin(), end = list.end(); iter != end;) {
					iter++->helper_rawptr->remove_func();
				}
			}
		}
	};

	template<class T>
	struct event : public func_manager< T > {
	public:
		event() {}
		void operator=(const std::function<T> &f) {
			func_manager< T >::register_handler(nullptr, f);
		}
	};

	template<class T>
	class prop : public existence_tester {
	public:
		using event_type = event< void(const T&) >;
		using func_type = std::function< void(const T &) >;
		using manager_t = func_manager< void(const T &) >;
	private:
		manager_t manager;
		T value;

		prop<T> &operator =(const prop<T> &) = delete;
	public:
		prop() : value() {}

		int get_handlers_number() const { return manager.get_handlers_number(); }

		bool get_critical_section() { return manager.get_critical_section(); }

		void detach(prop_handler* owner) {
			manager.detach(owner);
		}

		inline typename manager_t::handler_controler register_change(prop_handler* owner, const func_type& func, typename manager_t::priority p = 0, bool call_now = true) {
			auto res = manager.register_handler(owner, func, p);
			if (call_now && res.valid()) func(value);
			return res;
		}

		inline typename manager_t::handler_controler register_change(event_type& e, typename manager_t::priority p = 0) {
			auto func = [&e](const T&d) { e(d); };
			e.default_state = [this, EXISTENCE_FLAG](std::function<void(T)> f2) {
				if (THIS_EXISTS) f2(value);
			};

			return register_change(&e, func, p);
		}

		operator T() const {
			return value;
		}

		const T operator=(const T &r) {
			if (value != r) {
				set(r);
			}
			return r;
		}

		void set(const T &r) {
			if (manager.get_critical_section()) return;

			value = r;
			manager(value);
		}

		const bool operator==(const T &r) const {
			return value == r;
		}

		const T* operator->() const {
			return &value;
		}

		const T &get() const {
			return value;
		}
	};
} //namespace My

#endif /* PROPERTIES_H_ */
