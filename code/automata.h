#ifndef __automata_adfhaiofh_fh823hasguj45482934__
#define __automata_adfhaiofh_fh823hasguj45482934__
#include<stdio.h>
#include<iostream>
#include<stdlib.h>
//namespace hyperlex 1: template
//namespace hyperlex 2: DFA class declaring without members
/*
	class NFA;
	class sNFA;
	class sheetDFA;
	class DFA;
	class LR0;
	class LR1;
	class Gsheet;
*/
//namespace hyperlex 3: DFA class declaring 
//namespace hyperlex 4: grammer analysis declaring
/*
class grammerS;
template <class Cg> class LRNFA;
class LR0;
class LR1;
class Gsheet;
*/
//namespace hyperlex 5: template
namespace hyperlex
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
		void clear(void) {Count = 0;}
		void copy(const vector<T>& source);
		void append(const vector<T>& source);
		size_t capacity(void) const;
		inline size_t count(void) const {return Count;}
		void recapacity(size_t NewSize);
		void recount(size_t NewCount);
		void append(const T& element);
		size_t SearchAppend(const T& element);
		size_t pop(T& element);
		size_t top(T& element);
		void pop(void);
		size_t append(void);
		void swap(vector<T>& other);
		const T* ptr(void) const;
		T* ptr(void);
		const T* ptr(size_t offset)const;
		T* ptr(size_t offset);
		inline T const& operator[](size_t target) const{return content[target];}
		inline T& operator[](size_t target) {return content[target];}
		void value(const T& element);
		const T& top(void) const;
		T& top(void);
		//friend bool operator<(size_t i, const vector<T>& right);
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
		T* ptr(void);
		T* ptr(size_t offset);
		const T* ptr(size_t offset) const;
		T& operator[](size_t target);
		T& operator[](const size_t target) const;
		void value(const T& element);
		void Malloc(size_t NewSize);
		void Realloc(size_t NewSize);
		void Free(void);
	};
	template <class T> class tree
	{
	public:
		tree();
		~tree();
		void clear(void);
		void append(tree<T>* rear);
		void AppendChilds(tree<T>* ruined);
		void build(vector<tree<T>*>& input);
		void build(vector<tree<T>*>& input, size_t offset);
		void PostOrderTraversal(vector<tree<T>*>& output);
		inline T& root(void) { return content; }
		inline T const & root(void) const {return content;}
		size_t ChildCount(void) const;
		inline tree<T>* child(size_t No) const {return childs[No];}
		struct Iterator
		{
			tree<T>* target;
			int state;
		};
		class PostIterator
		{
		public:
			PostIterator() {}
			~PostIterator() {}
			void initial(tree<T>* root);
			int& state(void);
			tree<T>*& target(void);
			void next(void);
			bool still(void) const;
			inline void CutNow(void) const
			{
				stack.top().state = 1;
			}
		protected:
			vector<Iterator> stack;
			
		};
	private:
		vector<tree<T>*> childs;
		T content;
		tree<T>* parent;
		size_t No;
	};
	template <class T> class BiTree
	{
	public:
		BiTree();
		~BiTree();
		void clear(void);
		void build(BiTree<T>* left, BiTree<T>* right);
		void PreOrderTraversal(vector<BiTree<T>*>& output);
		void PreOrderTraversal(vector<BiTree<T>*>& output, vector<BiTree<T>*>& stack);
		void InOrderTraversal(vector<BiTree<T>*>& output);
		void InOrderTraversal(vector<BiTree<T>*>& output, vector<BiTree<T>*>& stack);
		void PostOrderTraversal(vector<BiTree<T>*>& output);
		void PostOrderTraversal(vector<BiTree<T>*>& output, vector<BiTree<T>*>& stack);

		T& content(void);
		const T& content(void) const;
		BiTree<T>*& left(void);
		BiTree<T>*& right(void);
		void*& label(void);
		BiTree<T>* copy(void);
		struct Iterator
		{
			BiTree<T>* target;
			int state;
		};
		class iterator
		{
		public:
			iterator() {}
			~iterator() {}
			void initial(BiTree<T>* root);
			int& state(void);
			BiTree<T>*& target(void);
			void next(void);
			bool still(void);
		protected:
			vector<Iterator> stack;

		};
	private:
		BiTree<T>* Left;
		BiTree<T>* Right;
		void* tempLabel;
		T Content;
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
		T* ptr(void);
		size_t count(void) const;
		size_t rear(void) const;
		size_t capacity(void) const;
		size_t head(void) const;
		T& operator[](size_t target) const;
	};
	class BufferChar : public buffer<char>
	{
	public:
		BufferChar();
		~BufferChar();
		//void operator<<(FILE* fp);
		char* ptr(void);
		char* CopyVector(void) const;
		size_t CopyVector(vector<char>& storage, size_t& length) const;
		void copy(const BufferChar& source);

		void Append(const char* c);

		void operator=(const char* input);
		void operator=(const BufferChar& in);

		void operator+=(const char* source);
		void operator+=(long int source);
		void operator+=(double source);
		void operator+=(const BufferChar& in);

		void operator<<(char c);
		void operator<<(const char* c);
		void operator<<(double e);
		void operator<<(int e);
		void operator<<(long long int e);
		void operator<<(BufferChar& in);
		void operator<<(FILE* fp);

		bool operator==(size_t length);
		bool operator!=(size_t length);
		bool operator==(const BufferChar& in);
		bool operator!=(const BufferChar& in);

		long int DequeueInt(void);
		double DequeueReal(void);
		char DequeueChar(void);
		char* DequeueString(void);
		char* DequeueId(void);
	private:
		char QueueHead(void);
	};
}
namespace hyperlex
{
	template <class T> class list
	{
	private:
		size_t Count;
		size_t Size;
		T* content;
	public:
		list();
		~list();
		void refresh(void);
		void refresh(size_t NewCount);
		void reshape(size_t NewSize);
		void renew(size_t NewSize);
		void append(const T& element);
		size_t SearchAppend(const T& element);
		size_t pop(T& element);
		void pop(void);
		size_t append(void);
		size_t size(void) const;
		size_t count(void) const;
		void swap(list<T>& other);
		const T* vector(void)const;
		T* vector(void);
		const T* vector(size_t offset)const;
		T* vector(size_t offset);
		const T& operator[](size_t target) const;
		T& operator[](size_t target);
		void value(const T& element);
		const T& top(void) const;
		T& top(void);
	};
	template <class T> class matlist
	{
		/*
		matlist(size_t column);
		column can not be 0.
		*/
	public:
		matlist(size_t column);
		matlist();
		~matlist();
		void refresh(void);
		void refresh(size_t column, size_t length);
		void refresh(size_t row, size_t column, size_t length);
		//void reshape(size_t NewSize);
		void append(const T* element);
		size_t SearchAdd(const T* element);
		//size_t pop(T* element);
		T* append(void);
		size_t column(void) const;
		size_t row(void) const;
		size_t length(void) const;
		void swap(matlist<T>& other);
		const T* operator[](size_t target) const;
		T* operator[](size_t target);
		void value(const T element);
	private:
		size_t Row;
		size_t Column;
		size_t Length;
		T* content;
	};
	template <class T> class vec
	{
	private:
		size_t Size;
		T* content;
	public:
		vec();
		~vec();
		size_t size(void) const;
		T* vector(void);
		T& operator[](size_t target);
		T& operator[](const size_t target) const;
		void Malloc(size_t NewSize);
		void Realloc(size_t NewSize);
		void Free(void);
	};
	template <class Cg, class Ca> class Dgraph
	{
	public:
		struct vortex
		{
			size_t first;
			size_t rear;
			Cg content;
		};
		struct arc
		{
			size_t from;
			size_t to;
			size_t next;
			Ca content;
		};
		list<vortex> vertice;
		buffer<arc> arcs;
		Dgraph();
		~Dgraph();
		Cg& operator[](size_t target);
		void refresh(void);
		void refresh(size_t newlength);
		size_t FirstArc(size_t) const;
		size_t NextArc(size_t) const;
		size_t ArcTo(size_t) const;
		size_t ArcCount(size_t) const;
		size_t append(size_t from, size_t to, const Ca & C);
		void append(const Dgraph<Cg, Ca>& right, size_t VerticeOffset);
		void Demo(FILE* fp) const;
	};
}
namespace hyperlex
{
	class NFA;
	class sNFA;
	class sheetDFA;
	class DFA;
	//class lexicalPanel;
	class grammerS;
	class LR0;
	class LR1;
	class Gsheet;
}
// input analysis and lexical analysis.
namespace hyperlex
{
	class Morpheme;
	class GrammarTree;
	
	class FilePath
	{
	public:
		FilePath();
		~FilePath();
		void build(const char* path);
		void operator+=(const FilePath& path);
		//the input path can't be 'this'。 A += A is wrong;
		bool operator==(const FilePath& rhs) const;
		char* print(char divide = '/')const;
		const char* path(void) const;
		void demo(FILE* fp = stdout)const;
		void copy(const FilePath& source);
		//the input path can't be 'this'。 A += A is wrong;
		void RearCut(void);
		void RearCutAppend(const FilePath& rhs);
		void RearOnlyAppend(const FilePath& rhs);
		void clear(void);
		void clean(void);
	private:
		bool absolute;
		vector<char*> PathUnit;
		char* FullPath;
		void refresh(void);
		void append_copy(const char* str);
	};
	class GrammarTree
	{
	public:
		GrammarTree();
		~GrammarTree();
		struct TreeInfor
		{
			bool implicit;
			bool rules;
			size_t site;
			size_t label;
			void* infor;
			TreeInfor(void);
			// rules:false, this is a leaf node, and its a node corresponding to
			// a lexical terminal symbol then site is a location of the lexical unit 
			// in the lexical sheet
			// rules: true, this may be not a leaf node, 
			// site is the corresponding production rules
		};
		enum GrammarError 
		{
			NoError = 0,
			Undone,
			ActionError,
			GotoError,
			UnExpectEnd
		};
		//template<typename T> int build(const char* reg);
		void Demo(FILE* fp, const Morpheme& input, const char* const* RulesName) const;
		void clear(void);
		template<typename T> int build(const Morpheme& input);

