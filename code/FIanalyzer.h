#ifndef _gfhauth348u89gjhrtwhjg89__FIanalyzer
#define _gfhauth348u89gjhrtwhjg89__FIanalyzer
//View the following Chinese text in Windows OS, Linux may not support the code format;
//郝一平开发的FI势能面调用 CUDA代码自动生成工具 20210831

//没学过《编译原理》的人很难看懂，也不需要改动，理解代码至少需要理解DFA（有限确定状态自动机）
//此外分析文本引入了《编译原理》中词法分析和语法分析的思想与工具。
//对于基本不变量存储文件
//一次前向扫描把输入文本转化为词法单元并且检测错误，确定fi数目，每个fi的长度与阶数。
//二次前向扫描把词法单元转化为特定方便识别的格式。扫描前为格式申请空间。
//一次扫描需要两个自动机，大自动机是class fi_DFA，转化文本为词法单元，小自动机是class FIgrammerDFA，对词法单元进行文法分析，识别错误fi数目，每个fi的长度与阶数
//文法分析一般要自动机加上一个辅助的栈结构才能完成，奈何fi的文法过于简单，用几个int变量代替这个栈。
//二次扫描也要一个迷你自动机，直接嵌入void FIunit::formula_give(int*target, int No, int length)中。
// class fi_formula为最终输出识别的fi易用格式的存储。 class FIunit 是fi的词法单元格式，是分析的中间量。

//我承认这样做有点过度开发，我写了整整一周才写好FI，势能面与输入参数三个文本解析的部分。
//所以不留文档。留个联系方式 QQ：649405039 mail:Hyper@dicp.ac.cn Hyper@mail.ustc.edu.cn
#include "libHyperAcc.h"
#include <iostream>
namespace FIanalyzer
{
	template <class T> class vector
	{
	protected:
		size_t Count;
		size_t Capacity;
		T* content;
	public:
		vector();
		~vector();
		void clear(void);
		void copy(const vector<T>& source);
		void append(const vector<T>& source);
		size_t capacity(void) const;
		size_t count(void) const;
		void recapacity(size_t NewSize);
		void recount(size_t NewCount);
		void append(const T& element);
		size_t SearchAppend(const T& element);
		size_t pop(T& element);
		size_t top(T& element);
		void pop(void);
		size_t append(void);
		void swap(vector<T>& other);
		const T* array(void) const;
		T* array(void);
		const T* array(size_t offset)const;
		T* array(size_t offset);
		const T& operator[](size_t target) const;
		T& operator[](size_t target);
		void value(const T& element);
		const T& top(void) const;
		T& top(void);
	};
	template <class T> class array
	{
	protected:
		size_t Length;
		T* content;
	public:
		array();
		~array();
		size_t length(void) const;
		T* vector(void);
		T* ptr(size_t offset);
		const T* ptr(size_t offset) const;
		T& operator[](size_t target);
		T& operator[](const size_t target) const;
		void value(const T& element);
		void Malloc(size_t NewSize);
		void Realloc(size_t NewSize);
		void Free(void);
	};
	template <class T> class buffer
	{
	protected:
		size_t Rear;
		size_t Capacity;
		size_t Head;
		T* content;
	public:
		buffer();
		~buffer();
		void clear(void);
		void append(const buffer<T>& other);
		size_t append(const T& element);
		size_t expand(void);
		void expand(const size_t L);
		bool dequeue(T& out);
		bool backspace(T& in);
		bool backspace(buffer<T>& in);
		T* array(void);
		const T* array(void) const;
		size_t count(void) const;
		size_t rear(void) const;
		size_t capacity(void) const;
		size_t head(void) const;
		T& operator[](size_t target) const;
	};
}
namespace FIanalyzer
{
	enum FIlexicalUnit
	{
		Identifier,// [a-z_A-Z][a-z_A-Z0-9]*
		Integer, // [0-9]+
		Assignment, // =
		Add, // +
		Multi, // *
		Power, // ** ^
		Left,// ( [ {
		Right,// ) ] }
		ERROR
	};
	class FI_DFA
	{
	public:
		enum action
		{
			wait,
			add,
			record,
			retrieve,
			halt
		};
		FI_DFA();
		~FI_DFA();
		int DFAstate(void);
		int DFAformerState(void);
		enum action DFAswicth(char in);
		enum FIlexicalUnit DFAresult(void);
		void lexical_analyse(hyperlex::CharBuffer& in);
	private:
		int state;
		int FormerState;
		enum action should;
	};

