//root_searching.h
//описывет интерфейс
#pragma once
#include<My\polinom.h>

using namespace std;
namespace My{
	typedef long double T;

extern "C++"{
	#pragma region описания, что требуют определения
	//есть функция одной переменной
	template<class X> struct func{
		virtual X get(const X& x) = 0;
	};

	//есть уравнение func(x) = 0, x є [a; b]
	//надо найти корень итерационным методом
	
	//получить результат в таком виде
	template<class T>
	struct result{
		//количество итераций
		unsigned long long n_iter;
		//погрешность
		T delta;
		//значение корня с погрешностью delta
		T root;
		//исходный промежуток поиска
		pair< T, T > range;
		result(const unsigned long long& n_iter, const T& delta, const T& root, const pair< T, T >& range) : n_iter(n_iter), delta(delta), root(root), range(range){}
	};

	//есть функции, которые могут дать производную
	template<class X>
	struct func_gives_d_dx : func<X>{
		virtual shared_ptr< func<T> > d_dx() const = 0;
	};

	enum class method : unsigned char{non, Binar, Simple, Newthon, Hord};
	template<class T>
	class conditions;

	template<class T>
	extern shared_ptr< result<T> > Binar_iter(const conditions<T>* const &c);
	template<class T>
	extern shared_ptr< result<T> > Simple_iter(const conditions<T>* const &c);
	template<class T>
	extern shared_ptr< result<T> > Newthon_iter(const conditions<T>* const &c);
	template<class T>
	extern shared_ptr< result<T> > Hord_iter(const conditions<T>* const &c);

	template<class T>
	//условие использования : для каждой из итеративных методов, на [a; b] P неприрывный (что выполняется для любого [a; b], так как Р - полином), на [a; b] Р имеет один корень.
	//для всех итер функций (кроме бинарной) на [a; b] первоя и вторая производные от Р должны сохранять знак.
	extern shared_ptr< conditions<T> > conditions_cast(const conditions<T>* const &c, const method& m);

	//с точностью
	extern T epsilon;
	#pragma endregion
}

	template<class T>
	class conditions_for_Binar : public conditions<T>{
	protected:
		friend shared_ptr< conditions<T> > conditions_cast<T>(const conditions<T>* const &c, const method& m);
		//friend shared_ptr< result<T> > Binar_iter<T>(const conditions<T>* const &c);
		conditions_for_Binar(const conditions& c) : conditions(c){}
	public:
		virtual method get_method() const override{
			return method::Binar;
		}
	};
	template<class T>
	class conditions_for_Simple : public conditions_for_Binar<T>{
		//f є C1[range]
		friend shared_ptr< conditions<T> > conditions_cast<T>(const conditions<T>* const &c, const method& m);
		//friend shared_ptr< result<T> > Simple_iter<T>(const conditions<T>* const &c);
		conditions_for_Simple(const conditions& c, const T& q, const T& k) : conditions_for_Binar<T>(c), q(q), k(k){}
	public:
		const T q;
		const T k;
		virtual method get_method() const override{
			return method::Simple;
		}
	};
	template<class T>
	class conditions_for_Newthon : public conditions_for_Binar<T>{
		//f є C2[range]
		friend shared_ptr< conditions<T> > conditions_cast<T>(const conditions<T>* const &c, const method& m);
		//friend shared_ptr< result<T> > Newthon_iter<T>(const conditions<T>* const &c);
		conditions_for_Newthon(const conditions& c, const T& x0) : conditions_for_Binar<T>(c), x0(x0){}
	public:
		const T x0;
		virtual method get_method() const override{
			return method::Newthon;
		}
	};
	template<class T>
	class conditions_for_Hord : public conditions_for_Binar<T>{
		//f є C2[range]
		friend shared_ptr< conditions<T> > conditions_cast<T>(const conditions<T>* const &c, const method& m);
		//friend shared_ptr< result<T> > Hord_iter<T>(const conditions<T>* const &c);
		conditions_for_Hord(const conditions& c_, const T& c, const T& x0) : conditions_for_Binar<T>(c_), c(c), x0(x0){}
	public:
		const T c, x0;
		virtual method get_method() const override{
			return method::Hord;
		}
	};
	template<class T>
	class conditions{
		public:
		//friend shared_ptr< conditions<T> > conditions_cast<T>(const conditions<T>* const &c, const method& m);
		//промежуток, на котором уравнение f(x) = 0; x є range
		const pair<T, T> range;
		//f є C[range]
		const shared_ptr< func<T> > f;
		conditions(const pair<T, T>& range, const shared_ptr< func<T> >& f) : range(min(range.first, range.second), max(range.first, range.second)), f(f){}
		virtual method get_method() const{
			return method::non;
		}
	};

