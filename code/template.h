#include <stdlib.h>
#include <cassert>
#include <new>
namespace TemplateSelf
{
	/* ================= forward decl ================= */

	template<class T> class vector;
	template<class T> class array;
	template<class T> class buffer;

	

	/* ================= traits ================= */
	//template<bool B> struct _hx_bool_type {};

	// 1) SFINAE 检测 _hx_bitwise_relocatable_tag
	template<class T> struct is_bitwise_relocatable {
	private:
		typedef char yes;
		struct no { char _[2]; };
		template<class U> static yes probe(typename U::_hx_bitwise_relocatable_tag*);
		template<class U> static no  probe(...);
	public:
		enum { value = (sizeof(probe<T>(0)) == sizeof(yes) ? 1 : 0) };
	};

	// 2) SFINAE 检测 _hx_trivial_destruct_tag
	template<class T> struct is_trivially_destructible_like {
	private:
		typedef char yes;
		struct no { char _[2]; };
		template<class U> static yes probe(typename U::_hx_trivial_destruct_tag*);
		template<class U> static no  probe(...);
	public:
		enum { value = (sizeof(probe<T>(0)) == sizeof(yes) ? 1 : 0) };
	};

	//template<class T> struct need_explicit_move { enum { value = 0 }; };
	// 2) 用 SFINAE 实现主 traits 的检测（保留你已有的其它专用化不变）
	template<class T> struct need_explicit_move {
	private:
		typedef char yes; 
		struct no { char _[2]; };
		template<class U> static yes probe(typename U::_hx_need_explicit_move_tag*);
		template<class U> static no  probe(...);
	public:
		enum { value = (sizeof(probe<T>(0)) == sizeof(yes) ? 1 : 0)};
	};

	/* 基础类型与指针：位可重定位 + 平凡析构 */
	template<> struct is_bitwise_relocatable<bool> { enum { value = 1 }; };
	template<> struct is_bitwise_relocatable<int> { enum { value = 1 }; };
	template<> struct is_bitwise_relocatable<unsigned int> { enum { value = 1 }; };
	template<> struct is_bitwise_relocatable<long> { enum { value = 1 }; };
	template<> struct is_bitwise_relocatable<unsigned long> { enum { value = 1 }; };
	template<> struct is_bitwise_relocatable<long long> { enum { value = 1 }; };
	template<> struct is_bitwise_relocatable<unsigned long long> { enum { value = 1 }; };
	//template<> struct is_bitwise_relocatable<size_t> { enum { value = 1 }; };
	template<> struct is_bitwise_relocatable<char> { enum { value = 1 }; };
	template<> struct is_bitwise_relocatable<unsigned char> { enum { value = 1 }; };
	template<> struct is_bitwise_relocatable<float> { enum { value = 1 }; };
	template<> struct is_bitwise_relocatable<double> { enum { value = 1 }; };
	template<class T> struct is_bitwise_relocatable<T*> { enum { value = 1 }; };

	template<> struct is_trivially_destructible_like<bool> { enum { value = 1 }; };
	template<> struct is_trivially_destructible_like<int> { enum { value = 1 }; };
	template<> struct is_trivially_destructible_like<unsigned int> { enum { value = 1 }; };
	template<> struct is_trivially_destructible_like<long> { enum { value = 1 }; };
	template<> struct is_trivially_destructible_like<unsigned long> { enum { value = 1 }; };
	template<> struct is_trivially_destructible_like<long long> { enum { value = 1 }; };
	template<> struct is_trivially_destructible_like<unsigned long long> { enum { value = 1 }; };
	//template<> struct is_trivially_destructible_like<size_t> { enum { value = 1 }; };
	template<> struct is_trivially_destructible_like<char> { enum { value = 1 }; };
	template<> struct is_trivially_destructible_like<unsigned char> { enum { value = 1 }; };
	template<> struct is_trivially_destructible_like<float> { enum { value = 1 }; };
	template<> struct is_trivially_destructible_like<double> { enum { value = 1 }; };
	template<class T> struct is_trivially_destructible_like<T*> { enum { value = 1 }; };

	/* 扩展：让带顶层 cv 的指针也视为可位搬迁 */
	template<class T> struct is_bitwise_relocatable<T* const> { enum { value = is_bitwise_relocatable<T*>::value }; };
	template<class T> struct is_bitwise_relocatable<T* volatile> { enum { value = is_bitwise_relocatable<T*>::value }; };
	template<class T> struct is_bitwise_relocatable<T* const volatile> { enum { value = is_bitwise_relocatable<T*>::value }; };

