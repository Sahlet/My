#ifndef __VECTOR_ALGEBRA_HPP__
#define __VECTOR_ALGEBRA_HPP__

//VectorAlgebra.h
#include <vector>
#include <ostream>

namespace My {
namespace {

#define OPERATION_RESULT(OPERATOR) decltype( std::declval<_T1_>() OPERATOR std::declval<_T2_>() )

#define VEC_OPERATIONS1(NAME, OPERATOR)                                                           \
template<class _T1_, class _T2_>                                                                  \
std::vector< OPERATION_RESULT(OPERATOR) >                                                         \
NAME(const std::vector< _T1_ >& v1, const std::vector< _T2_ >& v2) {                              \
  if (v1.size() != v2.size()) throw std::invalid_argument("v1 and v2 have different dimensions"); \
  std::vector< OPERATION_RESULT(OPERATOR) > res(v1.size());                                       \
  for (int i = 0; i < v1.size(); i++) { res[i] = v1[i] OPERATOR v2[i]; }                          \
  return std::move(res);                                                                          \
}                                                                                                 \
template<class _T1_, class _T2_>                                                                  \
std::vector< OPERATION_RESULT(OPERATOR) >                                                         \
NAME(const std::vector< _T1_ >& v1, const _T2_& v2) {                                             \
  std::vector< OPERATION_RESULT(OPERATOR) > res(v1.size());                                       \
  for (int i = 0; i < v1.size(); i++) { res[i] = v1[i] OPERATOR v2; }                             \
  return std::move(res);                                                                          \
}                                                                                                 \
template<class _T1_, class _T2_>                                                                  \
std::vector< OPERATION_RESULT(OPERATOR) >                                                         \
NAME(const _T1_& v1, const std::vector< _T2_ >& v2) {                                             \
  std::vector< OPERATION_RESULT(OPERATOR) > res(v2.size());                                       \
  for (int i = 0; i < v2.size(); i++) { res[i] = v1 OPERATOR v2[i]; }                             \
  return std::move(res);                                                                          \
}

#define VEC_OPERATIONS2(NAME, OPERATOR)                                                           \
template<class _T1_, class _T2_>                                                                  \
std::vector< _T1_ >& NAME(std::vector< _T1_ >& v1, const std::vector< _T2_ >& v2) {               \
  if (v1.size() != v2.size()) throw std::invalid_argument("v1 and v2 have different dimensions"); \
  for (int i = 0; i < v1.size(); i++) { v1[i] OPERATOR v2[i]; }                                   \
  return v1;                                                                                      \
}                                                                                                 \
template<class _T1_, class _T2_>                                                                  \
std::vector< _T1_ >& NAME(std::vector< _T1_ >& v1, const _T2_& v2) {                              \
  for (int i = 0; i < v1.size(); i++) { v1[i] OPERATOR v2; }                                      \
  return v1;                                                                                      \
}

VEC_OPERATIONS1(operator+, +);
VEC_OPERATIONS1(operator-, -);
VEC_OPERATIONS1(operator*, *);
VEC_OPERATIONS2(operator+=, +=);
VEC_OPERATIONS2(operator-=, -=);
VEC_OPERATIONS2(operator*=, *=);

template<class _T1_, class _T2_>
std::vector< _T1_ >& set_value(std::vector< _T1_ >& v1, const _T2_& v2) {
  for (int i = 0; i < v1.size(); i++) { v1[i] = v2; }
  return v1;
}


template<class _T1_, class _T2_, class _T3_>
_T1_ clamp(const _T1_& v1, const _T2_& v2, const _T3_& v3) {
  if (v1 < v2) return v2;
  if (v1 > v3) return v3;
  return v1;
}

template<class _T1_, class _T2_, class _T3_>
std::vector< _T1_ > clamp(const std::vector< _T1_ >& v1, const _T2_& v2, const _T3_& v3) {
  std::vector< _T1_ > res(v1.size());
  for (int i = 0; i < v1.size(); i++) {
    res[i] = clamp(v1[i], v2, v3);
  }
  return std::move(res);
}


template<class _T1_, class _T2_>
OPERATION_RESULT(*)
  operator,(const std::vector< _T1_ >& v1, const std::vector< _T2_ >& v2) {
    if (v1.size() != v2.size()) throw std::invalid_argument("v1 and v2 have different dimensions");
    OPERATION_RESULT(*) res = OPERATION_RESULT(*)();
    for (int i = 0; i < v1.size(); i++) { res += v1[i] * v2[i]; }
    return res;
  }

template<class T>
std::vector< decltype( -std::declval<T>() ) > operator-(const std::vector< T >& v) {
  std::vector< decltype( -std::declval<T>() ) > res(v.size());
  for (int i = 0; i < v.size(); i++) { res[i] = -v[i]; }
  return std::move(res);
}
}
}

namespace My {
template<class T>
inline std::ostream& operator << (std::ostream& os, const std::vector< T >& v) {
  os << "{";
  if (v.size()) {
    os << " " << v[0];
    for (int i = 1; i < v.size(); i++) { os << " ," << v[i]; }
  }
  os << " }";
  return os;
}
}

template<class T>
inline std::ostream& operator << (std::ostream& os, const std::vector< T >& v) {
  return My::operator<<(os, v);
}

#endif
