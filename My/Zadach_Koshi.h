//Zadach_Koshi.h
#pragma once
#include <functional>
#include <My\polinom.h>
typedef long double T;

namespace My{
	using namespace std;

	const T polinom<T>::eps = 1e-10;

	class Koshi_Conditions{
	public:
		const pair<T, T> range;
		const function<T (const T&, const T&)> func;
		const pair<T, T> point;
		Koshi_Conditions(const function<T (T, T)>& func, const pair<T, T>& point, const pair<T, T>& range) : func(func), point(point), range(min(range.first, range.second), max(range.first, range.second)){
			if (range.first == range.second) throw exception("empty range");
			if (point.first < range.first || point.first > range.second) throw exception("point out of range");
		}
	};

	class Koshi_Conditions_for_system{
		/*
		//funcs
		y' = f1(x, y, z);
		z' = f2(x, y, z);

		//start_counts
		y(x0) = y0;
		z(x0) = z0;
		*/
	public:
		const pair<T, T> range;
		const vector< function<T (const T& x, const vector< T >& counts)> > funcs;
		
		const T x0;
		const vector<T> start_counts;
		Koshi_Conditions_for_system(const pair<T, T>& range, const vector< function<T (const T&, const vector< T >& counts)> > funcs, const T& x0, const vector< T > start_counts) : range(min(range.first, range.second), max(range.first, range.second)), funcs(funcs), x0(x0), start_counts(start_counts){
			if (range.first == range.second) throw exception("empty range");
			if (x0 < range.first || x0 > range.second) throw exception("point out of range");
			if (funcs.size() == 0 || start_counts.size() == 0) throw exception("invalid parameter");
			if (funcs.size() != start_counts.size()) throw exception("invalid parameter");
		}
	};

	function<T (const T&)> get_poligon(vector< pair<T, T> > res){
		return [res](const T& x) -> T{
			if (!res.size()) throw exception();
			T k, b;
			//auto tmp = res;
			unsigned int i;
			if (res.size() == 1 || x < res[0].first) i = 0;
			else if (x >= res[res.size() - 1].first) i = res.size() - 2;
			else for (i = 0; res[i + 1].first <= x; i++);
			if (res.size() == 1){k = 0; b = res[0].second;}
			else{
				k = (res[i + 1].second - res[i].second) / (res[i + 1].first - res[i].first);
				b = res[i].second - k*res[i].first;
			}
			return k*x + b;
		};
	}

	vector< pair<T, T> > Euler_method(const Koshi_Conditions& k, unsigned int n = 51){
		if (n == 0) return vector< pair<T, T> >();
		unsigned int n_nodes_left = 0, n_nodes_right = n - 1;
		auto h = n != 1 ? (k.range.second - k.range.first) / (n - 1) : k.range.second - k.range.first;
		if (k.point.first != k.range.first){
			auto proporc = (k.point.first - k.range.first) / (k.range.second - k.range.first);
			n_nodes_left = proporc * (n - 1);
			n_nodes_right = (n - 1) - n_nodes_left;
			if (n_nodes_left) h = min((k.point.first - k.range.first) / n_nodes_left, h);
			if (n_nodes_right) h = min((k.range.second - k.point.first) / n_nodes_right, h);
		}
		vector< pair<T, T> > res(n);
		res[n_nodes_left] = k.point;
		if (n_nodes_right){
			auto i = n_nodes_left + 1, end = n;
			for (; i != end; i++){
				res[i].first = res[i - 1].first + h;
				res[i].second = res[i - 1].second + h * k.func(res[i - 1].first, res[i - 1].second);
			}
		}
		if (n_nodes_left){
			auto i = n_nodes_left;
			for (; i-- > 0;){
				res[i].first = res[i + 1].first - h;
				res[i].second = res[i + 1].second - h * k.func(res[i + 1].first, res[i + 1].second);
			}
		}
		return res;
	}

