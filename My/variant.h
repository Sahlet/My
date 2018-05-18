//variant.h

//made to copy semantic of Windows Forms UI object "tag" field
//you can write into variant object value of any type by some name and read it

#ifndef UTILS_VARIANT_H_
#define UTILS_VARIANT_H_
#include <memory>
#include <assert.h>
#include <map>
#include <string>

class variant
{

	class var_base
	{
		friend class variant;
	protected:

		var_base(std::reference_wrapper<const std::type_info> r) :
				type(r)
		{
		}
		virtual ~var_base()
		{
		}
		;
		virtual std::shared_ptr<var_base> clone() = 0;
	public:
		std::reference_wrapper<const std::type_info> type;
	};

	template<class T>
	class var_typed: public var_base
	{
	public:
		T value;

		var_typed(const T& v) :
				var_base(typeid(T)), value(v)
		{
		}

		virtual std::shared_ptr<var_base> clone() override
		{
			return std::make_shared < var_typed < T >> (value);
		}

		virtual ~var_typed()
		{
		}
		;
	};
	std::shared_ptr<var_base> typed;

public:
	//      LEAK_TEST
	variant()
	{
	}
	variant(const variant& r)
	{
		typed = r.typed;
	}
	virtual ~variant()
	{
		typed.reset();
	}

	template<class _T_>
	void operator =(_T_&& value)
	{
		using T = typename std::remove_cv< typename std::remove_reference< _T_ >::type >::type;

		if (typed && typed->type.get() == typeid(T)) {
			static_cast< var_typed< T >* >(typed.get())->value = std::forward< T >(value);
		} else {
			typed.reset(new var_typed< T >(std::forward< T >(value)));
		}
	}

	void operator = (variant value)
	{
		typed = value.typed->clone();
		if (!typed) typed = nullptr;
	}

	template<class T>
	T & as() {
		var_typed<T>* var = dynamic_cast<var_typed<T>*>(typed.get());

		if (var) return var->value;

		assert(false);
		throw std::logic_error("wrong cast type");
	}

	template<class T>
	T * as_ptr() {
		var_typed<T>* var = dynamic_cast<var_typed<T>*>(typed.get());

		if (var) return &var->value;

		return nullptr;
	}

	bool has_value()
	{
		return !!typed;
	}

	template<class T>
	bool exists()
	{
		return !!dynamic_cast<var_typed<T>*>(typed.get());
	}

	void clear()
	{
		typed.reset();
	}
};

class VariantProvider
{
	std::map<std::string, variant> params;

public:
	template<class T>
	T * read_param(const std::string &s) {
		auto it = params.find(s);

		if (it == params.end()) return nullptr;

		return (it->second).as_ptr<T>();
	}

	variant& get_param(const std::string &s)
	{
		return params[s];
	}

	template<class T>
	void write_param(const std::string &s, T&& value)
	{
		params[s] = std::forward<T>(value);
	}
};


#endif /* UTILS_VARIANT_H_ */
