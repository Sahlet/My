//SLAU.h
#pragma once
#include <My\square_matrix.h>
//#include <My\polinom.h>

namespace My{

	template<class T>
	class matrix_changer final : private square_matrix<T>{
	public:
		typedef bool (*is_matrix_correct)(const square_matrix<T>& m);
		static square_matrix<T> get_correct_matrix(const square_matrix<T>& m, const is_matrix_correct& func) throw(exception){
			auto dim = m.dimension();
			if (!dim) throw (range_error("empty matryx"));
			typedef vector < vector < T > > base;
			base buf(m);
			square_matrix<T> res;
			res.base::operator=(base(dim));

			flag *c = new flag[dim];
			long long * slots = new long long[dim];
			slots[0] = 0;
			long long line = 0;
		while (true){
			if (c[slots[line]]){
				if (c[slots[line]].line == line){
					buf[line] = std::move(res.base::operator[](slots[line]));
					c[slots[line]] = false;//строка освобождает столбец
					slots[line]++;
				}
				if (slots[line] < dim)
					while (c[slots[line]]){
						if (++slots[line] == dim) break;
					}
				if (slots[line] == dim){
					if (line-- == 0) break;
					continue;
				}
			}
			c[slots[line]] = line;//строка заняла столбец
			res.base::operator[](slots[line]) = std::move(buf[line]);
			if (line + 1 < dim) slots[++line] = 0;
			else if (func(res)) break;
		}
			delete[] c;
			delete[] slots;
			if (line < 0) throw exception("there is no correct condition of matrix");
			return res;
		}
		//подготовка
		static void preparation(square_matrix<T> &A, vector<T> &f, is_matrix_correct func){
			auto dim = A.dimension();
			if (f.size() != dim) throw exception("bad sizes");
			square_matrix<T> A_(dim + 1);
	
			for (auto i = 0u; i < dim; i++){
				for (auto j = 0u; j < dim; j++) A_[i][j] = A[i][j];
				A_[i][dim] = f[i];
				A_[dim][i] = 0;
			}
			A_[dim][dim] = 1;
			A_ = matrix_changer<T>::get_correct_matrix(A_, func);

			f = A_.get_transplanted()[dim];
			f.pop_back();
			A = sub_matrix(A_, pair<unsigned int, unsigned int>(0, 0), dim);
		}
	};

	template<class T>
	bool diagonal_hasnt_zeros(const square_matrix<T>& m){
		auto dim = m.dimension();
		for (auto i = 0u; i < dim; i++){
			if (m[i][i] == 0) return false;
		}
		return true;
	}

	template<class T>
	bool diagonal_main_minors_are_more_then_null(const square_matrix<T>& m){
		auto dim = m.dimension();
		auto point = pair<unsigned int, unsigned int>(0, 0);
		for (auto i = 1u; i <= dim; i++){
			if (sub_matrix(m, point, i).det() <= 0) return false;
		}
		return true;
	}

	template<class T>
	vector<T> SLAU_sqrt(square_matrix<T> A, vector<T> f) throw (exception){
		if (*A == 0) throw exception("det(A) = 0");
		if (f.size() != A.dimension()) throw exception("bad sizes");
		//проверка необходимости и достаточности положительной определенности не надо, поскольку det(A) != 0 и A приводится к положительно определенной ниже
		/*try{
			matrix_changer<T>::preparation(A, f, diagonal_main_minors_are_more_then_null);
		}catch(exception ex){
			throw exception((string(ex.what()) + "conditions are not met:\nthere is main minor equal to zero").c_str());
		}*/

		auto dim = A.dimension();

		f = A.get_transplanted() * f;
		A = A.get_transplanted() * A;
		//теперь A положительно определенная

		square_matrix<T> S(dim);
		T tmp = 0;
		struct pow_func {
			static double pow(double v, unsigned int p) {
				if (p == 0) return 1;
				for (int i = 1; i < p; i++) v *= v;
				return v;
			}
		};
		for (auto i = 0u; i < dim; i++){
			tmp = 0;
			for (auto k = 0u; k < i; k++) tmp += pow_func::pow(S[k][i], 2);
			S[i][i] = sqrt(A[i][i] - tmp);
			for (auto j = i + 1; j < dim; j++){
				tmp = 0;
				for (auto k = 0u; k < i; k++) tmp += S[k][i] * S[k][j];
				S[i][j] = (A[i][j] - tmp) / S[i][i];
			}
		}

		vector<T> y(dim);
		for (auto i = 0u; i < dim; i++){
			tmp = 0;
			for (auto k = 0u; k < i; k++) tmp += S[k][i] * y[k];
			y[i] = (f[i] - tmp) / S[i][i];
		}

		vector<T> x(dim);
		for (auto j = dim; j-- > 0;){
			tmp = 0;
			for (auto k = j + 1; k < dim; k++) tmp += S[j][k] * x[k];
			x[j] = (y[j] - tmp) / S[j][j];
		}

		return x;
	}

