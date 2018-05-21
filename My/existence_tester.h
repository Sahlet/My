//existence_tester.h

#ifndef __EXISTENCE_TESTER_H__
#define __EXISTENCE_TESTER_H__

#include <memory>

namespace My {

#define THIS_EXISTS (get_existence_tester())
#define EXISTENCE_FLAG get_existence_tester = [__existence_tester__=get_existence_tester()](){ return __existence_tester__; }

class existence_tester {
	std::shared_ptr<bool> __existence_flag__;
public:
	existence_tester() {
		__existence_flag__.reset(new bool(true));
	}
	~existence_tester() {
		*__existence_flag__ = false;
	}

	existence_tester& operator=(const existence_tester&) = delete;

	struct __tester__ {
		inline bool operator()() const {
			auto flag = get_flag();
			return flag && *flag;
		}
		inline operator bool() const {
			return (*this)();
		}
		inline bool operator==(const __tester__& obj) const {
			auto flag = get_flag();
			return flag && flag == obj.get_flag();
		}

		__tester__() {}
	private:
		friend class existence_tester;
		std::weak_ptr<bool> wflag;

		__tester__(std::weak_ptr<bool> wflag) : wflag(std::move(wflag)) {}
		inline std::shared_ptr<bool> get_flag() const { return wflag.lock(); }
	};

	inline __tester__ get_existence_tester() const {
		return __tester__(__existence_flag__);
	}
};

#endif /*__EXISTENCE_TESTER_H__*/

} // namespace My