	struct for_iter_methods{

	static string to_string(const method& m){
		switch (m){
			case method::Binar	: return "Binar";
			case method::Simple	: return "Simple";
			case method::Newthon: return "Newthon";
			case method::Hord	: return "Hord";
			default: return "";
		}
	}
	template<class Condit>
	static method get_method(){
		if (typeid(Condit) == typeid(conditions<T>)) return method::non;
		if (typeid(Condit) == typeid(conditions_for_Binar<T>)) return method::Binar;
		if (typeid(Condit) == typeid(conditions_for_Simple<T>)) return method::Simple;
		if (typeid(Condit) == typeid(conditions_for_Newthon<T>)) return method::Newthon;
		if (typeid(Condit) == typeid(conditions_for_Hord<T>)) return method::Hord;
		throw bad_typeid((string("there is no conrition ") + typeid(Condit).name()).c_str());
	}

	template<class Condit>
	static shared_ptr< conditions<T> > do_the_same_for_all_funcs(const conditions<T>* const &c, shared_ptr< result<T> > &res){
		auto u_c = conditions_cast<T>(c, get_method<Condit>());
		if (u_c && ((u_c->range.second - u_c->range.first) <= abs (epsilon))){
			res.reset(new result<T>(0, epsilon, (u_c->range.second + u_c->range.first) * 0.5, u_c->range));
			return nullptr;
		}
		return u_c;
	}


	//эти функции определяют соответствующие шаблоны типа T = long double
	#pragma region iter
	static shared_ptr< result<T> > Binar_iter(const conditions<T>* const &c){
		shared_ptr< result<T> > res;
		auto u_c_b = do_the_same_for_all_funcs< conditions_for_Binar<T> >(c, res);
		if (!u_c_b) return res;
		auto c_b = dynamic_cast<conditions_for_Binar<T>*>(u_c_b.get());
		unsigned long long i = 0;
		T a = c_b->range.first, b = c_b->range.second, avg = (a + b) / 2, tmp1, tmp2;
		while (abs(b - a) > epsilon){
			i++;
			avg = (a + b) / 2;
			if (((tmp1 = c_b->f->get(a)) * (tmp2 = c_b->f->get(avg))) < 0) b = avg;
			else if((tmp1 * tmp2) > 0) a = avg;
			else{//наткнулись на корень
				if(tmp1 == 0) avg = a;
				break;
			}
		}
		res.reset(new result<T>(i, epsilon, avg, c_b->range));
		return res;
	}
	static shared_ptr< result<T> > Simple_iter(const conditions<T>* const &c){
		shared_ptr< My::result<T> > res;
		auto u_c_s = do_the_same_for_all_funcs< conditions_for_Simple<T> >(c, res);
		if (!u_c_s) return res;
		auto c_s = dynamic_cast<conditions_for_Simple<T>*>(u_c_s.get());
		T x_ = (c_s->range.second + u_c_s->range.first) * 0.5, k = c_s->k, x = x_ - k * c_s->f->get(x_);
		unsigned long long i = 1;
		for (T q = (1 - c_s->q) * epsilon / c_s->q; (abs(x - x_)) > q; i++){
			x_ = x;
			x = x_ - k * c_s->f->get(x_);
		}
		res.reset(new result<T>(i, abs(x - x_) * c_s->q / (1 - c_s->q), x, c_s->range));
		return res;
	}

	/*//след функции кидает исключение, если итер процесс расходится
	//это связано с тем, что не просто реализовать поиск максимума модуля производной функции итерационного процесса для этих методов
	//(лучше не думать про это) (смотри Бойко стр. 26)*/

