//square_matrix.h
#pragma once
#include <My\matrix.h>
#include <stack>

namespace My{
template<class T = long double>
//у типа T должно быть преобразование из double (в частности из 0 и 1 так, чтоб конструктор по умолчянию соответствовал значению 0)
class square_matrix : public matrix<T>{
protected:
	struct flag{
			long long line;//номер строки (если номер < 0, то флаг false)
			flag(const long long &line = -1) : line(line){}
			operator bool (){
				if (line < 0 ) return false;
				else return true;
			}
			operator long long (){ return line; }
			flag& operator = (const bool &ch){
				if (ch == false) line = -1;
				else line = 0;
				return *this;
			}
			flag& operator = (const long long &line){
				this->line = line;
				return *this;
			}
	};
public:
	//размерность
	unsigned int dimension() const{ return this->size(); }
	square_matrix (const unsigned int& dim = 0) : matrix<T>(dim, dim){}
	square_matrix (square_matrix && obj) _NOEXCEPT : matrix<T>(std::move(obj)){}
	square_matrix (const matrix & obj) : matrix<T>(obj) throw(exception){
		if (this->height() != this->width()) throw(exception("not squre range"));
	}
	square_matrix (matrix && obj) _NOEXCEPT : matrix<T>(std::move(obj)){
		if (this->height() != this->width()) throw(exception("not squre range"));
	}
	square_matrix& operator=(square_matrix && obj){
		*((matrix<T>*)this) = std::move(obj);
		return *this;
	}
	square_matrix& operator=(matrix && obj){
		*((matrix<T>*)this) = std::move(obj);
		if (this->height() != this->width()) throw(exception("not squre range"));
		return *this;
	}
	static square_matrix get_E(unsigned int dim){
		square_matrix res(dim);
		for (auto i = 0u; i < dim; i++) res[i][i] = 1;
		return res;
	}
	square_matrix operator*(const square_matrix& m) const throw(exception){
		return std::move(square_matrix(*this) *= m);
	}
	vector<T> operator*(const vector<T>& v) const throw(exception){
		return std::move((matrix<T>&)square_matrix(*this) * v);
	}
	square_matrix operator*(const T& t) const {
		return std::move(square_matrix(*this) *= t);
	}
	square_matrix operator+(const square_matrix& m) const throw(exception) {
		return std::move(square_matrix(*this) += m);
	}
	square_matrix operator-(const square_matrix& m) const throw(exception) {
		return std::move(square_matrix(*this) -= m);
	}
	//взять детерминант
	T det() const{
		auto dim = this->size();
		if (!dim) throw (range_error("empty matryx"));
		T res = 0, tmp_res = 1;
		flag *c = new flag[dim];//показывает занят ли столбец
		long long * counts = new long long[dim];
		counts[0] = 0;
		long long line = 0;
		stack<T> way;
		//перебор строк
		while (true){
			if (c[counts[line]]){
				if (c[counts[line]].line == line){
					c[counts[line]] = false;//строка освобождает столбец
					counts[line]++;
					way.pop();
				}
				if (counts[line] < dim)
					while (c[counts[line]]){
						if (++counts[line] == dim) break;
					}
				if (counts[line] == dim){
					if (line-- == 0) break;
					continue;
				}
			}
			c[counts[line]] = line;//строка заняла столбец
			way.push((*(matrix*)this)[line][counts[line]]);
			if (line + 1 < dim){
				counts[++line] = 0;
				continue;
			} else {
				struct for_N{
			 		static int koef(long long *transposition, long long n){
			 			if (N(transposition, n)) return -1;
			 			else return 1;
			 		}
				private:
			 		//вернет степень для -1
					static long long N(long long *transposition, long long n){
				 		bool res = false;
				 		for (auto i = transposition + n / 2 + 1, end = transposition + n ; i != end; i++){
				 	 		for (auto j = i + 1; j != end; j++){
				 				if (*i > *j) res = !res;
				 			}
						}
						long long smaller_in_left;
						for (auto i = transposition + n / 2, end = transposition - 1 ; i != end; i--){
							smaller_in_left = 0;
				 	 		for (auto j = i - 1; j != end; j--){
				 				if (*i > *j) smaller_in_left++;
				 			}
				 			if ((*i - smaller_in_left) % 2) res = !res;// (*i - smaller_in_left) столько чисел ментше *i правее от i(незабывайте 0)
						}
						return res;
					}
				};
				T tmp_res = for_N::koef((long long *)c, dim);
				for (const auto& i : way._Get_container()) tmp_res *= i;
				res += tmp_res;
				tmp_res = 1;
			}
		}
		delete c;
		delete counts;
		return res;
	}
	//взять детерминант
	T operator*() const{
		return this->det();
	}
	//взять обратную матрицу
	square_matrix get_revers() const throw (exception){
		auto det = this->det();
		if (det == 0) throw exception("there is no revers");
		auto dim = this->dimension();
		square_matrix res(dim);
		set<unsigned int> lines, columns;
		auto iter = res.begin();
		for (auto i = 0u; i < dim; i++){
			lines.insert(i);
			for (auto j = 0u; j < dim; j++){
				columns.insert(j);
				*iter++ = _Pow_int(-1, (i + j) % 2) * ((square_matrix)sub_matrix(*this, lines, columns, false)).det();
				columns.erase(j);
			}
			lines.erase(i);
		}

		res = res.get_transplanted() * (1 / det);
		return res;
	}
	//взять обратную матрицу
	matrix operator!() const{
		return this->get_revers();
	}
};

template<class T>
string to_string(const square_matrix<T>& m){
	return My::to_string((matrix<T>&)m);
}

template<class T>
T det(const square_matrix<T> m){
	return *m;
}

template<class T>
square_matrix<T> sub_matrix(const matrix<T>& m, const pair<unsigned int, unsigned int>& point, const unsigned int& dim) throw(range_error){
	return sub_matrix(m, point, dim, dim);
}
}