	class FIgrammerDFA
	{
	private:
		int state;
		int No;
		int highest_order;
		int order_now;
		int item;
	public:
		FIgrammerDFA(/* args */);
		~FIgrammerDFA();
		void refresh(void);
		int DFAswicth(enum FIlexicalUnit in, int num);
		int DFAstate(void);
		int Site(void);
		int Order(void);
		int Item(void);
	};


	class FIunit
	{
	public:
		FIunit();
		~FIunit();
		void build(hyperlex::CharBuffer & in);
		void append(hyperlex::CharBuffer & now, enum FIlexicalUnit FI);
		void item_add(FIgrammerDFA& gra_dfa);
		void item_add(void);
		//==================================================
		void correctness_check(void);
		//==================================================
		void print(FILE* fp);
		void print(FILE* fp, int count);
		//==================================================
		enum FIlexicalUnit queue(void);
		int dequeue(void);
		void reset(void);
		int if_begin(void);
		void reset(int No);
		int if_still(int No);
		//==================================================
		int Formula_amount(void);
		int Order(int No);
		int Item(int No);
		int Site(int No);
		void formula_give(int* target, int No, int length);
		//==================================================
	private:
		// grammer analyse
		size_t length;
		size_t next;
		int* list;
		enum FIlexicalUnit* fi_formula;
		std::string fi;
		std::string bondlength;
		size_t head;
		// grammer analyse
		int FormulaAmount;
		size_t ItemLength;

		size_t* formula_beign;
		int* order;
		int* item;
		int* site;


	};

	class FIstruct
	{
	public:
		//friend class fi_gradient;
		FIstruct();
		~FIstruct();

		void build(FIunit& FI);
		void build(FILE* fp);



		void print(FILE* fp);
		void print(void);

	private:
		int FIamount;
		int BondLengthAmount;
		int* length;
		int* order;
		int* ItemAmount;
		int** formula;

		int* if_differential;
		int* differential_count;
		int* differ_count_per_fi;
		int differential_amount;
		int* differential_offset;

		int* differential_length;
		int** differential;

		int highest_order;
		int* order_amount;
		int* workload;

		void Build00Initiate(FIunit& FI);
		void Build01BondAmount(void);
		void Build02DifferentialInfro(void);
		void Build03OrderInfro(void);
		void Build04Differential(void);
		void Build05Workload(void);



		int dfi_scale(int No_fi);
		int dfi_dr_scale(unsigned int fi_No, unsigned int R_No);
		int* dfi_dr(unsigned int fi_No, unsigned int R_No);
	};

	class FIformula
	{
	public:
		//friend class fi_gradient;
		FIformula();
		~FIformula();

		void input(FIformula& old);
		void input(const char * file);
		void input(FIunit& FI);
		void input(FILE* fp);
		void build(void);

		void BondSort(void);
		void Sort(void);
		void Append(FIformula& FI);

		void print(FILE* fp);
		void print(void);
		void compare(FILE* fp, FIformula& background);
		double compute(size_t No, const double* R);
		std::string CSingleConfig(size_t No, size_t TAB, const char* RR, const char* FI, const char* middle);
		std::string CSingleConfig(const char* funcName, const char* RR, const char* FI);
		std::string CSingleConfig(size_t thread, const char* funcName, const char* RR, const char* FI);
		size_t BondCount(void);
		size_t FICount(void);
		int OrderGet(size_t No);
	private:
		void print(FILE* fp, int No, int *poly, int Order, int Item);
		int FIamount;
		int* length;
		int* order;
		int* ItemAmount;
		int** formula;


