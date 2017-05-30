//GammaNN.cpp

#ifndef __GAMMANN_HPP__
#define __GAMMANN_HPP__

#include <My/Perceptron.h>

/*
 *
 * GammaNN made for time series prediction
 *
 */

namespace My {

  typedef unsigned int UI;

  struct GammaNN_members;

  struct GammaNN_members_deleter {
    void operator()(GammaNN_members *ptr);
  };

  struct GammaNN {
    typedef std::vector< double > object;

    //object_dimention is data width
    //trace_size defines object count which perceptron can see
    GammaNN(
      My::matrix< double > data = My::matrix< double >(1, 1),
      const std::vector< US >& hidden = std::vector< US >(),
      US units = 1,
      US trace_size = 1
    ) throw (std::invalid_argument);

    GammaNN(const GammaNN&);
    GammaNN(GammaNN&&);
    GammaNN& operator=(const GammaNN&);
    GammaNN& operator=(GammaNN&&);

    US get_object_dimention();
    US get_units_number();
    US get_trace_size();
    US get_min_learn_pattern();
    US get_series_size();
    US get_src_series_size();
    std::vector< std::string > get_col_names();
    void set_col_names(std::vector< std::string > names) throw (std::invalid_argument);

    //returns sum of ERROR values
    double learn(const std::vector< UI >& patterns) throw (std::range_error);

    //returns ERROR value
    double learn(const UI& p) throw (std::range_error) {
      return learn({ p });
    }

    void clear_learning();

    object operator[](UI index);

    void write_to_stream(std::ostream& os) const;
    static GammaNN from_stream(std::istream& is);

  private:
    std::unique_ptr< GammaNN_members, GammaNN_members_deleter > members;
  };

}

// inline std::ostream& operator & (std::ostream& os, const My::GammaNN& g) {
//   g.write_to_stream(os);
//   return os;
// }
// inline std::istream& operator & (std::istream& is, My::GammaNN& g) {
//   g = std::move(My::GammaNN::from_stream(is));
//   return is;
// }
// inline std::ostream& operator << (std::ostream& os, const My::GammaNN& g) {
//   return os & g;
// }
// inline std::istream& operator >> (std::istream& is, My::GammaNN& g) {
//   return is & g;
// }

#endif