	template<class T> struct is_trivially_destructible_like<T* const> { enum { value = is_trivially_destructible_like<T*>::value }; };
	template<class T> struct is_trivially_destructible_like<T* volatile> { enum { value = is_trivially_destructible_like<T*>::value }; };
	template<class T> struct is_trivially_destructible_like<T* const volatile> { enum { value = is_trivially_destructible_like<T*>::value }; };

	/* 递归：vector<E> 仅当 E 也可位搬迁 */
	template<class E> struct is_bitwise_relocatable< vector<E> > {
		//enum { value = is_bitwise_relocatable<E>::value };
		enum { value = 0 };//为了安全起见，暂时关闭 vector 的位搬迁优化
	};
	template<class E> struct is_trivially_destructible_like< vector<E> > {
		enum { value = 0 }; /* 自身有非平凡析构逻辑，保守为 0 */
	};
	template<class E> struct need_explicit_move< vector<E> > { enum { value = 1 }; };

	template<class E> struct is_bitwise_relocatable< array<E> > { enum { value = 0 }; };
	template<class E> struct is_trivially_destructible_like< array<E> > { enum { value = 0 }; };
	template<class E> struct need_explicit_move< array<E> > { enum { value = 1 }; };

	template<class E> struct is_bitwise_relocatable< buffer<E> > { enum { value = 0 }; };
	template<class E> struct is_trivially_destructible_like< buffer<E> > { enum { value = 0 }; };
	template<class E> struct need_explicit_move< buffer<E> > { enum { value = 1 }; };


	//template<class E> struct need_explicit_move< gtreevNode<E> > { enum { value = need_explicit_move<E>::value }; };