	vector< pair<T, T> > Runge_Kutta_method(const Koshi_Conditions& k, unsigned int n = 51){
		if (n == 0) return vector< pair<T, T> >();
		unsigned int n_nodes_left = 0, n_nodes_right = n - 1;
		auto h = n != 1 ? (k.range.second - k.range.first) / (n - 1) : k.range.second - k.range.first;
		if (k.point.first != k.range.first){
			auto proporc = (k.point.first - k.range.first) / (k.range.second - k.range.first);
			n_nodes_left = proporc * (n - 1);
			n_nodes_right = (n - 1) - n_nodes_left;
			if (n_nodes_left) h = min((k.point.first - k.range.first) / n_nodes_left, h);
			if (n_nodes_right) h = min((k.range.second - k.point.first) / n_nodes_right, h);
		}
		vector< pair<T, T> > res(n);
		res[n_nodes_left] = k.point;
		struct for_K{
			static T K1(const T& x, const T& y, const T& h, const function<T (const T&, const T&)>& func){
				return h * func(x, y);
			}
			static T K2(const T& x, const T& y, const T& h, const function<T (const T&, const T&)>& func){
				return h * func(x + h / 2, y + K1(x, y, h, func) / 2);
			}
			static T K3(const T& x, const T& y, const T& h, const function<T (const T&, const T&)>& func){
				return h * func(x + h, y - K1(x, y, h, func) + 2 * K2(x, y, h, func));
			}
		};
		const static T p1 = 1.0/6;
		const static T p2 = 4.0/6;
		const static T p3 = 1.0/6;
		if (n_nodes_right){
			auto i = n_nodes_left + 1, end = n;
			for (; i != end; i++){
				res[i].first = res[i - 1].first + h;
				res[i].second = res[i - 1].second + (
					+ p1 * for_K::K1(res[i - 1].first, res[i - 1].second, h, k.func)
					+ p2 * for_K::K2(res[i - 1].first, res[i - 1].second, h, k.func)
					+ p3 * for_K::K3(res[i - 1].first, res[i - 1].second, h, k.func)
					);
			}
		}
		if (n_nodes_left){
			auto i = n_nodes_left;
			for (; i-- > 0;){
				res[i].first = res[i + 1].first - h;
				res[i].second = res[i + 1].second + (
					+ p1 * for_K::K1(res[i + 1].first, res[i + 1].second, -h, k.func)
					+ p2 * for_K::K2(res[i + 1].first, res[i + 1].second, -h, k.func)
					+ p3 * for_K::K3(res[i + 1].first, res[i + 1].second, -h, k.func)
					);
			}
		}
		return res;
	}

	//n - колич узлов, s - количество шагов(степень многочлена Лагранджа, что используется в методе)
	vector< pair<T, T> > Adams_method(const Koshi_Conditions& k, unsigned int n = 51, unsigned int s = 3){
		if (n == 0) return vector< pair<T, T> >();
		if (s >= n) return Euler_method(k, n);
		unsigned int n_nodes_left = 0, n_nodes_right = n - 1;
		auto h = n != 1 ? (k.range.second - k.range.first) / (n - 1) : k.range.second - k.range.first;
		if (k.point.first != k.range.first){
			auto proporc = (k.point.first - k.range.first) / (k.range.second - k.range.first);
			n_nodes_left = proporc * (n - 1);
			n_nodes_right = (n - 1) - n_nodes_left;
			if (n_nodes_left) h = min((k.point.first - k.range.first) / n_nodes_left, h);
			if (n_nodes_right) h = min((k.range.second - k.point.first) / n_nodes_right, h);
		}
		unsigned int n_nodes_left_without_Adams_n = min(s / 2, n_nodes_left), n_nodes_right_without_Adams_n = s - n_nodes_left_without_Adams_n;
		vector< pair<T, T> > res(n);
#pragma region получение s начальных значений
		auto tmp_n = n_nodes_left_without_Adams_n + n_nodes_right_without_Adams_n + 1;
		pair<T, T> tmp_range(k.point.first - n_nodes_left_without_Adams_n * h, k.point.first + n_nodes_right_without_Adams_n * h);
		auto tmp_vec = Runge_Kutta_method(Koshi_Conditions(k.func, k.point, tmp_range), tmp_n);

		for (auto i = 0u; i < tmp_vec.size(); i++) res[n_nodes_left - n_nodes_left_without_Adams_n + i] = tmp_vec[i];
#pragma endregion
//#pragma region получение s начальных значений
//		res[n_nodes_left] = k.point;
//		for (auto i = 0u; i < n_nodes_right_without_Adams_n; i++){
//			res[n_nodes_left + 1 + i].first = res[n_nodes_left + i].first + h;
//			res[n_nodes_left + 1 + i].second = res[n_nodes_left + i].second + h * k.func(res[n_nodes_left + i].first, res[n_nodes_left + i].second);
//		}
//		for (auto i = 0u; i < n_nodes_left_without_Adams_n; i++){
//			res[n_nodes_left - 1 - i].first = res[n_nodes_left - i].first - h;
//			res[n_nodes_left - 1 - i].second = res[n_nodes_left - i].second - h * k.func(res[n_nodes_left - i].first, res[n_nodes_left - i].second);
//		}
//#pragma endregion
		auto factor = [](const unsigned int& k) -> unsigned int{
			auto res = 1u;
			for (auto i = 1u; i <= k; i++) res *= i;
			return res;
		};
		vector<T> A(s + 1);
		polinom<T> P = 1;
		vector<T> tmp(2);
		tmp[1] = 1;
		//теперь P = П(x - i)|i = 0,n
		for (auto i = 0u; i <= s; i++){
			tmp[0] = i;
			P *= tmp;
		}
		//A[k] = ... (6.47 - смотри Бойко)
		for (auto i = 0u; i <= s; i++){
			tmp[0] = i;
			auto for_integr = (P / tmp).integr();
			A[i] = (pow(-1.0, i % 2) / (factor(i) * factor(s - i))) * for_integr(1);// - for_integr(0);//for_integr(0) is 0
		}

		for (auto i = n_nodes_left - n_nodes_left_without_Adams_n; i-- > 0;){
			res[i].first = res[i + 1].first - h;
			T sum = 0;
			for (auto k_ = 0u; k_ <= s; k_++) sum += A[k_] * k.func(res[i + 1 + k_].first, res[i + 1 + k_].second);
			res[i].second = res[i + 1].second - h * sum;
		}
		for (auto i = n_nodes_left + n_nodes_right_without_Adams_n + 1; i < n; i++){
			res[i].first = res[i - 1].first + h;
			T sum = 0;
			for (auto k_ = 0u; k_ <= s; k_++) sum += A[k_] * k.func(res[i - 1 - k_].first, res[i - 1 - k_].second);
			res[i].second = res[i - 1].second + h * sum;
		}
		return res;
	}

