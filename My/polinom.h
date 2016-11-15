//polinom.h
#pragma once
#include<vector>
#include<list>
#include<My\string.h>
#include<memory>
#pragma warning ( disable : 4996)

namespace My{

using namespace std;

template<class T = long double>
class Sturm;



template<class T = long double>
//typedef long double T; //не шаблон, потому что в конструкторе надо осуществлять проверку (смотри сам), а для осуществления проверки надо, чтоб тип T был double
class polinom{
	shared_ptr< vector<T> > p;
	shared_ptr< polinom > _d_dx;
	shared_ptr< list< polinom > > for_Sturm;
	friend class Sturm<T>;
//не удалять _MIN_VAL - это решает проблему с погрешностью в вычислениях (без этого тупит деление, умножение, сложение и вычитание многочленов)
//директива препроцессора чтоб не объявлять статический член класса и чтоб не изменять значение везде, где стоит _MIN_VAL, если надо изменить...
static const T eps;
public:
#pragma region main interface
	polinom(const vector<T> &p){
		static auto min_val = abs(eps);
		auto i = p.size(), n = i;
		for (; i != 0 && abs(p[--i]) <= min_val;) {n--;}
		if (n != 0){
			this->p.reset(new vector<T>(n));
			for (auto j = 0u; j < n; j++) (*this->p)[j] = p[j];
		}
	}
	polinom(vector<T>&& p) _NOEXCEPT{
		if (p.size()){
			static auto min_val = abs(eps);
			if (abs(p[p.size() - 1]) <= min_val) *this = polinom(static_cast< vector<T>& >(p));
			else this->p.reset(new vector<T>(std::move_if_noexcept(p)));
		}
	}
	polinom(const T& t) _NOEXCEPT{
		vector<T> v(1);
		v[0] = t;
		*this = polinom(std::move(v));
	}
	inline polinom(polinom&& P) _NOEXCEPT : p(std::move(P.p)){}
	inline polinom(){}
	inline operator bool()const{ return p; }
	//значение многочлена в точке (операций 2n)
	T operator()(const T& x) const {
		if (!p) return 0;
		T res = 0;
		for (auto i = p->rbegin(), end = p->rend(); i != end; i++) res = res * x + *i;
		return res;
	}
	inline const T& operator[](const unsigned int& i) const throw (exception){
		if(!p || p->size() <= i) return 0;
		return (*p)[i];
	}
	polinom operator-() const {
	if (!p) return polinom();
	vector<T> p_(*p);
	for (unsigned int n = p->size(), i = 0; i < n; i++) p_[i] = -(*p)[i];
	return polinom(std::move(p_));
}
	polinom operator+(const polinom& P) const {
	if(!P.p) return *this;
	if(!p) return P;
	auto p1 = this->p, p2 = P.p;
	auto n1 = this->p->size(), n2 = P.p->size();
	if (n1 < n2){
		swap(n1, n2);
		swap(p1, p2);
	}
	vector<T> res(n1);
	for (auto i = 0u; i < n2; i++)
		res[i] = (*p1)[i] + (*p2)[i];
	for (auto i = n2; i < n1; i++)
		res[i] = (*p1)[i];
	return polinom(std::move(res));
}
	inline polinom& operator+=(const polinom& P){
		return (*this = *this + P);
	}
	inline polinom operator-(const polinom& P) const {
		return (*this) + (-P);
	}
	polinom operator*(const polinom& P) const {
	if(!this->p || !P.p) return polinom();
	auto n1(this->p->size()), n2(P.p->size()), n = n1 + n2 - 1;
	vector<T> res(n);
	for (auto i = 0u, j = 0u; i < n1; i++)
		for (j = 0; j < n2; j++)
			res[i + j] += (*this->p)[i] * (*P.p)[j];
	return polinom(std::move(res));
}
	inline polinom& operator*=(const polinom& P){
		return (*this = *this * P);
	}
//	polinom operator*(const T& t) const {
//	if(!this->p || t == 0) return polinom();
//	auto n(this->p->size());
//	vector<T> res(n);
//	for (auto i = 0u; i < n; i++) res[i] = (*this->p)[i] * t;
//	return polinom(std::move(res));
//}
	//деление
	polinom operator/(const polinom& P) const {
	if(!this->p || !P.p) return polinom();//пусть деление на 0 будет 0, тогда определения остатка от деления работает...
	unsigned int n2 = P.p->size(), n1;
	polinom copy(*this), res, tmp_p;
	vector<T> tmp;
	while (copy && ((n1 = copy.p->size()) >= n2)){
		tmp = vector<T>(n1 - n2 + 1);
		tmp[n1 - n2] = copy[n1 - 1] / P[n2 - 1];
		tmp_p = polinom(std::move(tmp));
		copy = copy - P * tmp_p;
		res = res + tmp_p;
	}
	return res;
}
	//остаток от деления
	inline polinom operator%(const polinom& P) const {
		return *this - (*this / P) * P;
	}
	//степень
	inline unsigned int degree() const{
		return (p ? p->size() - 1 : 0);
	}
	inline bool operator==(const polinom& P){
		return p->operator==(*P.p);
	}
	bool operator!=(const polinom& P){
		return p->operator!=(*P.p);
	}
	//взять производную
	polinom d_dx() const {
		if (!_d_dx){
			if (!degree()) return polinom();
			auto n = this->p->size();
			vector<T>p_(n - 1);
			for (auto i = 1u; i < n; i++)
			p_[i - 1] = i * (*this->p)[i];
			const_cast<polinom*>(this)->_d_dx.reset(new polinom(std::move(p_)));
		}
		return *_d_dx;
	}
	//берет интеграл полинома по T аналитическими методами и добавляет к нему константу "С"
	polinom integr(const T& C = 0) const{
		auto d = this->degree() + 1;
		auto vec = vector<T>(d + 1);
		for (auto i = 1u; i <= d; i++) vec[i] = (*p)[i - 1] / i;
		vec[0] = C;
		return polinom(std::move(vec));
	}
	virtual string To_String(const string& firstly = "P(x) = ") const{
		unsigned int n = this->degree() + 1;
		string res = firstly;
		for (auto i = 0u; i < n - 1; i++) res += My::to_string/*<T>*/((*this)[i]) + "(x^" + My::to_string(i) + ") + ";
		res += My::to_string/*<T>*/((*this)[n - 1]) + "(x^" + My::to_string(n - 1) + ")";
		return res;
	}