		tree<TreeInfor>* GT;
		//error_record00 =  ture error infor from ACTION, otherwise from the sheet GOTO
		//error_record01 = lexical unit number;
		//error_record02 = (size_t)top content of stack;
		GrammarError error_record00;
		size_t error_record01;
		size_t error_record02;
	protected:
	};
	class Morpheme
	{
		/**
		* Structure storing lexical analysis results
		* @struct result
		* @member accept   the lexical unit is identified as
		* @member category Lexical category identifier
		* @member length   Length of matched lexical unit
		* @member begin    Starting position in input stream
		*/
	public:
		struct result
		{
			int accept;
			int category;
			size_t length;
			size_t begin;
			bool valid;
			size_t line;
			size_t file;
		};
		Morpheme();
		~Morpheme();
		char* Copy(size_t site) const;
		void append(const BufferChar& input, int accept, int category);
		void append(const char* fileName);
		void AppendEnd(int TerminalCount);
		void UnitMove(size_t from, size_t to);
		void CountReset(size_t count);
		void Demo(FILE* fp) const;
		size_t GetCount(void) const;
		const char* GetWord(size_t site) const;
		inline result const& operator[](const size_t target) const
		{
			return lex[target];
		}
		inline result const& operator[](const tree<GrammarTree::TreeInfor>* target) const
		{
			return lex[target->root().site];
		}
		char GetChar(size_t site) const;
		double GetReal(size_t site) const;
		long int GetInt(size_t site) const;
		char* GetString(size_t site) const;
		char* GetHeader(size_t site) const;
		bool& valid(size_t site);

		void insert(size_t from, size_t deleted, const Morpheme& src);
		void SetFile(size_t value);
		const char* GetFile(size_t value) const;
		size_t FileCount(void) const;
		void shrink(void);
		void sort(void);
		bool withTernimal(void)const;
		template<typename T> size_t Build(const char* reg);
		template<typename T> size_t Build(FILE* fp);
		template<typename T> size_t Build(const Morpheme& src);
		//size_t index;
		void clear(void);
		void ruin(void);
		void copy(const Morpheme& source);

		size_t initial(void) const;
		size_t next(size_t index) const;
		bool still(size_t index) const;
		int accept(size_t index) const;
		struct indexT
		{
			size_t UnitOffest;
			size_t CharOffset;
			inline indexT()
			{
				UnitOffest = 0;
				CharOffset = 0;
			}
		};
		
		void print(BufferChar & output) const;
		bool dequeue(char &out, indexT& index) const;
		void backspace(indexT& index, size_t count) const;
		bool operator==(const Morpheme& source) const;
	protected:
		size_t count;
		/*为了文法分析的便利性，
		最后一个符号必须是文件终止符号。在任意的词法单元编码规则下，
		文件终止符号和文件终止符号的种类对应的整数0。
		有点类似于C语言的字符串编码方式*/

		/*
		For the convenience of grammatical analysis, 
		the last symbol must be a ​​file terminator​​. 
		Under any lexical unit encoding rules, 
		the integer corresponding to the ​​file terminator​​ and its category is 
		​​0​​, similar to the string encoding method in the C language.
		*/

		//list<size_t> begin;
		//list<size_t> length;
		vector<const char*> SrcFile;
		vector<result> lex;
		vector<char> storage;

		void SetLine(void);
		size_t CountEnter(const char* unit);

		template<typename T> bool RunBuild(int& accept, BufferChar& result, BufferChar& input, BufferChar& intermediate);
		template<typename T> bool RunBuild(int& accept, BufferChar& result, const Morpheme& src, BufferChar& intermediate, indexT & index);
	};
	class RegularExp
	{
	public:
		RegularExp();
		~RegularExp();
		enum NodeType
		{
			Concatenation = 0,
			Alternation = 1,
			ZeroOrMore = 2,
			OneOrMore = 3,
			ZeroOrOne = 4,
			Range = 5,
		};
		struct item
		{
			int lower;
			int upper;
			NodeType type;
			void Demo(FILE* fp);
			void copy(const item& source);
		};

		void Demo(FILE* fp) const;
		static void Demo(FILE* fp, NodeType T);
		static void Demo(FILE* fp, int L, int U);
		void clear(void);

		int set(const char* input);

		void copy(RegularExp& source);
		void set(int L, int U);
		void set(int leaf);
		void set(BiTree<item>* reg);
		void set(BiTree<item>* reg, NodeType T);
		void set(BiTree<item>* regL, BiTree<item>* regR, NodeType T);
		void set(RegularExp* reg);
		void set(RegularExp* reg, NodeType T);
		void set(RegularExp* regL, RegularExp* regR, NodeType T);
		void set(RegularExp* regL, RegularExp* regR);
		void set(const Morpheme& eme, hyperlex::tree<GrammarTree::TreeInfor>* Tree);
		friend class sNFA;
	private:
		BiTree<item>* tree;
		void set(void);
		int fastBuild(Morpheme& eme);
		int standardBuild(Morpheme& eme);
	};
	class InputPanel
	{
	public:
		typedef const vector<const char*> cvccp;
		InputPanel();
		~InputPanel();
		
		int build_v02(const char* file);
		int build(FILE* fp);
		int build(const char* input);
		void demo(FILE* fp) const;
		void ErrorDemo(FILE* fp) const;
		int printL(FILE* fp, const char* nameL)const;
		int printG(FILE* output, FILE* infor, const char* nameG)const;

		struct RegContent
		{
			const char* name;
			long int priority;
			long int accept;
			size_t group; 
			//When two distinct regular expressions match simultaneously, 
			//the regular expression with the higher priority number is accepted.
			RegularExp* reg;
			RegContent();
			~RegContent();
			void SetName(const char* input);
		};
		struct RegGroup
		{
			const char* name;
			int priority;
			vector<RegContent*> regs;
			size_t prefix;
			RegGroup();
			~RegGroup();
			void SetName(const char* input);
		};
		struct Rules
		{
			const char* name;
			vector<long int> formula;
			bool implicit;
			Rules();
			~Rules();
			void SetName(const char* input);
			void demo(FILE* fp, cvccp& N, cvccp& T);
		};
		struct Group
		{
			size_t group;
			const char* name;
			vector<Rules*> rules;
			size_t count;
			size_t offset;
			bool implicit;
			Group();
			~Group();
			void SetName(const char* input);

		};
		enum errorType
		{
			NoError = 0,
			ConflictRegGroupName = 1,
			repeatRegName = 2,
			missingId = 3,
			repeatGGroupName = 4,
			repeatGName = 5,
			ErrorNonTernimal,
			WorngRuleBody,
			missingIdinRegdef,
			ErrorinputLEXICAL,
			PretreatLEXICAL,
			PretreatGRAMMAR,
			PretreatRepeat,
			PretreatOpenfail,
			ErrorinputGrammar,
			regGroupMissing,
			buildUndone,
		};
		friend class NFA;
		friend class grammerS;
	private:
		typedef hyperlex::tree<GrammarTree::TreeInfor> GLTree;
		typedef hyperlex::tree<GrammarTree::TreeInfor>::PostIterator GTIter;
		bool GrammarEnclosed;
		vector<RegGroup*> RegG;
		vector<RegContent*> regular;
		vector<Group*> GrammarG;

		vector<const char*> Terminal;
		vector<const char*> NontTerminal;
		char* RootName;

		vector<Rules*> rules;

		errorType errorCode;
		size_t errorInfor1;
		size_t errorInfor2;
		const char* errorInfor3;
		bool errorInfor4;

		Morpheme MorphemePre;
		Morpheme LexicalSource;

		int pretreatment(const char* input, Morpheme& output);

		void initial(void);
		void clear(void);
		/*
		demo of input file analysis;
		*/
		void demoL(FILE* fp) const;
		void demoG(FILE* fp) const;
		void printGName(FILE* output, FILE* infor, const char* nameG)const;
		void printImplicit(FILE* output, FILE* infor, const char* nameG)const;
		/*
		* input analysis;
		*/
		int buildGanalysis(const Morpheme& eme);
		int buildAll(const Morpheme& eme, GrammarTree& Tree);
		void buildLpost(void);

		int buildL(const Morpheme& eme, GLTree* Tree);
		void addVoidGroup(void);
		int RegGroupName(size_t& siteReturn, const Morpheme& eme, GLTree* Tree);
		int RegName(size_t& siteReturn, size_t group, const Morpheme& eme, GLTree* Tree);
		bool RegSearch(size_t& group, size_t& site, const char* name);
		int RegBuild(size_t siteReturn, size_t group, const Morpheme& eme, GLTree* Tree);
		void NeglectNullToken(Morpheme& eme) const;

		int buildG(const Morpheme& eme, GLTree* Tree);
		int buildRules(const Morpheme& eme, GLTree* Tree);
		void addAllGroup(void);
		void addAllGroup02(void);
		int GrammarGroup(size_t& GroupSite, const Morpheme& eme, GLTree* Tree);
		int RulesAppend(size_t GroupSite, GLTree* Name, const Morpheme& eme, GLTree* Tree);
		long int UnitToSymbol(GLTree* Tree, const Morpheme& eme);
		long int WildcardToSymbol(long int symbol, const Morpheme& eme, bool plus);
		void addTerminal(void);
		long int SymbolAdd(const char* symbol);
		long int UnitRegGroupAdd(const char* symbol);
		int NonTerminalSort(void);
		void ImplicitAdd(void);
	};

}

// lexical analysis

namespace hyperlex
{
	class ShrinkList
	{
	public:
		friend class sheetDFA;
		friend class DFA;
		ShrinkList(size_t AcceptCount, size_t size);
		~ShrinkList();
		void Insert(size_t site, size_t state);
		size_t DeleteNext(size_t now);
		size_t NewState(size_t Now);
		size_t Head(size_t site)const;
		size_t Count(void)const;
		void Demo(FILE* fp)const;
	private:
		vec<size_t> group;
		vec<size_t> next;
		size_t Statecount;
		list<size_t> head;
		list<size_t> rear;
	};
	struct Convert
	{
		char upper;
		char lower;
		bool epsilon;
		bool in(const char ele);
	};
	struct convert
	{
		int upper;
		int lower;
		//size_t to;
		bool in(const char ele);
		bool in(int ele);
	};
	class sNFA
	{
	public:
		friend class NFA;
		sNFA();
		~sNFA();
		//typedef buffer<Convert> converts;
		void refresh(void);
		void build(const RegularExp* Reg);

		//void build(const RegTree* Reg);
		//void build(const RegTree *Reg, buffer<size_t> &output, list<size_t>& s, list<sNFA*> &nfa);
		
		void Demo(FILE* fp) const;
	protected:
		void build(void);
		void build(const bool input[128]);
		void parallel(const sNFA& left, const sNFA& right);
		void serial(const sNFA& left, const sNFA& right);
		void any(const sNFA& right);
		void plus(const sNFA& right);
		void ZeroOrOne(const sNFA& right);

