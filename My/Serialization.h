#ifndef __SERIALIZATION_HPP__
#define __SERIALIZATION_HPP__

//Serialization.h
#include <vector>
#include <My/matrix.h>

namespace My {
namespace {

//-----------------------------------------------------------------------
//SERIALIZE
template< class T >
typename std::enable_if<std::is_fundamental<T>::value || std::is_enum<T>::value, std::ostream& >::type
  operator&(std::ostream& os, const T& v) {
    os.write((const char*)&v, sizeof(T));
    return os;
  }
template< class T >
typename std::enable_if<std::is_fundamental<T>::value || std::is_enum<T>::value, std::istream& >::type
  operator&(std::istream& is, T& v) {
    is.read((char*)&v, sizeof(T));
    return is;
  }
template< class T >
typename std::enable_if<std::is_compound<T>::value && !std::is_enum<T>::value, std::ostream& >::type
  operator&(std::ostream& os, const T& obj);
template< class T >
typename std::enable_if<std::is_compound<T>::value && !std::is_enum<T>::value, std::istream& >::type
  operator&(std::istream& is, T& obj);

//-----------------------------------------------------------------------
//VECTOR SERIALIZE
template< class T >
void serialize(std::ostream& os, const std::vector< T >& v) {
  os & v.size();
  for (auto& obj : v) { os & obj; }
}
template< class T >
void serialize(std::istream& is, std::vector< T >& v) {
  typename std::vector< T >::size_type size;
  is & size;
  v.resize(size);
  for (auto& obj : v) { is & obj; }
}

//-----------------------------------------------------------------------
//MATRIX SERIALIZE
template< class T >
void serialize(std::ostream& os, const matrix< T >& m) {
  os & m.width() & m.height();
  for (auto& obj : m) { os & obj; }
}
template< class T >
void serialize(std::istream& is, matrix< T >& m) {
  US w, h;
  is & w & h;
  m = matrix< T >(w, h);
  for (auto& obj : m) { is & obj; }
}

//-----------------------------------------------------------------------
//SERIALIZE
template< class T >
typename std::enable_if<std::is_compound<T>::value && !std::is_enum<T>::value, std::ostream& >::type
  operator&(std::ostream& os, const T& obj) {
    serialize(os, obj);
    return os;
  }
template< class T >
typename std::enable_if<std::is_compound<T>::value && !std::is_enum<T>::value, std::istream& >::type
  operator&(std::istream& is, T& obj) {
    serialize(is, obj);
    return is;
  }
}
}

#endif