	/* ============== move 派发（无 if 常量分支） ============== */
	template<class T, bool NeedMove> struct move_assign;
	template<class T>
	struct move_assign<T, true> {
		static void apply(T& dst, T& src) { dst.move(src); }
	};
	template<class T>
	struct move_assign<T, false> {
		static void apply(T& dst, T& src) { dst = src; }
	};
} 
namespace TemplateSelf
{
	/* ============== vector ================== */
	template <class T> class vector
	{
	protected:
		size_t Count;
		size_t Capacity;
		T* content;

		enum { use_realloc = is_bitwise_relocatable<T>::value };

		inline void destroy_range(size_t a, size_t b) {
			if (is_trivially_destructible_like<T>::value) return;
			if (!content) return;
			while (a < b) {
				(content + a)->~T();
				++a;
			}
		}
		inline void deallocate() {
			if (!content) return;
			free(content); /* 非位搬迁路径也使用 malloc/free 统一 */
			content = NULL;
		}
		inline void grow(size_t newCap) {
			if (newCap <= Capacity) return;
			if (use_realloc) {
				T* p = (T*)realloc((void*)content, newCap * sizeof(T));
				if (!p) ::abort();
				content = p;
				Capacity = newCap;
				return;
			}
			// 非位搬迁：新块 + (move or copy) + 析构旧
			T* newBuf = (T*)malloc(newCap * sizeof(T));
			if (!newBuf) ::abort();

			if (need_explicit_move<T>::value) {
				// 利用显式 move：减少深拷成本
				for (size_t i = 0; i < Count; ++i) {
					new (newBuf + i) T(); // 占位
					move_assign<T, need_explicit_move<T>::value>::apply(newBuf[i], content[i]);
				}
			}
			else {
				// 原拷贝路径
				for (size_t i = 0; i < Count; ++i) {
					new (newBuf + i) T(content[i]);
				}
				destroy_range(0, Count);
			}
			
			free(content);
			content = newBuf;
			Capacity = newCap;
		}
		inline void ensure_capacity_for(size_t add) {
			size_t need = Count + add;
			if (need <= Capacity) return;
			size_t nc = Capacity + Capacity / 4 + 8;
			if (nc < need) nc = need;
			grow(nc);
		}
		// 交换下标 i 与 j 的元素（move-aware）
		inline void swapItem(size_t i, size_t j) {
			if (i == j) return;
			T tmp; /* 依赖项目约定：非 POD 均提供默认构造与 move */
			move_assign<T, need_explicit_move<T>::value>::apply(tmp, content[i]);
			move_assign<T, need_explicit_move<T>::value>::apply(content[i], content[j]);
			move_assign<T, need_explicit_move<T>::value>::apply(content[j], tmp);
		}
	public:
		vector() :Count(0), Capacity(0), content(NULL) {}
		/* 拷贝构造 */
		vector(const vector& other) :Count(0), Capacity(0), content(NULL) {
			if (other.Count == 0) return;
			size_t n = other.Count;
			content = (T*)malloc(n * sizeof(T));
			if (!content) ::abort();
			for (size_t i = 0; i < n; ++i)
				new (content + i) T(other.content[i]);
			Count = Capacity = n;
		}
		~vector() {
			destroy_range(0, Count);
			deallocate();
			Capacity = 0;
			Count = 0;
		}
		/* 拷贝赋值 */
		vector& operator=(const vector& other) {
			if (this == &other) return *this;
			/* 先销毁本对象数据 */
			destroy_range(0, Count);
			if (other.Count > Capacity) {
				free(content);
				content = (T*)malloc(other.Count * sizeof(T));
				if (!content && other.Count) ::abort();
				Capacity = other.Count;
			}
			for (size_t i = 0; i < other.Count; ++i)
				new (content + i) T(other.content[i]);
			Count = other.Count;
			return *this;
		}
		// 位置：class vector<T> public: 内（例如拷贝赋值函数后面增加）
		/* 显式移动：C++03 风格，供 move_assign 调度使用 */
		void move(vector& src)
		{
			if (&src == this) return;
			// 释放当前资源
			destroy_range(0, Count);
			free(content);

			// 窃取
			content = src.content;
			Count = src.Count;
			Capacity = src.Capacity;

			// 置空源
			src.content = NULL;
			src.Count = 0;
			src.Capacity = 0;
		}

		// 在 class vector<T> public: 内合适位置追加
		inline size_t size() const { return count(); }
		inline void resize(size_t n) { recount(n); }

		inline size_t count() const { return Count; }
		inline size_t capacity() const { return Capacity; }

		inline const T* ptr() const { return content; }
		inline T* ptr() { return content; }
		inline const T* ptr(size_t off) const { return content + off; }
		inline T* ptr(size_t off) { return content + off; }

		inline const T& operator[](size_t i) const { return content[i]; }
		inline T& operator[](size_t i) { return content[i]; }

		inline void clear(void) {
			destroy_range(0, Count);
			Count = 0;
		}

		// 在 class vector<T> public: 区域内添加
		void reserve(size_t n) {
			if (n > Capacity) grow(n);
		}
		// 可选：保留旧名 recapacity -> 行为变成 reserve
		void recapacity(size_t NewSize) {
			if (NewSize > Capacity) grow(NewSize);
			if (NewSize < Count) {           // 仅截断，不收缩容量
				destroy_range(NewSize, Count);
				Count = NewSize;
			}
		}

		void recount(size_t NewCount) {
			if (NewCount > Capacity) grow(NewCount);
			if (NewCount < Count) {
				destroy_range(NewCount, Count);
				Count = NewCount;
				return;
			}
			if (NewCount > Count) {
				for (size_t i = Count; i < NewCount; ++i)
					new (content + i) T();
				Count = NewCount;
			}
		}

		size_t append(void) {
			ensure_capacity_for(1);
			new (content + Count) T();
			return Count++;
		}
		void append(const T& element) {
			ensure_capacity_for(1);
			new (content + Count) T(element);
			++Count;
		}
		void append(const vector& source) {
			if (!source.Count) return;
			ensure_capacity_for(source.Count);
			size_t base = Count;
			for (size_t i = 0; i < source.Count; ++i)
				new (content + base + i) T(source.content[i]);
			Count += source.Count;
		}
		/* 移动版单元素追加：优先尝试显式 move，否则退化为拷贝
   返回后 element 处于已移动或原值状态（取决于 need_explicit_move<T>） */
   /* 单元素追加（破坏性：element 被视为已 move 或保持原值；不析构 element） */
		void appendMove(T& element) {
			ensure_capacity_for(1);
			new (content + Count) T(); // 占位
			// move_assign 根据 need_explicit_move 常量分派：有 move() 用 move，否则用赋值
			move_assign<T, need_explicit_move<T>::value>::apply(content[Count], element);
			++Count;
		}

		/* 破坏性移动追加：将 source 全部元素搬到尾部；不显式析构 source 中元素，仅把 Count 置 0 */
		void appendMove(vector& source) {
			if (&source == this || source.Count == 0) return;

			// 目标为空，直接整体窃取（O(1)）
			if (Count == 0) {
				move(source); // 已把 source 置空
				return;
			}

			ensure_capacity_for(source.Count);
			size_t base = Count;

			// 逐元素占位 + move/赋值
			for (size_t i = 0; i < source.Count; ++i) {
				new (content + base + i) T();
				move_assign<T, need_explicit_move<T>::value>::apply(content[base + i], source.content[i]);
			}

			Count += source.Count;
			// 破坏性：source 清空（其缓冲里对象均已 move，按约定视为已析构，不再手动析构）
			source.Count = 0;
		}

		/* 批量插入（非破坏：elements 外部数组，不修改其内容，不析构任何已存在元素） */
		size_t insertMove(size_t pos, T* elements, size_t insertCount) {
			if (insertCount == 0) return (pos > Count ? Count : pos);
			if (pos > Count) pos = Count;

			ensure_capacity_for(insertCount);

			// 尾插：直接构造 + move/赋值
			if (pos == Count) {
				for (size_t k = 0; k < insertCount; ++k) {
					new (content + Count + k) T();
					move_assign<T, need_explicit_move<T>::value>::apply(content[Count + k], elements[k]);
				}
				Count += insertCount;
				return pos;
			}

			// 右移尾段：[pos, Count) -> [pos+insertCount, Count+insertCount)
			// 从尾到头，避免覆盖
			for (size_t i = 0; i < Count - pos; ++i) {
				size_t src = Count - 1 - i;
				size_t dst = src + insertCount;
				new (content + dst) T();
				move_assign<T, need_explicit_move<T>::value>::apply(content[dst], content[src]);
				// 不析构 src（移动后 src 为“已析构状态”或保留副本，统一在容器析构时处理）
			}

			// 放入新元素
			for (size_t k = 0; k < insertCount; ++k) {
				new (content + pos + k) T();
				move_assign<T, need_explicit_move<T>::value>::apply(content[pos + k], elements[k]);
			}

			Count += insertCount;
			return pos;
		}

		/* 破坏性插入：把 source 全部元素插入到 pos；不析构 source 中元素，仅置 source.Count=0 */
		size_t insertMove(size_t pos, vector& source) {
			if (&source == this) return (pos > Count ? Count : pos);
			size_t n = source.Count;
			if (n == 0) return (pos > Count ? Count : pos);
			if (pos > Count) pos = Count;

			// 尾插等价 appendMove
			if (pos == Count) {
				appendMove(source);
				return pos;
			}

			ensure_capacity_for(n);

			// 右移尾段
			for (size_t i = 0; i < Count - pos; ++i) {
				size_t src = Count - 1 - i;
				size_t dst = src + n;
				new (content + dst) T();
				move_assign<T, need_explicit_move<T>::value>::apply(content[dst], content[src]);
			}

			// 填入 source 的元素（破坏性 move）
			for (size_t i = 0; i < n; ++i) {
				new (content + pos + i) T();
				move_assign<T, need_explicit_move<T>::value>::apply(content[pos + i], source.content[i]);
			}

			Count += n;
			source.Count = 0;
			return pos;
		}
		/* 单元素版本（转调） */
		size_t insertMove(size_t pos, T& element) {
			return insertMove(pos, &element, 1);
		}

		void copy(const vector& source) {
			if (this == &source) return;
			clear();
			if (source.Count > Capacity) grow(source.Count);
			for (size_t i = 0; i < source.Count; ++i)
				new (content + i) T(source.content[i]);
			Count = source.Count;
		}
		size_t SearchAppend(const T& element) {
			for (size_t i = 0; i < Count; ++i)
				if (content[i] == element) return i;
			append(element);
			return Count - 1;
		}
		size_t pop(T& element) {
			if (Count == 0) return 0;
			Count -= 1;
			move_assign<T, need_explicit_move<T>::value>::apply(element, content[Count]);
			if (!is_trivially_destructible_like<T>::value)
				(content + Count)->~T();
			return Count + 1;
		}
		void pop(void) {
			if (Count == 0) return;
			Count -= 1;
			if (!is_trivially_destructible_like<T>::value)
				(content + Count)->~T();
		}
		size_t top(T& element) {
			if (Count == 0) return 0;
			element = content[Count - 1];
			return Count;
		}
		const T& top(void) const {
			assert(Count > 0 && "vector::top() called on empty vector");
			size_t idx = Count ? Count - 1 : 0;
			return content[idx];
		}
		T& top(void) {
			assert(Count > 0 && "vector::top() called on empty vector");
			size_t idx = Count ? Count - 1 : 0;
			return content[idx];
		}
		void value(const T& e) {
			for (size_t i = 0; i < Count; ++i) content[i] = e;
		}
		void swap(vector& other) {
			T* pc = other.content; other.content = content; content = pc;
			size_t v = other.Count; other.Count = Count; Count = v;
			v = other.Capacity; other.Capacity = Capacity; Capacity = v;
		}


		// —— 排序与有序性检查 ——
    // 升序堆排序（非稳定，O(n log n)）
		void sortAsc() {
			if (Count < 2) return;
			const size_t n = Count;
			// 建“最大堆”
			for (size_t i = (n / 2); i > 0; --i) siftDownAsc(i - 1, n - 1);
			// 选择置尾
			for (size_t end = n - 1; end > 0; --end) {
				swapItem(0, end);
				siftDownAsc(0, end - 1);
			}
		}
		// 降序堆排序（非稳定，O(n log n)）
		void sortDesc() {
			if (Count < 2) return;
			const size_t n = Count;
			// 建“最小堆”
			for (size_t i = (n / 2); i > 0; --i) siftDownDesc(i - 1, n - 1);
			// 选择置尾
			for (size_t end = n - 1; end > 0; --end) {
				swapItem(0, end);
				siftDownDesc(0, end - 1);
			}
		}
		// 检查 [offset, Count) 是否非降序（允许相等）
		bool checkAsc(size_t offset = 0) const {
			for (size_t i = offset; i + 1 < Count; ++i) {
				// 仅用 operator>
				if (content[i] > content[i + 1]) return false;
			}
			return true;
		}
		// 检查 [offset, Count) 是否非升序（允许相等）
		bool checkDesc(size_t offset = 0) const {
			for (size_t i = offset; i + 1 < Count; ++i) {
				// 仅用 operator<
				if (content[i] < content[i + 1]) return false;
			}
			return true;
		}

		bool operator==(const vector& other) const {
			if (Count != other.Count) return false;
			for (size_t i = 0; i < Count; ++i) {
				if (!(content[i] == other.content[i])) return false;
			}
			return true;
		}
		bool operator!=(const vector& other) const {
			return !(*this == other);
		}

		// 升序版本（与现有 binarySearchIndex 相同语义）：[offset,Count) 中二分查找
		bool binarySearchIndexAsc(const T& key, size_t& pos, size_t offset = 0) const {
			if (offset > Count) offset = Count;
			size_t lo = offset;
			size_t hi = Count;
			while (lo < hi) {
				size_t mid = lo + ((hi - lo) >> 1);
				if (content[mid] < key) {
					lo = mid + 1;
				}
				else if (key < content[mid]) {
					hi = mid;
				}
				else {
					pos = mid;
					return true;
				}
			}
			pos = lo; // 插入到第一个 >= key 的位置
			return false;
		}

		// 降序版本（非增序/从大到小）：[offset,Count) 中二分查找
		// 语义：返回第一个 <= key 的位置作为插入点，保持非增序
		bool binarySearchIndexDesc(const T& key, size_t& pos, size_t offset = 0) const {
			if (offset > Count) offset = Count;
			size_t lo = offset;
			size_t hi = Count; // [lo,hi)
			while (lo < hi) {
				size_t mid = lo + ((hi - lo) >> 1);
				if (content[mid] > key) {
					// mid 比 key 大，降序下应把 key 插到右侧
					lo = mid + 1;
				}
				else if (key > content[mid]) {
					// key 比 mid 大，降序下应把 key 插到左侧
					hi = mid;
				}
				else {
					pos = mid;
					return true;
				}
			}
			pos = lo; // 插入到第一个 <= key 的位置
			return false;
		}
    private:
	// 最大堆下滤：用于升序排序
	inline void siftDownAsc(size_t start, size_t end) {
		size_t root = start;
		for (;;) {
			size_t child = root * 2 + 1;
			if (child > end) break;
			size_t swapIdx = root;
			if (content[swapIdx] < content[child]) swapIdx = child;
			if (child + 1 <= end && content[swapIdx] < content[child + 1]) swapIdx = child + 1;
			if (swapIdx == root) return;
			swapItem(root, swapIdx);
			root = swapIdx;
		}
	}
	// 最小堆下滤：用于降序排序
	inline void siftDownDesc(size_t start, size_t end) {
		size_t root = start;
		for (;;) {
			size_t child = root * 2 + 1;
			if (child > end) break;
			size_t swapIdx = root;
			if (content[child] < content[swapIdx]) swapIdx = child;
			if (child + 1 <= end && content[child + 1] < content[swapIdx]) swapIdx = child + 1;
			if (swapIdx == root) return;
			swapItem(root, swapIdx);
			root = swapIdx;
		}
	}
	};

