#ifndef __VECTOR_ALGEBRA_HPP__
#define __VECTOR_ALGEBRA_HPP__

//VectorAlgebra.h
#include <vector>

namespace My {
namespace {

#define OPERATION_RESULT(OPERATOR) decltype( std::declval<_T1_>() OPERATOR std::declval<_T2_>() )

#define VEC_OPERATIONS(NAME, OPERATOR)                                                            \
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
}                                                                                                 \

VEC_OPERATIONS(operator+, +);
VEC_OPERATIONS(operator-, -);
VEC_OPERATIONS(operator*, *);





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

#endif
