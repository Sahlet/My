#ifndef __GAMMANN_HPP__
#define __GAMMANN_HPP__

#include <My/Perceptron.h>

namespace My {

  struct GammaNN_members;

  struct GammaNN {
    typedef std::vector< double > object;

    //object_dimention is data width
    GammaNN(
      My::matrix< double > data = My::matrix< double >(1, 1),
      const std::vector< US >& hidden = std::vector< US >(),
      US units = 1
    ) throw (std::invalid_argument);

    US get_object_dimention();
    US get_units_number();

    object operator[](int index);

    void write_to_stream(std::ostream& os) const;
    static GammaNN from_stream(std::istream& is);

  private:
    std::unique_ptr< GammaNN_members > members;
  };

}

inline std::ostream& operator & (std::ostream& os, const My::GammaNN& g) {
  g.write_to_stream(os);
  return os;
}
inline std::istream& operator & (std::istream& is, My::GammaNN& g) {
  g = std::move(My::GammaNN::from_stream(is));
  return is;
}
inline std::ostream& operator << (std::ostream& os, const My::GammaNN& g) {
  return os & g;
}
inline std::istream& operator >> (std::istream& is, My::GammaNN& g) {
  return is & g;
}

#endif