	//__________________________________________________________________________
#pragma endregion

};

//typedef long double T;
template<class T = long double>
class Sturm{
	//считает количество измененных знаков (как в теореме Штурма)
	static unsigned int N(const T& x, const list< polinom<T> > &l){
		auto res = 0u;
		T tmp1, tmp2;
		auto i = l.begin(), end_ = l.end();
		tmp1 = (*(i++))(x);
		for (; i != end_; i++){
			if ((tmp1 * (tmp2 = (*i)(x))) < 0) res++;
			tmp1 = tmp2;
		}
		return res;
	}
	//на основе теоремы Штурма находит функции Штурма для многочлена Р.
	//возвращает список с функциями Штурм упорядоченых так : f0, f1, f2 ... где f0 есьт входной многочлен
	static list< polinom<T> > get_Sturm(const polinom<T>& P){
		list< polinom<T> > l;//список многочленов Штурма
		if (!P) return l;
		auto r(P), r_(P.d_dx());
		l.push_back(r);
		while (r_){
			l.push_back(r_/*(1)*/);//добавляем многочлен Штурма. (1) - это метка (смотри несколько строк вниз)
			r_ = -(r % r_);//берем остаток от деления
			r = *(l.rbegin());//теперь r = (1)
		}
		return l;
	}

public:
	//находит интервалы из [a; b] с корнями многочлена Р (в каждом интервале 1 корень, причем корень не на границе промежутка (если корень есть какая - то граница промежутка [a; b], то все равно находит промежуток с 1 корнем, где корень остается одной из границ [a; b])).
	//length - указывает максимальную длину промежутков с одним корнем
	static list< pair<T, T> > get_intervals(const polinom<T>& P, T a, T b, const T& length){
		if (!P.for_Sturm){
			auto P_ = (polinom<T>*)&P;
			P_->for_Sturm.reset(new list< polinom<T> >(get_Sturm(P)));//список многочленов Штурма
		}
		typedef pair<T, T> pa;
		list< pa > res;
		if (!P || length == 0) return res;
		if (a == b || length == 0) {
			if (P(a) == 0) res.push_back(pa(a, a));
			if (a != b && P(b) == 0) res.push_back(pa(b, b));
			return res;
		}
		list< polinom<T> > &l = *P.for_Sturm;
		if (a > b){
			a = (a + b) / 2;
			b -= 2 * (b - a);
			a += a - b;
		}
		T avg;
		//два блока ифов - обработкa вариантов, когда на концах отрезка [a; b] есть корень (эта ситуация не подлежит условию теоремы Штурма)
		if (P(a) == 0){
			T tmp1 = a - 0.0001, tmp2 = a + (b - a) * 0.0001;//(b - a) * 0.0001 - чтоб не выйти из [a, b] справа
			do {
				while (P(tmp1 = (tmp1 + a) / 2) == 0){}
				while (P(tmp2 = (tmp2 + a) / 2) == 0){}
			} while (std::abs((N(tmp1, l) - (long long)N(tmp2, l))) != 1);
			res.push_back(pa(a, tmp2));
			a = tmp2;
		}
		bool for_last = false;
		T for_last_b, for_last_tmp;//прост чтоб промежутки в списке шли по возрастанию згачений границ
		if (P(b) == 0){
			for_last = true;
			T tmp1 = b - (b - a) * 0.0001, tmp2 = b + 0.0001;
			do {
				while (P(tmp1 = (tmp1 + b) / 2) == 0){}
				while (P(tmp2 = (tmp2 + b) / 2) == 0){}
			} while (std::abs((N(tmp1, l) - (long long)N(tmp2, l))) != 1);
			for_last_b = b;
			for_last_tmp = tmp1;
			b = tmp1;
		}
		T b1 = b;
		//
		long long tmp1;
		while (N(b, l) - N(a, l)){//пока на [a; b] есть корни
			while (abs(N(b, l) - (tmp1 = N(a, l))) != 1){
				for (avg = (a + b) / 2; P(avg) == 0; avg = (a + avg) / 2);//страховка от ноля в avg
				if (abs(N(avg, l) - tmp1) >= 1) b = avg;
				else a = avg;
			}
			while (abs(b - a) > length){
				for (avg = (a + b) / 2; P(avg) == 0; avg = (a + avg) / 2);//страховка от ноля в avg
				if ((P(a) * P(avg)) < 0)b = avg;
				else a = avg;
			}
			res.push_back(pa(a, b));
			a = b;
			b = b1;
		}
		if (for_last) res.push_back(pa(for_last_tmp, for_last_b));
		return res;
	}
};

}