		int BondLengthAmount;
		//int* if_differential;
		//int* differential_count;
		//int* differ_count_per_fi;
		//int differential_amount;
		//int* differential_offset;

		//int* differential_length;
		//int** differential;

		//int highest_order;
		//int* order_amount;
		//int* workload;

		void Build00Initiate(FIunit& FI);

		//void Build02DifferentialInfro(void);
		//void Build03OrderInfro(void);
		//void Build04Differential(void);
		//void Build05Workload(void);
		void sortFI(int* poly, int L, int Order, int Item);
		// 0 gt
		// 1 lt
		// 2 eq
		int compare(int i, int j);
		int compare(const int* left, int OrderL, int ItemL, const int* right, int OrderR, int ItemR);

		void SortItem(int* mono, int Order);
		void SwapItem(int* poly, int Order, int i, int j);
		void SwapPoly(int i, int j);
		int CompareItem(const int* left, const int* right, int Order);
	};

	void FItransform(const char *FIname, const char *DataName, const char *OutputName);
	void FItransform(FILE* FIfile, FILE* DataFile, FILE* OutputName);

	void FItransformPthread(int thread, const char* FIname, const char* DataName, const char* OutputName);
	void FItransformPthread(int thread, FILE* FIfile, FILE* DataFile, FILE* OutputName);

	void FItransformCsingle(size_t thread, const char* FIname, const char* funcName, const char* OutputName);
	void FItransformCsingle(size_t thread, FILE* FIfile, const char* funcName, FILE* OutputFile);


	int FItransformPthread(int thread, size_t row, size_t column, vector<double>& bondlength, size_t ld, vector<double>& output, FIformula* formula);
}
namespace FIanalyzer
{
	class xyz
	{
	public:
		friend class xyzFile;
		xyz();
		~xyz();
		void Print(double* mat) const;
		void PrintBondlength(double* mat) const;
		void demo(FILE* fp) const;
		bool operator==(const xyz& right) const;
		bool operator!=(const xyz& right) const;
		int build(double Energy, const vector<char*>& Name, const vector<double>& xyzSource);
		int build(const xyz& source);
		void permutation(const vector<size_t>& group);
		size_t AtomCount(void)const;
		//permutation: name[i] = name_old[group[i]];
	protected:
		double E;
		size_t count;
		vector<char*> name;
		vector<double> XYZ;
		bool Force;
		bool compare(const xyz& right) const;
		void permutation(const vector<size_t>& group, vector<char*>& tempN, vector<double> &tempXYZ);
	};
	class xyzFile
	{
	public:
		xyzFile();
		~xyzFile();
		//int build(const char* path, const char* name);
		//int build(const char* name);
		//int build(FILE*fp);
		size_t check(void) const;
		size_t count(void) const;
		int append(xyz*element);
		double* Print(void) const;
		void demo(FILE* fp) const;
		void tail(FILE* fp, size_t N) const;
		void demo(FILE* fp, size_t lower, size_t upper) const;
		void PrintR(vector<double> & output) const;
		int permutation(const vector<size_t>& group);
		xyz*& operator[](size_t target);

		double EnergyUpper(void) const;
		double EnerguLower(void) const;
		void OutputErange(vector<double>&output) const;
	private:
		vector<xyz*> content;
		size_t atom;

	};


}
#include<stdlib.h>
namespace FIanalyzer
{
	template <class T> vector<T>::vector()
	{
		Count = 0;
		Capacity = 0;
		content = NULL;
	}
	template <class T> vector<T>::~vector()
	{
		Count = 0;
		Capacity = 0;
		free(content);
		content = NULL;
	}
	template <class T> void vector<T>::copy(const vector<T>& source)
	{
		size_t i;
		Count = source.Count;
		Capacity = Count;
		content = (T*)realloc(content, Capacity * sizeof(T));
		for (i = 0; i < Count; i++)
			content[i] = source.content[i];
	}
	template <class T> void vector<T>::append(const vector<T>& source)
	{
		size_t i, NewCount;
		NewCount = Count + source.Count;
		if (NewCount > Capacity)
		{
			content = (T*)realloc(content, NewCount * sizeof(T));
			Capacity = NewCount;
		}
		for (i = 0; i < source.Count; i++)
			content[i + Count] = source.content[i];
		Count = NewCount;
	}
	template <class T> void vector<T>::append(const T& element)
	{
		size_t should;
		should = append();
		content[should] = element;
	}

