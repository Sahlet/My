#ifndef __GAMMANN_HPP__
#define __GAMMANN_HPP__

#include <My/Perceptron.h>

namespace My {

  struct GammaNN_fields;

  struct GammaNN {
    typedef std::vector< double > object;

    GammaNN(std::vector< object > data, int units = 1);

    object operator[](int index);

  private:
    std::unique_ptr< GammaNN_fields > fields;
  };

}

#endif