		void parallel(const sNFA* left, const sNFA* right);
		void serial(const sNFA* left, const sNFA* right);
		void any(const sNFA* right);
		void plus(const sNFA* right);
		void ZeroOrOne(const sNFA* right);
	private:
		Dgraph<int, Convert> graph;
		// content of vertice of graph is meanless
		// state = 0 is the begin
		size_t StateAmount;
		//DirectedGraph graph;
		size_t accepted;
		//converts CC;
	};
	class NFA
	{
	public:
		friend class sheetDFA;
		NFA(const sNFA& single);
		NFA(const sNFA* const* multiple, size_t count);
		//NFA(const lexicalPanel&lP);
		NFA(const InputPanel& lP);
		~NFA();
		void initial(bool* state, list<size_t>& stack) const;
		void closure(bool* state, list<size_t> &stack) const;
		size_t move(const bool* from, bool* to, const char ele, list<size_t>& stack) const;
		size_t GetAccepted(const bool* state, bool* accept) const;
		size_t FirstAccepted(const bool* accept) const;
		void Demo(FILE* fp) const;
	private:
		// content of vertice of graph is label of accpet
		// state = 0 is the begin
		// No i accepted state has graph.vertice[??].content = i
		Dgraph<int, Convert> graph;
		size_t StateAmount;
		size_t accepted;
		list<size_t> priority;
	private:
		void build(const sNFA* const* multiple, size_t count);
		//vec<bool> state;
	};
	class sheetDFA
	{
	public:
		friend class DFA;
		sheetDFA(const NFA & nfa);
		~sheetDFA();
		void shrink(void);
		bool ifDeadAccept(void)const;
		int next(int state, const char c)const;
		int action(int state)const;
		
		void Demo(FILE*fp)const ;
		static void Demo(FILE* fp, const bool *state, size_t n);
	private:
		size_t count;
		size_t AcceptCount;
		size_t LiveAcceptCount;
		matlist<int> sheet;
		list<int> accept;
		//matlist<int> accept;
		void acceptSet(const matlist<bool>& Dstates, const NFA& nfa);
		void shrink(const ShrinkList* SL);
	};
	enum NodeType
	{
		Concatenation = 0,
		Alternation = 1,
		ZeroOrMore = 2,
		OneOrMore = 3,
		AnyOne = 4,
	};
	class DFA
	{
	public:
		DFA(const sheetDFA* dfa);
		~DFA();
		int next(int state, const char c)const;
		int action(int state)const;
		void Demo(FILE* fp)const;
		void Cprint(FILE* fp, const char* name)const;
		void CppPrint(FILE* fp, const char* name)const;
		void CprintAccept(FILE* fp, const char* name, const char** const category, const char** const accept)const;
		void CppPrintAccept(FILE* fp, const char* name, const char** const category, const char** const accept)const;

		static void Cprint(FILE* fp, convert& CC);
		static void Demo(FILE* fp, int L, int U);
		static void Demo(FILE* fp, convert &CC);

		size_t StateAmountGet(void) const;
		int GraphStateGet(const size_t site) const;
	private:
		Dgraph<int, convert> graph;
		size_t StateAmount;
		size_t AcceptCount;
		void CprintCore(FILE* fp, const char* name)const;
		void CprintAcceptCore(FILE* fp, const char* name, const char** const category, const char** const accept)const;
	};
}
namespace hyperlex
{
	template<typename T> size_t Morpheme::Build(FILE* fp)
	{
		BufferChar input;
		BufferChar result;
		BufferChar intermediate;
		int accept;
		int error = 0;
		char now;
		input << fp;
		ruin();

		while (RunBuild<T>(accept, result, input, intermediate))
		{
			if (accept != 0) append(result, accept, T::GroupGet(accept));
			else
			{
				input.dequeue(now);
				result.append(now);
				append(result, -1, -1);
				AppendEnd(0);
				SetLine();
				return count;
			}
		}
		AppendEnd(0);
		SetLine();
		return error;
	}
	template<typename T> size_t Morpheme::Build(const char* reg)
	{
		BufferChar input;
		BufferChar result;
		BufferChar intermediate;
		int accept;
		int error = 0;
		char now;
		ruin();
		input = reg;
		while (RunBuild<T>(accept, result, input, intermediate))
		{
			if (accept != 0) append(result, accept, T::GroupGet(accept));
			else
			{
				input.dequeue(now);
				result.append(now);
				append(result, -1, -1);
				AppendEnd(0);
				SetLine();
				return count;
			}
		}
		AppendEnd(0);
		SetLine();
		
		return error;
	}
	template<typename T> size_t Morpheme::Build(const Morpheme& src)
	{
		//BufferChar input;
		BufferChar result;
		BufferChar intermediate;
		int accept = 0;
		int error = 0;
		char now;
		indexT index;
		ruin();
		for (size_t i = 0; i < src.SrcFile.count(); i++)
		{
			append(src.SrcFile[i]);
		}
		size_t record = index.UnitOffest;
		while (RunBuild<T>(accept, result, src, intermediate, index))
		{
			//std::cout << "count: " << count << std::endl;
			//std::cout << "record: " << record << std::endl;
			//std::cout << "accept: " << accept << std::endl;
			//std::cout << "index.UnitOffest: " << index.UnitOffest << std::endl;
			//std::cout << "index.CharOffset: " << index.CharOffset << std::endl;
			if (accept != 0) append(result, accept, T::GroupGet(accept));
			else
			{
				src.backspace(index, 1);
				result.append(now);
				append(result, -1, -1);
				AppendEnd(0);
				SetLine();
				return count;
			}
			lex[count - 1].line = src.lex[record].line;
			lex[count - 1].file = src.lex[record].file;
			record = index.UnitOffest;
		}
		AppendEnd(0);
		return 0;
	}
	template<typename T> bool Morpheme::RunBuild(int& accept, BufferChar& result, BufferChar& input, BufferChar& intermediate)
	{
		/*
	example:
	1 aa
	2 aaBB
	3 Bcc
	input: aaBcc
	*/
		char now;
		//char cc;
		int state, acc;
		int action;
		intermediate.clear();
		state = 0;
		acc = 0;
		action = 0;
		accept = 0;
		result.clear();
		while (input.dequeue(now))
		{
			/*state switch*/
			/*change here to get a different automata*/
			state = T::next(state, now);
			acc = T::action(state);
			/*change here to get a different automata*/
			accept = acc != 0 ? acc : accept;
			switch (action)
			{
			case 0://initial
				if (state != 0 && acc == 0)
				{
					intermediate.append(now);
					action = 1;
				}
				else if (state != 0 && acc != 0)
				{
					result.append(now);
					action = 2;
				}
				else
				{
					input.backspace(now);
					return true;
				}
				break;
			case 1://run and waiting for accept
				if (state == 0)
				{
					input.backspace(now);
					input.backspace(intermediate);
					return true;
				}
				else if (acc != 0)
				{
					result.append(intermediate);
					result.append(now);
					intermediate.clear();
					action = 2;
				}
				else intermediate.append(now);//continue 
				break;
			case 2://accept
				if (state == 0)//accept
				{
					input.backspace(now);
					//accept = last;
					return true;
				}
				else if (acc == 0)
				{
					intermediate.append(now);
					action = 1;
				}
				else result.append(now);
				break;
			}
		}
		if (action == 1)
			input.backspace(intermediate);
		return action != 0;
	}
	template<typename T> bool Morpheme::RunBuild(int& accept, BufferChar& result, const Morpheme& src, BufferChar& intermediate, indexT& index)
	{
		/*
	example:
	1 aa
	2 aaBB
	3 Bcc
	input: aaBcc
	*/
		char now;
		//char cc;
		int state, acc;
		int action;
		indexT RecoverRecord;
		intermediate.clear();
		state = 0;
		acc = 0;
		action = 0;
		accept = 0;
		result.clear();
		RecoverRecord = index;
		while (src.dequeue(now, index))
		{
			/*state switch*/
			/*change here to get a different automata*/
			state = T::next(state, now);
			acc = T::action(state);
			/*change here to get a different automata*/
			accept = acc != 0 ? acc : accept;
			switch (action)
			{
			case 0://initial
				if (state != 0 && acc == 0)
				{
					intermediate.append(now);
					action = 1;
				}
				else if (state != 0 && acc != 0)
				{
					result.append(now);
					action = 2;
				}
				else
				{
					index = RecoverRecord;
					//input.backspace(now);
					return true;
				}
				break;
			case 1://run and waiting for accept
				if (state == 0)
				{
					index = RecoverRecord;
					//input.backspace(now);
					//input.backspace(intermediate);
					return true;
				}
				else if (acc != 0)
				{
					result.append(intermediate);
					result.append(now);
					intermediate.clear();
					action = 2;
				}
				else intermediate.append(now);//continue 
				break;
			case 2://accept
				if (state == 0)//accept
				{
					src.backspace(index, 1);
					//input.backspace(now);
					//accept = last;
					return true;
				}
				else if (acc == 0)
				{
					intermediate.append(now);
					action = 1;
				}
				else result.append(now);
				break;
			}
		}
		if (action == 1)
			src.backspace(index, intermediate.count());
			//input.backspace(intermediate);
		return action != 0;
	}
	
	
	template<typename T> int GrammarTree::build(const Morpheme& input)
	{
		vector<int> stack;
		vector<tree<TreeInfor>*> TempTree;
		tree<TreeInfor>* TreeNow, * TreeChild;
		bool DoNext;
		size_t length, head, i, begin_;// , inputCount_;
		int symbol, top, temp, information;
		int GoFull, GoD, GoI, error;
		typename T::type type;
		stack.append(0);
		head = input.initial();
		//inputCount_ = input.GetCount();
		DoNext = true;
		do
		{
			if (!input.still(head))
			{
				error = -1;
				error_record00 = GrammarTree::UnExpectEnd;
				error_record01 = head - 1;
				error_record02 = (size_t)top;
				break;
			}
			top = stack.top();
			temp = T::ACTION[top][input.accept(head)];
			information = temp / 4;
			type = (typename T::type)(temp % 4);
			//printf( "T = %5d, top = %5d, information = %5d, type = %5d, ", input[head].accept, top, information, (int)type);
			//printf("head = %5zu, lex = %s, \n", head, input.GetWord(head));
			switch (type)
			{
			case T::accept:
				error = 0;
				DoNext = false;
				clear();
				begin_ = TempTree.count() - 1;
				TreeNow = new tree<TreeInfor>;
				TreeNow->root().implicit = false;
				TreeNow->root().rules = true;
				TreeNow->root().site = 0;
				TreeNow->root().infor = NULL;
				TreeNow->build(TempTree, begin_);
				TempTree.pop();
				GT = TreeNow;
				//TempTree.pop(GT);
				break;
			case T::error:
				error_record00 = GrammarTree::ActionError;
				error_record01 = head;
				error_record02 = (size_t)top;
				//printf("error_record01: %zu\n", error_record01);
				//printf("error_record02: %zu\n", error_record02);
				error = temp;
				DoNext = false;
				break;
			case T::push:
				
				//printf("<%8zu, %4d: %4d , %s>\n", head, input[head].category, input[head].accept, input.GetWord(head));
				stack.append(information);
				TreeNow = new tree<TreeInfor>;
				TreeNow->root().implicit = false;
				TreeNow->root().rules = false;
				TreeNow->root().site = head;
				TreeNow->root().infor = NULL;
				TempTree.append(TreeNow);		
				head = input.next(head);
				
				break;
			case T::reduce:
				symbol = T::RulesToSymbol[information];
				length = T::RulesLength[information];
				begin_ = TempTree.count() - length;

				TreeNow = new tree<TreeInfor>;
				TreeNow->root().implicit = (bool)T::Implicit[(size_t)information];
				TreeNow->root().rules = true;
				TreeNow->root().site = information;
				TreeNow->root().infor = NULL;
				//TreeNow->build(TempTree, begin_);
				
				for (i = 0; i < length; i++)
				{
					stack.pop();
					TreeChild = TempTree[begin_ + i];
					if (TreeChild->root().implicit)
						TreeNow->AppendChilds(TreeChild);
					else TreeNow->append(TreeChild);
					TempTree[begin_ + i] = NULL;
				}
				TempTree.recount(begin_);
				GoFull = T::GOTO[stack.top()][symbol];
				GoD = GoFull / 4;
				GoI = GoFull % 4;
				//printf("<%6d: %6d, %6d>\n", GoFull, GoD, GoI);
				if ((typename T::type)GoI == T::push)
				{
					stack.append(GoD);
					TempTree.append(TreeNow);
				}
				else
				{
					error_record00 = GrammarTree::GotoError;
					error_record01 = head;
					error_record02 = (size_t)top;
					error = GoFull;
					DoNext = false;
				}
				break;
			}

		} while (DoNext);

		for (i = 0; i < TempTree.count(); i++)
		{
			//printf("<%8zu>\n", i);
			if (TempTree[i] != NULL)
			{
				//printf("####\n");
				TempTree[i]->clear();
				delete TempTree[i];
			}
		}
		return error;
	}
}
// grammer analysis
namespace hyperlex
{
	class grammerS
	{
	public:
		friend class LR0;
		friend class LR1;
		friend class Gsheet;
		grammerS();
		~grammerS();
		int build(const InputPanel& input);
		int build(FILE* fp);
		int build(BufferChar& input);
		void Demo(FILE* fp) const;
		void Cprint(FILE* fp) const;
		void Demo(FILE* fp, size_t rule) const;
		void Demo(FILE* fp, size_t rule, size_t dot) const;
		const char* SymbolGet(long long int index) const;
		const long long int* vector(size_t No)const;
		void SetExample(void);
		struct production
		{
			size_t symbol;
			size_t length;
			size_t begin;
			size_t LengthWithoutVoid;
		};
		enum ErrorCode
		{
			done = 0,
			undone = 1,
			unexpectedTernimal = 2,
			missingdivision = 3,
			missingRulesOrWrongSymbol = 4,
		};
	private:
		ErrorCode ERROR;
		size_t count;// count of non-terminal
		size_t TerminalCount;
		//[0,count-1] non-terminal
		//[count,TerminalCount + count -1]; terminal
		//TerminalCount + count: epsilon
		//TerminalCount + count + 1: END-EOF
		long long int epsilon;
		long long int end;
		list<size_t> degeneracy;//has length of count.
		//degeneracy[i]: rules' count of non-terminal[i]
		list<size_t> prefix;//has length of count
		//prefix[i]: rules' begining index of non-terminal[i]
		list<const char*> name;//has length of count
		list<const char*> ternimal;
		list<production> rules;//has length of count of all rules
		list<long long int> all;//Production Rules