	template<class T>
	vector<T> SLAU_simple_iter(square_matrix<T> A, vector<T> f, const T& e, unsigned int& n_iter) throw (exception){
		if (e <= 0) throw exception(("bad eps = " + to_string(e)).c_str());
		if (*A == 0) throw exception("det(A) = 0");

		/*if (f.size() != A.dimension()) throw exception("bad sizes");
		try{
			matrix_changer<T>::preparation(A, f, diagonal_hasnt_zeros);
		}catch(exception ex){
			throw exception((string(ex.what()) + "conditions are not met:\ndet = 0").c_str());
		}*/
		n_iter = 0;
		
		//http://orloff.am.tpu.ru/chisl_metod/Lab3/iter.htm
		//http://lectoriy.mipt.ru/lecture/Maths-NumAnalysis-L04-Aristova-140924.02

	#pragma region 1-й способ
		/*
		K * | A*x = f
		K*A*x = K*f

		пусть	:
			K = (!A - Q)
			B = Q*A
			g = K*f => g = (!A - Q)*f

		(!A - Q)*A*x = + g
		x - B*x = g
		x = B*x + g

		условия	:
			|| B || < 1
		решение	:
			 / det(Q) не 0
			|  det(A) не 0
			 \ Q -> min
		*/
		///*
		square_matrix<T> E(square_matrix<T>::get_E(A.dimension())), Q(E), B;
		while ((B = Q * A).abs() >= 1) Q *= 0.1;
		vector<T> g = (!A - Q) * f;
		auto q = B.abs();
		//*/
	#pragma endregion

		
	#pragma region 2-й способ (не надо считать обратную матрицу !A) (почему-то не работает)
		/*
		пусть A - положительно определенная (привести к полож опр можно так new_A = A.get_transplanted() * A)
		тогда все ее собственные значения L_i > 0, i = 1...n
			A*x = f | * a
			x = x - a*A*x + a*f
			x = (E - a*A)*x + a*f
				B = (E - a*A)
				g = a*f
			пришли к
			x = B*x + g
			
			собственные значения матрици B равны	1 - a*L_i. i = 1...n

			чтоб итерационный процесс сходился - необходимо и достаточно , чтоб		abs(1 - a*L_i) < 1
			поскольку A п.опр. => a є (0; 2/A.abs)
		*/
		/*
		f = A.get_transplanted() * f;
		A = A.get_transplanted() * A;
		T a = 1 / A.abs(), q;//a in (0; 2/A.abs)
		square_matrix<T> B, E(square_matrix<T>::get_E(A.dimension()));
		while ((q = (B = E - A * a).abs()) >= 1) a *= 0.5;
		vector<T> g = E * a * f;
		*/
	#pragma endregion

		vector<T> x, x_ = g;

		T delta = 2 * e, tmp;
		while (delta > e){
			x = B * x_ + g;
			delta = (matrix<T>(x - x_).abs()) * q / (1 - q);
			x_ = x;
			n_iter++;
		}
		
		return x;
	}
	template<class T>
	vector<T> SLAU_simple_iter(const square_matrix<T>& A, const vector<T>& f, const double& e) throw (exception){
		unsigned int n_iter;
		return SLAU_simple_iter<T>(A, f, e, n_iter);
	}
	//решает систему методом Гауса
	template<class T>
	vector<T> SLAU_Gaus(const square_matrix<T>& A, vector<T> f) throw (exception){
		if (*A == 0) throw exception("det(A) = 0");
		if (f.size() != A.dimension()) throw exception("bad sizes");
		auto n = A.dimension();
		auto a = A.operator vector< vector<T> >();
		//приводим "а" к триугольному виду
		for (auto i = 0u; i < n; i++){
			if (a[i][i] == 0){//если a[i][i] == 0  -  меняем строку с первой, где a[k][i] != 0
				for (auto k = i + 1; k < n; k++){
					if (a[k][i] != 0){
						swap(a[k], a[i]);
						swap(f[k], f[i]);
						break;
					}
				}
			}
			auto tmp = a[i][i];
			for (auto k = i; k < n; k++){ a[i][k] /= tmp; }
			f[i] /= tmp;
			for (auto k = i + 1; k < n; k++){
				if((tmp = a[k][i]) != 0){
					for (auto z = i; z < n; z++){ a[k][z] -= a[i][z] * tmp; }
					f[k] -= f[i] * tmp;
				}
			}
		}
		//сейчас в "а" ниже диагонали 0, диагонать еденичная, выше диагонали что-то
		for (auto i = n - 1; i >/*=*/ 0; i--)
			for (auto j = i; j > 0; j--){
				f[j - 1] -= a[j - 1][i] * f[i];
				//a[j][i] = 0;
			}
		return f;
	}
	
	
	typedef long double T;
	//находит ФСР
	//	среди столбцов матрици А могут быть базисные векторы.
	//	если такие есть, то функция вернет разложенные по этому базису векторы.
	//	результат имет вид ВВ (вектора векторов(vector< vector<T> >)) (индекс вектора в ВВ соответствует номеру столбца в A).
	//	если выйдет так, что А не имеет ФСР - результатом будет вектор пустых векторов (что логично, так как выражать не из чего)
	//template<class T>
	vector< vector<T> > FSR(const matrix<T>& A, const T epsilon = 1e-10) throw (exception){
		auto w = A.width(), h = A.height(), n = w < h ? w : h;
		if (n == 0) return vector< vector<T> >();
		auto a = A.operator vector< vector<T> >();
		//приводим "а" к трапециевидному виду
		auto abs_eps = abs(epsilon);
		//if (abs_eps == 0 || abs_eps >= 0.1) abs_eps = 1e-10;
		for (auto i = 0u, j = 0u; i < w && j < h; i++, j++){//j - строка, i - столбец
			for (auto k = j; k < h; k++){
				if (abs(a[k][i]) <= abs_eps) a[k][i] = 0;
			}
			if (a[j][i] == 0) for (;i < w; i++){//если a[j][i] == 0  -  меняем строку с первой, где a[k][i] != 0
				auto k = j + 1;
				for (; k < h; k++){
					if (a[k][i] != 0){
						swap(a[k], a[j]);
						break;
					}
				}
				if (k != h) break;
			}
			if (i == w) break;
			auto tmp = a[j][i];
			for (auto k = i; k < w; k++){ a[j][k] /= tmp; }
			for (auto k = j + 1; k < h; k++){
				if((tmp = a[k][i]) != 0){
					a[k][i] = 0;//на всяк пожарный(чтоб не оставалось арифметич погрешности)
					for (auto z = i + 1; z < w; z++){ if (abs(a[k][z] -= a[j][z] * tmp) <= abs_eps) a[k][z] = 0; }
				}
			}
		}
		/*//количество независимых (автономных) векторов (количество столбцов у которых все коэфициенты - нули)
		auto n_independent = 0u;
		for (auto i = 0u; i < w; i++){
			bool ch = true;
			for (auto j = 0u; j < h; j++){
				if (a[i][j] != 0){
					ch = false;
					break;
				}
			}
			if (ch) n_independent++;
		}*/
		auto ranc = n;//ранг матрицы A
		while (ranc){
			ranc--;
			auto j = w;
			for(;j && a[ranc][j - 1] == 0; j--);
			if (j){
				ranc++;
				break;
			}
		}
		auto res = vector< vector<T> >(w);
		auto dim = w - ranc;//размерность векторного пространства
		if (dim == 0) return res;

		stack< unsigned int > left_number, right_number;//номера векторов в текущей строке для текущего столбца слева, справа
		auto n_basys = dim;//номер базисного вектора, который выбирается сейчас
		for (auto i = ranc; i > 0; i--){//перебираем строки с конца
			for (auto j = 0u; j < w; j++){
				if (a[i - 1][j] != 0) left_number.push(j);
			}
			if (!right_number.empty()) right_number = stack< unsigned int >();//опустошили
			while (left_number.size()){//пока слева остались ненулевые слогаемые в i-той строке
				if(!res[left_number.top()].size()){//если этот вектор еще не определен
					res[left_number.top()] = vector<T>(dim);
					if (left_number.size() > 1)/*если слева остались ненулевые слогаемые в i-той строке*/
						res[left_number.top()][--n_basys] = 1;//это теперь базисный вектор
					else{
						/*если справа есть через что выразить этот вектор в i-той строке*/
						if (right_number.size()){
							auto& courent_vec = res[left_number.top()];
							for (const auto& num : right_number._Get_container()){
								vector<T> tmp(dim);
								for (auto j = 0u; j < dim; j++) tmp[j] = a[i - 1][num] * res[num][j];
								courent_vec = courent_vec - tmp;
							}
						}//else - вектор остается нулевым (его не нулевой коэффициент один в строке)
					}
				}
				right_number.push(left_number.top());
				left_number.pop();
			}
		}
		//задаем базисные вектора, через которы не выражается ни один из res кроме них самих
		//(у соответствующих столбцов в A все коэфициенты - нули)
		for (auto& i : res){
			if (!i.size()){
				i = vector<T>(dim);
				i[--n_basys] = 1;
			}
		}
		return res;
	}
}