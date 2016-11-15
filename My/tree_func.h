//tree_func.h
#pragma once
#include<functional>
#include<memory>
#include<ostream>
#include<set>
#include<map>
#include<vector>
#include<stack>
#include<typeinfo>
#include<string>
#include<strstream>
#include<iostream>


namespace My{
using namespace std;
typedef long double T;

template<class T>
vector<T> get_vector(unsigned int n, ...){
	T* ptr = (T*)(&n + 1);
	vector<T> res(n);
	for(auto& i : res) i = *ptr++;
	return res;
}

class tree_func;
struct Node;
//#define u_p_N unique_ptr<Node> //вообще надо unique_ptr, но компилятор кидает каую-то лажу
#define u_p_N shared_ptr<Node>

struct Node{
	T operator()(const vector<T>& v) const {return this->f(v);}
	//возвращает значение узла в точке v
	virtual T f(const vector<T>& v) const = 0;
	//возвращает производную по n-ой переменной
	virtual u_p_N d_xn(unsigned int n = 0) const = 0;
	//возвращает минимизированый узел эквивалентный этому (можно использовать функцию, как функцию копирования)
	virtual u_p_N min() const = 0;
	virtual ~Node(){}
};
struct _value : public Node{
	T val;
	_value(const T& val) : val(val){}
	virtual T f(const vector<T>& v)  const override {return val;}
	virtual u_p_N d_xn(unsigned int n = 0)  const override {
		return u_p_N(new _value(0));
	};
	virtual u_p_N min()  const override{
		return u_p_N(new _value(val));
	};
};
#define get_value(val) u_p_N(new _value(val))

struct _x_n : public Node{
protected:
	unsigned int n;
public:
	_x_n(const unsigned int & n) : n(n){}
	virtual T f(const vector<T>& v)  const override {
		return v[n];
	}
	virtual u_p_N d_xn(unsigned int n = 0) const override {
		if (n == this->n) return u_p_N(new _value(1));
		return u_p_N(new _value(0));
	};
	virtual u_p_N min() const override{
		return u_p_N(new _x_n(n));
	};
};
struct _if_else : public Node{
protected:
	u_p_N conditions, first, second;
public:
	_if_else(u_p_N conditions, u_p_N first, u_p_N second) : conditions(std::move(conditions)), first(std::move(first)), second(std::move(second)) {
		if (!this->conditions || !this->first || !this->second) throw exception_ptr();
	}
	virtual T f(const vector<T>& v)  const override {
		if (conditions->f(v)) return first->f(v);
		else return second->f(v);
	}
	virtual u_p_N d_xn(unsigned int n = 0)  const override {
		auto c_m = conditions->min();
		auto d_f = first->d_xn(n), d_s = second->d_xn(n);
		return u_p_N(new _if_else(conditions->min(), first->d_xn(n), second->d_xn(n)));
	};
	virtual u_p_N min()  const override{
		auto c_m = conditions->min();
		if(auto tmp = dynamic_cast<_value*>(c_m.get())){
			if(tmp->val) return first->min();
			else return second->min();
		}
		return u_p_N(new _if_else(std::move(c_m), first->min(), second->min()));
	};
};

struct _operator : public Node{};

//
struct _uno : public _operator{
	friend class tree_func;
protected:
	u_p_N ptr;
public:
	_uno(u_p_N ptr) : ptr(std::move(ptr)){
		if (!this->ptr) throw exception_ptr();
	}
};
struct _uno_pre : public _uno{
	_uno_pre(u_p_N ptr) : _uno(std::move(ptr)){}
};
struct _uno_post : public _uno{
	_uno_post(u_p_N ptr) : _uno(std::move(ptr)){}
};

//арифм
struct _uno_plus : public _uno_pre{
	_uno_plus(u_p_N ptr) : _uno_pre(std::move(ptr)){}
	virtual T f(const vector<T>& v)  const override {return +ptr->f(v);}
	virtual u_p_N d_xn(unsigned int n = 0)  const override {
		return ptr->d_xn(n);
	};
	virtual u_p_N min()  const override{
		return ptr->min();
	};
};
struct _uno_minus : public _uno_pre{
	_uno_minus(u_p_N ptr) : _uno_pre(std::move(ptr)){}
	virtual T f(const vector<T>& v)  const override {return -ptr->f(v);}
	virtual u_p_N d_xn(unsigned int n = 0)  const override {
		return u_p_N (new _uno_minus(ptr->d_xn(n)));
	};
	virtual u_p_N min()  const override{
		auto sub_res = ptr->min();
		if (auto tmp = dynamic_cast<_uno_minus*>(sub_res.get())) return std::move(tmp->ptr);
		if (auto tmp = dynamic_cast<_value*>(sub_res.get())) return u_p_N(new _value(-tmp->val));
		return u_p_N (new _uno_minus(std::move(sub_res)));
	};
};

//лог
struct _not : public _uno_pre{
	_not(u_p_N ptr) : _uno_pre(std::move(ptr)){}
	virtual T f(const vector<T>& v)  const override {return !ptr->f(v);}
	virtual u_p_N d_xn(unsigned int n = 0)  const override {
		return get_value(0);
	};
	virtual u_p_N min()  const override{
		auto sub_res = ptr->min();
		if (auto tmp = dynamic_cast<_value*>(sub_res.get())) return get_value(!tmp->val);
		return u_p_N(new _not(std::move(sub_res)));
	};
};

////
struct _bi : public _operator{
	friend class tree_func;
protected:
	u_p_N l, r;
public:
	_bi(u_p_N l, u_p_N r) : l(std::move(l)), r(std::move(r)){
		if (!this->l || !this->r) throw exception_ptr();
	}
	virtual u_p_N d_xn(unsigned int n = 0)  const override {return get_value(0);}
	virtual ~_bi(){}
};

//арифм
struct _plus : public _bi{
	_plus(u_p_N l, u_p_N r) : _bi(std::move(l), std::move(r)){}
	virtual T f(const vector<T>& v)  const override {return l->f(v) + r->f(v);}
	virtual u_p_N d_xn(unsigned int n = 0)  const override {
		return u_p_N(new _plus(l->d_xn(n), r->d_xn(n)));
	};
	virtual u_p_N min()  const override{
		auto l_m = this->l->min(), r_m = this->r->min();
		auto l_v = dynamic_cast<_value*>(l_m.get()), r_v = dynamic_cast<_value*>(r_m.get());
		if (l_v && l_v->val == 0) return r_m;
		if (r_v && r_v->val == 0) return l_m;
		if (l_v && r_v) return get_value(l_v->val + r_v->val);
		return u_p_N(new _plus(std::move(l_m), std::move(r_m)));
	};
};
struct _pow : public _bi{
	_pow(u_p_N l, u_p_N r) : _bi(std::move(l), std::move(r)){}
	virtual T f(const vector<T>& v)  const override {return pow(l->f(v), r->f(v));}
	virtual u_p_N d_xn(unsigned int n = 0)  const override;
	virtual u_p_N min()  const override{
		auto l_m = this->l->min(), r_m = this->r->min();
		auto l_v = dynamic_cast<_value*>(l_m.get()), r_v = dynamic_cast<_value*>(r_m.get());
		if (r_v && r_v->val == 0) return get_value(1);
		if (l_v && l_v->val == 0) return get_value(0);
		if (l_v && r_v) return get_value(pow(l_v->val, r_v->val));
		return u_p_N(new _pow(std::move(l_m), std::move(r_m)));
	};
};
struct _mult : public _bi{
	_mult(u_p_N l, u_p_N r) : _bi(std::move(l), std::move(r)){}
	virtual T f(const vector<T>& v)  const override { return l->f(v) * r->f(v);}
	virtual u_p_N d_xn(unsigned int n = 0)  const override {
		return u_p_N(new _plus(u_p_N(new _mult(r->d_xn(n), l->min())), u_p_N(new _mult(r->min(), l->d_xn(n)))));
	};
	virtual u_p_N min()  const override{
		auto l_m = this->l->min(), r_m = this->r->min();
		auto l_v = dynamic_cast<_value*>(l_m.get()), r_v = dynamic_cast<_value*>(r_m.get());
		if (l_v && l_v->val == 0 || r_v && r_v->val == 0) get_value(0);
		if (l_v && r_v) return get_value(l_v->val * r_v->val);
		return u_p_N(new _mult(std::move(l_m), std::move(r_m)));
	};
};
struct _div : public _bi{
	_div(u_p_N l, u_p_N r) : _bi(std::move(l), std::move(r)){}
	virtual T f(const vector<T>& v)  const override {return l->f(v) / r->f(v);}
	virtual u_p_N d_xn(unsigned int n = 0)  const override {
		auto tmp = this->min();
		if (dynamic_cast<_div*>(tmp.get())) return u_p_N(new _mult(l->min(), u_p_N(new _pow(r->min(),get_value(-1)))))->d_xn(n);
		else return tmp->d_xn(n);
	};
	virtual u_p_N min()  const override{
		auto l_m = this->l->min(), r_m = this->r->min();
		auto l_v = dynamic_cast<_value*>(l_m.get()), r_v = dynamic_cast<_value*>(r_m.get());
		if (l_v && l_v->val == 0) return get_value(0);
		if (l_v && r_v) return u_p_N(new _value(l_v->val / r_v->val));
		return u_p_N(new _div(std::move(l_m), std::move(r_m)));
	};
};
struct _minus : public _bi{
	_minus(u_p_N l, u_p_N r) : _bi(std::move(l), std::move(r)){}
	virtual T f(const vector<T>& v)  const override {return l->f(v) - r->f(v);}
	virtual u_p_N d_xn(unsigned int n = 0)  const override {
		return u_p_N(new _minus(l->d_xn(n), r->d_xn(n)));
	};
	virtual u_p_N min()  const override{
		auto l_m = this->l->min(), r_m = this->r->min();
		auto l_v = dynamic_cast<_value*>(l_m.get()), r_v = dynamic_cast<_value*>(r_m.get());
		if (l_v && l_v->val == 0) return u_p_N(new _uno_minus(std::move(r_m)))->min();
		if (r_v && r_v->val == 0) return l_m;
		if (l_v && r_v) return get_value(l_v->val - r_v->val);
		return u_p_N(new _minus(std::move(l_m), std::move(r_m)));
	};
};

//лог
struct _less : public _bi{
	_less(u_p_N l, u_p_N r) : _bi(std::move(l), std::move(r)){}
	virtual T f(const vector<T>& v)  const override {return l->f(v) < r->f(v);}
	virtual u_p_N min()  const override{
		auto l_m = this->l->min(), r_m = this->r->min();
		auto l_v = dynamic_cast<_value*>(l_m.get()), r_v = dynamic_cast<_value*>(r_m.get());
		if (l_v && r_v) return get_value(l_v->val < r_v->val);
		return u_p_N(new _less(std::move(l_m), std::move(r_m)));
	};
};
struct _less_or_is : public _bi{
	_less_or_is(u_p_N l, u_p_N r) : _bi(std::move(l), std::move(r)){}
	virtual T f(const vector<T>& v)  const override {return l->f(v) <= r->f(v);}
	virtual u_p_N min()  const override{
		auto l_m = this->l->min(), r_m = this->r->min();
		auto l_v = dynamic_cast<_value*>(l_m.get()), r_v = dynamic_cast<_value*>(r_m.get());
		if (l_v && r_v) return get_value(l_v->val <= r_v->val);
		return u_p_N(new _less_or_is(std::move(l_m), std::move(r_m)));
	};
};
struct _more : public _bi{
	_more(u_p_N l, u_p_N r) : _bi(std::move(l), std::move(r)){}
	virtual T f(const vector<T>& v)  const override {return l->f(v) > r->f(v);}
	virtual u_p_N min()  const override{
		auto l_m = this->l->min(), r_m = this->r->min();
		auto l_v = dynamic_cast<_value*>(l_m.get()), r_v = dynamic_cast<_value*>(r_m.get());
		if (l_v && r_v) return get_value(l_v->val > r_v->val);
		return u_p_N(new _more(std::move(l_m), std::move(r_m)));
	};
};
struct _more_or_is : public _bi{
	_more_or_is(u_p_N l, u_p_N r) : _bi(std::move(l), std::move(r)){}
	virtual T f(const vector<T>& v)  const override {return l->f(v) >= r->f(v);}
	virtual u_p_N min()  const override{
		auto l_m = this->l->min(), r_m = this->r->min();
		auto l_v = dynamic_cast<_value*>(l_m.get()), r_v = dynamic_cast<_value*>(r_m.get());
		if (l_v && r_v) return get_value(l_v->val >= r_v->val);
		return u_p_N(new _more_or_is(std::move(l_m), std::move(r_m)));
	};
};
struct _equally : public _bi{
	_equally(u_p_N l, u_p_N r) : _bi(std::move(l), std::move(r)){}
	virtual T f(const vector<T>& v)  const override {return l->f(v) == r->f(v);}
	virtual u_p_N min()  const override{
		auto l_m = this->l->min(), r_m = this->r->min();
		auto l_v = dynamic_cast<_value*>(l_m.get()), r_v = dynamic_cast<_value*>(r_m.get());
		if (l_v && r_v) return get_value(l_v->val == r_v->val);
		return u_p_N(new _equally(std::move(l_m), std::move(r_m)));
	};
};
struct _not_equally : public _bi{
	_not_equally(u_p_N l, u_p_N r) : _bi(std::move(l), std::move(r)){}
	virtual T f(const vector<T>& v)  const override {return l->f(v) != r->f(v);}
	virtual u_p_N min()  const override{
		auto l_m = this->l->min(), r_m = this->r->min();
		auto l_v = dynamic_cast<_value*>(l_m.get()), r_v = dynamic_cast<_value*>(r_m.get());
		if (l_v && r_v) return get_value(l_v->val != r_v->val);
		return u_p_N(new _not_equally(std::move(l_m), std::move(r_m)));
	};
};
struct _and : public _bi{
	_and(u_p_N l, u_p_N r) : _bi(std::move(l), std::move(r)){}
	virtual T f(const vector<T>& v)  const override {return l->f(v) && r->f(v);}
	virtual u_p_N min()  const override{
		auto l_m = this->l->min(), r_m = this->r->min();
		auto l_v = dynamic_cast<_value*>(l_m.get()), r_v = dynamic_cast<_value*>(r_m.get());
		if (l_v && l_v->val == 0 || r_v && r_v->val == 0) get_value(0);
		if (l_v && r_v) return get_value(l_v->val && r_v->val);
		return u_p_N(new _and(std::move(l_m), std::move(r_m)));
	};
};
struct _or : public _bi{
	_or(u_p_N l, u_p_N r) : _bi(std::move(l), std::move(r)){}
	virtual T f(const vector<T>& v)  const override {return l->f(v) || r->f(v);}
	virtual u_p_N min()  const override{
		auto l_m = this->l->min(), r_m = this->r->min();
		auto l_v = dynamic_cast<_value*>(l_m.get()), r_v = dynamic_cast<_value*>(r_m.get());
		if (l_v && l_v->val == 0 && r_v && r_v->val == 0) get_value(0);
		if (l_v && r_v) return get_value(l_v->val || r_v->val);
		return u_p_N(new _or(std::move(l_m), std::move(r_m)));
	};
};

struct _abs : public _if_else{
	_abs(u_p_N arg) : _if_else(u_p_N(new _more_or_is(arg->min(), u_p_N(new _value(0)))), arg->min(), u_p_N(new _uno_minus(arg->min()))) {}
};
struct _func : public Node{
protected:
	const vector< u_p_N > args;
	vector<T> get_value_vector_in(const vector<T>& v) const {
		vector<T> res(args.size());
		for (size_t i = 0; i < res.size(); i++) res[i] = args[i]->f(v);
		return res;
	}
	static vector< u_p_N > to_vector(u_p_N arg){
		vector< u_p_N > res(1);
		res[0] = std::move(arg);
		return std::move(res);
	}
	static vector< u_p_N > to_vector(u_p_N arg1, u_p_N arg2){
		vector< u_p_N > res(2);
		res[0] = std::move(arg1);
		res[1] = std::move(arg2);
		return std::move(res);
	}
	_func(vector< u_p_N > args) : args(std::move(args)){
		for (auto& i : this->args)
			if(!i) throw exception_ptr();
	}
	/*
	взять частичную производную по n-тому параметру
	пример реализации:
		частная производная exp(x) по первому параметру (не аргументу) - это exp(x).
		теперь на место всех нужных аргументов надо подставить соответствующие копии аргументов из args.
		в нашем примере 1 параметр => на мето x пойдет копия первого члена вектора args.
		возвращаем готорую частную производную
	*/
	virtual u_p_N get_partial_d(const vector< u_p_N >& args, unsigned int n = 0) const = 0;
	//взять значение функции в точке
	virtual T get_value_in(const vector<T>& v) const = 0;
	//нужно переопределить так, чтоб можно было получить функцию
	virtual _func* get_func(vector< u_p_N > args) const = 0;
public:
	virtual u_p_N d_xn(unsigned int n = 0) const override final{
		u_p_N res = get_value(0);
		for (size_t i = 0; i < args.size(); i++) res = u_p_N(new _plus(u_p_N(new _mult(args[i]->d_xn(n), get_partial_d(args, i))), std::move(res)));
		return res;
	}
	virtual u_p_N min()  const override final{
		vector< u_p_N > min_args(args.size());
		//bool ch = true;
		for (size_t i = 0; i < min_args.size(); i++){
			min_args[i] = args[i]->min();
			//ch = ch && dynamic_cast<_value*>(min_args[i].get());
		}
		/*if (ch){
			vector< T > v(min_args.size());
			for (size_t i = 0; i < v.size(); i++) v[i] = dynamic_cast<_value*>(min_args[i].get())->val;
			return get_value(this->get_value_in(v));
		} else */
		return u_p_N(get_func(std::move(min_args)));
	}
	virtual T f(const vector<T>& v) const override final{
		return get_value_in(get_value_vector_in(v));
	}
};
struct _exp : public _func{
	_exp(u_p_N arg) : _func(to_vector(std::move(arg))){}
protected:
	virtual u_p_N get_partial_d(const vector< u_p_N >& args, unsigned int n = 0) const{
		//n is 0
		return u_p_N(new _exp(args[0]->min()));
	}
	virtual T get_value_in(const vector<T>& v) const{ return exp(v[0]); }
	virtual _func* get_func(vector< u_p_N > args) const{ return new _exp(std::move(args[0])); }
};
struct _log : public _func{
	_log(u_p_N arg) : _func(to_vector(std::move(arg))){}
protected:
	virtual u_p_N get_partial_d(const vector< u_p_N >& args, unsigned int n = 0) const{
		//n is 0
		return u_p_N(new _if_else(u_p_N(new _more_or_is(args[0]->min(), get_value(0))),
			u_p_N(new _div(get_value(1), args[0]->min())), get_value(numeric_limits<T>::signaling_NaN())));
	}
	virtual T get_value_in(const vector<T>& v) const{ return log(v[0]); }
	virtual _func* get_func(vector< u_p_N > args) const{ return new _log(std::move(args[0])); }
};
struct _sin : public _func{
	_sin(u_p_N arg) : _func(to_vector(std::move(arg))){}
protected:
	virtual u_p_N get_partial_d(const vector< u_p_N >& args, unsigned int n = 0) const;
	virtual T get_value_in(const vector<T>& v) const{ return sin(v[0]); }
	virtual _func* get_func(vector< u_p_N > args) const{ return new _sin(std::move(args[0])); }
};
struct _cos : public _func{
	_cos(u_p_N arg) : _func(to_vector(std::move(arg))){}
protected:
	virtual u_p_N get_partial_d(const vector< u_p_N >& args, unsigned int n = 0) const{
		//n is 0
		return u_p_N(new _uno_minus(u_p_N(new _sin(args[0]->min()))));
	}
	virtual T get_value_in(const vector<T>& v) const{ return cos(v[0]); }
	virtual _func* get_func(vector< u_p_N > args) const{ return new _cos(std::move(args[0])); }
};
struct _mod_1 : public _func{
	_mod_1(u_p_N arg) : _func(to_vector(std::move(arg))){}
protected:
	virtual u_p_N get_partial_d(const vector< u_p_N >& args, unsigned int n = 0) const{
		//n is 0
		return get_value(1);
	}
	virtual T get_value_in(const vector<T>& v) const{ return fmodl(v[0], 1); }
	virtual _func* get_func(vector< u_p_N > args) const{ return new _mod_1(std::move(args[0])); }
};
struct _mod_2 : public _func{
	_mod_2(u_p_N arg1, u_p_N arg2) : _func(to_vector(std::move(arg1), std::move(arg2))){}
protected:
	virtual u_p_N get_partial_d(const vector< u_p_N >& args, unsigned int n = 0) const{
		return get_value(1);
	}
	virtual T get_value_in(const vector<T>& v) const{ return fmodl(v[0], v[2]); }
	virtual _func* get_func(vector< u_p_N > args) const{ return new _mod_2(std::move(args[0]), std::move(args[1])); }
};
struct _trunc : public _func{
	_trunc(u_p_N arg) : _func(to_vector(std::move(arg))){}
protected:
	virtual u_p_N get_partial_d(const vector< u_p_N >& args, unsigned int n = 0) const{
		//n is 0
		return get_value(0);
	}
	virtual T get_value_in(const vector<T>& v) const{ return v[0] - fmodl(v[0], 1); }
	virtual _func* get_func(vector< u_p_N > args) const{ return new _trunc(std::move(args[0])); }
};

u_p_N _sin::get_partial_d(const vector< u_p_N >& args, unsigned int n) const{
	//n is 0
	return u_p_N(new _cos(args[0]->min()));
}
struct _some_func : public _func{
	_some_func(u_p_N func, vector< u_p_N > args) : func(std::move(func)), _func(std::move(args)) {}
protected:
	u_p_N func;
	virtual u_p_N get_partial_d(const vector< u_p_N >& args, unsigned int n = 0) const{
		vector< u_p_N > args_copy(args.size());
		for (size_t i = 0; i < args.size(); i++) args_copy[i] = args[i]->min();
		return u_p_N(new _some_func(func->d_xn(n), std::move(args_copy)));
	}
	virtual T get_value_in(const vector<T>& v) const{ return func->f(v); }
	virtual _func* get_func(vector< u_p_N > args) const{ return new _some_func(func->min(), std::move(args)); }
};
struct _bracket : public Node{
protected:
	u_p_N ptr;
public:
	_bracket(u_p_N ptr) : ptr(std::move(ptr)){if(!this->ptr) throw exception_ptr();}
	virtual T f(const vector<T>& v)  const override {return ptr->f(v);}
	virtual u_p_N d_xn(unsigned int n = 0)  const override { return ptr->d_xn(n); }
	virtual u_p_N min()  const override{return ptr->min();};
};
struct _print : public Node{
protected:
	u_p_N ptr;
public:
	_print(u_p_N ptr) : ptr(std::move(ptr)){}
	virtual T f(const vector<T>& v)  const override {
		auto res = ptr->f(v);
		cout << res;
		return res;
	}
	virtual u_p_N d_xn(unsigned int n = 0)  const override {
		return u_p_N (new _print(ptr->d_xn(n)));
	};
	virtual u_p_N min()  const override{
		return u_p_N (new _print(ptr->min()));
	};
};
///////////////////////
u_p_N _pow::d_xn(unsigned int n) const{
	/*
	y = f(x)^g(x)
	ln(y) = g(x)*ln(f(x))
	ln(y)' = (g(x)*ln(f(x)))'
	y'/y = (g(x)*ln(f(x)))' 
	y' = y*(g(x)*ln(f(x)))'
	*/
	return u_p_N(new _mult(
		this->min(),
		u_p_N(new _mult
				(
					r->min(),
					u_p_N(new _log(l->min()))
				)
			 )->d_xn(n)
		)
	);
};

const auto max_priority = 8u;
//чем больше число вернет - тем больше приоритет
unsigned int get_priority(Node* node){
	auto bi = dynamic_cast<_bi*>(node);
	if (!bi) return max_priority;
	if (dynamic_cast<_pow*>(bi)) return 7;
	if (dynamic_cast<_mult*>(bi) || dynamic_cast<_div*>(bi)) return 6;
	if (dynamic_cast<_plus*>(bi) || dynamic_cast<_minus*>(bi)) return 5;
	if (dynamic_cast<_less*>(bi) || dynamic_cast<_more*>(bi) || dynamic_cast<_less_or_is*>(bi) || dynamic_cast<_more_or_is*>(bi)) return 4;
	if (dynamic_cast<_equally*>(bi) || dynamic_cast<_not_equally*>(bi)) return 3;
	if (dynamic_cast<_and*>(bi)) return 2;
	if (dynamic_cast<_or*>(bi)) return 1;
}

/*
//представляет собой интерпритатор
//функция задается так:
	//	[func_name] = (param_name_1, param_name_2, ... ,param_name_n) "some_actions"... ;
	//если параметр func_name задан - то функция запоминается.
	//фукция print(arg) выводит значение аргумента в стандартный поток и возвращает ее значение.
	//еслы вызвать execute - во входном потоке можно писать (выводит число (число может придти из функции)) и echo(просто пишет текст до точки с запятой)
	//	echo f3(2) = ; print(f3(2)); 
	//пример задания функций:
	//	f = (x, y, z) x + y*2 - z^2'z + 5;
	//	  = () f(1, 3, 7)*6 + 4;
	//	  = () cos(pi()) - = (x)(cos(x)'x'x)(pi())
	//	f1= (x) x^-1 == 1/x;
	//	f2= (x) pow(x,-1) == 1/x;
	//	f3= (x) if (f1(x) == f2(x)) print(25) else print(100);
	//	tan = (x) sin(x) / cos(x);
	//	square = (x1, x2, a) x1 <= abs(a) && x1 >= -abs(a) && x2 <= abs(a) && x2 >= -abs(a);
	//можно приделать комынды вывода данных.(пока не сделано - не хо бо пока не надо)
*/
class tree_func final : public Node{
	shared_ptr<u_p_N> ptr;
	struct inStream{
		static struct Attributes{
					static const int max_operator_len, max_word_len;
					static enum class symbol : unsigned char {
	tmp,/*любой cимвол, что не есть те, что дальше*/
	if_/*if*/,
	else_/*else*/,
	//print_/*print*/,
	//echo_/*echo*/,
	dot/*.*/,
	coma/*,*/,
	bracket/*(*/,
	close_bracket/*)*/,
	log_addition/*!*/,
	pow/*^*/,
	apostrophe/*'*/,
	mult/* * */,
	div/*/*/,
	plus/*+*/,
	minus/*-*/,
	less/*<*/,
	less_or/*<=*/,
	more/*>*/,
	more_or/*>=*/,
	equally/*==*/,
	not_equally/*!=*/,
	and/*&&*/,
	or/*||*/,
	set/*=*/,
	separator/*;*/
};
					//множество операторов
					set<symbol> operators;
					//множество унарных префиксных операторов
					set<symbol> uno_pre;
					//множество бинарных операторов
					set<symbol> bi;
					//важный символ (может разделитель, может еще что-то)
					set<char> important_symbols;
					//важный слова (if, else...)
					set<symbol> important_words;
					struct {
						map <string, symbol> field;
						symbol operator[](const string& key){
							if (!field.count(key)) return symbol::tmp;
							return field[key];
						}
					} symbol_translator;
					Attributes(){
	uno_pre.clear();
	uno_pre.insert(symbol::plus); uno_pre.insert(symbol::minus); uno_pre.insert(symbol::log_addition);
	important_symbols.clear();
	string s1 = string(";()=!<>|&+-^/* ,'\n\t") + (char)EOF;
	for (auto i = s1.begin(), end = s1.end(); i != end; i++) important_symbols.insert(*i);
	#pragma region symbol_translator
	symbol_translator.field.clear();
	bi.clear();
	//
	symbol_translator.field["("] = symbol::bracket/*(*/;
	symbol_translator.field[")"] = symbol::close_bracket/*)*/;
	symbol_translator.field["if"] = symbol::if_/*if*/;
	symbol_translator.field["else"] = symbol::else_/*else*/;
	//symbol_translator.field["print"] = symbol::print_;
	//symbol_translator.field["echo"] = symbol::echo_;
	symbol_translator.field[";"] = symbol::separator/*;*/;
	symbol_translator.field[","] = symbol::coma/*,*/;
	symbol_translator.field["."] = symbol::dot;
	symbol_translator.field["="] = symbol::set;	
	symbol_translator.field["!"] = symbol::log_addition;
	symbol_translator.field["'"] = symbol::apostrophe;
	bi.insert(symbol_translator.field["^"] = symbol::pow);
	bi.insert(symbol_translator.field["*"] = symbol::mult);
	bi.insert(symbol_translator.field["/"] = symbol::div);
	bi.insert(symbol_translator.field["+"] = symbol::plus);
	bi.insert(symbol_translator.field["-"] = symbol::minus);
	bi.insert(symbol_translator.field["<"] = symbol::less);
	bi.insert(symbol_translator.field["<="] = symbol::less_or);
	bi.insert(symbol_translator.field[">"] = symbol::more);
	bi.insert(symbol_translator.field[">="] = symbol::more_or);
	bi.insert(symbol_translator.field["=="] = symbol::equally);
	bi.insert(symbol_translator.field["!="] = symbol::not_equally);
	bi.insert(symbol_translator.field["&&"] = symbol::and);
	bi.insert(symbol_translator.field["||"] = symbol::or);
	#pragma endregion
	operators = bi;
	for (auto var : uno_pre) operators.insert(var);
	important_words.clear();
	important_words.insert(symbol::if_);
	important_words.insert(symbol::else_);
	//important_words.insert(symbol::echo_);
	//important_words.insert(symbol::print_);
					}
				} attributes;

