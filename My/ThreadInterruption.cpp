#include "ThreadInterruption.h"
#include "mutex"
namespace My {

	static _declspec(thread) std::shared_ptr< std::atomic< bool > >* this_thread_interrupt_flag = nullptr;
	static _declspec(thread) class interrupt_flag_checker {
	public:
		inline bool is_set() const {
			return this_thread_interrupt_flag && *this_thread_interrupt_flag && (*this_thread_interrupt_flag)->load();
		}

		inline void reset() {
			if (this_thread_interrupt_flag && *this_thread_interrupt_flag) (*this_thread_interrupt_flag)->store(false);
		}

		inline std::weak_ptr< std::atomic< bool > > get_wptr() {
			if (!this_thread_interrupt_flag) return std::weak_ptr< std::atomic< bool > >();
			return std::weak_ptr< std::atomic< bool > >(*this_thread_interrupt_flag);
		}
	} this_thread_interrupt_flag_checker;

	interruptible_thread::interrupt_flag::interrupt_flag(std::weak_ptr< std::atomic< bool > > flag) : flag(flag) {}

	interruptible_thread::interrupt_flag interruptible_thread::interrupt_flag::get_interrupt_flag() {
		return interruptible_thread::interrupt_flag(this_thread_interrupt_flag_checker.get_wptr());
	}

	void interruptible_thread::interrupt_flag::set() {
		auto shared_flag = flag.lock();
		if (shared_flag) shared_flag->store(true);
	}

	void interruption_point() {
		if (this_thread_interrupt_flag_checker.is_set()) {
			this_thread_interrupt_flag_checker.reset();
			throw interruption();
		}
	}

	std::thread interruptible_thread::get_thread(std::function<void()> f) {
		return std::thread([f]() {
			std::shared_ptr< std::atomic< bool > > ptr(std::make_shared< std::atomic< bool > >(false));
			this_thread_interrupt_flag = &ptr;
			f();
		});
	}
}