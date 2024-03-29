//matrix.h
#pragma once
#include <My/string.h>
#include <vector>
#include <set>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <exception>

namespace My{

//???? ?????? ???????????? matrix::abs - ?? ???????? ??????????
//matrix<Type_name>::abs_ matrix<Type_name>::abs_ptr = matrix<Type_name>::std_abs;
//Type_name - ??? ????


typedef unsigned short US;

template<class T> class matrix;

template<class T>
class for_square_brackets {
  friend class My::matrix< T >;
  T* ptr;
  US w;
  for_square_brackets(const for_square_brackets&)  = default;
  for_square_brackets(for_square_brackets&&)  = default;
  for_square_brackets& operator=(for_square_brackets&&) = delete;
  for_square_brackets(T* data, US w, US i) : ptr(data + i*w), w(w) {}
public:

  for_square_brackets& operator=(const for_square_brackets& obj) throw (std::invalid_argument) {
    if (obj.w != w) throw (std::invalid_argument("for_square_brackets::operator=(const for_square_brackets& obj) : dimention error"));
    if (ptr == obj.ptr) return *this;
    for (int i = 0; i < w; i++) {
      ptr[i] = obj.ptr[i];
    }
    return *this;
  }
  for_square_brackets& operator=(const std::vector< T >& obj) throw (std::invalid_argument) {
    if (obj.size() != w) throw (std::invalid_argument("for_square_brackets::operator=(const std::vector< T >& obj) : dimention error"));
    for (int i = 0; i < w; i++) {
      ptr[i] = obj[i];
    }
    return *this;
  }
  T& operator[](const US& j) throw(std::range_error) {
    if (j >= w) throw std::range_error("j >= w");
    return ptr[j];
  }
  const T& operator[](const US& j) const throw(std::range_error) {
    return (*const_cast< for_square_brackets* >(this))[j];
  }

  operator std::vector< T >() const {
    std::vector< T > res(w);
    for (int i = 0; i < w; i++) {
      res[i] = ptr[i];
    }
    return std::move(res);
  }
};

//typedef long double T;
template<class T = long double>
class matrix {
	typedef T (*abs_)(const matrix&);
	static abs_ abs_ptr;
private:
	US w = 0, h = 0;
	std::vector< T > vec;
public:
	typedef typename std::vector<T>::iterator iterator;
	typedef typename std::vector<T>::const_iterator const_iterator;
	matrix(const US& width = 0, const US& height = 0) : w(width), h(height), vec(w*h) {}
	matrix(const matrix&) = default;
	matrix(matrix &&) = default;
	matrix& operator = (const matrix& m) = default;
	matrix& operator = (matrix&& m) = default;

	matrix(const std::vector<T>& v) : w(v.size()), h(1), vec(v) {}
	matrix(std::vector<T>&& v) : w(v.size()), h(1), vec(std::move(v)) {}
	matrix(const US& width, const US& height, const std::vector<T>& v) : w(width), h(height) {
		if (width * height != v.size()) throw std::invalid_argument("width * height != v.size()");
		vec = v;
	}
	matrix(const US& width, const US& height, std::vector<T>&& v) : w(width), h(height) {
		if (width * height != v.size()) throw std::invalid_argument("width * height != v.size()");
		vec = std::move(v);
	}
	matrix(std::initializer_list< std::vector< T > > list) {
		if (!(h = list.size())) return;
		w = list.begin()->size();
		for (const std::vector< T >& v : list) {
			if (v.size() != w) throw std::invalid_argument("vectors in initializer_list have different size");
		}

		vec.resize(w*h);
		auto data = vec.data();
		for (const std::vector< T >& v : list) {
			for (auto& obj : v) {
				*data++ = std::move(obj);
			}
		}
	}

	matrix& operator = (const std::vector<T>& v) {
		return (*this) = matrix(v);
	}
	matrix& operator = (std::vector<T>&& v) {
		return  (*this) = matrix(std::move(v));
	}
	template< typename _T1_ >
	matrix& operator = (_T1_ v) {
	  for (auto& value : *this) {
	    value = v;
	  }
	  return *this;
	}

	inline const US& get_w() const {return w;}
	inline const US& get_h() const {return h;}
	inline const US& width() const { return w; }
	inline const US& height() const { return h; }
	inline std::pair<US, US> dimension() const { return std::pair< US, US >(w, h); }
	const std::vector< T >& get_vec() const { return vec; };

