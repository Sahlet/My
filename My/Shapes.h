//Shapes.h
#pragma once
#include<map>
#include<vector>
#include<list>
#include<memory>
#include<functional>
#include<string>
#include<gcroot.h>


namespace My{
using namespace std;
using namespace System::Drawing;
using namespace System::Threading;
typedef long double T;

struct point{
	T x, y;
	point(const T& x = 0, const T& y = 0) : x(x), y(y){}
	operator vector<T>(){
		vector<T> res(2);
		res[0] = x;
		res[1] = y;
		return res;
	}
	point operator + (const point& r) const { return point(this->x + r.x, this->y + r.y); }
	point operator - () const { return point(-this->x, -this->y); }
	point operator - (const point& r) const { return (*this) + -r; }
	bool operator==(const point& p) const {
		return x == p.x && y == p.y;
	}
	bool operator!=(const point& p) const {
		return !(*this == p);
	}
};
point operator*(const T& k, const point& p){
	return point(p.x * k, p.y * k);
}
point operator*(const point& p, const T& k){
	return k * p;
}


struct Pixel{
	unsigned char blue;
	unsigned char green;
	unsigned char red;
	unsigned char alpha;
	Pixel(unsigned char blue = 0, unsigned char green = 0, unsigned char red = 0, unsigned char alpha = 0) : blue(blue), green(green), red(red), alpha(alpha) {}
	Pixel(const Color& c){
		auto tmp_bitmap = gcnew Bitmap(1, 1);
		tmp_bitmap->SetPixel(0, 0, c);
		auto tmp_bitmapData = tmp_bitmap->LockBits(Rectangle(0, 0, 1, 1), Imaging::ImageLockMode::ReadOnly, tmp_bitmap->PixelFormat);
		*this = *(Pixel*)(void*)tmp_bitmapData->Scan0;
		tmp_bitmap->UnlockBits(tmp_bitmapData);
	}
	operator Color(){
		auto tmp_bitmap = gcnew Bitmap(1, 1);
		auto tmp_bitmapData = tmp_bitmap->LockBits(Rectangle(0, 0, 1, 1), Imaging::ImageLockMode::ReadOnly, tmp_bitmap->PixelFormat);
		*(Pixel*)(void*)tmp_bitmapData->Scan0 = *this;
		tmp_bitmap->UnlockBits(tmp_bitmapData);
		return tmp_bitmap->GetPixel(0, 0);
	}
	bool empty()const{return blue == 0 && green == 0 && red == 0 && alpha == 0;}
	operator bool()const{return !empty();}
	Pixel operator+(const Pixel& p)const{
		return Pixel(*this) += p;
	}
	Pixel& operator+=(const Pixel& p){
		/*this->blue = ((int)this->blue + p.blue) / 2 + ((int)this->blue + p.blue) % 2;
		this->green = ((int)this->green + p.green) / 2 + ((int)this->green + p.green) % 2;
		this->red= ((int)this->red + p.red) / 2 + ((int)this->red + p.red) % 2;
		this->alpha = ((int)this->alpha + p.alpha) / 2 + ((int)this->alpha + p.alpha) % 2;*/

		this->blue = this->blue + p.blue;
		this->green = this->green + p.green;
		this->red = this->red + p.red;
		this->alpha = this->alpha + p.alpha;

//		uint32 ALPHABLEND_PM(uint32 dst, uint32 src)
//{
//    uint not_a = 256 - ALPHA(src);
//    return = src + (((not_a * BLUEx256(dst))>>16) |
//                   (((not_a * GREENx256(dst))>>8) & 0xff00) |
//                   (((not_a * REDx256(dst))) & 0xff0000) |
//                   (((not_a * ALPHAx256(dst))<<8) & 0xff000000));
//}

//		uint32 ALPHABLEND_PM(uint32 dst, uint32 src)
//{
//    uint8 not_a = 255 - ALPHA(src);
//    return src + ((multbl[not_a][dst & 0xff]) |
//                (((uint)multbl[not_a][(dst >> 8) & 0xff]) << 8) |
//                (((uint)multbl[not_a][(dst >> 16) & 0xff]) << 16) |
//                (((uint)multbl[not_a][(dst >> 24) & 0xff]) << 24));
//}

		//unsigned char ba = p.alpha;
		//if (ba == 0) return *this;				
		//float a = (float)((double)(ba)* (1.0 / 255.0));
		//float not_a = 1.0f - a;
		//this->blue = (float(this->blue) * not_a) + p.blue;
		//this->green = (float(this->green) * not_a) + p.green;
		//this->red = (float(this->red) * not_a) + p.red;
		//this->alpha = (float(this->alpha) * not_a) + p.alpha;

		/*unsigned int& res = *(unsigned int*)(this);
		unsigned char not_a = 255 - p.alpha;
		res = *(unsigned *)(&p) + ((not_a*res & 0xff) |
                ((not_a*(res >> 8) & 0xff) << 8) |
                ((not_a*(res >> 16) & 0xff) << 16) |
                ((not_a*(res >> 24) & 0xff) << 24));
		*/

		return *this;
	}
	bool operator==(const Pixel& p) const {
		return this->blue == p.blue && this->green == p.green && this->red == p.red && this->alpha == p.alpha;
	}
	bool operator!=(const Pixel& p) const {
		return !(*this == p);
	}