		matlist<bool> first;//matrix of count * (TerminalCount + 2)
		matlist<bool> follow;//matrix of count * (TerminalCount + 2)
	private:
		void append(const char* input);
		void Tappend(const char* input);
		size_t SearchAppend(const char* input);
		long long int SearchTerminal(const char* input);
		int nextTest(int state, const char c);
		int actionTest(int state);
		bool RunBuild(int&accept, BufferChar& result, BufferChar& input, BufferChar& intermediate);
	private:
		
		void FirstBiuld(void);
		bool IfEpsilon(long long int site) const;
		bool FirstOr(long long int symbol, bool* to) const;
		bool Or(const bool* from, bool* to) const;
		bool FollowOr(const bool* from, size_t to);
		bool AddFirstBeta(bool* result, size_t L, const long long int* in) const;
		void FirstBeta(bool* result, size_t L, const long long int*in) const;
		void FollowBiuld(void);
	};
	template <class Cg> class LRNFA
	{
	public:
		struct vortex
		{
			size_t first;
			size_t rear;
			Cg content;
		};
		struct arc
		{
			size_t from;
			size_t to;
			size_t next;
			bool epsilon;
			long long int symbol;
			//bool in(long long int ele);
		};
		list<vortex> vertice;
		buffer<arc> arcs;
		LRNFA();
		~LRNFA();
		Cg& operator[](size_t target);
		const Cg& operator[](size_t target) const;
		void refresh(void);
		void refresh(size_t newlength);
		size_t VortexAppend(const Cg& element);
		size_t FirstArc(size_t) const;
		size_t NextArc(size_t) const;
		size_t ArcTo(size_t) const;
		size_t ArcCount(size_t) const;
		size_t append(size_t from, size_t to, bool Epsilon, long long int Symbol);
		void Demo(FILE* fp) const;

		void initial(bool* state, list<size_t>& stack) const;
		void closure(bool* state, list<size_t>& stack) const;
		size_t move(const bool* from, bool* to, long long int ele, list<size_t>& stack) const;
		void build(matlist<int>& sheet, matlist<bool> &Dstates, size_t NonTcount, size_t Tcount) const;
	};
	class LR0
	{
	public:
		friend class Gsheet;
		LR0(const grammerS *G);
		~LR0();
		void Demo(FILE*fp, const grammerS* G);
		struct item
		{
			size_t rules;
			size_t dot;
		};
	private:
		LRNFA<item> graph;

		size_t TerminalCount;
		size_t NonTerminalCount;
		matlist<int> sheet;
		matlist<bool> Dstates;
		//void build(const grammerS* G);
		
	};
	class LR1
	{
	public:
		friend class Gsheet;
		LR1(const grammerS* G);
		~LR1();
		void Demo(FILE* fp, const grammerS* G);
		void Demo(FILE* fp, size_t site, const grammerS* G);
		struct item
		{
			size_t rules;
			size_t dot;
			long long int symbol;
			bool operator==(const item R) const;
		};
	private:
		LRNFA<item> graph;

		size_t TerminalCount;
		size_t NonTerminalCount;
		matlist<int> sheet;
		matlist<bool> Dstates;
		size_t SearchAppend(const item& I, bool& IfAppend);
		//void build(const grammerS* G);
	};
	class Gsheet
	{
	public:
		Gsheet();
		~Gsheet();
		enum type
		{
			accept = 0,
			error = 1,
			push = 2,
			reduce = 3,
		};
		struct infor
		{
			type action;
			int state;
		};
		enum ErrorType
		{
			NoError = 0,
			uninitialized = 1,
			ConflictACTION = 2,
			ConflictGOTO = 3,
		};
		struct ErrorInfor
		{
			size_t i;
			size_t j;
			type ActionOld;
			int StateOld;
			type ActionNew;
			int StateNew;
		};
		void build(const LR0* lr, const grammerS* G);
		void build(const LR1* lr, const grammerS* G);
		int build(const matlist<int>& sheet, const matlist<bool>& Dstates, const grammerS* G);
		void build(void);//after
		void Demo(FILE* fp) const;
		void Demo(FILE* fp, const LR0* lr, const grammerS* G)const;
		void Demo(FILE* fp, const LR1* lr, const grammerS* G)const;

		infor Goto(size_t state, size_t symbol) const;
		infor Action(size_t state, size_t symbol) const;
		int GetSymbol(size_t rules) const;
		void Cprint(const char* name, FILE* fp)const;
		void CppPrint(const char* name, FILE* fp)const;
		void CppStructPrint(const char* name, FILE* fp, const grammerS * grammer)const;
		void CppStructPrint01(const char* name, FILE* fp, const grammerS* grammer)const;
		void CppStructPrint02(const char* name, FILE* fp, const grammerS* grammer)const;
		// now site 0 is END-EOF, site 1-N Are N terminal symbol
		static const char* TypeToChar(type TT);
		ErrorType ErrorTypeGet(void);
		ErrorInfor ErrorInforGet(void);
	private:
		size_t StateCount;
		size_t TerminalCount;
		size_t NonTerminalCount;
		size_t RulesCount;

		infor* ACTION;
		infor* GOTO;
		int* RulesToSmbol;
		int* RulesLength;

		ErrorType ET;
		ErrorInfor EI;

		void build_singleErrorAdd(void);

		bool gotoAdd(size_t i, size_t j, type Action, int State);
		bool actionAdd(size_t i, size_t j, type Action, int State);
		bool gotoCheck(size_t i, size_t j);
		bool actionCheck(size_t i, size_t j);
		void Demo01(FILE* fp)const;
		void Demo02(FILE* fp)const;
		void Demo03(FILE* fp)const;

		//void CppStructPrint01(const char* name, FILE* fp, const grammerS* grammer)const;
		//void CppStructPrint02(const char* name, FILE* fp, const grammerS* grammer)const;
	};
}
#define SizeMax ((size_t)0xffffffffffffffff)
#define CharSize ((size_t)(1 << (sizeof(char) * 8 - 1)))


