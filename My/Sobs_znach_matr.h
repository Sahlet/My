//Sobs_znach_matr.h
#pragma once
#include<My\polonom_root_searching.h>
#include<My\SLAU.h>

namespace My{
#pragma region определение

//точность
	T epsilon = 0.00001;
	const T polinom<T>::eps = 1e-11;

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
}

namespace My{
	typedef long double T;
	//характеристический полином
	polinom<T> get_haract_polinom(const square_matrix<T>& A){
		auto dim = A.dimension();
		if (dim == 0) return polinom<T>();
		square_matrix< polinom<T> > lambda_A(dim);//матрица полиномов, для находжения характеристического полинома матрици А
		/*
			lambda_A - матрица вида
			|a11 - L, a12, a13, ... , a1n|
			|a11, a12 - L, a13, ... , a1n|
			 ............................
			|a11, a12, a13, ... , a1n - L|

		*/
		vector<T> v;
		auto iter = A.begin();
		for (auto& i : lambda_A){
			v = vector<T>(1);
			v[0] = *iter++;
			i = std::move(v);
		}
		for (auto i = 0u; i < dim; i++){
			v = vector<T>(2);
			v[0] = A[i][i];
			v[1] = -1;
			lambda_A[i][i] = std::move(v);
		}
		return lambda_A.det() * (polinom<T>)_Pow_int(-1, dim);//получили характеристический полином
	}
	//собственные значения матрици
	vector<T> get_sobs_znach(const square_matrix<T>& A){
		auto Haracter = get_haract_polinom(A);//получили характеристический полином
		//находим нули характеристического многочрена
		auto intervals = Sturm<T>::get_intervals(Haracter, -5000, +5000, 10);
		vector<T> roots(intervals.size());
		shared_ptr< func<T> > f(new polinom_to_func_adapter<T>(Haracter));
		{
			auto iter = intervals.begin();
			for (auto& i : roots){
				conditions<T> c(*iter++, f);
				i = for_iter_methods::Newthon_iter(&c)->root;
			}
		}
		return roots;
	}
	//вернет собственные значения и соответствующие им собственные векторы
	map< T, list< vector<T> > > get_sobs_znach_i_vec(const square_matrix<T>& A){
		auto dim = A.dimension();
		auto roots = get_sobs_znach(A);
		map< T, list< vector<T> > > res;
		auto E = square_matrix<T>::get_E(dim);
		for (const auto& root : roots){
			matrix<T> decomposition = FSR/*<T>*/(A - E * root);
			auto vec_space_dim = decomposition.width();//размерность векторного пространста
			for (auto i = 0u; i < vec_space_dim; i++){
				vector<T> e_i(vec_space_dim);
				e_i[i] = 1;//e_i теперь один из базисных векторов
				auto sobs_vec = decomposition * e_i; //получили один из собственных векторов для собственного значения root
				T koef = 0;
				for (const auto& i : sobs_vec) if (abs(koef) < abs(i)) koef = i;
				if (koef != 0) for (auto& i : sobs_vec) i /= koef;//приводим вектор sobs_vec к виду, который треб по условию : первая (кубическая) норма ||sobs_vec|| = max( abs(sobs_vec[i])|i = 1,vec_space_dim ) = 1
				res[root].push_back(sobs_vec);
			}
		}
		return res;
	}
}