	/*Pixel& operator = (const Pixel& p){
		this->blue = p.blue;
		this->green = p.green;
		this->red = p.red;
		this->alpha = p.alpha;
	}*/
};

T zero(const T& time){return 0;}

struct rectangle{
	point west_north_corner;
	T w, h;
	rectangle(const point& p, const T& w, const T& h) : west_north_corner(p), w(w), h(h){}
};
struct screen{
	T w, h;
	screen(const T& w, const T& h) : w(abs(w)), h(abs(h)){}
	operator bool()const{
		return w && h;
	}
};
point back_to_rectangle(const rectangle& rect, const point& p){
	static const auto signum = [](const T& t) -> T{
		if(t == 0) return 0;
		return t > 0 ? 1 : -1;
	};
	static const auto trunc = [](const T& t) -> T{
		return t - fmodl(t, 1);
	};
	point center(rect.west_north_corner.x + rect.w / 2, rect.west_north_corner.y + rect.h / 2);
	auto tmp = p - center;
	return center + point(tmp.x - rect.w * (signum(trunc(tmp.x / (rect.w / 2))) - trunc((tmp.x - signum(tmp.x) * (rect.w / 2)) / (rect.w))),
			  tmp.y - rect.h * (signum(trunc(tmp.y / (rect.h / 2))) - trunc((tmp.y - signum(tmp.y) * (rect.h / 2)) / (rect.h))));
}
point back_to_screan(const screen& rect, const point& p){
	static const auto signum = [](const T& t) -> T{
		if(t == 0) return 0;
		return t > 0 ? 1 : -1;
	};
	static const auto trunc = [](const T& t) -> T{
		return t - fmodl(t, 1);
	};
	return point(p.x - rect.w * (signum(trunc(p.x / (rect.w / 2))) + trunc((p.x - signum(p.x) * (rect.w / 2)) / (rect.w))),
			  p.y - rect.h * (signum(trunc(p.y / (rect.h / 2))) + trunc((p.y - signum(p.y) * (rect.h / 2)) / (rect.h))));
}

class shape2D{
public:
	Pixel color;
	function<bool (const point& p)> range;
	function<T (const T& time)> x, y, corner;
	//центр
	point centre;
private:
	point new_centre;
	T last_time, cs, sn;
public:
	static const class Range_count{
		T range_count;
	public:
		Range_count(const T& range_count = 20) : range_count(range_count){
			if (range_count > 100) this->range_count = 100;
			else if(range_count < 10) this->range_count = 10;
			else this->range_count = range_count;
		}
		Range_count& operator = (const T& range_count){
			if (range_count > 100) this->range_count = 100;
			else if(range_count < 10) this->range_count = 10;
			else this->range_count = range_count;
			return *this;
		}
		operator T() const {return range_count;}
	} range_count;
	//k - коэффициент линейной пропорции к исходным размерам фигуры (обязательно в промежутке [0; 1])
	class K{
		T k;
	public:
		K(const T& k_ = 1) : k(k_){
			if (k > 1) this->k = 1;
			else if(k < 0) this->k = 0;
			else this->k = k;
		}
		K& operator = (const T& k_){
			if (k_ > 1) k = 1;
			else if(k_ < 0) k = 0;
			else k = k_;
			return *this;
		}
		operator T() const {return k;}
	} k;
	shape2D(
		const Pixel& color,
		function<bool (const point& p)> range,
		function<T (const T& time)> x = zero,
		function<T (const T& time)> y = zero,
		function<T (const T& time)> corner = zero, const T& k = 1)
		throw(exception)
		: color(color), range(range), x(x), y(y), corner(corner), k(k), last_time(numeric_limits<T>::signaling_NaN())
	{
		const size_t resolution = 100;
		point C;
		size_t n_points = 0;
		auto eps_div_2 = (range_count) / resolution;
		for (size_t i = 0; i < resolution; i++){
			for (size_t j = 0; j < resolution; j++){
				if (this->range(point(eps_div_2 + (2 * range_count * i) / resolution - range_count, eps_div_2 + (2 * range_count * j) / resolution - range_count))){
					n_points++;
					C.x += eps_div_2 + (2 * range_count * i) / resolution - range_count;
					C.y += eps_div_2 + (2 * range_count * j) / resolution - range_count;
				}
			}
		}
		if (n_points) this->centre = point(C.x / n_points, C.y / n_points);
		else throw exception(("Не удалось найти центр фигуры. Возмжные причины:\n____ фигура не входит в каваднат " + to_string(2*range_count) + "X" + to_string(2*range_count) + " с центром в начале координат;\n____ фигура слишком маленькая").c_str());
	}
	//scr - экран, куда отрображается фигура
	unique_ptr<Pixel> get_color(point p, const screen& scr, T time){
		#pragma region перенос
		if (time != last_time) new_centre = point(x(time), y(time));
		p = back_to_screan(scr, p - (new_centre - centre));
		#pragma endregion
		#pragma region поворот
		if (time != last_time){
			auto alpha = -corner(time);
			cs = cos(alpha);
			sn = sin(alpha);
		}
		p = p - centre;
		p = centre + point(cs * p.x - sn * p.y, sn * p.x + cs * p.y);
		#pragma endregion
		#pragma region "коэффициент линейной пропорции к исходным размерам фигуры"
		p = (T(1)/T(k))*(p - centre) + centre;
		#pragma endregion
		if (time != last_time) last_time = time;

		if(p.y >= -range_count && p.y < range_count && p.x >= -range_count && p.x < range_count && range(p))
			return unique_ptr<Pixel>(new Pixel(color));
		return nullptr;
	}
	Pixel get_color() const { return color; }
	void set_color(const Pixel& p) { color = p; }
	point get_centre()const { return centre; };
};
const shape2D::Range_count shape2D::range_count = 80;

template<class T>
class Singleton{
protected:
	static shared_ptr<T> ptr;
	static bool was_del;
	Singleton();
	Singleton(const Singleton& s);
	Singleton& operator = (const Singleton& s);
public:
	static T& get_ref(){
		if (was_del) throw exception();
		if (ptr) return *ptr;
		ptr.reset(new T());
		return *ptr;
	}
	static shared_ptr<T> get_ptr(){
		if (was_del) throw exception();
		if (ptr) return ptr;
		ptr.reset(new T());
		return ptr;
	}
	static void del(){
		was_del = true;
		ptr = shared_ptr<T>(nullptr);
	}
};

class bitmap{
	unique_ptr<Pixel, default_delete<Pixel[]> > ptr;
	size_t w, h;
	gcroot<Bitmap^> bits;
	gcroot<Imaging::BitmapData^> data;
	void del(){
		if (data.operator System::Drawing::Imaging::BitmapData ^() != nullptr){
			bits->UnlockBits(data);
			data = nullptr;
			bits = nullptr;
		}
	}
public:
	bitmap(size_t w, size_t h) : w(w), h(h), ptr(new Pixel[w * h]){
		for (Pixel* i = ptr.get(), *end = i + w * h; i != end; i++) *i = Pixel();
	}
	bitmap(bitmap&& obj) : ptr(std::move(obj.ptr)), w(obj.w), h(obj.h), bits(obj.bits), data(obj.data){
		obj.data = nullptr;
		obj.bits = nullptr;
	}
	bitmap(const bitmap& obj) : w(obj.w), h(obj.h){
		ptr.reset(new Pixel[w * h]);
		if (obj.ptr)for (Pixel* i = obj.ptr.get(), *end = i + w * h, *res_ptr = ptr.get(); i != end; i++, res_ptr++) *res_ptr = *i;
		else for (Pixel* i = (Pixel*)((void*)obj.data->Scan0), *end = i + w * h, *res_ptr = ptr.get(); i != end; i++, res_ptr++) *res_ptr = *i;
	}
	Pixel& operator()(const size_t& x, const size_t& y){
		if (x >= w || y >= h) throw range_error("out of range");
		if (ptr) return ptr.get()[x + w * y];
		return *((Pixel*)(void*)data->Scan0 + x + w * y);
	}
	const Pixel& operator()(const size_t& x, const size_t& y) const { return const_cast<bitmap* const>(this)->operator()(x, y); }
	operator Bitmap^() const {
		if (!w || !h) return nullptr;
		Bitmap^ res = gcnew Bitmap(w, h);
		auto tmp_bitmapData = res->LockBits(Rectangle(0, 0, w, h), Imaging::ImageLockMode::WriteOnly, res->PixelFormat);
		if (ptr) for (Pixel* i = ptr.get(), *end = i + w * h, *res_ptr = (Pixel*)(void*)tmp_bitmapData->Scan0; i != end; i++, res_ptr++) *res_ptr = *i;
		else for (Pixel* i = (Pixel*)(void*)data->Scan0, *end = i + w * h, *res_ptr = (Pixel*)(void*)tmp_bitmapData->Scan0; i != end; i++, res_ptr++) *res_ptr = *i;
		res->UnlockBits(tmp_bitmapData);
		return res;
	}
	size_t get_w() const {return w;}
	size_t get_h() const {return h;}
	bool can_move_to_Bitmap() const {return bool(!ptr);}
	bitmap& operator = (const bitmap& obj){
		del();
		this->w = obj.w; this->h = obj.h;
		ptr.reset(new Pixel[w * h]);
		if (obj.ptr)for (Pixel* i = obj.ptr.get(), *end = i + w * h, *res_ptr = ptr.get(); i != end; i++, res_ptr++) *res_ptr = *i;
		else for (Pixel* i = (Pixel*)((void*)obj.data->Scan0), *end = i + w * h, *res_ptr = ptr.get(); i != end; i++, res_ptr++) *res_ptr = *i;
		return *this;
	}
	Bitmap^ move_to_Bitmap(){
		this->w = 0;
		this->h = 0;
		auto res = bits;
		this->del();
		return res;
	}
	bitmap(Bitmap^ b) : w(b->Width), h(b->Height), bits(b), data(b->LockBits(Rectangle(0, 0, w, h), Imaging::ImageLockMode::WriteOnly, b->PixelFormat)){}
	~bitmap(){
		del();
	}
};
	struct record{
		record(const shape2D& shape) : shape(shape), start_shape(shape), for_tail(), write_tail(), show_tail(), show_shape(true){}
		void to_start_parameters(){
			set_shape(start_shape);
			set_tail(false);
			set_show_shape(true);
		}
		void set_tail(const bool& b){
			if (b != (bool)for_tail){
				if (b){
					write_tail = true;
					//for_tail.reset(new map<long long, map<long long, Pixel> >());
					for_tail.reset(new map<T, shape2D >());
				} else {
					write_tail = false; show_tail = false;
					//for_tail.reset< map<long long, map<long long, Pixel> > >(nullptr);
					for_tail.reset< map<T, shape2D > >(nullptr);
				}
			}
		}
		void set_shape(const shape2D& shape){
			this->shape = shape;
		}
		void set_write_tail(const bool& b){
			if (b != write_tail){
				if (b){
					if (!for_tail) set_tail(true);
					write_tail = true;
				} else write_tail = false;
			}
		}
		void set_show_tail(const bool& b){
			if (b != show_tail){
				if (b){
					if (!for_tail) set_tail(true);
					show_tail = true;
				} else show_tail = false;
			}
		}
		void set_show_shape(const bool& b){
			show_shape = b;
			if (!show_shape) set_write_tail(false);
		}
		bool get_tail()const{
			return for_tail;
		}
		T get_k()const{
			return shape.k;
		}
		shape2D& get_shape(){
			return shape;
		}
		bool get_write_tail()const{
			return write_tail;
		}
		bool get_show_tail()const{
			return show_tail;
		}
		bool get_show_shape()const{
				return show_shape;
		}
		Pixel get_color(){return shape.get_color();}
		/*Pixel get_color(long long x, long long y, const screen& scr, const T& time){
			if (show_shape){
				if (get_tail()){
					Pixel colop_in_tail = (*for_tail).count(x) && (*for_tail)[x].count(y) ? (*for_tail)[x][y] : Pixel();
					if (colop_in_tail != shape.get_color()){
						auto color_ptr = shape.get_color(point(x, y), scr, time);
						if (color_ptr){
							if (write_tail) (*for_tail)[x][y] = *color_ptr;
							return *color_ptr;
						} else if (show_tail) return colop_in_tail;
						return Pixel();
					} else {
						if (show_tail) return colop_in_tail;
						else{
							auto color_ptr = shape.get_color(point(x, y), scr, time);
							return color_ptr ? *color_ptr : Pixel();
						}
					}
				} else {
					auto color_ptr = shape.get_color(point(x, y), scr, time);
					return color_ptr ? *color_ptr : Pixel();
				}
			} else if (show_tail) return (*for_tail).count(x) && (*for_tail)[x].count(y) ? (*for_tail)[x][y] : Pixel();
			return Pixel();
		}*/
		Pixel get_color(const long long& x, const long long& y, const screen& scr, const T& time){
			check_time(time);
			point p(x, y);
			const auto in_tail = get_Pixel_in_tail(p, scr);
			if (show_shape && in_tail != shape.color){
				auto color_ptr = shape.get_color(point(x, y), scr, time);
				if (color_ptr) return *color_ptr;
			}
			return in_tail;
		}