#include<stdlib.h>
namespace hyperlex
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
	template <class T> const T* vector<T>::ptr(void) const
	{
		return content;
	}
	template <class T> T* vector<T>::ptr(void)
	{
		return content;
	}
	template <class T> const T* vector<T>::ptr(size_t offset)const
	{
		return content + offset;
	}
	template <class T> T* vector<T>::ptr(size_t offset)
	{
		return content + offset;
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
	//template <class T> bool operator<(size_t i, const vector<T>& right)
	//{
	//	return i < right.Count;
	//}


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
	template <class T> T* array<T>::ptr(void)
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

	template <class T> tree<T>::tree()
	{
		parent = NULL;
		No = 0;
	}
	template <class T> tree<T>::~tree()
	{
	}
	template <class T> void tree<T>::clear(void)
	{
		vector<tree<T>*> deleted;
		size_t i;
		PostOrderTraversal(deleted);
		for (i = 1; i < deleted.count(); i++)
			delete deleted[i - 1];
		childs.recapacity(0);
	}
	template <class T> void tree<T>::PostOrderTraversal(vector<tree<T>*>& output)
	{
		vector<bool> StackState;
		vector<tree<T>*> StackSite;
		tree<T>* here;
		bool label;
		size_t i;
		for (i = childs.count(); i != 0; i--)
		{
			StackSite.append(childs[i - 1]);
			StackState.append(false);
		}
		while (StackState.pop(label) != 0)
		{
			StackSite.pop(here);
			if (here == NULL) continue;
			if (label) output.append(here);
			else
			{
				StackSite.append(here);
				StackState.append(true);

				for (i = here->childs.count(); i != 0; i--)
				{
					StackSite.append(here->childs[i - 1]);
					StackState.append(false);
				}
			}
		}
		output.append((tree<T>*)this);
	}
	template <class T> void tree<T>::append(tree<T>* rear)
	{
		childs.append(rear);
		rear->parent = this;
		rear->No = childs.count() - 1;
	}
	template <class T> void tree<T>::AppendChilds(tree<T>* ruined)
	{
		size_t i;
		tree<T>* rear;
		for (i = 0; i < ruined->ChildCount(); i++)
		{
			rear = ruined->child(i);
			childs.append(rear);
			rear->parent = this;
			rear->No = childs.count() - 1;
		}
		ruined->childs.clear();
		delete ruined;
	}
	template <class T> void tree<T>::build(vector<tree<T>*>& input)
	{
		size_t i;
		childs.recount(input.count());
		for (i = 0; i < input.count(); i++)
		{
			childs[i] = input[i];
			childs[i]->parent = this;
			childs[i]->No = i;
		}
	}
	template <class T> void tree<T>::build(vector<tree<T>*>& input, size_t offset)
	{
		size_t i;
		if (input.count() <= offset)
		{
			childs.recount(0);
			return;
		}
		childs.recount(input.count() - offset);
		for (i = 0; i < input.count() - offset; i++)
		{
			childs[i] = input[i + offset];
			childs[i]->parent = this;
			childs[i]->No = i;
		}
	}

	template <class T> size_t tree<T>::ChildCount(void) const
	{
		return childs.count();
	}

	template <class T> int& tree<T>::PostIterator::state(void)
	{
		return stack.top().state;
	}
	template <class T> tree<T>*& tree<T>::PostIterator::target(void)
	{
		return stack.top().target;
	}
	template <class T> void tree<T>::PostIterator::initial(tree<T>* root)
	{
		Iterator now;
		stack.clear();
		if (root != NULL)
		{
			now.state = 0;
			now.target = root;
			stack.append(now);
		}
	}
	template <class T> void tree<T>::PostIterator::next(void)
	{
		size_t i; 
		Iterator now, parent;
		tree<T>* here;
		if (stack.pop(parent) != 0)
		{
			if (parent.state == 0)
			{
				parent.state = 1;
				stack.append(parent);
				here = parent.target;
				now.state = 0;
				for (i = here->childs.count(); i != 0; i--)
				{
					now.target = here->childs[i - 1];
					stack.append(now);
				}
			}
		}
		
	}
	template <class T> bool tree<T>::PostIterator::still(void) const
	{
		return stack.count() != 0;
	}


	template <class T> BiTree<T>::BiTree()
	{
		Left = NULL;
		Right = NULL;
		tempLabel = NULL;
	}
	template <class T> BiTree<T>::~BiTree()
	{
		Left = NULL;
		Right = NULL;
	}
	template <class T> int& BiTree<T>::iterator::state(void)
	{
		return stack.top().state;
	}
	template <class T> BiTree<T>*& BiTree<T>::iterator::target(void)
	{
		return stack.top().target;
	}
	template <class T> void BiTree<T>::iterator::initial(BiTree<T>* root)
	{
		Iterator now;
		stack.clear();
		if (root != NULL)
		{
			now.state = 0;
			now.target = root;
			stack.append(now);
		}
	}
	template <class T> void BiTree<T>::iterator::next(void)
	{
		size_t i;
		Iterator now, parent;
		now.state = 0;
		if (stack.pop(parent) != 0)
		{
			if (parent.state == 0)
			{
				parent.state = 2;
				stack.append(parent);
				if (parent.target->Right != NULL)
				{
					now.target = parent.target->Right;
					stack.append(now);
				}
				parent.state = 1;
				stack.append(parent);
				if (parent.target->Left != NULL)
				{
					now.target = parent.target->Left;
					stack.append(now);
				}
			}
		}

	}
	template <class T> bool BiTree<T>::iterator::still(void)
	{
		return stack.count() != 0;
	}

	template <class T> void BiTree<T>::clear(void)
	{
		vector<BiTree<T>*> output;
		size_t i;
		PostOrderTraversal(output);
		for (i = 1; i < output.count(); i++) delete output[i - 1];
		Left = NULL;
		Right = NULL;
	}
	template <class T> BiTree<T>* BiTree<T>::copy(void)
	{
		vector<BiTree<T>*> output;
		size_t i;
		BiTree<T>* now, *temp;
		PostOrderTraversal(output);
		for (i = 0; i < output.count(); i++)
		{
			now = output[i];
			temp = new BiTree<T>;
			temp->Content.copy(now->Content);
			if (now->Left != NULL)
			{
				temp->Left = (BiTree<T>*)now->Left->tempLabel;
				now->Left->tempLabel = NULL;
			}
			if (now->Right != NULL)
			{
				temp->Right = (BiTree<T>*)now->Right->tempLabel;
				now->Right->tempLabel = NULL;
			}
			now->tempLabel = (void*)temp;
		}
		now = (BiTree<T>*)tempLabel;
		tempLabel = NULL;
		return now;
	}
	template <class T> void BiTree<T>::build(BiTree<T>* left_, BiTree<T>* right_)
	{
		Left = left_;
		Right = right_;
	}
	template <class T> T& BiTree<T>::content(void)
	{
		return Content;
	}
	template <class T> const T& BiTree<T>::content(void) const
	{
		return Content;
	}
	template <class T> BiTree<T>*& BiTree<T>::left(void)
	{
		return Left;
	}
	template <class T> BiTree<T>*& BiTree<T>::right(void)
	{
		return Right;
	}
	template <class T> void*& BiTree<T>::label(void)
	{
		return tempLabel;
	}
	template <class T> void BiTree<T>::PreOrderTraversal(vector<BiTree<T>*>& output)
	{
		vector<BiTree<T>*> stack;
		PreOrderTraversal(output, stack);
	}
	template <class T> void BiTree<T>::PreOrderTraversal(vector<BiTree<T>*>& output, vector<BiTree<T>*>& stack)
	{
		BiTree<T>* now;
		stack.clear();
		stack.append(this);
		while (stack.pop(now) != 0)
		{
			if (now != NULL)
			{
				output.append(now);
				stack.append(now->Right);
				stack.append(now->Left);
			}
		}
	}
	template <class T> void BiTree<T>::InOrderTraversal(vector<BiTree<T>*>& output)
	{
		vector<BiTree<T>*> stack;
		InOrderTraversal(output, stack);
	}
	template <class T> void BiTree<T>::InOrderTraversal(vector<BiTree<T>*>& output, vector<BiTree<T>*>& stack)
	{
		BiTree<T>* now;
		stack.clear();
		now = this;
		while (now != NULL || stack.count() != 0)
		{
			while (now != NULL)
			{
				stack.append(now);
				now = now->Left;
			}
			stack.pop(now);
			output.append(now);
			now = now->Right;
		}
	}
	template <class T> void BiTree<T>::PostOrderTraversal(vector<BiTree<T>*>& output)
	{
		vector<BiTree<T>*> stack;
		PostOrderTraversal(output, stack);
	}
	template <class T> void BiTree<T>::PostOrderTraversal(vector<BiTree<T>*>& output, vector<BiTree<T>*>& stack)
	{
		BiTree<T>* now;
		BiTree<T>* LeftMost, * LastOutput;
		stack.clear();
		now = this;
		LastOutput = NULL;
		while (now != NULL || stack.count() != 0)
		{
			while (now != NULL)
			{
				stack.append(now);
				now = now->Left;
			}
			LeftMost = stack.top();
			if (LeftMost->Right == NULL || LeftMost->Right == LastOutput)
			{
				output.append(LeftMost);
				LastOutput = LeftMost;
				stack.pop();
			}
			else
				now = LeftMost->Right;
		}
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
		size_t should, new_size;
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
	template <class T> T* buffer<T>::ptr(void)
	{
		//append('\0');
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
namespace hyperlex
{
	template <class T> list<T>::list()
	{
		Count = 0;
		Size = 0;
		content = NULL;
	}
	template <class T> list<T>::~list()
	{
		Count = 0;
		Size = 0;
		free(content);
		content = NULL;
	}
	template <class T> void list<T>::append(const T& element)
	{
		size_t should;
		should = append();
		content[should] = element;
	}
	template <class T> size_t list<T>::SearchAppend(const T& element)
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
	template <class T> void list<T>::refresh(void)
	{
		Count = 0;
	}
	template <class T> void list<T>::refresh(size_t NewCount)
	{
		content = (T*)realloc(content, NewCount * sizeof(T));
		Size = NewCount;
		Count = NewCount;
	}
	template <class T> void list<T>::renew(size_t NewCount)
	{
		if (NewCount > Size)
		{
			//std::cout <<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<< std::endl;
			//std::cout << "NewCount: " << NewCount << "Size: " << Size << std::endl;
			//std::cout << "old content: " << content << std::endl;
			content = (T*)realloc(content, NewCount * sizeof(T));
			//std::cout << "new content: " << content << std::endl;
			Size = NewCount;
		}
		Count = NewCount;
	}
	template <class T> void list<T>::reshape(size_t NewSize)
	{
		if (NewSize <= Count) Count = NewSize;
		content = (T*)realloc(content, NewSize * sizeof(T));
		Size = NewSize;
	}
	template <class T> size_t list<T>::pop(T& element)
	{
		if (Count != 0)
		{
			Count -= 1;
			element = content[Count];
			return Count + 1;
		}
		return Count;
	}
	template <class T> void list<T>::pop(void)
	{
		if (Count != 0) Count -= 1;
	}
	template <class T> size_t list<T>::append(void)
	{
		size_t should, new_size;
		if (Count >= Size)
		{
			new_size = (Size + Size / 4 + 8);
			content = (T*)realloc(content, new_size * sizeof(T));
			Size = new_size;
		}
		should = Count;
		Count += 1;
		return should;
	}
	template <class T> size_t list<T>::count(void) const
	{
		return Count;
	}
	template <class T> size_t list<T>::size(void) const
	{
		return Size;
	}
	template <class T> void list<T>::swap(list<T>& other)
	{
		T* middle;
		middle = other.content;
		other.content = content;
		content = middle;

		size_t MM;

		MM = other.Count;
		other.Count = Count;
		Count = MM;

		MM = other.Size;
		other.Size = Size;
		Size = MM;
	}
	template <class T> const T* list<T>::vector(void)const
	{
		return content;
	}
	template <class T> T* list<T>::vector(void)
	{
		return content;
	}
	template <class T> const T* list<T>::vector(size_t offset)const
	{
		return content + offset;
	}
	template <class T> T* list<T>::vector(size_t offset)
	{
		return content + offset;
	}
	template <class T> const T& list<T>::operator[](size_t target) const
	{
		return content[target];
	}
	template <class T> T& list<T>::operator[](size_t target)
	{
		return content[target];
	}
	template <class T> void list<T>::value(const T& element)
	{
		size_t i;
		for (i = 0; i < Count; i++)
			content[i] = element;
	}
	template <class T> const T& list<T>::top(void) const
	{
		size_t target;
		target = Count != 0 ? Count - 1 : 0;
		return content[target];
	}
	template <class T> T& list<T>::top(void)
	{
		size_t target;
		target = Count != 0 ? Count - 1 : 0;
		return content[target];
	}


	template <class T> matlist<T>::matlist()
	{
		Row = 0;
		Column = 1;
		Length = 0;
		//fprintf(stdout, "matlist(): row(): %zu, column(): %zu,  length(): %zu.\n", Row, Column, Length);
		content = NULL;
	}
	template <class T> matlist<T>::matlist(size_t column)
	{
		Row = 0;
		Column = column;
		Length = 0;
		content = NULL;
	}
	template <class T> matlist<T>::~matlist()
	{
		Row = 0;
		Column = 0;
		Length = 0;
		free(content);
		content = NULL;
	}
	template <class T> void matlist<T>::refresh(void)
	{
		Row = 0;
	}
	template <class T> void matlist<T>::refresh(size_t row, size_t column, size_t length)
	{
		Row = row;
		Column = column;
		Length = length;
		//free(content);
		//fprintf(stdout, "refresh: row(): %zu, column(): %zu,  length(): %zu.\n", row, column, length);
		content = (T*)realloc(content, column * length * sizeof(T));
		//if(content == NULL) fprintf(stdout, "refresh: content == NULL.\n");
		//else fprintf(stdout, "refresh: content == %zu.\n", (size_t)content);
	}
	template <class T> void matlist<T>::refresh(size_t column, size_t length)
	{
		Row = 0;
		Column = column;
		Length = length;
		//free(content);
		content = (T*)realloc(content, column * length * sizeof(T));
	}
	template <class T> void matlist<T>::append(const T* element)
	{
		size_t i;
		T* line;
		line = append();
		for (i = 0; i < Column; i++) line[i] = element[i];

	}
	//size_t pop(T* element);
	template <class T> T* matlist<T>::append(void)
	{
		size_t should, new_size;
		if (Row >= Length)
		{
			new_size = (Length + Length / 4 + 8);
			content = (T*)realloc(content, Column * new_size * sizeof(T));
			Length = new_size;
			//std::cout << "content: " << content << ", new_size: " << new_size << "Row: " << Row << std::endl;
		}
		should = Row;
		Row += 1;
		return content + should * Column;
	}

	template <class T> size_t matlist<T>::SearchAdd(const T* element)
	{
		size_t i, j;
		const T* temp;
		for (i = 0; i < Row; i++)
		{
			temp = content + i * Column;
			for (j = 0; j < Column; j++)
				if (temp[j] != element[j]) break;
			if (j == Column) return i;
		}
		append(element);
		return Row - 1;
	}
	template <class T> size_t matlist<T>::column(void) const
	{
		return Column;
	}
	template <class T> size_t matlist<T>::row(void) const
	{
		return Row;
	}
	template <class T> size_t matlist<T>::length(void) const
	{
		return Length;
	}
	template <class T> void matlist<T>::swap(matlist<T>& other)
	{
		T* middle;
		middle = other.content;
		other.content = content;
		content = middle;

		size_t MM;

		MM = other.Column;
		other.Column = Column;
		Column = MM;

		MM = other.Row;
		other.Row = Row;
		Row = MM;

		MM = other.Length;
		other.Length = Length;
		Length = MM;
	}
	template <class T> const T* matlist<T>::operator[](size_t target) const
	{

		return content + target * Column;
	}
	template <class T> T* matlist<T>::operator[](size_t target)
	{
		//fprintf(stdout, "template <class T> T* matlist<T>::operator[](size_t target)\n");
		//fprintf(stdout, "target: %zu, Column: %zu, content: %zu\n", target, Column, (size_t)content);
		return content + target * Column;
	}
	template <class T> void matlist<T>::value(const T element)
	{
		size_t i;
		for (i = 0; i < Row * Column; i++)
		{
			content[i] = element;
		}
	}

	template <class T> vec<T>::vec()
	{
		Size = 0;
		content = NULL;
	}
	template <class T> vec<T>::~vec()
	{
		Size = 0;
		free(content);
		content = NULL;
	}
	template <class T> size_t vec<T>::size(void) const
	{
		return Size;
	}
	template <class T> T* vec<T>::vector(void)
	{
		return content;
	}
	template <class T> T& vec<T>::operator[](size_t target)
	{
		return content[target];
	}
	template <class T> T& vec<T>::operator[](const size_t target) const
	{
		return content[target];
	}
	template <class T> void vec<T>::Malloc(size_t NewSize)
	{
		Size = NewSize;
		content = (T*)malloc(NewSize * sizeof(T));
	}
	template <class T> void vec<T>::Realloc(size_t NewSize)
	{
		content = (T*)realloc(content, NewSize * sizeof(T));
		Size = NewSize;
	}
	template <class T> void vec<T>::Free(void)
	{
		Size = 0;
		free(content);
		content = NULL;
	}


}
namespace hyperlex
{
	template <class Cg, class Ca> Dgraph<Cg, Ca>::Dgraph()
	{

	}
	template <class Cg, class Ca> Dgraph<Cg, Ca>::~Dgraph()
	{

	}
	template <class Cg, class Ca> Cg& Dgraph<Cg, Ca>::operator[](size_t target)
	{
		return vertice.content[target].content;
	}
	template <class Cg, class Ca> void Dgraph<Cg, Ca>::refresh(void)
	{
		vertice.refresh();
		arcs.clear();
	}
	template <class Cg, class Ca> void Dgraph<Cg, Ca>::refresh(size_t newlength)
	{
		size_t i;
		vertice.refresh(newlength);
		arcs.clear();
		for (i = 0; i < vertice.count(); i++)
		{
			vertice[i].first = SizeMax;
			vertice[i].rear = SizeMax;
		}
	}
	template <class Cg, class Ca> size_t Dgraph<Cg, Ca>::FirstArc(size_t site) const
	{
		return vertice[site].first;
	}
	template <class Cg, class Ca> size_t Dgraph<Cg, Ca>::NextArc(size_t site) const
	{
		return arcs[site].next;
	}
	template <class Cg, class Ca> size_t Dgraph<Cg, Ca>::ArcTo(size_t site) const
	{
		return arcs[site].to;
	}
	template <class Cg, class Ca> size_t Dgraph<Cg, Ca>::ArcCount(size_t site) const
	{
		size_t count_, first_;
		count_ = 0;
		first_ = vertice[site].first;
		while (first_ != SizeMax)
		{
			first_ = arcs[first_].next;
			count_ += 1;
		}
		return count_;
	}
	template <class Cg, class Ca> size_t Dgraph<Cg, Ca>::append(size_t from, size_t to, const Ca& C)
	{
		size_t should;
		should = arcs.expand();
		arcs[should].from = from;
		arcs[should].to = to;
		arcs[should].content = C;
		arcs[should].next = SizeMax;
		if (vertice[from].first == SizeMax)
		{
			vertice[from].first = should;
			vertice[from].rear = should;
		}
		else
		{
			arcs[vertice[from].rear].next = should;
			vertice[from].rear = should;
		}
		return should;
	}
	template <class Cg, class Ca> void Dgraph<Cg, Ca>::append(const Dgraph<Cg, Ca>& right, size_t VerticeOffset)
	{
		size_t ArcsOffset;
		size_t i, length, temp;
		ArcsOffset = arcs.rear();
		length = right.vertice.count();
		for (i = 0; i < length; i++)
		{
			temp = (right.vertice[i].first == SizeMax ? SizeMax : right.vertice[i].first + ArcsOffset);
			vertice[i + VerticeOffset].first = temp;
			temp = (right.vertice[i].rear == SizeMax ? SizeMax : right.vertice[i].rear + ArcsOffset);
			vertice[i + VerticeOffset].rear = temp;
		}
		//begin = arcs.site();
		arcs.append(right.arcs);
		//arcs.expand(right.arcs.count());
		length = right.arcs.count();
		for (i = ArcsOffset; i < length + ArcsOffset; i++)
		{
			arcs[i].from += VerticeOffset;
			arcs[i].to += VerticeOffset;
			temp = (arcs[i].next == SizeMax ? SizeMax : arcs[i].next + ArcsOffset);
			arcs[i].next = temp;
		}
	}
	template <class Cg, class Ca> void Dgraph<Cg, Ca>::Demo(FILE* fp)const 
	{
		fprintf(fp, "vertice.count() = %llu\n", (long long int)vertice.count());
		fprintf(fp, "arcs.count() = %llu\n", (long long int)arcs.count());
		//vertice.refresh();
		//arcs.refresh();
	}

	template <class Cg> LRNFA<Cg>::LRNFA()
	{

	}
	template <class Cg> LRNFA<Cg>::~LRNFA()
	{

	}
	template <class Cg> Cg& LRNFA<Cg>::operator[](size_t target)
	{
		return vertice[target].content;
	}
	template <class Cg> const Cg& LRNFA<Cg>::operator[](size_t target) const
	{
		return vertice[target].content;
	}
	template <class Cg> void LRNFA<Cg>::refresh(void)
	{
		vertice.refresh();
		arcs.refresh();
	}
	template <class Cg> void LRNFA<Cg>::refresh(size_t newlength)
	{
		size_t i;
		vertice.refresh(newlength);
		arcs.clear();
		for (i = 0; i < vertice.count(); i++)
		{
			vertice[i].first = SizeMax;
			vertice[i].rear = SizeMax;
		}
	}
	template <class Cg> size_t LRNFA<Cg>::VortexAppend(const Cg& element)
	{
		size_t length;
		vortex V;
		length = vertice.count();
		V.first = SizeMax;
		V.rear = SizeMax;
		V.content = element;
		vertice.append(V);
		return length;
	}
	template <class Cg> size_t LRNFA<Cg>::FirstArc(size_t site) const
	{
		return vertice[site].first;
	}
	template <class Cg> size_t LRNFA<Cg>::NextArc(size_t site) const
	{
		return arcs[site].next;
	}
	template <class Cg> size_t LRNFA<Cg>::ArcTo(size_t site) const
	{
		return arcs[site].to;
	}
	template <class Cg> size_t LRNFA<Cg>::ArcCount(size_t site) const
	{
		size_t count_, first_;
		count_ = 0;
		first_ = vertice[site].first;
		while (first_ != SizeMax)
		{
			first_ = arcs[first_].next;
			count_ += 1;
		}
		return count_;
	}
	template <class Cg> size_t LRNFA<Cg>::append(size_t from, size_t to, bool Epsilon, long long int Symbol)
	{
		size_t should;
		should = arcs.expand();
		arcs[should].from = from;
		arcs[should].to = to;
		arcs[should].epsilon = Epsilon;
		arcs[should].symbol = Symbol;
		arcs[should].next = SizeMax;
		if (vertice[from].first == SizeMax)
		{
			vertice[from].first = should;
			vertice[from].rear = should;
		}
		else
		{
			arcs[vertice[from].rear].next = should;
			vertice[from].rear = should;
		}
		return should;
	}
	template <class Cg> void LRNFA<Cg>::Demo(FILE* fp) const
	{
		fprintf(fp, "vertice.count() = %llu\n", (long long int)vertice.count());
		fprintf(fp, "arcs.count() = %llu\n", (long long int)arcs.count());
		//vertice.refresh();
		//arcs.refresh();
	}
	template <class Cg> void LRNFA<Cg>::initial(bool* state, list<size_t>& stack) const
	{
		size_t i;
		state[0] = true;
		for (i = 1; i < vertice.count(); i++) state[i] = false;
		closure(state, stack);
	}
	template <class Cg> void LRNFA<Cg>::closure(bool* state, list<size_t>& stack) const
	{
		size_t delta;
		size_t Scount;
		size_t i;
		size_t now, arc, temp;
		Scount = 0;
		delta = 0;
		stack.refresh();
		for (i = 0; i < vertice.count(); i++)
			if (state[i])
			{
				Scount += 1;
				stack.append(i);
			}
		while (stack.pop(now) != 0)
		{
			arc = vertice[now].first;
			while (arc != SizeMax)
			{
				if (arcs[arc].epsilon == true)
				{
					temp = arcs[arc].to;
					if (!state[temp])
					{
						state[temp] = true;
						stack.append(temp);
						delta += 1;
					}
				}
				arc = arcs[arc].next;
			}
		}
	}
	template <class Cg> size_t LRNFA<Cg>::move(const bool* from, bool* to, long long int ele, list<size_t>& stack) const
	{
		size_t i;
		size_t Scount;
		size_t now, arc;
		Scount = 0;
		for (i = 0; i < vertice.count(); i++) to[i] = false;
		for (i = 0; i < vertice.count(); i++)
			if (from[i])
			{
				arc = vertice[i].first;
				while (arc != SizeMax)
				{
					if (arcs[arc].symbol == ele && arcs[arc].epsilon == false)
					{
						now = arcs[arc].to;
						Scount += (to[now] ? 0 : 1);
						to[now] = true;
					}
					arc = arcs[arc].next;
				}
			}
		closure(to, stack);
		return Scount;
	}
	template <class Cg> void LRNFA<Cg>::build(matlist<int>& sheet, matlist<bool>& Dstates, size_t NonTcount, size_t Tcount) const
	{
		list<size_t> stack;
		list<size_t> auxiliary;
		//matlist<bool> Dstates(graph.vertice.count());
		size_t now, old;
		bool* state;
		vec<bool> to;
		long long int ele, symbol;
		size_t Scount, TotalSymbol;
		TotalSymbol = NonTcount + Tcount;
		sheet.refresh(0, TotalSymbol, 4);
		Dstates.refresh(0, vertice.count(), 4);
		to.Malloc(vertice.count());
		auxiliary.reshape(vertice.count());
		auxiliary.refresh();
		state = Dstates.append();

		initial(state, auxiliary);
		//return;
		stack.append(0);
		sheet.append();
		//return;
		while (stack.pop(now) != 0)
		{
			//Demo(stdout, state, nfa.StateAmount);
			for (ele = 0; (size_t)ele < TotalSymbol; ele++)
			{
				state = Dstates[now];
				symbol = ele < NonTcount ? ele : -(long long int)((ele + 1) - NonTcount);
				Scount = move(state, to.vector(), symbol, auxiliary);
				//std::cout << "Scount: " << Scount << ");" << std::endl;
				if (Scount == 0) sheet[now][(size_t)ele] = -1;
				else
				{
					//std::cout << "Scount: " << Scount << ", symbol = " << symbol << std::endl;
					old = Dstates.row();
					sheet[now][(size_t)ele] = (int)(Dstates.SearchAdd(to.vector()));
					if (old != Dstates.row())
					{
						sheet.append();
						stack.append(old);
					}
					//if (old != Dstates.row()) Demo(stdout, Dstates[old], nfa.StateAmount);
				}
				//std::cout << "Scount: " << Scount << ");" << std::endl;
			}
		}
		//std::cout << "Dstates.row(): " << Dstates.row() << std::endl;
		//Demo(stdout, Dstates[old], nfa.StateAmount);
		//count = Dstates.row();
		//acceptSet(Dstates, nfa);
	}
}
namespace hyperlex
{

}
namespace hyperlex
{
	int PostfixSwitch(char c);
	bool IfHexa(char c);
	int SwitchHexa(char c);
	int SwitchOcta(char c);


	char dequeue(const char* list, size_t end, size_t& head);

	long int IntGet(const char* list, size_t end, size_t& head);
	double RealGet(int& state, const char* list, size_t end, size_t& head);
	char CharGet(const char* list, size_t end, size_t& head);

	char CharGet(int& error, const char* list, size_t end, size_t& head);
}

#include <cstdlib>
#include <cassert>
#include <cstdio>
#include <string.h>
//#include <string>
namespace hyperlex
{
	// StringPool 类实现
	// 这是一个字符串池，用于存储唯一的字符串并提供快速访问
	// 它使用哈希表和向量来管理字符串，确保每个字符串都有一个唯一的ID。
	class StringPool
	{
	private:
		// 哈希表节点结构
		struct HashNode
		{
			char* key;// 存储字符串
			size_t id;// 唯一ID
			HashNode* next;// 指向下一个节点的指针

			HashNode(const char* k, size_t idx) {
				key = strdup(k);
				id = idx;
				next = NULL;
			}

			~HashNode() {
				free(key);
			}
		};
		//哈希表中有字符串唯一的存储副本，
		//stringArray不存储字符串的副本，而是存储指向字符串的指针。
		static const size_t INITIAL_BUCKETS = 64;// 初始桶数量
		static const double LOAD_FACTOR;// 哈希表负载因子

		// 哈希表相关成员
		HashNode** buckets;
		size_t bucketCount;// 桶数量
		size_t nodeCount;// 哈希表节点数量

		// 向量相关成员
		char** stringArray;// 存储字符串的数组
		size_t arraySize;// 数组当前大小
		size_t arrayCapacity;// 数组容量

		// 哈希函数 (djb2)
		inline size_t hash(const char* str) const
		{
			size_t hash = 5381;
			int c;
			while ((c = *str++)) {
				//hash = ((hash << 4) + hash) ^ c;//Fermat prime  17: 0.2
				hash = ((hash << 5) + hash) ^ c;//: 0.3
				//hash = ((hash << 8) + hash) ^ c;//Fermat prime 257 : 0.7
				//hash = (hash >> 16) ^ (hash & 0xFFFF); // 混合高位与低位[2,6](@ref)
				hash = (hash * 101) ^ ((hash << 5) + (hash >> 2)) ^ c;
			}
			return hash;
		}

		// 在桶中查找字符串
		inline HashNode* findInBucket(size_t index, const char* key) const
		{
			HashNode* current = buckets[index];
			while (current)
			{
				if (strcmp(current->key, key) == 0)
				{
					return current;
				}
				current = current->next;
			}
			return NULL;
		}

		// 扩展哈希表
		inline void resizeHashTable()
		{
			size_t newBucketCount = bucketCount * 2;
			HashNode** newBuckets = new HashNode * [newBucketCount]();

			for (size_t i = 0; i < bucketCount; i++)
			{
				HashNode* current = buckets[i];
				while (current) {
					HashNode* next = current->next;
					size_t newIndex = hash(current->key) % newBucketCount;

					current->next = newBuckets[newIndex];
					newBuckets[newIndex] = current;

					current = next;
				}
			}
			//std::cout << "Resizing hash table from " << bucketCount << " to " << newBucketCount << " buckets.\n";
			delete[] buckets;
			buckets = newBuckets;
			bucketCount = newBucketCount;
		}

		// 扩展数组
		inline void resizeArray()
		{
			size_t newCapacity = arrayCapacity ? arrayCapacity * 2 : 8;
			char** newArray = new char* [newCapacity];

			if (stringArray)
			{
				for (size_t i = 0; i < arraySize; i++) {
					newArray[i] = stringArray[i];
				}
				delete[] stringArray;
			}

			stringArray = newArray;
			arrayCapacity = newCapacity;
		}

	public:
		StringPool()
		{
			buckets = new HashNode * [INITIAL_BUCKETS]();
			bucketCount = INITIAL_BUCKETS;
			nodeCount = 0;

			stringArray = NULL;
			arraySize = 0;
			arrayCapacity = 0;
		}

		~StringPool() {
			// 清理哈希表
			for (size_t i = 0; i < bucketCount; i++) {
				HashNode* current = buckets[i];
				while (current) {
					HashNode* next = current->next;
					delete current;
					current = next;
				}
			}
			delete[] buckets;

			// 清理数组
			delete[] stringArray;
		}

		// 添加字符串，返回唯一ID
		inline size_t append(const char* src) 
		{
			if (!src) return -1;

			size_t index = hash(src) % bucketCount;
			HashNode* node = findInBucket(index, src);
			if (node) {
				return node->id;
			}

			if (arraySize >= arrayCapacity)
			{
				resizeArray();
			}

			//char* newStr = strdup(src);
			const size_t newId = arraySize;
			arraySize++;
			//stringArray[arraySize++] = newStr;

			HashNode* newNode = new HashNode(src, newId);
			//HashNode* newNode = new HashNode(newStr, newId);
			newNode->next = buckets[index];
			buckets[index] = newNode;
			nodeCount++;

			if (static_cast<double>(nodeCount) / bucketCount > LOAD_FACTOR) {
				resizeHashTable();
			}

			stringArray[newId] = newNode->key;

			return newId;
		}

		// 根据ID获取字符串
		inline const char* getString(size_t id) const {
			if (id >= arraySize) return NULL;
			return stringArray[id];
		}

		inline const char* operator[](size_t id) const {
			return getString(id);
		}

		// 检查字符串是否存在于池中
		inline bool contains(const char* src) const {
			if (!src) return false;
			size_t index = hash(src) % bucketCount;
			return findInBucket(index, src) != NULL;
		}

		// 获取池中字符串数量
		inline size_t size() const {
			return arraySize;
		}

		// 获取哈希表元素数量
		inline size_t count() const {
			return nodeCount;
		}

		// 调试函数
		inline void demo(FILE* fp = stdout) const
		{
			fprintf(fp, "StringPool contains %zu strings:\n", arraySize);
			fprintf(fp, "LOAD FACTOR is %.6lf\n", LOAD_FACTOR);
			size_t colli = countCollisions();
			fprintf(fp, "Count of collisions %zu, collisions rate: %.6lf\n", colli, (double)colli / bucketCount);
			for (size_t i = 0; i < arraySize; i++) {
				fprintf(fp, "ID: %zu, String: %s\n", i, stringArray[i]);
			}

			for (size_t i = 0; i < arraySize; i++) {
				if (!contains(stringArray[i])) {
					fprintf(fp, "Warning: String '%s' not found in hash table!\n", stringArray[i]);
				}
			}

			if (arraySize != nodeCount) {
				fprintf(fp, "Warning: Size mismatch between vector (%zu) and hash table (%zu)!\n",
					arraySize, nodeCount);
			}
		}

		// 统计哈希冲突总数
		inline size_t countCollisions() const {
			size_t collisionCount = 0;
			for (size_t i = 0; i < bucketCount; ++i) {
				HashNode* node = buckets[i];
				if (node) {
					// 跳过桶中的首节点，只统计链表中的后续节点（冲突节点）
					node = node->next;
					while (node) {
						++collisionCount;
						node = node->next;
					}
				}
			}
			return collisionCount;
		}
		inline void copy(const StringPool& source)
		{
			// 1. 自赋值检查
			if (this == &source) return;

			// 2. 清理当前对象（避免内存泄漏）
			clear();
			for (size_t i = 0; i < source.size(); i++)
			{
				// 获取源对象中当前索引的字符串
				const char* str = source.getString(i);
				append(str);
			}

		}
		inline void append(const StringPool& source, hyperlex::vector<size_t>& NewId)
		{
			// 调整输出向量大小以匹配源对象的字符串数量
			NewId.recount(source.size());

			// 遍历源对象中的所有字符串
			for (size_t i = 0; i < source.size(); i++)
			{
				// 获取源对象中当前索引的字符串
				const char* str = source.getString(i);
				if (!str) {
					NewId[i] = static_cast<size_t>(-1); // 处理无效字符串
					//continue;
				}
				else
				{
					NewId[i] = this->append(str);
				}
				// 在当前对象中查找字符串
				//size_t bucketIndex = hash(str) % bucketCount;
				//HashNode* node = findInBucket(bucketIndex, str);

				//if (node) {
					// 字符串已存在：使用现有ID
				///	NewId[i] = node->id;
				//}
				//else {
					// 字符串不存在：添加到当前对象并获取新ID
				//	NewId[i] = this->append(str);
				//}
			}
		}
		inline void clear()
		{
			// 1. 释放哈希表
			for (size_t i = 0; i < bucketCount; ++i) {
				HashNode* current = buckets[i];
				while (current)
				{
					HashNode* next = current->next;
					delete current; // 析构HashNode（内部释放key）
					current = next;
				}
			}
			delete[] buckets;// 1.1 重新分配哈希表和字符串数组
			buckets = new HashNode * [INITIAL_BUCKETS]();
			bucketCount = INITIAL_BUCKETS;
			nodeCount = 0;

			delete[] stringArray;
			stringArray = NULL;
			arraySize = 0;
			arrayCapacity = 0;


		}
		friend void test_string_pool();
	};
	
}

#undef SizeMax 
#undef CharSize

#endif

/*
	template <class T> class stack
	{
	private:
		size_t Top;
		size_t Size;
		T* content;
	public:
		stack();
		~stack();
		void clear(void);
		void reshape(size_t NewSize);
		void push(const T& element);
		size_t pop(T& element);
		size_t size(void) const;
		size_t top(void) const;
		T& operator[](size_t target);
	};

		template <class T> stack<T>::stack(void)
	{
		Top = 0;
		Size = 0;
		content = NULL;
	}
	template <class T> stack<T>::~stack(void)
	{
		Top = 0;
		Size = 0;
		free(content);
		content = NULL;
	}
	template <class T> void stack<T>::clear(void)
	{
		Top = 0;
	}
	template <class T> void stack<T>::reshape(size_t NewSize)
	{
		content = (T*)realloc(content, NewSize * sizeof(T));
		Size = NewSize;
		Top = Top <= Size ? Top : Size;
	}
	template <class T> void stack<T>::push(const T& element)
	{
		content[Top] = element;
		Top += 1;
	}
	template <class T> size_t stack<T>::pop(T& element)
	{
		if (Top != 0)
		{
			Top -= 1;
			element = content[Top];
			return Top + 1;
		}
		return Top;
	}
	template <class T> size_t stack<T>::size(void) const
	{
		return Size;
	}
	template <class T> size_t stack<T>::top(void) const
	{
		return Top;
	}
	template <class T> T& stack<T>::operator[](size_t target)
	{
		return content[target];
	}

*/

/*
template <class T> size_t Bitree<T>::append_t(const Bitree<T>& source, buffer<size_t>& output, list<size_t>& s, bool key)
	{
		//size_t newHead;
		size_t now, new_node, temp;
		new_node = 0;
		//std::cout << "new_node site: " << &new_node << std::endl;
		//std::cout << "%rbx in stack: " << *(&new_node + 6) << std::endl;
		//std::cout << "%rbx in stack: " << *(&new_node + 5) << std::endl;
		//std::cout << "%rbx in stack: " << *(&new_node + 4) << std::endl;
		//std::cout << "new_node: " << new_node << std::endl;
		s.refresh();
		//std::cout << "%rbx in stack: " << *(&new_node + 4) << std::endl;
		output.refresh();
		//std::cout << "%rbx in stack: " << *(&new_node + 4) << std::endl;
		new_node = SizeMax;
		source.postorderTraversal(output, s);
		//std::cout << "%rbx in stack: " << *(&new_node + 4) << std::endl;
		s.refresh();
		//std::cout << "%rbx in stack: " << *(&new_node + 4) << std::endl;
		s.renew(source.Size);
		//std::cout << "%rbx in stack: " << *(&new_node + 4) << std::endl;
		while (output.dequeue(now))
		{
			new_node = NewNodeOffset();
			//std::cout << "new_node: " << new_node << std::endl;
			//std::cout << "now: " << now << std::endl;
			temp = source.Nodes[now].left;
			(Nodes + new_node)->left = (temp == SizeMax ? SizeMax : s[temp]);
			//std::cout << "source.Nodes[now].left: " << temp << std::endl;
			//std::cout << "(Nodes + new_node)->left: " << (Nodes + new_node)->left << std::endl;
			temp = source.Nodes[now].right;
			(Nodes + new_node)->right = (temp == SizeMax ? SizeMax : s[temp]);
			//std::cout << "source.Nodes[now].right: " << temp << std::endl;
			//std::cout << "(Nodes + new_node)->right: " << (Nodes + new_node)->right << std::endl;
			(Nodes + new_node)->content = source.Nodes[now].content;
			s[now] = new_node;
			//if (newHead == SizeMax) newHead = new_node;
			//newHead = new_node;
		}
		//std::cout << "newHead: " << newHead << std::endl;
		//std::cout << "return newHead: " << new_node << "\n\n" << std::endl;
		//std::cout << "%rbx in stack: " << *(&new_node + 4) << std::endl;
		return new_node;
	}
	template <class T> void Bitree<T>::append_t(const Bitree<T>& L, const Bitree<T>& R, size_t parent, bool key)
	{
		buffer<size_t> output;
		list<size_t> s;
		size_t temp;
		size_t as[15];
		//if(key) std::cout << "============key is on: ============" << std::endl;
		//else std::cout << "============key is off: ============" << std::endl;
		removal((Nodes + parent)->left, output, s);
		removal((Nodes + parent)->right, output, s);
		//std::cout << "parent: " << parent << std::endl;
		//std::cout << "(Nodes + parent)->left: " << (Nodes + parent)->left << std::endl;
		(Nodes + parent)->left = append(L, output, s);
		//std::cout << "parent: " << parent << std::endl;
		temp = (Nodes + parent)->right;
		//std::cout << "(Nodes + parent)->left: " << (Nodes + parent)->left << std::endl;
		//std::cout << "(Nodes + parent)->right: " << temp << std::endl;
		(Nodes + parent)->right = 10007;
		//std::cout << "(Nodes + parent)->right: " << (Nodes + parent)->right << std::endl;
		//std::cout << "temp ptr: " << &temp << std::endl;
		//std::cout << "(Nodes + parent) ptr: " << (Nodes + parent) << std::endl;
		//std::cout << "(Nodes + parent)->left ptr: " << (size_t) &((Nodes + parent)->left) << std::endl;
		//std::cout << "(Nodes + parent)->right ptr: " << &((Nodes + parent)->right) << std::endl;
		//if(key) std::cout << "append(R, output, s);: " << append(R, output, s) << std::endl;
		//if (key)
		//{
			//temp = append(R, output, s);
			//Nodes[parent].right = temp;
		//}
		if (key) Nodes[parent].right = append_t(R, output, s, key);
		else Nodes[parent].right = append(R, output, s);


		//std::cout << "parent: " << parent << std::endl;
		//std::cout << "(Nodes + parent)->right(temp): " << temp << std::endl;
		//std::cout << "(Nodes + parent)->right: " << Nodes[parent].right << std::endl;
		if (key) exit(1);
	}
*/
