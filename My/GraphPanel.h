//GraphPanel.h
#pragma once
#include <functional>
#include <list>
#include <msclr/auto_gcroot.h>
using namespace std;
typedef long double T;

namespace My{

	template <class T1, class T2>
	struct my_pair{
		T1 t1;
		T2 t2;
		my_pair(const T1& t1, const T2& t2) : t1(t1), t2(t2){}
	};
	struct Pixel{
		char blue;
		char green;
		char red;
		char alpha;
	};

	using namespace System::Drawing;
	using namespace std;
	
	public ref class GraphPanel : public System::Windows::Forms::Panel{
		//рисует графики функций из списка functions в bitmap
		//func_rec - прямоугольник плрскости Оxy который нужно нарисовать в bitmap
	RectangleF^ func_rec;
	list< my_pair <function<T (const T&)>, gcroot<Color^> > >* functions;
	void init(const list< my_pair <function<T (const T&)>, gcroot<Color^> > >& functions){
		this->functions = nullptr;
		try{
			this->functions = new list< my_pair <function<T (const T&)>, gcroot<Color^> > >(functions);
			refresh_picture();
		}catch(...){
			delete this->functions;
			throw;
		}
	}
	public:
		GraphPanel(const list< my_pair <function<T (const T&)>, gcroot<Color^> > >& functions, const pair<T, T>& range) {
			InitializeComponent();
			//
			//TODO: добавьте код конструктора
			//
			func_rec = gcnew RectangleF(range.first, (range.second - range.first) * 0.5, range.second - range.first, (range.second - range.first));
			init(functions);
		}
		//отношение меры y к x
		GraphPanel(const list< my_pair <function<T (const T&)>, gcroot<Color^> > >& functions, const pair<T, T>& range, const T& k) {
			InitializeComponent();
			//
			//TODO: добавьте код конструктора
			//
			func_rec = gcnew RectangleF(range.first, k * (range.second - range.first) * 0.5, range.second - range.first, k * (range.second - range.first));
			init(functions);
		}
		GraphPanel(const list< my_pair <function<T (const T&)>, gcroot<Color^> > >& functions) {
			InitializeComponent();
			//
			//TODO: добавьте код конструктора
			//
			func_rec = gcnew RectangleF(-this->Width * 0.05, this->Height * 0.05, this->Width * 0.1, this->Height * 0.1);
			init(functions);
		}

		void refresh_picture(){
			//auto now = Environment::TickCount;

			auto bitmap = gcnew Bitmap(this->Width, this->Height);
			//auto bitmap = gcnew Bitmap(this->panel1->Width * 100, this->panel1->Height);
			auto bitmapData = bitmap->LockBits(Rectangle(0, 0, bitmap->Width, bitmap->Height), Imaging::ImageLockMode::WriteOnly, bitmap->PixelFormat);
			//для задания цвета
			auto tmp_bitmap = gcnew Bitmap(1, 1);
			Pixel p;
			try{

				auto w = bitmap->Width, h = bitmap->Height;

				char* ptr = (char*)((void*)bitmapData->Scan0);
				int line_size = bitmapData->Stride, line_size_div_Pixel = line_size / sizeof(Pixel);
				Pixel* Pixel_ptr = (Pixel*)ptr, *Pixel_end_ptr = Pixel_ptr + line_size_div_Pixel * h;

				//оси
				tmp_bitmap->SetPixel(0, 0, Color::Black);
				auto tmp_bitmapData = tmp_bitmap->LockBits(Rectangle(0, 0, 1, 1), Imaging::ImageLockMode::ReadOnly, tmp_bitmap->PixelFormat);
				p = *(Pixel*)(void*)tmp_bitmapData->Scan0;
				tmp_bitmap->UnlockBits(tmp_bitmapData);
				for (Pixel* iter = (Pixel_ptr + (h / 2) * line_size_div_Pixel), *end = iter + line_size_div_Pixel; iter != end; iter++) *iter = p;
				for (Pixel* iter = Pixel_ptr + line_size_div_Pixel / 2, *end = iter + h * line_size_div_Pixel; iter != end; iter += line_size_div_Pixel) *iter = p;
				unsigned int i = 1, len = 8;
				for (Pixel* iter = Pixel_ptr + line_size_div_Pixel / 2 + line_size_div_Pixel, *end = iter + len * line_size_div_Pixel; iter != end; iter += line_size_div_Pixel, i++){
					if (iter - i >= Pixel_ptr && iter - i < Pixel_end_ptr) *(iter - i) = p;
					if (iter + i >= Pixel_ptr && iter - i < Pixel_end_ptr) *(iter + i) = p;
				}
				for (Pixel* l_iter = Pixel_ptr + (h / 2 - len) * line_size_div_Pixel + line_size_div_Pixel, *r_iter = Pixel_ptr + (h / 2 + len) * line_size_div_Pixel + line_size_div_Pixel; l_iter != r_iter; l_iter += line_size_div_Pixel, r_iter -= line_size_div_Pixel, i--){
					if (l_iter - i >= Pixel_ptr && l_iter - i < Pixel_end_ptr) *(l_iter - i) = p;
					if (r_iter - i >= Pixel_ptr && r_iter - i < Pixel_end_ptr) *(r_iter - i) = p;
				}

				T delta = func_rec->Width / bitmap->Width;
				T M = func_rec->Y, m = func_rec->Y - func_rec->Height, Y_Delta = M - m;

				#pragma omp parallel for
				for (int i = 0;  i < functions->size(); i++){
					auto iter = functions->begin();
					for (auto j = 0u;  j < i; j++, iter++);

					T x = func_rec->X, y;
					//определяем цвет
					auto tmp_bitmap = gcnew Bitmap(1, 1);
					tmp_bitmap->SetPixel(0, 0, (Color)(Color^)iter->t2);
					auto tmp_bitmapData = tmp_bitmap->LockBits(Rectangle(0, 0, 1, 1), Imaging::ImageLockMode::ReadOnly, tmp_bitmap->PixelFormat);
					Pixel p = *(Pixel*)(void*)tmp_bitmapData->Scan0;
					tmp_bitmap->UnlockBits(tmp_bitmapData);

					auto f = iter->t1;
					y = f(x);
					int k;
					long long y_old = (h - (int)(((y - m) / Y_Delta) * h)), y_new;
					if (y <= m || y > M) (y > M) ? y_old = -1 : y_old = h;
					for (auto bit_x = 0u; bit_x < w; bit_x++, x += delta){
						y = f(x);
						if (_isnan(y)){
							y_old = -2;
							continue;
						}
						y_new = (h - (int)(((y - m) / Y_Delta) * h));
						if (y > m && y <= M && y_new < h){
							*(Pixel_ptr + bit_x + (line_size_div_Pixel * y_new)) = p;
						}else (y > M) ? y_new = -1 : y_new = h;
						if (y_old == -2) y_old = y_new;
						if (y_old != y_new){
							k = y_old < y_new ? 1 : -1;
							y_old += k;
							for (; y_old != y_new; y_old += k){
								*(Pixel_ptr + bit_x + (line_size_div_Pixel * y_old)) = p;
							}
						}
					}
				}
			}catch(...){
				bitmap->UnlockBits(bitmapData);
				throw;
			}
			bitmap->UnlockBits(bitmapData);
			this->BackgroundImage = bitmap;

			//MessageBox::Show((Environment::TickCount - now).ToString());
		}

private: System::Void panel1_SizeChanged(System::Object^  sender, System::EventArgs^  e) {
			 refresh_picture();
		 }
private: System::Void up_Click(System::Object^  sender, System::EventArgs^  e) {
			 func_rec->Y += func_rec->Height * 0.1;
			 refresh_picture();
		 }
private: System::Void down_Click(System::Object^  sender, System::EventArgs^  e) {
			 func_rec->Y -= func_rec->Height * 0.1;
			 refresh_picture();
		 }
private: System::Void left_Click(System::Object^  sender, System::EventArgs^  e) {
			 func_rec->X -= func_rec->Width * 0.1;
			 refresh_picture();
		 }
private: System::Void right_Click(System::Object^  sender, System::EventArgs^  e) {
			 func_rec->X += func_rec->Width * 0.1;
			 refresh_picture();
		 }
private: System::Void plus_Click(System::Object^  sender, System::EventArgs^  e) {
			 func_rec->X += func_rec->Width * 0.05;
			 func_rec->Y -= func_rec->Height * 0.05;
			 func_rec->Height *= 0.9;
			 func_rec->Width *= 0.9;
			 refresh_picture();
		 }
private: System::Void minus_Click(System::Object^  sender, System::EventArgs^  e) {
			 func_rec->Height /= 0.9;
			 func_rec->Width /= 0.9;
			 func_rec->X -= func_rec->Width * 0.05;
			 func_rec->Y += func_rec->Height * 0.05;
			 refresh_picture();
		 }

	protected:
		/// <summary>
		/// Освободить все используемые ресурсы.
		/// </summary>
		~GraphPanel()
		{
			delete functions;
			functions = nullptr;
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  minus;
	private: System::Windows::Forms::Button^  plus;
	private: System::Windows::Forms::Button^  right;
	private: System::Windows::Forms::Button^  left;
	private: System::Windows::Forms::Button^  down;
	private: System::Windows::Forms::Button^  up;
	protected: 

	private:
		/// <summary>
		/// Требуется переменная конструктора.
		/// </summary>
		System::ComponentModel::Container ^components;


#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->minus = (gcnew System::Windows::Forms::Button());
			this->plus = (gcnew System::Windows::Forms::Button());
			this->right = (gcnew System::Windows::Forms::Button());
			this->left = (gcnew System::Windows::Forms::Button());
			this->down = (gcnew System::Windows::Forms::Button());
			this->up = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// this
			// 
			this->Controls->Add(this->minus);
			this->Controls->Add(this->plus);
			this->Controls->Add(this->right);
			this->Controls->Add(this->left);
			this->Controls->Add(this->down);
			this->Controls->Add(this->up);
			this->Dock = System::Windows::Forms::DockStyle::Fill;
			this->Location = System::Drawing::Point(0, 0);
			this->Name = L"GraphPanel";
			this->Size = System::Drawing::Size(798, 500);
			this->TabIndex = 0;
			this->SizeChanged += gcnew System::EventHandler(this, &GraphPanel::panel1_SizeChanged);
			this->ResumeLayout(false);
			// 
			// minus
			// 
			this->minus->Location = System::Drawing::Point(80, 28);
			this->minus->Name = L"minus";
			this->minus->Size = System::Drawing::Size(23, 23);
			this->minus->TabIndex = 5;
			this->minus->Text = L"-";
			this->minus->UseVisualStyleBackColor = true;
			this->minus->Click += gcnew System::EventHandler(this, &GraphPanel::minus_Click);
			// 
			// plus
			// 
			this->plus->Location = System::Drawing::Point(55, 28);
			this->plus->Name = L"plus";
			this->plus->Size = System::Drawing::Size(23, 23);
			this->plus->TabIndex = 4;
			this->plus->Text = L"+";
			this->plus->UseVisualStyleBackColor = true;
			this->plus->Click += gcnew System::EventHandler(this, &GraphPanel::plus_Click);
			// 
			// right
			// 
			this->right->Location = System::Drawing::Point(106, 28);
			this->right->Name = L"right";
			this->right->Size = System::Drawing::Size(48, 23);
			this->right->TabIndex = 3;
			this->right->Text = L"right";
			this->right->UseVisualStyleBackColor = true;
			this->right->Click += gcnew System::EventHandler(this, &GraphPanel::right_Click);
			// 
			// left
			// 
			this->left->Location = System::Drawing::Point(5, 28);
			this->left->Name = L"left";
			this->left->Size = System::Drawing::Size(48, 23);
			this->left->TabIndex = 2;
			this->left->Text = L"left";
			this->left->UseVisualStyleBackColor = true;
			this->left->Click += gcnew System::EventHandler(this, &GraphPanel::left_Click);
			// 
			// down
			// 
			this->down->Location = System::Drawing::Point(55, 53);
			this->down->Name = L"down";
			this->down->Size = System::Drawing::Size(48, 23);
			this->down->TabIndex = 1;
			this->down->Text = L"down";
			this->down->UseVisualStyleBackColor = true;
			this->down->Click += gcnew System::EventHandler(this, &GraphPanel::down_Click);
			// 
			// up
			// 
			this->up->Location = System::Drawing::Point(55, 4);
			this->up->Name = L"up";
			this->up->Size = System::Drawing::Size(48, 23);
			this->up->TabIndex = 0;
			this->up->Text = L"up";
			this->up->UseVisualStyleBackColor = true;
			this->up->Click += gcnew System::EventHandler(this, &GraphPanel::up_Click);
		}
#pragma endregion
};

}