	private:
		inline void check_time(const T& time){
			static volatile bool check = false;
			while (check){}
			check = true;
			if(write_tail && !for_tail->count(time))
				for_tail->insert(pair<T, shape2D>(time, shape));
			check = false;
		}
		inline Pixel get_Pixel_in_tail(const point& p, const screen& scr){
			if (!show_tail) return Pixel();
			unique_ptr<Pixel> res;
			for (auto i = for_tail->rbegin(), end = for_tail->rend(); !res && i != end; i++){
				res = i->second.get_color(p, scr, i->first);
			}
			return res ? *res : Pixel();
		}
		shape2D shape, start_shape;
		bool write_tail, show_tail, show_shape;
		//shared_ptr< map<long long, map<long long, Pixel> > > for_tail;
		shared_ptr< map<T, shape2D> > for_tail;
	};
	
	extern void* CreateThread_(void(*func)(void*), void* arg);
	ref class for_void_ptr{
	public:
		void* ptr;
		for_void_ptr(void* const& ptr) : ptr(ptr) {}
	};
	class cmds_manager{
		list< function<void ()> >* cmds;
		//когда писал, еще не умел пользоваться примитивами синхронизации
		bool thread_out, thread_is_working, my_list_1, my_list_2;
		static void func(System::Object^ arg){
			cmds_manager *this_ = (cmds_manager *)((for_void_ptr^)arg)->ptr;
			this_->thread_is_working = true;
			while (!this_->thread_out){
				do{Thread::Sleep(1);}
				while (this_->my_list_2);
				this_->my_list_1 = true;
				if (this_->cmds->size()){
					(*this_->cmds->begin())();
					this_->cmds->pop_front();
				}
				this_->my_list_1 = false;
			}
			this_->thread_is_working = false;
		}
	public:
		cmds_manager() : cmds(new list< function<void ()> >()), thread_out(false), thread_is_working(false), my_list_1(false), my_list_2(false){
			for_void_ptr^ void_ptr = gcnew for_void_ptr((void*)this);
			Thread^ thread = gcnew Thread(gcnew ParameterizedThreadStart(func));
			thread->Start(void_ptr);
			//CreateThread_(func, (void*)this);
		}
		~cmds_manager(){
			thread_out = true;
			while (thread_is_working);
			delete cmds;
		}
		cmds_manager& operator << (function<void ()> func){
			my_list_2 = true;
			do{Thread::Sleep(1);}
			while (my_list_1);
			cmds->push_back( std::move(func) );
			my_list_2 = false;
			return *this;
		}

	};
class shape2D_container : protected map<string, record>{
	friend class Singleton<shape2D_container>;
private:
	mutable cmds_manager cmds;
	shape2D_container() : buf(cmds, *this), scr(cmds, *this){}
public:
	class bmp_buf;
	class screen_{
		friend class bmp_buf;
		screen scr;
		cmds_manager& cmds;
		shape2D_container& owner;
	public:
		screen_(cmds_manager& cmds, shape2D_container& owner) : cmds(cmds), owner(owner), scr(0, 0) {}
		screen_& operator=(const screen& scr){
			if (scr.h != this->scr.h || scr.h != this->scr.h)
				cmds << [this, scr]()->void {
					this->scr = scr;
					this->owner.buf.refresh_buf();
				};
			return *this;
		}
		operator screen(){
			volatile bool ch(true);
			cmds << [&ch]()->void{
				ch = false;
			};
			while(ch);
			return scr;
		}
	} scr;
	class bmp_buf{
		friend class screen_;
		friend class shape2D_container;
		cmds_manager& cmds;
		shape2D_container& owner;
		class bmp{
			bool ready, need_yet;
			gcroot<Bitmap^> val;
		public:
			bmp(const screen& scr, const T& time) : ready(false), need_yet(true){
				struct for_ready{
					bool& ready;
					for_ready(bool& ready) : ready(ready) {}
					~for_ready(){ready = true;}
				} for_ready_(ready);
				if(!scr) return;
				bitmap res(gcnew Bitmap(scr.w, scr.h));
				long long w = scr.w, h = scr.h, minus_w = -w, minus_h = -h;
				auto& container_ref = Singleton< shape2D_container >::get_ref();
				if (container_ref.size()){
					#pragma omp parallel for
					for (long long y = -h / 2; y < h / 2; y++){
						#pragma omp parallel for
						for (long long x = -w / 2; x < w / 2; x++){
							if(need_yet){
								Pixel& p = res(x + w / 2, (h - 1) - (y + h / 2));
								for (auto& i : container_ref)
									p += i.second.get_color(x , y, scr, time);
							}
						}
					}
				}
				if (!need_yet) return;
				this->val = res.move_to_Bitmap();
			}
			inline const bool& is_ready(){ return ready; }
			void is_not_need_allready_(){ need_yet = false; }
			Bitmap^ get_val(){
				while (!ready);
				return val;
			}
		};
		shared_ptr<bmp> bmp_ptr;
		T time;
		void refresh_buf(){
			auto& bmp_ptr = this->bmp_ptr;
			auto scr = owner.scr.scr;
			auto time = this->time;
			if(bmp_ptr && !bmp_ptr->is_ready()){
				bmp_ptr->is_not_need_allready_();
				while (!bmp_ptr->is_ready());
			}
			bmp_ptr = shared_ptr<bmp>(nullptr);
			bmp_ptr.reset(new bmp(scr, time));
		}
	public:
		bmp_buf(cmds_manager& cmds, shape2D_container& owner, const size_t& size = 5) : cmds(cmds), owner(owner), time(0){
			cmds << [this]()->void{
				refresh_buf();
			};
		}
		Bitmap^ get_picture(){
			static gcroot<Bitmap^> res;
			if (!bmp_ptr || !bmp_ptr->is_ready()) return res;
			res = bmp_ptr->get_val();
			time += 1e-2;
			cmds << [this]()->void{
				refresh_buf();
			};
			return res;
		}
	} buf;
	void insert(const string& name, const record& rec){
		cmds << [this, name, rec]()->void{
			const_cast<shape2D_container*>(this)->map<string, record>::insert( pair<string, record>(name, rec));
			this->buf.refresh_buf();
		};
	}
	void del(const string& name){
		cmds << [this, name]()->void{
			if (const_cast<shape2D_container*>(this)->map<string, record>::count(name))
				const_cast<shape2D_container*>(this)->map<string, record>::erase(
					const_cast<shape2D_container*>(this)->map<string, record>::find(name)
				);
			this->buf.refresh_buf();
		};
	}
	record& at(const string& name){
		volatile bool ch = true;
		struct rec_ref{
			record& rec;
			rec_ref(record& rec) : rec(rec){}
		};
		unique_ptr<rec_ref> rec_ref_ptr;
		cmds << [&]()->void{
			if (const_cast<shape2D_container*>(this)->map<string, record>::count(name)) 
				rec_ref_ptr.reset(new rec_ref(
					this->map<string, record>::at(name)
				));
			ch = false;
		};
		while(ch);
		if(!rec_ref_ptr) throw exception();
		return (*rec_ref_ptr).rec;
	}
	size_t count(const string& name) const{
		volatile bool ch(true);
		size_t res;
		cmds << [&]()->void{
			res = this->map<string, record>::count(name);
			ch = false;
		};
		while(ch);
		return res;
	}
};

shared_ptr<shape2D_container> Singleton<shape2D_container>::ptr;
bool Singleton<shape2D_container>::was_del = false;

}