	template <class T> size_t vector<T>::SearchAppend(const T& element)
	{
		size_t i, j;
		const T* temp;
		for (i = 0; i < Count; i++)
		{
			if (content[i] == element) return i;
		}
		append(element);
		return Count - 1;
	}
	template <class T> void vector<T>::clear(void)
	{
		Count = 0;
	}
	template <class T> void vector<T>::recount(size_t NewCount)
	{
		if (NewCount > Capacity)
		{
			//std::cout <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< std::endl;
			//std::cout << "NewCount: " << NewCount << "Capacity: " << Capacity << std::endl;
			//std::cout << "old content: " << content << std::endl;
			content = (T*)realloc(content, NewCount * sizeof(T));
			//std::cout << "new content: " << content << std::endl;
			Capacity = NewCount;
		}
		Count = NewCount;
	}
	template <class T> void vector<T>::recapacity(size_t NewSize)
	{
		if (NewSize <= Count) Count = NewSize;
		content = (T*)realloc(content, NewSize * sizeof(T));
		Capacity = NewSize;
	}
	template <class T> size_t vector<T>::pop(T& element)
	{
		if (Count != 0)
		{
			Count -= 1;
			element = content[Count];
			return Count + 1;
		}
		return Count;
	}
	template <class T> size_t vector<T>::top(T& element)
	{
		if (Count != 0)
		{
			element = content[Count - 1];
		}
		return Count;
	}
	template <class T> void vector<T>::pop(void)
	{
		if (Count != 0) Count -= 1;
	}
	template <class T> size_t vector<T>::append(void)
	{
		size_t should, new_size;
		if (Count >= Capacity)
		{
			new_size = (Capacity + Capacity / 4 + 8);
			content = (T*)realloc(content, new_size * sizeof(T));
			Capacity = new_size;
		}
		should = Count;
		Count += 1;
		return should;
	}
	template <class T> size_t vector<T>::count(void) const
	{
		return Count;
	}
	template <class T> size_t vector<T>::capacity(void) const
	{
		return Capacity;
	}
	template <class T> void vector<T>::swap(vector<T>& other)
	{
		T* middle;
		middle = other.content;
		other.content = content;
		content = middle;

		size_t MM;

		MM = other.Count;
		other.Count = Count;
		Count = MM;

		MM = other.Capacity;
		other.Capacity = Capacity;
		Capacity = MM;
	}
	template <class T> const T* vector<T>::array(void) const
	{
		return content;
	}
	template <class T> T* vector<T>::array(void)
	{
		return content;
	}
	template <class T> const T* vector<T>::array(size_t offset)const
	{
		return content + offset;
	}
	template <class T> T* vector<T>::array(size_t offset)
	{
		return content + offset;
	}
	template <class T> const T& vector<T>::operator[](size_t target) const
	{
		return content[target];
	}
	template <class T> T& vector<T>::operator[](size_t target)
	{
		return content[target];
	}
	template <class T> void vector<T>::value(const T& element)
	{
		size_t i;
		for (i = 0; i < Count; i++)
			content[i] = element;
	}
	template <class T> const T& vector<T>::top(void) const
	{
		size_t target;
		target = Count != 0 ? Count - 1 : 0;
		return content[target];
	}
	template <class T> T& vector<T>::top(void)
	{
		size_t target;
		target = Count != 0 ? Count - 1 : 0;
		return content[target];
	}



