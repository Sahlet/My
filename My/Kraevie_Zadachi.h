//Kraevie_Zadachi.h
#include<My\Zadach_Koshi.h>
#include<My\SLAU.h>

//typedef long double T;
namespace My {
	using namespace std;

#pragma region определение

template<class T>
vector<T> My::operator+(const vector<T>& l, const vector<T>& r) throw(exception){
	if (l.size() != r.size()) throw(runtime_error("bad size of vector"));
	vector<T> res(l.size());
	auto k = res.begin(), end_k = res.end();
	for (auto i = l.begin(), j = r.begin(); k != end_k; (*k++ = *i++ + *j++));
	return res;
}
template<class T>
vector<T> My::operator-(const vector<T>& v) throw(exception){
	vector<T> res(v.size());
	auto k = res.begin(), end_k = res.end();
	for (auto i = v.begin(); k != end_k; (*k++ = - *i++));
	return res;
}
template<class T>
vector<T> My::operator-(const vector<T>& l, const vector<T>& r) throw(exception){
	return l + (-r);
}
//скалярное произведение
template<class T>
T My::operator,(const vector<T>& l, const vector<T>& r) throw(exception){
	if (l.size() != r.size()) throw(runtime_error("bad size of vectors"));
	T res = T();
	for (auto i1 = l.begin(), i2 = r.begin(), end1 = l.end(); i1 != end1; i1++, i2++) res += *i1 * *i2;
	return res;
}
template<class T>
string My::to_string(const vector<T>& v){
	return My::to_string(matrix<T>(v).get_transplanted());
}

matrix<T>::abs_ matrix<T>::abs_ptr = matrix<T>::std_abs;

#pragma endregion

	class Kraevaya_Zadacha_Conditions{
		/*
		y”(x) + p(x)*y'(x) + q(x)*y(x) = f(x),      xє[a, b]                          (1)
		p, q, f - неприрывны на xє[a, b]

		alpha0 y(a) + alpha1 y'(a) = A;     beta0 y(b) + beta1 y'(b) = B.             (2)
		|alpha0| + |alpha1| != 0, |beta0| + |beta1| != 0.
		*/
	public:
		const pair<T, T> range;
		const function<T (const T&)> p, q, f;
		
		const T alpha0, alpha1, A;
		const T beta0, beta1, B;
		const vector<T> starn_counts;
		Kraevaya_Zadacha_Conditions(const pair<T, T>& range,
			const function<T (const T&)> p, const function<T (const T&)> q, const function<T (const T&)> f,
			const T& alpha0, const T& alpha1, const T& A,
			const T& beta0, const T& beta1, const T& B) :
					range(min(range.first, range.second), max(range.first, range.second)),
					p(p), q(q), f(f),
					alpha0(alpha0), alpha1(alpha1), A(A),
					beta0(beta0), beta1(beta1), B(B)
		{
			if (range.first == range.second) throw exception("empty range");
			if (alpha0 == 0 && alpha1 == 0) throw exception("invalid parameter");
			if (beta0 == 0 && beta1 == 0) throw exception("invalid parameter");
		}
	};


	pair<Koshi_Conditions_for_system, Koshi_Conditions_for_system>//u and v (alpha0*u(range.first) + alpha1*u'(range.first) = 0)
	reduction_to_two_Koshi_problems(const Kraevaya_Zadacha_Conditions& k){
		auto p = k.p, q = k.q, f = k.f;
		vector< function<T (const T&, const vector< T >& counts)> > funcs(2);
		vector<T> start_counts(2);
		//////////////////////////////////////////////////////////
		/*
			u'(x) = z(x);
			z'(x) = -p(x)*z(x) - q(x)*u(x)
		*/
		funcs[0] = [](const T& x, const vector< T >& counts) -> T{
			return counts[1];
		};
		funcs[1] = [p, q](const T& x, const vector< T >& counts) -> T{
			return -p(x)*counts[1] -q(x)*counts[0];
		};
		start_counts[0] = k.alpha1;
		start_counts[1] = -k.alpha0;
		Koshi_Conditions_for_system for_u(k.range, funcs, k.range.first, start_counts);
		//////////////////////////////////////////////////////////
		/*
			v'(x) = z(x);
			z'(x) = -p(x)*z(x) - q(x)*v(x) + f(x)
		*/
		funcs[1] = [p, q, f](const T& x, const vector< T >& counts) -> T{
			return -p(x)*counts[1] -q(x)*counts[0] + f(x);
		};
		if (k.alpha1 != 0) {
			start_counts[0] = 1;
			start_counts[1] = k.A / k.alpha1 - k.alpha0;
		} else {
			start_counts[0] = k.A / k.alpha0 - k.alpha1;
			start_counts[1] = 1;
		}
		Koshi_Conditions_for_system for_v(k.range, funcs, k.range.first, start_counts);
		return pair<Koshi_Conditions_for_system, Koshi_Conditions_for_system>(for_u, for_v);
	}