	//если итерационный процесс расходится - вызовет себя рекурсивно с зауженым интервалом в conditions*
	static shared_ptr< result<T> > Newthon_iter(const conditions<T>* const &c) /*throw (exception)*/{
		//напоминалка : из-за того, что вторая производная не меняет знак - цикличных переходов не будет
		shared_ptr< result<T> > res;
		auto u_c_n = do_the_same_for_all_funcs< conditions_for_Newthon<T> >(c, res);
		if (!u_c_n) return res;
		auto c_n = dynamic_cast<conditions_for_Newthon<T>*>(u_c_n.get());
		auto f = c_n->f, df_dx = ((func_gives_d_dx<T>*)c_n->f.get())->d_dx();
		T a = c_n->range.first, b = c_n->range.second,
			x_ = c_n->x0, x = x_ - f->get(x_) / df_dx->get(x_);
		unsigned long long i = 1;
		T min_abs_df_dx = min(abs(df_dx->get(a)), abs(df_dx->get(b)));//так можно (екстремума df_dx в [a; b] нет по условию)
		T f_x = f->get(x);
		for (; /*(abs(x - x_)) > epsilon * abs(f_x) / min_abs_df_dx*/ abs(f_x) / min_abs_df_dx > epsilon; i++){
			#pragma region обработка расходимости
			if (x > b || x < a){
				//throw exception("Newthon method failed with looking for root");//сообщение о расхождении ряда x_n
				pair<T, T> new_range;
				(f->get((b + a) * 0.5) * f->get(a)) > 0 ?
					new_range = pair<T, T>((b + a) * 0.5, b) :
					new_range = pair<T, T>(a, (b + a) * 0.5) ;
				shared_ptr< conditions<T> > c_2(new conditions<T>(new_range, f));//условия с новым промежутком
				auto c_n_2 = conditions_cast<T>(c_2.get(), method::Newthon);//получили нвые условия
				return Newthon_iter(c_n_2.get());
			}
			#pragma endregion
			x_ = x;
			x = x_ - f_x / df_dx->get(x_);
			f_x = f->get(x);
		}
		res.reset(new result<T>(i, abs(f_x) / min_abs_df_dx, x, c_n->range));
		return res;
	}
	//если итерационный процесс расходится - вызовет себя рекурсивно с зауженым интервалом в conditions*
	static shared_ptr< result<T> > Hord_iter(const conditions<T>* const &c) /*throw (exception)*/{
		shared_ptr< result<T> > res;
		auto u_c_h = do_the_same_for_all_funcs< conditions_for_Hord<T> >(c, res);
		if (!u_c_h) return res;
		auto c_h = dynamic_cast<conditions_for_Hord<T>*>(u_c_h.get());
		auto f = c_h->f, df_dx = ((func_gives_d_dx<T>*)c_h->f.get())->d_dx();
		T a = c_h->range.first, b = c_h->range.second,
			c_ = c_h->c,
			f_c = f->get(c_),
			x_ = c_h->x0,
			x = x_ - f->get(x_) * (x_ - c_) / (f->get(x_) - f_c);
		unsigned long long i = 1;
		T min_abs_df_dx = min(abs(df_dx->get(a)), abs(df_dx->get(b)));//так можно (екстремума df_dx в [a; b] нет по условию)
		T f_x = f->get(x);
		for (; /*(abs(x - x_)) > epsilon * abs(f_x) / min_abs_df_dx*/ abs(f_x) / min_abs_df_dx > epsilon; i++){
			#pragma region обработка расходимости
			if (x > b || x < a){
				//throw exception("Hord method failed with looking for root");//сообщение о расхождении ряда x_n
				pair<T, T> new_range;
				(f->get((b + a) * 0.5) * f->get(a)) > 0 ?
					new_range = pair<T, T>((b + a) * 0.5, b) :
					new_range = pair<T, T>(a, (b + a) * 0.5) ;
				shared_ptr< conditions<T> > c_2(new conditions<T>(new_range, f));//условия с новым промежутком
				auto c_h_2 = conditions_cast<T>(c_2.get(), method::Hord);//получили нвые условия
				return Hord_iter(c_h_2.get());
			}
			#pragma endregion
			x_ = x;
			x = x_ - f_x * (x_ - c_) / (f_x - f_c);
			f_x = f->get(x);
		}
		res.reset(new result<T>(i, abs(f_x) / min_abs_df_dx, x, c_h->range));
		return res;
	}
	#pragma endregion 
	//
	};

	inline string to_string(const method& m){
		return for_iter_methods::to_string(m);
	}
}