		istream& in;
		stack<char> stack_of_char;
		void put(const char &ch){
			stack_of_char.push(ch);
		}
		void put(const string &s){
			for (auto i = s.rbegin(), rend = s.rend(); i != rend; i++){
				inStream::put(*i);
			}
		}
		char get(){
			char res;
			if (stack_of_char.size()){
				res = stack_of_char.top();
				stack_of_char.pop();
			} else res = in.get();
			return res;
		}
		string getline(){
			string res;
	bool checker = true;
	//
		auto i = stack_of_char._Get_container().begin(), end = stack_of_char._Get_container().end();
		char *buf = new char[stack_of_char.size() + 1], *tmp = buf;
		unsigned int count = 0;
		for (; i != end && checker;){
			if ((*tmp++ = *i++) == L'\n') { checker = false; tmp--; }
			count++;
		}
		for (unsigned int i = 0; i < count; i++) stack_of_char.pop();
		*tmp = L'\0';
		res = string(buf);
		delete[] buf;
	//
	if (checker){//небыло L'\n' в стеке
		string tmp;
		std::getline(in, tmp);
		res += tmp;
	}
	return res;
		}
		inStream(istream &in) : in(in){
			if (!in.good()) throw exception("bad stream");
		}
		void throw_syntax_error(const string& s) throw(string){
			throw "\nsyntax error:\n" + s;
		}
		enum class flag : char{name_err = -3, length_err = -1, context = 0, some_separator, operator_, important_word, number};
		//считывает символ
		//flag показывает, какой символ считался
		//flag = 0 - незарезервированый символ
		//flag = 1 - значемый символ не оператор
		//flag = 2 - оператор
		//flag = 3 - значемое слово
		//flag = 4 - литерал число
		//особая ситуация кидает текст, значение которого дополняестя значением флага
		//flag = -1 - читаемый символ слишком длинный
		//flag = -3 - имя не может начинаться с цифры
		string get_symbol(flag& flag) throw(string){
	static char buf[/*Attributes::max_word_len*/256 + 1];
	static int len;
	static bool ch;
	string res = "";
	while (true){//пропуск пробелов энтеров таб
		buf[0] = get();
		if (buf[0] == ' ' || buf[0] == '\n' || buf[0] == '\t'){} else break;
	}
	len = 0; ch = true;
	while (ch){
		if (len >= attributes.max_operator_len || !attributes.important_symbols.count(buf[len])){
			put(buf[len]);
			ch = false;
		} else buf[++len] = get();
	}
	//если первый символ был важным - зайдем в проверку, на наличие оператора в buf
	static int i;
	if (len){
		buf[len] = '\0';
		res = string(buf);
		if (len >= 2 && res[0] == '/' && (res[1] == '/' || res[1] == '*')){//комментарий (символы L'/' L'*' значемые, а max_operator_len >= 2)
			for (auto i = ++(++res.begin()), end = res.end(); i != end; i++) put(*i);
			if (res[1] == '/') getline();//читаем строку до начала новой строки
			else if (res[1] == '*'){ //читаем, пока не */
				i = 0;
				while (good()){
					buf[0] = get();
					if (buf[0] == '*') i = 1;
					else if (buf[0] == '/' && i == 1) break;
					else i = 0;
				}
			}
			return get_symbol(flag);
		}
		for (i = len - 1; i >= 0; i--){
			//пытаемся найти в res самый большой оператор
			if (attributes.operators.count(attributes.symbol_translator[res])){
				flag = flag::operator_;//оператор
				break;
			} else if (i != 0){
				put(buf[i]);
				res.erase(--res.end()); //убираем символ
			} else {/*остался один символ, он "важный" и он не оператор*/
				flag = flag::some_separator;//важный символ
				break;
			}
		}
	} else { //пришли сюда - значит считать надо не оператор и не важный символ, а какое-то слова
		//сейчас, все символы, что были считаны - возвращены в in
		while (!(attributes.important_symbols.count(buf[len++] = get()))){
			if (len > attributes.max_word_len){// is   if (len == max_word_len + 1)
				break;
			}
		}
		put(buf[--len]);//вернем последний считаный символ и в том случае если он был важным, и в том, когда считывается слишком длинный символ
		buf[len] = '\0';
		res = string(buf);
		if (len == attributes.max_word_len){//если (len == max_word_len) то 2 варианта : либо последний символ, который считали, оказался важным, либо привысили лимит длины символа (все как написано в цикле выше)
			while (!(attributes.important_symbols.count(buf[0] = get()))){
				res += buf[0]; // это происходит редко - поэтому можно переписывать всю соку заново каждый разпри добавлении символа
			}
			put(buf[0]);
			if(res.length() > attributes.max_word_len){
				flag = flag::length_err;
				throw ("слишком длинное слово \n" + res);
			}
		}
		if (iswdigit(res[0])){
			bool was_dot = false;
			for (auto i = ++res.begin(), end = res.end(); i != end; i++){
				if (*i == '.' && !was_dot) {was_dot = true; continue;}
				if (!iswdigit(*i)){
					flag = flag::name_err;
					throw "имя не может начинаться с цифры \n" + res;
				}
			}
			flag = flag::number;
		} else {
			if (attributes.important_words.count(attributes.symbol_translator[res])) flag = flag::important_word;
			else flag = flag::context;
		}
	}
	return res;
		}
		bool good() throw(...){
			return in.good();
		}
	};
	unsigned int n_args;
	vector< u_p_N > get_args(inStream &in, const map<string, unsigned int>& params) throw (string){
		string symbol;
		inStream::flag symbol_flag;
		if((symbol = in.get_symbol(symbol_flag)) != "(") throw "ожидается '(' на месте\n' " + symbol + "'";
		vector< u_p_N > arguments;
		if ((symbol = in.get_symbol(symbol_flag)) != ")"){
			in.put(L' ');
			in.put(symbol);
			while (true){
				arguments.push_back(get_Node(in, params));
				if ((symbol = in.get_symbol(symbol_flag)) == ")") break;
				if (symbol != ",") throw "ожидается запятая на месте\n' " + symbol + "'";
			}
		}
		//if (arguments.size() > max_params_n) throw L"процедура не может иметь " + std::to_wstring(arguments.size()) + L" параметров\n" + L"процедура может иметь не больше чем " + std::to_wstring(max_params_n) + L" параметров";
		return arguments;
	}
	map<string, unsigned int> get_params(inStream &in) throw (string){
		size_t numder_of_parameter = 0;
		string symbol;
		inStream::flag symbol_flag;
		if((symbol = in.get_symbol(symbol_flag)) != "(") throw "ожидается '(' на месте\n' " + symbol + "'";
		map<string, unsigned int> parameters;
		if ((symbol = in.get_symbol(symbol_flag)) != ")"){
			while (true){
				if (symbol_flag != inStream::flag::context) throw "ожидается имя параметра на месте\n' " + symbol + "'";
				if (parameters.count(symbol)) throw "имя параметра \n' " + symbol + "'\n уже есть среди имен параметров";
				parameters[symbol] = numder_of_parameter++;
				if ((symbol = in.get_symbol(symbol_flag)) == ")") break;
				if (symbol != ",") throw "ожидается запятая на месте\n' " + symbol + "'";
				symbol = in.get_symbol(symbol_flag);
			}
		}
		return parameters;
	}
	u_p_N get_Node(inStream &in, const map<string, unsigned int>& params) throw (string){
		u_p_N left;
		inStream::flag symbol_flag;
		auto symbol = in.get_symbol(symbol_flag);
		auto identifier = in.attributes.symbol_translator[symbol];
		#pragma region if else
		if (identifier == inStream::Attributes::symbol::if_){
				if ((symbol = in.get_symbol(symbol_flag)) != "(") in.throw_syntax_error("после 'if' ожидается выражение в скобоках '(' ')'");
				auto first = get_Node(in, params);
				if ((symbol = in.get_symbol(symbol_flag)) != ")") in.throw_syntax_error("после 'if' ожидается выражение в скобоках '(' ')'");
				auto second = get_Node(in, params);
				if ((symbol = in.get_symbol(symbol_flag)) != "else") in.throw_syntax_error("после 'if' должен быть 'else'");
				auto third = get_Node(in, params);
				left.reset(new _if_else(std::move(first), std::move(second), std::move(third)));
		} else 
		#pragma endregion
		#pragma region опледеление и вызов анонимной функции (без имени)
		if (symbol == "="){
			in.put("= ");
			left.reset(new _some_func(u_p_N(new tree_func(in)), get_args(in, params)));
		}else
		#pragma endregion
		if (symbol_flag == inStream::flag::operator_ && in.attributes.uno_pre.count(identifier)){
			#pragma region унарный оператор
				switch(identifier){
				case inStream::Attributes::symbol::plus:			left.reset(new _uno_plus(get_value(0)));		break;
				case inStream::Attributes::symbol::minus:			left.reset(new _uno_minus(get_value(0)));		break;
				case inStream::Attributes::symbol::not_equally:		left.reset(new _not(get_value(0)));				break;
				}
				u_p_N operand = get_Node(in, params);
				if(get_priority(operand.get()) < get_priority(left.get())){
					Node* ptr = operand.get();
					if (dynamic_cast<_bi*>(operand.get())){
						dynamic_cast<_uno*>(left.get())->ptr = std::move(dynamic_cast<_bi*>(operand.get())->l);
						dynamic_cast<_bi*>(operand.get())->l = std::move(left);
					} else {
						dynamic_cast<_uno*>(left.get())->ptr = std::move(dynamic_cast<_uno*>(operand.get())->ptr);
						dynamic_cast<_uno*>(operand.get())->ptr = std::move(left);
					}
					left = std::move(operand);
					u_p_N tmp;
					while((dynamic_cast<_bi*>(ptr) && get_priority(dynamic_cast<_uno*>(dynamic_cast<_bi*>(ptr)->l.get())->ptr.get()) < get_priority(dynamic_cast<_bi*>(ptr)->l.get())) ||
						  (dynamic_cast<_uno*>(ptr) && get_priority(dynamic_cast<_uno*>(dynamic_cast<_uno*>(ptr)->ptr.get())->ptr.get()) < get_priority(dynamic_cast<_uno*>(ptr)->ptr.get())) ){
						if (auto ptr_ = dynamic_cast<_bi*>(ptr)){
							tmp = std::move(ptr_->l);
							ptr_->l = std::move(dynamic_cast<_uno*>(tmp.get())->ptr);
							if (auto l_ = dynamic_cast<_bi*>(ptr_->l.get())){
								dynamic_cast<_uno*>(tmp.get())->ptr = std::move(l_->l);
								l_->l = std::move(tmp);
								ptr = l_;
							} else {
								auto ptr__ = dynamic_cast<_uno*>(ptr_->l.get());
								dynamic_cast<_uno*>(tmp.get())->ptr = std::move(ptr__->ptr);
								ptr__->ptr = std::move(tmp);
								ptr = ptr__;
							}
						} else {
							auto ptr__ = dynamic_cast<_uno*>(ptr);
							tmp = std::move(ptr__->ptr);
							ptr__->ptr = std::move(dynamic_cast<_uno*>(tmp.get())->ptr);
							//if (auto l_ = dynamic_cast<_bi*>(ptr__->ptr.get())){ //этот вариант закоментирован, потому что если ptr это _uno*, то l_ не может быть _bi*
							//	dynamic_cast<_uno*>(tmp.get())->ptr = std::move(l_->l);
							//	l_->l = std::move(tmp);
							//	ptr = l_;
							//} else {
								auto ptr___ = dynamic_cast<_uno*>(ptr__->ptr.get());
								dynamic_cast<_uno*>(tmp.get())->ptr = std::move(ptr___->ptr);
								ptr___->ptr = std::move(tmp);
								ptr = ptr___;
							//}
						}
					}
				} else ((_uno*)left.get())->ptr = std::move(operand);
			#pragma endregion
			return left;
		} else if (symbol_flag == inStream::flag::number || symbol_flag == inStream::flag::context){
			#pragma region операнды
				#pragma region литерал число
				if (symbol_flag == inStream::flag::number) left.reset(new _value(atof(symbol.c_str())));
				else
				#pragma endregion
				if (symbol_flag == inStream::flag::context){
					auto name = symbol;
					in.put((symbol = in.get_symbol(symbol_flag)) + " ");
					#pragma region вызов хранимой функции 
					if (symbol == "(" || symbol == "="){
						if (symbol == "=") {
							in.put(' ');
							in.put(name);
							auto tmp = tree_func(in);
						}
						left.reset(new _named_func(name, get_args(in, params)));
					} else
					#pragma endregion
					#pragma region имя параметра
					{
						if (!params.count(name)) in.throw_syntax_error("there is no parameter " + name + " in parameter list.");
						left.reset(new _x_n(params.at(name)));
					}
					#pragma endregion
				}
			#pragma endregion
		} else
		if (symbol_flag == inStream::flag::some_separator && identifier == inStream::Attributes::symbol::bracket){
			#pragma region скобочка (
				left.reset(new _bracket(get_Node(in, params)));
				if ((symbol = in.get_symbol(symbol_flag)) != ")"){
					in.put(L' ');
					in.put(symbol);//ложим назад - не относиться к дереву операций
					throw exception("скобок '(' больше, чем ')'");
				}
			#pragma endregion
		} else {
			in.put(L' ');
			in.put(symbol);//ложим назад - не относиться к дереву операций
			throw ("ожидается операнд на месте\n' " + symbol + "'");
		}

		//сюда придем, только если выполнятся второй, третий или четвертый if
		symbol = in.get_symbol(symbol_flag);
		while (symbol == "'"){
			symbol = in.get_symbol(symbol_flag);
			if (!params.count(symbol)) in.throw_syntax_error(symbol + " have to be parameter name");
			left = left->d_xn(params.at(symbol));
			symbol = in.get_symbol(symbol_flag);
		}
		identifier = in.attributes.symbol_translator[symbol];
		#pragma region формирование узла
		//try{
		if (symbol_flag == inStream::flag::operator_ && in.attributes.bi.count(identifier)){
				u_p_N central, right = get_Node(in, params);
				switch (identifier){
				case inStream::Attributes::symbol::pow:				central.reset(new _pow(std::move(left), std::move(right)));			break;
				case inStream::Attributes::symbol::mult:			central.reset(new _mult(std::move(left), std::move(right)));		break;
				case inStream::Attributes::symbol::div:				central.reset(new _div(std::move(left), std::move(right)));			break;
				case inStream::Attributes::symbol::plus:			central.reset(new _plus(std::move(left), std::move(right)));		break;
				case inStream::Attributes::symbol::minus:			central.reset(new _minus(std::move(left), std::move(right)));		break;
				case inStream::Attributes::symbol::less:			central.reset(new _less(std::move(left), std::move(right)));		break;
				case inStream::Attributes::symbol::less_or:			central.reset(new _less_or_is(std::move(left), std::move(right)));	break;
				case inStream::Attributes::symbol::more:			central.reset(new _more(std::move(left), std::move(right)));		break;
				case inStream::Attributes::symbol::more_or:			central.reset(new _more_or_is(std::move(left), std::move(right)));	break;
				case inStream::Attributes::symbol::equally:			central.reset(new _equally(std::move(left), std::move(right)));		break;
				case inStream::Attributes::symbol::not_equally:		central.reset(new _not_equally(std::move(left), std::move(right)));	break;
				case inStream::Attributes::symbol::and:				central.reset(new _and(std::move(left), std::move(right)));			break;
				case inStream::Attributes::symbol::or:				central.reset(new _or(std::move(left), std::move(right)));			break;
				}

				if(get_priority(dynamic_cast<_bi*>(central.get())->r.get()) < get_priority(central.get())){
					auto hand = std::move(dynamic_cast<_bi*>(central.get())->r);
					dynamic_cast<_bi*>(central.get())->r = std::move(dynamic_cast<_bi*>(hand.get())->l);
					dynamic_cast<_bi*>(hand.get())->l = std::move(central);
					central = std::move(hand);
					u_p_N tmp;
					auto ptr = dynamic_cast<_bi*>(central.get());
					while(get_priority(dynamic_cast<_bi*>(dynamic_cast<_bi*>(ptr)->l.get())->r.get()) < get_priority(dynamic_cast<_bi*>(ptr)->l.get())){
						tmp = std::move(ptr->l);
						ptr->l = std::move(dynamic_cast<_bi*>(tmp.get())->r);
						ptr = dynamic_cast<_bi*>(ptr->l.get());
						dynamic_cast<_bi*>(tmp.get())->r = std::move(ptr->l);
						ptr->l = std::move(tmp);
					}
				}
				return central;
			} else {
				in.put(L' ');
				in.put(symbol);//ложим назад - не относиться к дереву операций
				return left;
				//if (bracket_balance) throw(L"ожидается бинарный оператор или ')' на месте\n' " + symbol + L"'");
				//else throw(L"ожидается бинарный оператор или ';' на месте\n' " + symbol + L"'");
			}
		//}catch(wstring ex){
		//	throw;
		//}
		#pragma endregion
	//} catch(...) {
	//	throw;
	//}
	}
	struct _named_func : public _func{
		string name;
		size_t n;
		_named_func (string name, vector< u_p_N > args) : _func(std::move(args)), name(name), n(this->args.size()) {}
		Node* func() const throw (exception){
			if (!tree_func::stored_funcs.count(name) || !tree_func::stored_funcs[name].count(n)) throw exception(("there is no stored_func " + name + " witn " + to_string(n) + " args").c_str());
			return tree_func::stored_funcs[name][n].get();
		}
		virtual u_p_N get_partial_d(const vector< u_p_N >& args, unsigned int n = 0) const {
			vector< u_p_N > args_copy(args.size());
			for (size_t i = 0; i < args.size(); i++) args_copy[i] = args[i]->min();
			return u_p_N(new _some_func(func()->d_xn(n), std::move(args_copy)));
		}
		virtual T get_value_in(const vector<T>& v) const { return func()->f(v); }
		virtual _func* get_func(vector< u_p_N > args) const{ return new _named_func(name, std::move(args)); }
	};
	void init(inStream& in) throw (string){
		inStream::flag flag;
		string symbol = in.get_symbol(flag), name;
		if (flag == inStream::flag::context){
			name = symbol;
			symbol = in.get_symbol(flag);
		}
		if(symbol != "=") in.throw_syntax_error("it have to be '=' sumbol");
		auto params = get_params(in);
		n_args = params.size();
		u_p_N res;
		if (name != ""){
			if (tree_func::stored_funcs.count(name) && tree_func::stored_funcs[name].count(n_args)) in.throw_syntax_error("there is one stored_func " + name + " witn " + to_string(n_args) + " args");
			tree_func::stored_funcs[name][params.size()] = get_Node(in, params);
			tree_func::stored_funcs[name][params.size()] = tree_func::stored_funcs[name][params.size()]->min();
			res = tree_func::stored_funcs[name][params.size()]->min();
		} else res = get_Node(in, params);
		ptr.reset(new u_p_N(res->min()));
	}
	tree_func(inStream& in) throw (string){
		init(in);
	}
public:
	static map< string, map<unsigned int, u_p_N > > stored_funcs;