	/* ===================== array<T> 替换版 ===================== */
	template <class T> class array
	{
	protected:
		size_t Length;
		T* content;
		enum { use_realloc = is_bitwise_relocatable<T>::value };
		void destroy_range(size_t a, size_t b) {
			if (is_trivially_destructible_like<T>::value) return;
			if (!content) return;
			while (a < b) {
				(content + a)->~T();
				++a;
			}
		}
	public:
		array() :Length(0), content(NULL) {}
		/* 拷贝构造 */
		array(const array& other) :Length(0), content(NULL) {
			if (other.Length == 0) return;
			content = (T*)malloc(other.Length * sizeof(T));
			if (!content) ::abort();
			for (size_t i = 0; i < other.Length; ++i)
				new (content + i) T(other.content[i]);
			Length = other.Length;
		}
		/* 拷贝赋值 */
		array& operator=(const array& other) {
			if (this == &other) return *this;
			destroy_range(0, Length);
			if (other.Length != Length) {
				free(content);
				content = NULL;
				if (other.Length) {
					content = (T*)malloc(other.Length * sizeof(T));
					if (!content) ::abort();
				}
				Length = other.Length;
			}
			for (size_t i = 0; i < Length; ++i)
				new (content + i) T(other.content[i]);
			return *this;
		}
		/* 显式移动（供 move_assign 调用） */
		void move(array& src) {
			if (&src == this) return;
			destroy_range(0, Length);
			free(content);
			content = src.content;
			Length = src.Length;
			src.content = NULL;
			src.Length = 0;
		}
		~array() {
			destroy_range(0, Length);
			free(content);
			content = NULL;
			Length = 0;
		}

		size_t length() const { return Length; }
		size_t count() const { return Length; }
		T* ptr() { return content; }
		const T* ptr() const { return content; }
		T* ptr(size_t off) { return content + off; }
		const T* ptr(size_t off) const { return content + off; }

		T& operator[](size_t i) { return content[i]; }
		const T& operator[](size_t i) const { return content[i]; }

		/* 分配并默认构造（旧实现未构造对象，这里改为安全构造） */
		void Malloc(size_t NewSize) {
			destroy_range(0, Length);
			free(content);
			content = NULL;
			Length = 0;
			if (!NewSize) return;
			content = (T*)malloc(NewSize * sizeof(T));
			if (!content) ::abort();
			for (size_t i = 0; i < NewSize; ++i)
				new (content + i) T();
			Length = NewSize;
		}
		/* 重新分配：保留前 min(旧,新) 个元素 */
		void Realloc(size_t NewSize) {
			if (NewSize == Length) return;
			if (use_realloc) {
				size_t old = Length;
				T* p = (T*)realloc(content, NewSize * sizeof(T));
				if (!p && NewSize) ::abort();
				content = p;
				if (NewSize > old) {
					for (size_t i = old; i < NewSize; ++i)
						new (content + i) T();
				}
				else { /* 缩小销毁尾部 */
					destroy_range(NewSize, old);
				}
				Length = NewSize;
				return;
			}
			/* 安全路径 */
			T* newBuf = NULL;
			if (NewSize) {
				newBuf = (T*)malloc(NewSize * sizeof(T));
				if (!newBuf) ::abort();
				size_t toCopy = (NewSize < Length) ? NewSize : Length;
				for (size_t i = 0; i < toCopy; ++i)
					new (newBuf + i) T(content[i]);
				for (size_t i = toCopy; i < NewSize; ++i)
					new (newBuf + i) T();
			}
			destroy_range(0, Length);
			free(content);
			content = newBuf;
			Length = NewSize;
		}
		void Free() {
			destroy_range(0, Length);
			free(content);
			content = NULL;
			Length = 0;
		}
		void value(const T& e) {
			for (size_t i = 0; i < Length; ++i) content[i] = e;
		}
	};