	template <class T> array<T>::array()
	{
		Length = 0;
		content = NULL;
	}
	template <class T> array<T>::~array()
	{
		Length = 0;
		free(content);
		content = NULL;
	}
	template <class T> size_t array<T>::length(void) const
	{
		return Length;
	}
	template <class T> T* array<T>::vector(void)
	{
		return content;
	}
	template <class T> T* array<T>::ptr(size_t offset)
	{
		return content + offset;
	}
	template <class T> const T* array<T>::ptr(size_t offset) const
	{
		return content + offset;
	}
	template <class T> T& array<T>::operator[](size_t target)
	{
		return content[target];
	}
	template <class T> T& array<T>::operator[](const size_t target) const
	{
		return content[target];
	}
	template <class T> void array<T>::Malloc(size_t NewSize)
	{
		Length = NewSize;
		content = (T*)malloc(NewSize * sizeof(T));
	}
	template <class T> void array<T>::Realloc(size_t NewSize)
	{
		content = (T*)realloc(content, NewSize * sizeof(T));
		Length = NewSize;
	}
	template <class T> void array<T>::Free(void)
	{
		Length = 0;
		free(content);
		content = NULL;
	}
	template <class T> void array<T>::value(const T& element)
	{
		size_t i;
		for (i = 0; i < Length; i++)
			content[i] = element;
	}


	template <class T> buffer<T>::buffer()
	{
		Rear = 0;
		Capacity = 0;
		Head = 0;
		content = NULL;
	}
	template <class T> buffer<T>::~buffer()
	{
		Rear = 0;
		Capacity = 0;
		Head = 0;
		free(content);
		content = NULL;
	}
	template <class T> void buffer<T>::append(const buffer<T>& other)
	{
		size_t L, i;
		L = other.count();
		if (Rear + L > Capacity)
		{
			content = (T*)realloc(content, (Capacity + L + 4) * sizeof(T));
			Capacity = Capacity + L + 4;
		}
		for (i = 0; i < L; i++) content[i + Rear] = other.content[i + other.Head];
		Rear += L;
		return;
	}
	template <class T> size_t buffer<T>::append(const T& element)
	{
		size_t should;
		should = expand();
		content[should] = element;
		return should;
	}
	template <class T> void buffer<T>::clear(void)
	{
		Head = 0;
		Rear = 0;
	}
	template <class T> size_t buffer<T>::expand(void)
	{
		size_t should, i, new_size;
		if (Rear >= Capacity)
		{
			new_size = (Capacity + Capacity / 4 + 8);
			content = (T*)realloc(content, new_size * sizeof(T));
			Capacity = new_size;
		}
		should = Rear;
		Rear += 1;
		return should;
	}
	template <class T> void buffer<T>::expand(size_t L)
	{
		if (Rear + L > Capacity)
		{
			content = (T*)realloc(content, (Capacity + L + 4) * sizeof(T));
			Capacity = Capacity + L + 4;
		}
		return;
	}
	template <class T> bool buffer<T>::dequeue(T& out)
	{
		if (Head < Rear)
		{
			out = content[Head];
			Head += 1;
			return true;
		}
		return false;
	}
	template <class T> bool buffer<T>::backspace(T& in)
	{
		if (Head != 0)
		{
			Head -= 1;
			content[Head] = in;
			return true;
		}
		return false;
	}
	template <class T> bool buffer<T>::backspace(buffer<T>& in)
	{
		size_t i, LL;
		LL = in.Rear - in.Head;
		if (LL <= Head)
		{
			Head -= LL;
			for (i = 0; i < LL; i++)
			{
				content[Head + i] = in[in.Head + i];
			}
			in.clear();
			return true;
		}
		return false;
	}
	template <class T> T* buffer<T>::array(void)
	{
		//append('\0');
		return content + Head;
	}
	template <class T> const T* buffer<T>::array(void) const
	{
		return content + Head;
	}
	template <class T> size_t buffer<T>::count(void) const
	{
		return (size_t)(Rear - Head);
	}
	template <class T> size_t buffer<T>::rear(void) const
	{
		return Rear;
	}
	template <class T> size_t buffer<T>::capacity(void) const
	{
		return Capacity;
	}
	template <class T> size_t buffer<T>::head(void) const
	{
		return Head;
	}
	template <class T> T& buffer<T>::operator[](size_t target) const
	{
		return content[target];
	}




}


#endif // !_gfhauth348u89gjhrtwhjg89__FIanalyzer