	T& operator()(const US& i, const US& j) throw(std::range_error) {
		return vec[j + i*w];
	}
	const T& operator()(const US& i, const US& j) const throw(std::range_error) {
		return (*const_cast< matrix* >(this))(i, j);
	}

	T& operator[](const std::pair< US, US > &coordinates) throw(std::range_error) {
		return vec[coordinates.second + coordinates.first*w];
	}
	const T& operator[](const std::pair< US, US > &coordinates) const throw(std::range_error) {
		return (*const_cast< matrix* >(this))[coordinates];
	}

	for_square_brackets< T > operator[](const US& i) throw(std::range_error) {
		if (i >= h) throw std::range_error("i >= h");
		return for_square_brackets< T >(vec.data(), w, i);
	}
	const for_square_brackets< T > operator[](const US& i) const throw(std::range_error) {
		return (*const_cast< matrix* >(this))[i];
	}

	iterator begin() {
		return vec.begin();
	}
	iterator end() {
		return vec.end();
	}
	const_iterator begin() const {
		return vec.begin();
	}
	const_iterator end() const {
		return vec.end();
	}

	static abs_ set_abs(abs_ value) {
		abs_ res = abs_ptr;
		abs_ptr = value;
		return res;
	}

	//????? ??????????????????
	matrix get_transplanted() const {
		matrix res(h, w);
		for (auto i = 0u; i < h; i++){
			for (auto j = 0u; j < w; j++){
				res(j, i) = (*this)(i, j);
			}
		}
		return res;
	}
	//????? ??????????????????
	matrix operator~() const {
		return this->get_transplanted();
	}

	std::vector<T> operator*(const std::vector<T>& v) const throw(std::exception) {
		if (v.size() != w) throw std::runtime_error("v.size() != w");
		std::vector<T> res(h);
		auto data = vec.data();
		for (US i = 0; i < h; i++) {
			for (US j = 0; j < w; j++) {
				res[i] += *data++ * v[j];
			}
		}
		return res;
	}
	matrix& operator*=(const std::vector<T>& v) throw(std::exception) {
		return *this = *this * v;
	}
	matrix& operator*=(const matrix& m) throw(std::exception) {
		if (w != m.h) throw(std::runtime_error("bad size of matix"));
		matrix res(m.w, h);

		for (US i = 0; i < res.h; i++) {
			for (US j = 0; j < res.w; j++) {
				for (US k = 0; k < w; k++) {
					res(i, j) += (*this)(i, k) * m(k, j);
				}
			}
		}

		*this = std::move(res);
		return *this;
	}
	matrix operator*(const matrix& m) const throw(std::exception) {
		return std::move(matrix(*this) *= m);
	}
	template< typename _T1_ >
	matrix& operator*=(_T1_ t) throw(std::exception) {
		for (auto& i : (*this)) i *= t;
		return *this;
	}
	matrix operator*(const T& t) const {
		return std::move(matrix(*this) *= t);
	}
	matrix& operator+=(const T& t) {
		for (auto& i : *this) i += t;
		return *this;
	}
	matrix operator+(const T& t) const {
		return std::move(matrix(*this) += t);
	}
	matrix& operator+=(const matrix& m) throw (std::invalid_argument) {
		if (this->w != m.w || this->h != m.h) throw std::invalid_argument("bad sizes");
		auto iter = m.begin();
		for (auto& i : *this) i += *iter++;
		return *this;
	}
	matrix operator+(const matrix& m) const throw(std::exception) {
		return std::move(matrix(*this) += m);
	}
	matrix operator-() const throw(std::exception) {
		matrix res(*this);
		for (auto& i : res) i = -i;
		return res;
	}
	matrix& operator-=(const matrix& m) throw (std::invalid_argument) {
		if (this->width() != m.width() || this->height() != m.height()) throw std::invalid_argument("bad sizes");
		auto iter = m.begin();
		for (auto& i : *this) i -= *iter++;
		return *this;
	}
	matrix operator-(const matrix& m) const throw(std::exception) {
		return std::move(matrix(*this) -= m);
	}
	inline bool operator==(const matrix& m){
		return vec == m.vec;
	}
	inline bool operator!=(const matrix& m) {
		return !((*this) == m);
	}