	vector< pair<T, T> > reduction_and_Runge_method(const Kraevaya_Zadacha_Conditions& k, const unsigned int& n = 10){
		auto two_Koshi_problems = reduction_to_two_Koshi_problems(k);
		auto res1 = Runge_Kutta_method(two_Koshi_problems.first, n), res2 = Runge_Kutta_method(two_Koshi_problems.second, n);
		T C;
		C = k.beta0 * res1[0][res1.size() - 1].second + k.beta1 * res1[1][res1.size() - 1].second;
		if (C == 0) throw exception("invalid parameter");
		C = (k.B - (k.beta0 * res2[0][res1.size() - 1].second + k.beta1 * res2[1][res1.size() - 1].second)) / C;
		if (C == 0) throw exception("invalid parameter");

		vector< pair<T, T> > res(res1[0].size());
		for (int i = 0; i < res.size(); i++){
			res[i].first = res1[0][i].first;
			res[i].second = C * res1[0][i].second + res2[0][i].second;
		}
		return res;
	}

	struct C2_func{
		function<T (const T&)> f, dfdx, d2fdx2;
	};
	function<T (const T&)> kollokaci_method(const Kraevaya_Zadacha_Conditions& k, const vector< C2_func >& C2_coordinate_functions){
		T A_ = k.A, B_ = k.B, a = k.range.first, b = k.range.second;
		unsigned int n = C2_coordinate_functions.size();
		#define L(_C2_func, x) (_C2_func.d2fdx2(x) + k.p(x)*_C2_func.dfdx(x) + k.q(x)*_C2_func.f(x))
		
		C2_func u0_funcs = {
			[A_, B_, a, b](const T& x) -> T{return A_ + (B_ - A_)*(x - a)/(b - a);},
			[A_, B_, a, b](const T& x) -> T{return (B_ - A_)/(b - a);},
			[](const T& x) -> T{return 0;}
		};
		/*
		C2_func u0_funcs = {
			[A_, B_, a, b](const T& x) -> T{return A_ + (B_ - A_)*pow((x - a), 2)/pow((b - a), 2);},
			[A_, B_, a, b](const T& x) -> T{return 2*(B_ - A_)*pow((x - a), 1)/pow((b - a), 2);},
			[A_, B_, a, b](const T& x) -> T{return 2*(B_ - A_)/pow((b - a), 2);}
		};
		*/
			
		vector<T> coefficients;
		
		if (n){
			square_matrix<T> m(n);
			vector<T> free(n);
			T node;
			for (int i = 0; i < n; i++){
				node = (a + b) / 2 + ((b - a) / 2)*cos(3.145 * (2*i - 1) /(T) (2*n));
				free[i] = k.f(node) - L(u0_funcs, node);
				for (int j = 0; j < n; j++) m[i][j] = L(C2_coordinate_functions[j], node);
			}
			//auto det = *m;
			coefficients = /*SLAU_Gaus*/SLAU_sqrt(m, free);
		}

		auto u0 = u0_funcs.f;
		vector< function<T (const T&)> > coordinate_functions(n);
		for (int i = 0; i < n; i++) coordinate_functions[i] = C2_coordinate_functions[i].f;
		return [u0, coordinate_functions, coefficients](const T& x) -> T{
			T res = u0(x);
			unsigned int n = coordinate_functions.size();
			for (int i = 0; i < n; i++) res += coefficients[i]*coordinate_functions[i](x);
			return res;
		};
	}

}