	/* ===================== buffer<T> 替换版 ===================== */
	template <class T> class buffer
	{
	protected:
		mutable size_t Rear;
		mutable size_t Capacity;
		mutable size_t Head;
		mutable T* content;

		enum { use_realloc = is_bitwise_relocatable<T>::value };

		void destroy_range(size_t a, size_t b) const {
			if (is_trivially_destructible_like<T>::value) return;
			if (!content) return;
			while (a < b) {
				(content + a)->~T();
				++a;
			}
		}
		/* 将 [Head,Rear) 前移到 0（必要时） */
		void compact_if_needed() const {
			if (Head == 0) return;
			if (!content) return;
			size_t cnt = Rear - Head;
			if (!cnt) { Head = Rear = 0; return; }
			/* 非位搬迁类型：逐个移动构造 */
			if (!use_realloc) {
				for (size_t i = 0; i < cnt; ++i) {
					if (i != Head + i)
						new (content + i) T(content[Head + i]);
				}
			}
			else {
				/* 位搬迁：memmove 即可 */
				memmove(content, content + Head, cnt * sizeof(T));
			}
			/* 销毁旧位置（非平凡） */
			if (!is_trivially_destructible_like<T>::value) {
				for (size_t i = cnt; i < cnt + Head; ++i)
					(content + i)->~T();
			}
			Head = 0;
			Rear = cnt;
		}
		void ensure_capacity_for(size_t add) const {
			size_t need = Rear + add;
			if (need <= Capacity) return;
			/* 先尝试挪前空洞 */
			if (Head && (need - Head) <= Capacity) {
				compact_if_needed();
				if (Rear + add <= Capacity) return;
			}
			size_t newCap = Capacity + Capacity / 4 + 8;
			if (newCap < need) newCap = need;
			if (use_realloc && Head == 0) {
				T* p = (T*)realloc((void*)content, newCap * sizeof(T));
				if (!p) ::abort();
				content = p;
				Capacity = newCap;
				return;
			}
			/* 安全路径：新缓冲，自 0 起复制 */
			size_t cnt = Rear - Head;
			T* newBuf = (T*)malloc(newCap * sizeof(T));
			if (!newBuf) ::abort();
			for (size_t i = 0; i < cnt; ++i)
				new (newBuf + i) T(content[Head + i]);
			destroy_range(Head, Rear);
			free(content);
			content = newBuf;
			Capacity = newCap;
			Head = 0;
			Rear = cnt;
		}
	public:
		buffer() :Rear(0), Capacity(0), Head(0), content(NULL) {}
		buffer(const buffer& other) :Rear(0), Capacity(0), Head(0), content(NULL) {
			size_t cnt = other.count();
			if (!cnt) return;
			content = (T*)malloc(cnt * sizeof(T));
			if (!content) ::abort();
			for (size_t i = 0; i < cnt; ++i)
				new (content + i) T(other.content[other.Head + i]);
			Capacity = cnt;
			Head = 0;
			Rear = cnt;
		}
		buffer& operator=(const buffer& other) {
			if (this == &other) return *this;
			destroy_range(Head, Rear);
			Head = Rear = 0;
			if (other.count() > Capacity) {
				free(content);
				content = (T*)malloc(other.count() * sizeof(T));
				if (!content && other.count()) ::abort();
				Capacity = other.count();
			}
			size_t cnt = other.count();
			for (size_t i = 0; i < cnt; ++i)
				new (content + i) T(other.content[other.Head + i]);
			Head = 0;
			Rear = cnt;
			return *this;
		}
		void move(buffer& src) {
			if (&src == this) return;
			destroy_range(Head, Rear);
			free(content);
			content = src.content;
			Rear = src.Rear;
			Capacity = src.Capacity;
			Head = src.Head;
			src.content = NULL;
			src.Head = src.Rear = src.Capacity = 0;
		}
		~buffer() {
			destroy_range(Head, Rear);
			free(content);
			content = NULL;
			Head = Rear = Capacity = 0;
		}

		void clear() {
			destroy_range(Head, Rear);
			Head = Rear = 0;
		}

		/* 追加另一个 buffer (按顺序复制) */
		void append(const buffer& other) {
			size_t cnt = other.count();
			if (!cnt) return;
			ensure_capacity_for(cnt);
			for (size_t i = 0; i < cnt; ++i)
				new (content + Rear + i) T(other.content[other.Head + i]);
			Rear += cnt;
		}

		size_t append(const T& e) {
			ensure_capacity_for(1);
			new (content + Rear) T(e);
			return Rear++;
		}

		/* 移动版单元素追加：若类型声明需要显式 move 则调用 move_assign，否则退化为拷贝 */
		void appendMove(T& element) {
			ensure_capacity_for(1);
			if (need_explicit_move<T>::value) {
				new (content + Rear) T();
				move_assign<T, need_explicit_move<T>::value>::apply(content[Rear], element);
			}
			else {
				new (content + Rear) T(element);
			}
			++Rear;
		}

		/* 破坏性移动追加：把 source 的全部有效元素 [source.Head, source.Rear) 移到当前尾部
		   之后 source 清空 (Head=Rear=0)。source 与 this 相同则无操作 */
		void appendMove(buffer& source) {
			if (&source == this) return;
			size_t scount = source.count();
			if (scount == 0) return;

			/* 目标当前为空：直接偷整块（O(1）） */
			if (Rear == Head) {
				move(source);
				/* 如果希望偷来后紧凑，可启用：
				   // if (Head != 0) compact_if_needed();
				*/
				return;
			}

			ensure_capacity_for(scount);
			size_t base = Rear;

			if (is_bitwise_relocatable<T>::value) {
				/* 位可搬迁：直接字节复制 */
				memcpy(content + base,
					source.content + source.Head,
					scount * sizeof(T));
				if (!is_trivially_destructible_like<T>::value) {
					source.destroy_range(source.Head, source.Rear);
				}
				Rear += scount;
				source.Head = source.Rear = 0;
				return;
			}

			/* 非位搬迁：逐元素构造 + 析构 */
			for (size_t i = 0; i < scount; ++i) {
				T* src_slot = source.content + source.Head + i;
				new (content + base + i) T(*src_slot); // 当前仅有拷贝构造
				if (!is_trivially_destructible_like<T>::value)
					src_slot->~T();
			}
			Rear += scount;
			source.Head = source.Rear = 0;
		}

		/* 与旧接口兼容：expand 仅预留一个槽并默认构造 */
		size_t expand() {
			ensure_capacity_for(1);
			new (content + Rear) T();
			return Rear++;
		}
		void expand(const size_t L) {
			if (!L) return;
			ensure_capacity_for(L);
			for (size_t i = 0; i < L; ++i)
				new (content + Rear + i) T();
			Rear += L;
		}
		bool dequeue(T& out) {
			if (Head >= Rear) return false;
			move_assign<T, need_explicit_move<T>::value>::apply(out, content[Head]);
			if (!is_trivially_destructible_like<T>::value)
				(content + Head)->~T();
			++Head;
			if (Head == Rear) { Head = Rear = 0; } /* 为空复位 */
			return true;
		}
		bool backspace(T& in) {
			if (Head == 0) return false;
			--Head;
			/* 位置 Head 已被视为空位 */
			new (content + Head) T(in);
			return true;
		}
		bool backspace(buffer& in) {
			size_t LL = in.count();
			if (LL > Head) return false;
			/* 先把 in 中元素搬到前面空洞 */
			for (size_t i = 0; i < LL; ++i) {
				new (content + Head - LL + i) T(in.content[in.Head + i]);
			}
			Head -= LL;
			in.clear();
			return true;
		}

		T* ptr() { return content + Head; }
		const T* ptr() const { return content + Head; }
		size_t count() const { return Rear - Head; }
		size_t rear() const { return Rear; }
		size_t capacity() const { return Capacity; }
		size_t head() const { return Head; }

		const T& operator[](size_t idx) const { return content[idx]; }
		inline T& operator[](size_t i) { return content[i]; }
	};
}