	/*??????? ||matrix|| = MAX{
		SUM{
			abs(A[i][j]) | j ? { 0 , ... , matrix.width - 1 }
		} | i ? { 0 , ... , matrix.height - 1 }
	}*/
	static T std_abs(const matrix& m) throw (std::exception) {
		//if (m.abs_ptr && abs_ptr != abs) abs_ptr(m);
		if (m.w == 0 || m.h == 0) throw (std::range_error("empty matryx"));
		T res = 0, tmp_res;
		for (US i = 0; i < m.height(); i++) {
			tmp_res = 0;
			for (US j = 0; j < m.width(); j++) tmp_res += std::abs(m(i, j));
			res = std::max(res, tmp_res);
		}
		return res;
	}
	//??????? ||matrix||
	T abs() throw (std::exception) {
		return abs_ptr(*this);
	}
	static T abs(const matrix& m) throw (std::exception) {
		return abs_ptr(m);
	}
	virtual std::string to_string() const {

	  auto to_string = [](double d) -> std::string {
	    //return My::to_string(d);
	    return std::to_string(d);
	  };

		int max_len = 0;
		for (const auto& value : (*this)) {
			max_len = std::max((int)to_string(value).length(), max_len);
		}


		std::stringstream res;
		int i = 0;
		std::string spaces(max_len, ' ');
		for (const auto& value : (*this)) {
			res << '|';
			int old_length = res.tellp();
			res << to_string(value);
			int new_length = res.tellp();
			res << (spaces.c_str() + (new_length - old_length));

			if (++i == w) {
				i = 0;
				res << '|' << '\n';
			}
		}

		return res.str();
	}
};

template<class T>
matrix< T > element_mult(const matrix< T >& l, const matrix< T >& r) throw(std::exception) {
  if (l.width() != r.width() || l.height() != r.height()) throw(std::runtime_error("bad size of matix"));
  auto res = l;

  auto iter = r.begin();

  for (auto& elem : res) {
    elem *= *iter++;
  }

  return res;
}

template<class T>
std::vector<T> operator*(const std::vector<T>& v, const matrix<T>& m) throw(std::exception) {
	if (v.size() != m.height()) throw std::runtime_error("v.size() != w");
	std::vector<T> res(m.width());
	auto iter = m.begin();
	for (US i = 0; i < m.height(); i++) {
		for (US j = 0; j < m.width(); j++) {
			res[j] += *iter++ * v[i];
		}
	}
	return res;
}

template<class T>
matrix<T> operator*(const T& t, const matrix<T>& m) {
	return m * t;
}

template<class T>
matrix<T> operator+(const T& t, const matrix<T>& m) {
	return m + t;
}

template<class T>
std::string to_string(const matrix<T>& m) {
	return m.to_string();
}

//typedef long double T;
template<class T>
matrix<T> sub_matrix(const matrix<T>& m, const std::pair<US, US>& point, const US& width, const US& height) throw(std::range_error) {
	if (point.first + width > m.width() || point.second + height > m.height()) throw (std::range_error("out of range"));
	matrix<T> res(width, height);
	for (US i = 0; i < height; i++) {
		for (US j = 0; j < width; j++) {
			res(i, j) = m(point.first + i, point.second + j);
		}
	}
	return res;
}

template<class T>
//???? flag - true - ?????? ???????, ? ??????? ????????? ???????? ?? ??????????? ????? ? ????????, ??? ? lines ? colums,
//???? flag - false - ?????? ???????, ? ??????? ????????? ???????? ??? ?? ?????? ?? ? ?????? ?? ? ???????, ??? ? lines ? colums,
matrix<T> sub_matrix(const matrix<T>& m, const std::set<US>& lines, const std::set<US>& columns, const bool& flag_) throw(std::range_error) {
	auto w_m = m.width(), h_m = m.height(), w_res = 0u, h_res = 0u;
	for(const auto& i : lines) {
		if (i >= h_m) throw (std::range_error("out of range"));
	}
	for(const auto& i : columns) {
		if (i >= w_m) throw (std::range_error("out of range"));
	}

	if (flag_) {
		w_res = columns.size();
		h_res = lines.size();
	} else {
		w_res = w_m - columns.size();
		h_res = h_m - lines.size();
	}

	matrix<T> res(w_res, h_res);
	auto iter = res.begin();
	if (flag_) {
		for (auto i = 0u; i < h_m; i++) {
			for (auto j = 0u; j < w_m; j++) {
				if (lines.count(i) && columns.count(j))
					*iter++ = m[i][j];
			}
		}
	} else {
		for (auto i = 0u; i < h_m; i++) {
			for (auto j = 0u; j < w_m; j++) {
				if (!(lines.count(i) || columns.count(j)))
					*iter++ = m[i][j];
			}
		}
	}
	return res;
}

}
