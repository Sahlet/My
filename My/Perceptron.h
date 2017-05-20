#ifndef __PERCEPTRON_HPP__
#define __PERCEPTRON_HPP__

#include <vector>
#include <list>
#include <ostream>
#include <istream>
#include <memory>
#include <stdexcept>

#include <My/matrix.h>
#include <My/VectorAlgebra.h>
#include <My/Serialization.h>

namespace My {

	class Perceptron {
		std::vector< matrix< double > > weights;

		struct flushable {
			std::vector< matrix< double > > weights_gradients;
			std::vector< std::vector< double > > outputs;
			int last_batch_size = 0;
			std::vector< matrix< double > > weights_gradients_accumulator;
			double speed = 1;
			double prev_global_error = 0, cur_global_error = 0;
		};
		std::unique_ptr< flushable > context;

		template< template< class > class Container, class T >
		inline static std::vector< T > to_vector(const Container< T >& container) throw (std::out_of_range) {
			auto size = container.size();
			std::vector< T > vec;
			vec.capacity(size);
			int i = 0;
			for (auto& obj : container) {
				if (i > size) throw std::out_of_range();
				vec.emplace_back(std::forward< T >(obj));
			}
			return std::move(vec);
		}

		void copy(const Perceptron& p);
		void move(Perceptron& p) noexcept;
	public:
		Perceptron(
			US inputs = 0,
			US outputs = 0,
			const std::vector< US >& hidden = std::vector< US >()
		) throw (std::invalid_argument);
		Perceptron(const Perceptron& p) { this->copy(p); }
		Perceptron(Perceptron&& p) noexcept { this->move(p); }
		~Perceptron();

		Perceptron& operator=(const Perceptron& p) { this->copy(p); }
		Perceptron& operator=(Perceptron&& p) {
			this->move(p);
			return *this;
		}

		int inputs_count() const;
		int outputs_count() const;

		std::vector< double > operator()(std::vector< double > input) const throw (std::invalid_argument);
		std::vector< double > forward_prop(std::vector< double > input) throw (std::invalid_argument);

		void flush();
		bool flushed() { return !context || !context->last_batch_size; }
		void release_buffer() {
		  context.reset();
		}

		//errors in back_prop method
		typedef std::vector< double > errors;

		//for building composite structures where this object is intermediate
		//-- parameter 'errors' is errors that passing to outputs of perceptron (back_prop method)
		//-- parameter 'flush' responds for changing weights: if flush is true - weights changes in the function; if flush is false - gradients of weights will accumulate
		//-- result of function is errors of perceptron inputs (vector of inputs errors = (SCALAR_PRODUCT((deltas next layer),weights(from input to next layer)) where input = 1:inputs_count))
		errors put_errors(errors e, bool flush = true) throw (std::runtime_error);

		struct pattern {
			std::vector< double > input;
			std::vector< double > output;

			pattern(std::pair< std::vector< double >, std::vector< double > > data) :
			  input(std::move(data.first)), output(std::move(data.second)) {}
			pattern(std::vector< double > input, std::vector< double > output) :
			  input(std::move(input)), output(std::move(output)) {}
		};

		//returns sum of ERROR values
		double back_prop(const std::vector< pattern >& patterns) throw (std::invalid_argument);

		//returns ERROR value
		double back_prop(const pattern& p) throw (std::invalid_argument) {
			return back_prop(std::vector< pattern >{ p });
		}

		void write_to_stream(std::ostream& os) const;
		static Perceptron from_stream(std::istream& is);
	};
}

inline std::ostream& operator & (std::ostream& os, const My::Perceptron& p) {
  p.write_to_stream(os);
  return os;
}
inline std::istream& operator & (std::istream& is, My::Perceptron& p) {
  p = std::move(My::Perceptron::from_stream(is));
  return is;
}
inline std::ostream& operator << (std::ostream& os, const My::Perceptron& p) {
  return os & p;
}
inline std::istream& operator >> (std::istream& is, My::Perceptron& p) {
  return is & p;
}

#endif
