//polinom_root_searching.h
//���������� ��������� ��������� � My\root_searching.h
#pragma once
#include<My\root_searching.h>

namespace My{


	template<class T>
	class polinom_to_func_adapter : public func_gives_d_dx<T>{
	public:
		polinom<T> P;
		polinom_to_func_adapter(const polinom<T>& P) : P(P){}
		virtual T get(const T& x) { return P(x); }
		virtual shared_ptr< func<T> > d_dx() const  {
			return shared_ptr< func<T> >( new polinom_to_func_adapter( P.d_dx()) );
		}

	};
	template<class T>
	shared_ptr< conditions<T> > conditions_cast(const conditions<T>* const &c, const method& m){
		struct for_good_range{
			//���� - �������� ��� ����� ������ � ������ ����������� � ���������� [a; b], ���������� �������� ���������� � ������ ���������� P;
			//����, ��� ������� ��������, ������ ��������������� �������� ����� � ��������� ��������� - �� ����� ��������� ������������ �������
			static pair<T, T> get_good_range(const polinom<T>& P1, const polinom<T>& P2,  pair<T, T> p){
				if ((P1(p.first) * P1(p.second)) > 0) throw exception("������ ������������ � conditions_cast");
				polinom<T> P = P1, d = P2;
				list< pair<T, T> > d_intervals = Sturm<T>::get_intervals(d, p.first, p.second, p.second - p.first);
				auto i = d_intervals.begin(), end = d_intervals.end();
				if (i != end){
					bool ch = false;//�������, ������ �� ������ ���������� P � ����-�� �� ����������� � d_intervals
					for (; i != end; i++){
						if ((P(i->first) * P(i->second)) <= 0){
							ch = true;
							p.first = i->first;
							p.second = i->second;
							break;
						}
					}
					if(ch){//���� ch - � [p.first; p.second] ���� ������ ���������� P � ������ ���������� d
						T avg, tmp1, tmp2;
						while (abs(p.second - p.first) > epsilon){
							avg = (p.first + p.second) / 2;
							if (((tmp1 = P(p.first)) * (tmp2 = P(avg))) < 0) p.second = avg;
							else if((tmp1 * tmp2) > 0) p.first = avg;
							else{//���������� �� ������
								if(tmp1 == 0) avg = p.first;
								p.first = avg; p.second = avg;
								break;
							}
							if (d(p.first) * d(p.second) > 0){//>= 0 �� ������� - ���� �� ���� ��������� ������ �����������, �� ��� �� ����� ������
								//������� - � [p.first; p.second] d �� ����� ������, � P ����� 1
								break;
							}
						}
					} else {//���������, � ����� �������������, ��� �� ������ � d_intervals, ��������� ������ ���������� P (���������� � d_intervals ���� �� ����������� ������ �����������)
						T b1 = p.first;
						for (i = d_intervals.begin(); i != end; i++){
							if ((P(p.first) * P(i->first)) <= 0) { p.first = b1; p.second = i->first; break; }
							else b1 = i->second;
						}
					}
				}
				return p;
			}
		};
		auto c_ = dynamic_cast<const conditions<T>* const>(c);
		shared_ptr< conditions<T> > res;
		if(!c_) return res;//throw bad_cast((string("there is not conditions< ") + typeid(T).name() + " > in c").c_str());
		if (m == method::non) return shared_ptr< conditions<T> >(new conditions<T>(*c));
		method identifer = c->get_method();
		if (m == method::Binar && (unsigned char)identifer >= (unsigned char)method::Binar) return shared_ptr< conditions<T> >(new conditions_for_Binar<T>(*c));
		if (m == identifer){
			if (m == method::Simple) return shared_ptr< conditions<T> >(new conditions_for_Simple<T>(*c, ((conditions_for_Simple<T>*)c)->q, ((conditions_for_Simple<T>*)c)->k));
			if (m == method::Newthon) return shared_ptr< conditions<T> >(new conditions_for_Newthon<T>(*c,((conditions_for_Newthon<T>*)c)->x0));
			if (m == method::Hord) return shared_ptr< conditions<T> >(new conditions_for_Hord<T>(*c, ((conditions_for_Hord<T>*)c)->c, ((conditions_for_Hord<T>*)c)->x0));
		}
		shared_ptr< conditions<T> > tmp_res(new conditions<T>(*c));
			auto adapter = dynamic_cast<polinom_to_func_adapter<T>*>(c->f.get());
			if (!adapter) return res;//throw bad_cast((string("c->f is not polinom_to_func_adapter<") + typeid(T).name() + ">*").c_str());
		polinom<T> P = adapter->P;
			//��������� �������1 - range ���� ������ ���� ������
			if (identifer == method::non){
					auto eps = abs(epsilon);
					auto n_pows = 0u;
					for (; eps < 1; eps *= 10, n_pows++);
				//������ ������������ ����� ��� ������� � ������
				auto max_length_for_range = epsilon * pow(10.0, n_pows + 1);
				auto l = Sturm<T>::get_intervals(P, c->range.first, c->range.second, max_length_for_range);
		if (!l.size()) return res;//throw bad_cast("there is no root in c->range");
				else tmp_res.reset(new conditions<T>(*l.begin(), c->f));//����� ������ �� �����������
			}
		if (m == method::Binar) return shared_ptr< conditions<T> >(new conditions_for_Binar<T>(*tmp_res.get()));
			//��������� �������2 - ������ ����������� �� range ���������� � �� ������ ����
			if ((unsigned char)identifer <= (unsigned char)method::Binar) tmp_res.reset(new conditions<T>(for_good_range::get_good_range(P, P.d_dx(), tmp_res->range), c->f));
			//��������� �������3 - ������ ����������� �� range ���������� � �� ������ ����
/*A*/		if (/*m != method::Simple (����� �������, ���� ����� ���� ����� ���������� �������� ��� f' � ������ Simple_iter (������ 'B')) &&*/ (unsigned char)identifer <= (unsigned char)method::Simple) tmp_res.reset(new conditions<T>(for_good_range::get_good_range(P, P.d_dx().d_dx(), tmp_res->range), c->f));
		//��� ������� x0
		switch (m){
		case method::Simple:{
/*B*/		/*���� f' �� ����� ����������� �� [a; b] (������ 'A')*/
			auto df = P.d_dx();
			T M_abs_df = abs(df(tmp_res->range.first)), m_abs_df = abs(df(tmp_res->range.second));
			if (M_abs_df < m_abs_df) swap(M_abs_df, m_abs_df);
			T k = df((tmp_res->range.first + tmp_res->range.second) * 0.5) > 0 ? 2 / (M_abs_df + m_abs_df) : - 2 / (M_abs_df + m_abs_df);
			T q = max(abs(1 - k * df(tmp_res->range.first)), abs(1 - k * df(tmp_res->range.second)));
			res.reset(new conditions_for_Simple<T>(*tmp_res.get(), q, k));
							}
			break;
		case method::Newthon:{
			auto ddf = P.d_dx().d_dx();
			//���. 26
			T x0 = (P(tmp_res->range.first) * ddf(tmp_res->range.first)) >= 0 ? tmp_res->range.first : tmp_res->range.second;
			res.reset(new conditions_for_Newthon<T>(*tmp_res.get(), x0));
							 }
			break;
		case method::Hord:{
			auto ddf = P.d_dx().d_dx();
			//���. 30
			T c = (P(tmp_res->range.first) * ddf(tmp_res->range.first)) > 0 ? tmp_res->range.first : tmp_res->range.second;//(1.19)
			T x0 = c == tmp_res->range.first ? tmp_res->range.second : tmp_res->range.first;//(1.20)
			res.reset(new conditions_for_Hord<T>(*tmp_res.get(), c, x0));
						  }
			break;
		default:
			break;
		}
		return res;
	}

}