	vector< vector< pair<T, T> > > Runge_Kutta_method(const Koshi_Conditions_for_system& k, unsigned int n = 51){
		if (n == 0) return vector< vector< pair<T, T> > >(k.funcs.size());
		unsigned int n_nodes_left = 0, n_nodes_right = n - 1;
		auto h = (n != 1) ? (k.range.second - k.range.first) / (n - 1) : k.range.second - k.range.first;
		if (k.x0 != k.range.first){
			auto proporc = (k.x0 - k.range.first) / (k.range.second - k.range.first);
			n_nodes_left = proporc * (n - 1);
			n_nodes_right = (n - 1) - n_nodes_left;
			if (n_nodes_left) h = min((k.x0 - k.range.first) / n_nodes_left, h);
			if (n_nodes_right) h = min((k.range.second - k.x0) / n_nodes_right, h);
		}
		vector< vector< pair<T, T> > > res(k.funcs.size());
		for (int i = 0; i < k.funcs.size(); i++){
			res[i] = vector< pair<T, T> >(n);
			res[i][n_nodes_left] = pair<T, T>(k.x0, k.start_counts[i]);
		}
		struct for_K{
			static T K1(const T& x, const vector<T>& counts, const T& h, const function<T (const T&, const vector<T>&)>& func, const vector< function<T (const T&, const vector< T >&)> >& funcs){
				return h * func(x, counts);
			}
			static T K2(const T& x, const vector<T>& counts, const T& h, const function<T (const T&, const vector<T>&)>& func, const vector< function<T (const T&, const vector< T >&)> >& funcs){
				vector<T> new_count(funcs.size());
				for (int i = 0; i < funcs.size(); i++) new_count[i] = counts[i] + K1(x, counts, h, funcs[i], funcs) / 2;
				return h * func(x + h / 2, new_count);
			}
			static T K3(const T& x, const vector<T>& counts, const T& h, const function<T (const T&, const vector<T>&)>& func, const vector< function<T (const T&, const vector< T >&)> >& funcs){
				vector<T> new_count(funcs.size());
				for (int i = 0; i < funcs.size(); i++) new_count[i] = counts[i] - K1(x, counts, h, funcs[i], funcs) + 2 * K2(x, counts, h, funcs[i], funcs);
				return h * func(x + h, new_count);
			}
		};
		const static T p1 = 1.0/6;
		const static T p2 = 4.0/6;
		const static T p3 = 1.0/6;
		if (n_nodes_right){
			auto i = n_nodes_left + 1, end = n;
			for (; i != end; i++){
				vector<T> prev_counts(k.funcs.size());
				for (int j = 0; j < k.funcs.size(); j++) prev_counts[j] = res[j][i - 1].second;
				for (int j = 0; j < k.funcs.size(); j++){
					res[j][i].first = res[j][i - 1].first + h;
					res[j][i].second = res[j][i - 1].second + (
						+ p1 * for_K::K1(res[j][i - 1].first, prev_counts, h, k.funcs[j], k.funcs)
						+ p2 * for_K::K2(res[j][i - 1].first, prev_counts, h, k.funcs[j], k.funcs)
						+ p3 * for_K::K3(res[j][i - 1].first, prev_counts, h, k.funcs[j], k.funcs)
						);
				}
			}
		}
		if (n_nodes_left){
			auto i = n_nodes_left;
			for (; i-- > 0;){
				vector<T> prev_counts(k.funcs.size());
				for (int j = 0; j < k.funcs.size(); j++) prev_counts[j] = res[j][i + 1].second;
				for (int j = 0; j < k.funcs.size(); j++){
					res[j][i].first = res[j][i + 1].first - h;
					res[j][i].second = res[j][i + 1].second + (
						+ p1 * for_K::K1(res[j][i + 1].first, prev_counts, -h, k.funcs[j], k.funcs)
						+ p2 * for_K::K2(res[j][i + 1].first, prev_counts, -h, k.funcs[j], k.funcs)
						+ p3 * for_K::K3(res[j][i + 1].first, prev_counts, -h, k.funcs[j], k.funcs)
						);
				}
			}
		}
		return res;
	}

}