	tree_func(istream& in_) throw (string){
		inStream in(in_);
		init(in);
		inStream::flag flag;
		if (in.get_symbol(flag) != ";") in.throw_syntax_error("there is no function separator ';'");
	}
	tree_func(const string& s) throw (string){
		auto in_ = strstream(const_cast<char*>(s.c_str()), s.length());
		inStream in(in_);
		init(in);
		inStream::flag flag;
		if (in.get_symbol(flag) != ";") in.throw_syntax_error("there is no function separator ';'");
	}
	/*static void execute(istream& in){
		while (in.good()){
			auto tmp = 
		}
	}
	static void execute(string s){
		execute(strstream(const_cast<char*>(s.c_str()), s.length()));
	}*/
	unsigned int get_n_args(){ return n_args; }
	virtual T f(const vector<T>& v) const{
		if(v.size() != n_args) throw exception("wrong vector_size");
		return (*ptr)->f(v);
	}
	virtual u_p_N d_xn(unsigned int n = 0)  const override {
		return (*ptr)->d_xn(n);
	};
	virtual u_p_N min()  const override{
		return (*ptr)->min();
	};
};
tree_func::inStream::Attributes tree_func::inStream::attributes;
const int tree_func::inStream::Attributes::max_operator_len = 2;
const int tree_func::inStream::Attributes::max_word_len = 256;
map< string, map<unsigned int, u_p_N > > tree_func::stored_funcs = []()->map< string, map<unsigned int, u_p_N > > {
	map< string, map<unsigned int, u_p_N > > res;
	res["pi"][0] = u_p_N(new _value(3.14159265358979323846));
	res["e"][0] = u_p_N(new _value(2.71828182845904523536));
	res["log"][1] = u_p_N(new _log( u_p_N( new _x_n(0)) ));
	res["exp"][1] = u_p_N(new _exp( u_p_N( new _x_n(0)) ));
	res["sin"][1] = u_p_N(new _sin( u_p_N( new _x_n(0)) ));
	res["cos"][1] = u_p_N(new _cos( u_p_N( new _x_n(0)) ));
	res["exp"][1] = u_p_N(new _exp( u_p_N( new _x_n(0)) ));
	res["abs"][1] = u_p_N(new _abs( u_p_N( new _x_n(0)) ));
	res["mod"][1] = u_p_N(new _mod_1( u_p_N( new _x_n(0)) ));
	res["trunc"][1] = u_p_N(new _trunc( u_p_N( new _x_n(0)) ));
	//res["print"][1] = u_p_N(new _print(u_p_N( new _x_n(0))));
	res["pow"][2] = u_p_N(new _pow( u_p_N( new _x_n(0)), u_p_N( new _x_n(1)) ));
	res["mod"][2] = u_p_N(new _mod_2( u_p_N( new _x_n(0)), u_p_N( new _x_n(1)) ));
	return res;
}();

}