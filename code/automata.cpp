#include"automata.h"
using namespace hyperlex;
#define SizeMax ((size_t)0xffffffffffffffff)
#define CharSize ((size_t)(1 << (sizeof(char) * 8 - 1)))

const double hyperlex::StringPool::LOAD_FACTOR = 0.64;

#include<stdlib.h>
//typedef size_t uint;
//typedef long long int sint;
#include<iostream>
#include <bitset>
static char* CopyMalloc(const char* s);
static void CopyNc(const char* src, char* dst, size_t length);
static bool compare(const char* str1, const char* str2);
static void write_escaped_string(const char* str, FILE* file);
static size_t strlength(const char* str);
static void strfree(const char** strs, size_t length);
static void inverse(list<size_t>& out, const list<size_t>& in);
static int PostfixSwitch_small(char c);

static bool isIdBegin(char c);
static bool isIdNumber(char c);


BufferChar::BufferChar()
{
}
BufferChar::~BufferChar()
{
}
void BufferChar::operator<<(FILE* fp)
{
	char c;
	while (feof(fp) == 0)
	{
		c = (const char)getc(fp);
		append(c);
	}
	if ((int)content[Rear - 1] == (int)EOF) Rear -= 1;
}
void BufferChar::operator=(const char* input)
{
	size_t i;
	Head = 0;
	Rear = 0;
	for (i = 0; input[i] != '\0'; i++) append(input[i]);
	append('\0');
	Rear -= 1;
}
void BufferChar::operator=(const BufferChar& in)
{
	clear();
	append(in);
}
void BufferChar::operator+=(const char* input)
{
	size_t i;
	for (i = 0; input[i] != '\0'; i++) append(input[i]);
	append('\0');
	Rear -= 1;
}
void BufferChar::operator+=(long int source)
{
	//size_t i;
	long long int r, ele, top;
	char stack[32];
	ele = source;
	if (source < 0)
	{
		ele = -ele;
		append('-');
	}
	if (ele == 0) append('0');
	top = 0;
	while (ele != 0)
	{
		r = ele % 10;
		ele = ele / 10;
		stack[top] = ((char)r) + '0';
		top += 1;
	}
	while (top != 0)
	{
		top -= 1;
		append(stack[top]);
	}
}
void BufferChar::operator+=(const BufferChar& in)
{
	append(in);
}
void BufferChar::operator+=(double source)
{
	// Step 1: 分解double的二进制表示
	union { double d; unsigned long long i; } u = { source };
	unsigned long long bits = u.i;

	// 提取符号位
	int sign = (bits >> 63) ? 1 : 0;
	bits &= 0x7FFFFFFFFFFFFFFF;

	// 提取指数和尾数
	int exponent = (bits >> 52) & 0x7FF;
	unsigned long long mantissa = bits & 0x000FFFFFFFFFFFFF;

	// 处理特殊值（简化处理非规格化数）
	if (exponent == 0) { // 非规格化数
		exponent = -1022;
	}
	else { // 规格化数
		mantissa |= 0x0010000000000000; // 添加隐含的1
		exponent -= 1023;
	}

	// Step 2: 转换为十进制科学计数法
	// 计算 log10(2^exponent) = exponent * log10(2)
	double log2_10 = 3.32192809488736234787; // log2(10)
	int decExponent = static_cast<int>(exponent / log2_10);

	// 手动调整有效数字
	double factor = 1.0;
	for (int i = 0; i < decExponent; ++i) factor *= 10;
	double normalized = source / factor;

	// Step 3: 生成字符串（简化实现）
	//char* p = result;

	// 符号位
	//if (sign) *p++ = '-';
	if (sign) append('-');
	// 整数部分
	int integer = static_cast<int>(normalized);
	
	append('0' + integer); //*p++ = '0' + integer;
	append('.'); // *p++ = '.';

	// 小数部分（17位）
	double fraction = normalized - integer;
	for (int i = 0; i < 17; ++i) {
		fraction *= 10;
		int digit = static_cast<int>(fraction);
		append('0' + digit);// *p++ = '0' + digit;
		fraction -= digit;
	}

	// 指数部分
	append('E'); //*p++ = 'e';
	if (decExponent >= 0) {
		append('+'); //*p++ = '+';
	}
	else {
		append('-'); //*p++ = '-';
		decExponent = -decExponent;
	}

	// 指数数值
	char expStr[5];
	char* ep;
	for (ep = expStr; decExponent > 0; ep++)
	{
		*ep = (char)((int)'0' + (decExponent % 10));
		decExponent /= 10;
	}
	for (ep--; ep >= expStr; ep--) append(*ep);


	//*p = '\0';
}

void BufferChar::operator<<(char c)
{
	append(c);
	return;
}
void BufferChar::operator<<(const char* c)
{
	Append(c);
}
void BufferChar::operator<<(double e)
{
	char temp[32];
	sprintf(temp, "%+24.16e", e);
	Append(temp);
	return;
}
void BufferChar::operator<<(int e)
{
	char temp[32];
	sprintf(temp, "%d", e);
	Append(temp);
	return;
}
void BufferChar::operator<<(long long int e)
{
	char temp[32];
	sprintf(temp, "%lld", e);
	Append(temp);
	return;
}
void BufferChar::operator<<(BufferChar& in)
{
	append(in);
	in.clear();
	return;
}

bool BufferChar::operator==(const BufferChar& in)
{
	size_t i;
	const char* L, * R;
	if ((Rear - Head) != (in.Rear - in.Head)) return false;
	L = content + Head;
	R = in.content + in.Head;
	for (i = 0; i < (Rear - Head); i++)
		if (L[i] != R[i]) return false;
	return true;
}
bool BufferChar::operator!=(const BufferChar& in)
{
	size_t i;
	const char* L, * R;
	if ((Rear - Head) != (in.Rear - in.Head)) return true;
	L = content + Head;
	R = in.content + in.Head;
	for (i = 0; i < (Rear - Head); i++)
		if (L[i] != R[i]) return true;
	return false;
}
bool BufferChar::operator==(size_t length)
{
	return length == count();
}
bool BufferChar::operator!=(size_t length)
{
	return length != count();
}
void BufferChar::Append(const char* c)
{
	size_t i;
	for (i = 0; i < c[i] != '\0'; i++)
	{
		append(c[i]);
	}
	append('\0');
	Rear -= 1;
}


char* BufferChar::ptr(void)
{
	append('\0');
	Rear -= 1;
	return content + Head;
}
char* BufferChar::CopyVector(void) const
{
	char* Newchar;
	size_t i;
	Newchar = (char*)malloc(sizeof(char) * (Rear - Head + 1));
	for (i = 0; i < Rear - Head; i++)
		Newchar[i] = content[i + Head];
	Newchar[i] = '\0';
	return Newchar;
}
size_t BufferChar::CopyVector(vector<char>& storage, size_t& length) const
{
	size_t i;
	size_t Newchar;
	length = Rear - Head;
	Newchar = storage.count();
	for (i = 0; i < Rear - Head; i++)
		storage.append(content[i + Head]);
	return Newchar;
}
void BufferChar::copy(const BufferChar& source)
{
	size_t i;
	size_t length;
	length = source.Rear - source.Head;
	expand(length);
	for (i = 0; i < length; i++)
		content[Rear + i] = source.content[i + source.Head];
	Rear += length;
}




long int BufferChar::DequeueInt(void)
{
	return IntGet(content, Rear, Head);
}
double BufferChar::DequeueReal(void)
{
	int state;
	return RealGet(state, content, Rear, Head);
}
char BufferChar::DequeueChar(void)
{
	return CharGet(content, Rear, Head);
}
char* BufferChar::DequeueString(void)
{
	BufferChar middle;
	char* temp, c, index;
	int state, error;
	state = 0;
	do
	{
		switch (state)
		{
		case 0:
			dequeue(index);
			if (index == '\"') state = 1;
			else if (index == (char)EOF) state = -1;
			break;
		case 1:
			index = QueueHead();
			if (index == '\"') state = -2;
			else if (index == (char)EOF) state = -3;
			else
			{
				c = CharGet(error, content, Rear, Head);
				if (c != (char)EOF) middle << c;
			}
			break;
		}
	} while (state >= 0);
	temp = middle.CopyVector();
	return temp;
}
char* BufferChar::DequeueId(void)
{
	BufferChar middle;
	char* temp, c;
	int state, error;
	state = 0;
	do
	{
		switch (state)
		{
		case 0:
			dequeue(c);
			if (isIdBegin(c))
			{
				state = 1;
				middle << c;
			}
			else if (c == (char)EOF) state = -2;
			break;
		case 1:
			dequeue(c);
			if (isIdBegin(c) || isIdNumber(c))
			{
				state = 1;
				middle << c;
			}
			else state = -1;
			break;
		}
	} while (state >= 0);
	temp = middle.CopyVector();
	return temp;
}

char BufferChar::QueueHead(void)
{
	if (Head >= Rear) return EOF;
	return content[Head];
}

Morpheme::Morpheme()
{
	count = 0;
	AppendEnd(0);
	//index = 0;
}
Morpheme::~Morpheme()
{
	ruin();
}
char* Morpheme::Copy(size_t site) const
{
	char* Newchar;
	size_t i, offset__, length__;
	if (site >= count) return NULL;
	offset__ = lex[site].begin;
	length__ = lex[site].length;
	Newchar = (char*)malloc(sizeof(char) * (length__ + 1));
	for (i = 0; i < length__; i++)
		Newchar[i] = storage[i + offset__];
	Newchar[i] = '\0';
	return Newchar;
}
size_t Morpheme::GetCount(void) const
{
	return count;
}
const char* Morpheme::GetWord(size_t site) const
{
	return storage.ptr(lex[site].begin);
}
void Morpheme::append(const BufferChar& input, int accept, int category)
{
	size_t offset__, length__;
	result temp;
	offset__ = input.CopyVector(storage, length__);
	count += 1;
	storage.append('\0');
	//begin.append(offset__);
	//length.append(length__);
	temp.accept = accept;
	temp.category = category;
	temp.length = length__;
	temp.begin = offset__;
	temp.valid = true;
	temp.line = 0;
	temp.file = 0;
	lex.append(temp);
	return;
}
void Morpheme::append(const char* fileName)
{
	SrcFile.append(CopyMalloc(fileName));
}
void Morpheme::AppendEnd(int TerminalCount)
{
	result temp;
	count += 1;
	storage.append('#');
	storage.append('\0');
	//begin.append(offset__);
	//length.append(length__);
	temp.valid = true;
	temp.accept = TerminalCount;
	temp.category = TerminalCount;
	temp.length = 1;
	temp.line = 0;
	temp.file = 0;
	temp.begin = storage.count() - 2;
	lex.append(temp);
}
void Morpheme::Demo(FILE* fp)const
{
	fprintf(fp, "count = %zu\n", count);
	for (size_t i = 0; i < count; i++)
	{
		const char* temp = NULL;
		if (lex[i].valid) temp = "( valid )";
		else temp = "(invalid)";

		fprintf(fp, "No[%6zu]: file: %4zu, line: %6zu, unit: ", i, lex[i].file, lex[i].line);
		fprintf(fp, "<%4d %s : %4d, ", lex[i].category, temp, lex[i].accept);
		write_escaped_string(storage.ptr(lex[i].begin), fp);
		fprintf(fp, ">\n");
	}
}
char Morpheme::GetChar(size_t site) const
{
	size_t head;
	const char* input;
	int error;
	input = storage.ptr(lex[site].begin);
	head = 1;
	if (input[0] != '\'') return input[0];
	else return CharGet(error, input, lex[site].length, head);
}
double Morpheme::GetReal(size_t site) const
{
	int state;
	size_t Head;
	size_t Rear;
	Head = lex[site].begin;
	Rear = Head + lex[site].length;
	return RealGet(state, storage.ptr(), Rear, Head);
}
long int Morpheme::GetInt(size_t site) const
{
	size_t Head;
	size_t Rear;
	Head = lex[site].begin;
	Rear = Head + lex[site].length;
	return IntGet(storage.ptr(), Rear, Head);
}
char* Morpheme::GetString(size_t site) const
{
	BufferChar BC;
	BC = GetWord(site);
	return BC.DequeueString();
}
#include <stdlib.h>
#include <string.h>

static char* string_from_header(const char* header) 
{
	if (header == NULL) return NULL;

	// 处理空字符串的情况
	if (*header == '\0') return CopyMalloc("\"\"");

	const char* start = header;
	const char* end = header + strlength(header) - 1;

	// 跳过开头的空格和'<'
	while (*start != '\0' && (*start == ' ' || *start == '<')) {
		start++;
	}

	// 跳过结尾的空格和'>'
	while (end >= start && (*end == ' ' || *end == '>')) {
		end--;
	}

	// 计算有效内容长度
	size_t len = (end >= start) ? (end - start + 1) : 0;

	// 分配内存：2个双引号 + 内容长度 + 结束符
	char* result = (char*)malloc((len + 3) * sizeof(char));
	if (result == NULL) return NULL;

	// 构建结果字符串
	result[0] = '"';
	if (len > 0) {
		CopyNc(start, result + 1, len);
		//strncpy(result + 1, start, len);
	}
	result[len + 1] = '"';
	result[len + 2] = '\0';

	return result;
}
char* Morpheme::GetHeader(size_t site) const
{
	const char* temp = GetWord(site);
	char* header = string_from_header(temp);
	BufferChar BC;
	BC = header;
	free(header);
	return BC.DequeueString();
}
void Morpheme::UnitMove(size_t from, size_t to)
{
	lex[to] = lex[from];
}
void Morpheme::CountReset(size_t NewCount)
{
	count = NewCount;
}

bool Morpheme::withTernimal(void)const
{
	if (lex.count() == 0) return false;
	return lex[lex.count() - 1].accept == 0 && lex[lex.count() - 1].category == 0;
}
void Morpheme::insert(size_t from, size_t deleted, const Morpheme& src)
{
	size_t NewCount;
	/*
	A:[0,from)
	B:[from, from + deleted) = [from, rear)
	C:[from + deleted, count - 1) = [rear, count - 1)
	D:[count - 1, count - 1] END-EOF

	a:[0,from)
	b:[from, from + NewCount)
	c:[from + NewCount, NewCount + count - deleted - 1)
	d:[NewCount + count - deleted - 1, NewCount + count - deleted - 1]
	*/
	NewCount = src.count - 1;
	if (from + 1 >= count) from = count - 1; 
	if (from + deleted + 1 >= count) deleted = count - from - 1;
	lex.recount(NewCount + count - deleted);
	size_t rear_ = from + deleted;
	size_t RearCount = count - rear_;
	size_t OldStorage = storage.count();
	
	for (size_t i = RearCount; i > 0; i--)
	{
		lex[i - 1 + NewCount + from] = lex[i - 1 + rear_];
	}
	storage.append(src.storage);
	for (size_t i = 0; i < NewCount; i++)
	{
		lex[i + from] = src.lex[i];
		lex[i + from].begin += OldStorage;
	}
	count = lex.count();
}
void Morpheme::shrink(void)
{
	size_t offset = 0;
	for (size_t i = 0; i < lex.count(); i++)
	{
		if (lex[i].valid)
		{
			if (i != offset)
			{
				lex[offset] = lex[i];
			}
			offset += 1;
		}
	}
	sort();
}
void Morpheme::sort(void)
{
	vector<char> temp;
	temp.copy(storage); 
	size_t offset = 0;
	for (size_t i = 0; i < lex.count(); i++)
	{
		size_t from = lex[i].begin;
		size_t length = lex[i].length;
		for (size_t j = 0; j < length; j++)
		{
			storage[offset + j] = temp[from + j];
		}
		//storage[offset + length] = temp[from + length];
		storage[offset + length] = '\0';
		lex[i].begin = offset;
		offset += length + 1;
	}
	storage.recount(offset);
}
void Morpheme::SetFile(size_t value)
{
	for (size_t i = 0; i < count; i++)
	{
		lex[i].file = value;
	}
	lex[count - 1].file = 0;
}
const char* Morpheme::GetFile(size_t value) const
{
	return SrcFile[value];
}
size_t Morpheme::FileCount(void) const
{
	return SrcFile.count();
}
void Morpheme::print(BufferChar& input) const
{
	size_t countLex;
	countLex = lex.count() - 1;
	for (size_t i = 0; i < countLex; i++)
	{
		size_t from = lex[i].begin;
		size_t length = lex[i].length;
		for (size_t j = 0; j < length; j++)
		{
			input.append(storage[from + j]);
		}
	}
}
bool Morpheme::dequeue(char& out, indexT& index) const
{
	size_t &now = index.UnitOffest;
	size_t& offset = index.CharOffset;
	while (true)
	{
		if (now + 1 >= count) return false;

		if (offset < lex[now].length) break;
		now += 1;

		if (lex[now].valid)
			offset = 0;
		else
			offset = lex[now].length;
	} 
	out = storage[lex[now].begin + offset];
	offset += 1;
	return true;
}
void Morpheme::backspace(indexT& index, size_t count) const
{
	size_t& now = index.UnitOffest;
	size_t& offset = index.CharOffset;
	for (size_t i = 0; i < count; i++)
	{
		while (offset == 0)
		{
			if (now == 0) break;
			now -= 1;
			if (lex[now].valid)
				offset = lex[now].length;
			else
				offset = 0;
		}
		offset -= 1;
	}
}
bool Morpheme::operator==(const Morpheme& source) const
{
	if (count != source.count) return false;
	//if (SrcFile.count() != source.SrcFile.count()) return false;
	for (size_t i = 0; i < SrcFile.count(); i++)
	{
		//if (!compare(SrcFile[i], source.SrcFile[i])) return false;
	}
	if (lex.count() != source.lex.count()) return false;
	for (size_t i = 0; i < lex.count(); i++)
	{
		
		if (lex[i].accept != source.lex[i].accept) return false;
		if (lex[i].category != source.lex[i].category) return false;
		if (lex[i].valid != source.lex[i].valid) return false;
		if (lex[i].length != source.lex[i].length) return false;
		if (lex[i].file != source.lex[i].file) return false;
		if (lex[i].line != source.lex[i].line) return false;
		if(!compare(GetWord(i), source.GetWord(i))) return false;
		//std::cout << "i: " << i << std::endl;
	}
	//std::cout << "i: " << i << std::endl;
	return true;
}

void Morpheme::ruin(void)
{
	count = 0;
	lex.clear();
	storage.clear();
	for (size_t i = 0; i < SrcFile.count(); i++)
	{
		char* temp = (char*)SrcFile[i];
		free(temp);
	}
	SrcFile.clear();
}
void Morpheme::clear(void)
{
	ruin();
	AppendEnd(0);
}
void Morpheme::copy(const Morpheme& source)
{
	clear();
	count = source.count;
	lex.copy(source.lex);
	storage.copy(source.storage);
	SrcFile.recount(source.SrcFile.count());
	for (size_t i = 0; i < source.SrcFile.count(); i++)
	{
		SrcFile[i] = CopyMalloc(source.SrcFile[i]);
	}
}

bool& Morpheme::valid(size_t site)
{
	return lex[site].valid;
}
size_t Morpheme::initial(void) const
{
	size_t index;
	for (index = 0; index < count; index++)
		if(lex[index].valid) return index;
	return index;
}
size_t Morpheme::next(size_t index) const
{
	for (index = index + 1; index < count; index++)
		if (lex[index].valid) return index;
	return index;
}
bool Morpheme::still(size_t index) const
{
	return index < count;
}
int Morpheme::accept(size_t index)const
{
	return lex[index].accept;
}
size_t Morpheme::CountEnter(const char* unit)
{
	size_t i, Entercount;
	Entercount = 0;
	for (i = 0; unit[i] != '\0'; i++)
		Entercount += (unit[i] == '\n' ? 1 : 0);
	return Entercount;
}
void Morpheme::SetLine(void)
{
	const char* temp;
	size_t i, LineNo;
	LineNo = 0;
	for (i = 0; i < count; i++)
	{
		lex[i].line = LineNo;
		temp = GetWord(i);
		LineNo += CountEnter(temp);
	}
	lex[count - 1].line = 0;
}

GrammarTree::GrammarTree()
{
	error_record00 = Undone;
	error_record01 = 0;
	error_record02 = 0;
	GT = NULL;

	//printf("GrammarTree::GrammarTree()\n");
}
GrammarTree::~GrammarTree()
{
	clear();
}
void GrammarTree::Demo(FILE* fp, const Morpheme& input, const char* const* RulesName) const
{
	size_t site, i, LabelNow;
	//vector<tree<TreeInfor>* >stack;
	tree<TreeInfor>* now;
	size_t childCount__;
	//GT->PostOrderTraversal(stack);
	//stack.append(GT);
	hyperlex::tree<GrammarTree::TreeInfor>* Tree;
	hyperlex::tree<GrammarTree::TreeInfor>::PostIterator iterator;
	LabelNow = 0;
	iterator.initial(GT);
	//fprintf(fp, "Here: %zu\n", (size_t)GT);
	while (iterator.still())
	{
		now = iterator.target();
		if (iterator.state() == 0)
		{
			now->root().label = LabelNow;
			LabelNow += 1;
		}
		//fprintf(fp, "Tree[%zu](%d): ", now->root().site, now->root().rules);
		//fprintf(fp, "childs[%zu]: \n", now->ChildCount());
		iterator.next();
	}

	iterator.initial(GT);
	while (iterator.still())
	{
		now = iterator.target();
		if (iterator.state() == 0)
		{
			site = now->root().site;
			fprintf(fp, "Tree[%zu]: ", now->root().label);
			if (now->root().rules)
			{
				childCount__ = now->ChildCount();
				fprintf(fp, "rules[%zu]: %s\n\t", site, RulesName[site]);
				fprintf(fp, "childs[%zu]: ", childCount__);
				for (i = 1; i < childCount__; i++)
					fprintf(fp, "%zu, ", now->child(i - 1)->root().label);
				if(childCount__ != 0)fprintf(fp, "%zu\n", now->child(childCount__ - 1)->root().label);
			}
			else
			{
				fprintf(fp, "symbol: %zu: ", site);
				fprintf(fp, "<%4d : %4d , %s>\n", input[site].category, input[site].accept, input.GetWord(site));
			}
			
		}
		iterator.next();
	}
}
void GrammarTree::clear(void)
{
	if (GT != NULL)
	{
		GT->clear();
		delete GT;
	}
	GT = NULL;
	error_record00 = Undone;
	error_record01 = 0;
	error_record02 = 0;
}
GrammarTree::TreeInfor::TreeInfor(void)
{
	implicit = false;
	rules = false;
	site = 0;
	label = 0;
	infor = NULL;
}


RegularExp::RegularExp()
{
	tree = NULL;
}
RegularExp::~RegularExp()
{
	clear();
}
void RegularExp::item::Demo(FILE* fp)
{
	RegularExp::Demo(fp, lower, upper);
	RegularExp::Demo(fp, type);
}
void RegularExp::item::copy(const item& source)
{
	lower = source.lower;
	upper = source.upper;
	type = source.type;
}
void RegularExp::Demo(FILE* fp, RegularExp::NodeType T)
{
	switch (T)
	{
	case hyperlex::RegularExp::Concatenation:
		fprintf(fp, "Concatenation\n");
		break;
	case hyperlex::RegularExp::Alternation:
		fprintf(fp, "Alternation\n");
		break;
	case hyperlex::RegularExp::ZeroOrMore:
		fprintf(fp, "ZeroOrMore\n");
		break;
	case hyperlex::RegularExp::OneOrMore:
		fprintf(fp, "OneOrMore\n");
		break;
	case hyperlex::RegularExp::ZeroOrOne:
		fprintf(fp, "ZeroOrOne\n");
		break;
	case hyperlex::RegularExp::Range:
		fprintf(fp, "Range\n");
		break;
	default:
		fprintf(fp, "???????????\n");
		break;
	}
}
void RegularExp::Demo(FILE* fp, int L, int U)
{
	if (U != L)
	{
		if (L < 32 || L == 127) fprintf(fp, "[\\%d-", L);
		else fprintf(fp, "[%c-", (char)L);
		if (U < 32 || U == 127) fprintf(fp, "\\%d]", U);
		else fprintf(fp, "%c]", (char)U);
	}
	else
	{
		if (L < 32 || L == 127) fprintf(fp, "[\\%d]", L);
		else fputc((char)L, fp);
	}
}
void RegularExp::Demo(FILE* fp) const
{
	//buffer<size_t> output;
	//tree.inorderTraversal(output);
	vector<BiTree<item>*> stack;
	vector<int> state;
	size_t i;
	int SS;
	NodeType TT;
	BiTree<item>* now;
	if (tree != NULL)
	{
		state.append(0);
		stack.append(tree);
	}
	while (state.pop(SS) != 0)
	{
		now = stack.top();
		TT = now->content().type;
		//now->content().Demo(stdout);
		//printf("%d\n", SS);

		if (SS == 0)
		{
			if ((TT == Alternation || TT == Concatenation) && state.count() != 0)
				fputc('(', fp);
			state.append(1);
			if (now->left() != NULL)
			{
				state.append(0);
				stack.append(now->left());
			}
		}
		else if (SS == 1)
		{
			switch (TT)
			{
			case RegularExp::Concatenation:
				break;
			case RegularExp::Alternation:
				fputc('|', fp);
				break;
			case RegularExp::ZeroOrMore:
				fprintf(fp, "^{*}");
				break;
			case RegularExp::OneOrMore:
				fprintf(fp, "^{+}");
				break;
			case RegularExp::ZeroOrOne:
				fputc('?', fp);
				break;
			case RegularExp::Range:
				Demo(fp, now->content().lower, now->content().upper);
				break;
			default:
				break;
			}
			state.append(2);
			if (now->right() != NULL)
			{
				state.append(0);
				stack.append(now->right());
			}
		}
		else
		{
			if ((TT == Alternation || TT == Concatenation) && state.count() != 0)
				fputc(')', fp);
			stack.pop();
		}
	}
}

void RegularExp::set(void)
{
	if (tree != NULL)
	{
		tree->clear();
	}
	else
		tree = new BiTree<item>();

}
void RegularExp::clear(void)
{
	if (tree != NULL)
	{
		tree->clear();
		delete tree;
	}
	tree = NULL;
}
void RegularExp::set(int L, int U)
{
	set();
	tree->content().type = Range;
	tree->content().lower = L;
	tree->content().upper = U;
}
void RegularExp::set(int leaf)
{
	set(leaf, leaf);
}
void RegularExp::set(BiTree<item>* reg)
{
	if (tree != NULL)
	{
		tree->clear();
		delete tree;
	}
	tree = reg;
	reg = NULL;
}
void RegularExp::set(BiTree<item>* reg, RegularExp::NodeType T)
{
	set();
	tree->content().type = T;
	tree->left() = reg;
}
void RegularExp::set(BiTree<item>* regL, BiTree<item>* regR, RegularExp::NodeType T)
{
	set();
	tree->content().type = T;
	tree->left() = regL;
	tree->right() = regR;
}
void RegularExp::set(RegularExp* reg)
{
	if (tree != NULL)
	{
		tree->clear();
		delete tree;
	}
	tree = reg->tree;
	reg->tree = NULL;
}
void RegularExp::set(RegularExp* reg, NodeType T)
{
	set();
	tree->content().type = T;
	tree->left() = reg->tree;
	reg->tree = NULL;
}
void RegularExp::set(RegularExp* regL, RegularExp* regR, NodeType T)
{
	set();
	tree->content().type = T;
	tree->left() = regL->tree;
	tree->right() = regR->tree;
	regL->tree = NULL;
	regR->tree = NULL;
}
void RegularExp::set(RegularExp* regL, RegularExp* regR)
{
	set();
	tree->content().type = Range;
	tree->content().lower = regL->tree->content().lower;
	tree->content().upper = regR->tree->content().upper;
}
struct RegExp
{
	enum regular
	{
		_left_ = 1,
		_right_ = 2,
		_begin_ = 3,
		_end_ = 4,
		_range_ = 5,
		_star_ = 6,
		_plus_ = 7,
		_ZeroOrOne_ = 8,
		_idchar_ = 9,
		_CommonChar_ = 10,
		_Or_ = 11
	};
	enum group
	{
		__braket__ = 1,
		__superscript__ = 2,
		__char__ = 3,
		__Or__ = 4
	};
	static int next(int state, const char c);
	static int action(int state);
	static int GroupGet(int state);
};
int RegExp::next(int state, const char c)
{
	switch (state)
	{
	case 0:
		if (c == '\'') return 14;
		else if (c == '(') return 1;
		else if (c == ')') return 2;
		else if (c == '*') return 6;
		else if (c == '+') return 7;
		else if (c == '-') return 5;
		else if ('0' <= c && c <= '9') return 9;
		else if (c == '\?') return 8;
		else if ('A' <= c && c <= 'Z') return 9;
		else if (c == '[') return 3;
		else if (c == ']') return 4;
		else if (c == '_') return 9;
		else if ('a' <= c && c <= 'z') return 9;
		else if (c == '|') return 11;
		else return 0;
	case 1:
		return 0;
	case 2:
		return 0;
	case 3:
		return 0;
	case 4:
		return 0;
	case 5:
		return 0;
	case 6:
		return 0;
	case 7:
		return 0;
	case 8:
		return 0;
	case 9:
		return 0;
	case 10:
		return 0;
	case 11:
		return 0;
	case 12:
		if (c == '\'') return 10;
		else return 0;
	case 13:
		if (c == (char)0) return 12;
		else if (c == '\"') return 12;
		else if (c == '\'') return 12;
		else if ('0' <= c && c <= '7') return 16;
		else if (c == '\?') return 12;
		else if (c == 'X') return 15;
		else if (c == '\\') return 12;
		else if ('a' <= c && c <= 'b') return 12;
		else if (c == 'f') return 12;
		else if (c == 'n') return 12;
		else if (c == 'r') return 12;
		else if (c == 't') return 12;
		else if (c == 'v') return 12;
		else if (c == 'x') return 15;
		else return 0;
	case 14:
		if (' ' <= c && c <= '!') return 12;
		else if ('#' <= c && c <= '&') return 12;
		else if ('(' <= c && c <= '[') return 12;
		else if (c == '\\') return 13;
		else if (']' <= c && c <= '~') return 12;
		else return 0;
	case 15:
		if ('0' <= c && c <= '9') return 17;
		else if ('A' <= c && c <= 'F') return 17;
		else if ('a' <= c && c <= 'f') return 17;
		else return 0;
	case 16:
		if (c == '\'') return 10;
		else if ('0' <= c && c <= '7') return 18;
		else return 0;
	case 17:
		if (c == '\'') return 10;
		else if ('0' <= c && c <= '9') return 12;
		else if ('A' <= c && c <= 'F') return 12;
		else if ('a' <= c && c <= 'f') return 12;
		else return 0;
	case 18:
		if (c == '\'') return 10;
		else if ('0' <= c && c <= '7') return 12;
		else return 0;
	}
	return 0;
}
int RegExp::action(int state)
{
	switch (state)
	{
	case 1:
		return 1;//braket: left
	case 2:
		return 2;//braket: right
	case 3:
		return 3;//braket: begin
	case 4:
		return 4;//braket: end
	case 5:
		return 5;//braket: range
	case 6:
		return 6;//superscript: star
	case 7:
		return 7;//superscript: plus
	case 8:
		return 8;//superscript: ZeroOrOne
	case 9:
		return 9;//char: idchar
	case 10:
		return 10;//char: CommonChar
	case 11:
		return 11;//Or: Or
	}
	return 0;
}
int RegExp::GroupGet(int accept)
{
	switch (accept)
	{
	case 1:
		return 1;//braket: left
	case 2:
		return 1;//braket: right
	case 3:
		return 1;//braket: begin
	case 4:
		return 1;//braket: end
	case 5:
		return 1;//braket: range
	case 6:
		return 2;//superscript: star
	case 7:
		return 2;//superscript: plus
	case 8:
		return 2;//superscript: ZeroOrOne
	case 9:
		return 3;//char: idchar
	case 10:
		return 3;//char: CommonChar
	case 11:
		return 4;//Or: Or
	}
	return 0;
}
struct RegExpG
{
	enum type
	{
		accept = 0,
		error = 1,
		push = 2,
		reduce = 3
	};
	static const size_t StateCount;
	static const size_t NonTerminalCount;
	static const size_t TerminalCount;
	static const size_t RulesCount;
	static const int GOTO[23][7];
	static const int ACTION[23][12];
	static const int RulesToSymbol[15];
	static const int RulesLength[15];
	static const char* const RulesName[15];
	static const int Implicit[15];
};
const size_t RegExpG::StateCount = 23;
const size_t RegExpG::NonTerminalCount = 7;
const size_t RegExpG::TerminalCount = 11;
const size_t RegExpG::RulesCount = 15;
const int RegExpG::GOTO[23][7] = { \
{1, 6, 10, 14, 18, 22, 26}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 78, 18, 22, 26}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 62, 10, 14, 18, 22, 26}, \
{1, 1, 1, 1, 1, 1, 46}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 54}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 74, 14, 18, 22, 26}, \
{1, 1, 1, 78, 18, 22, 26}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1} };
//==============================
const int RegExpG::ACTION[23][12] = { \
{1, 30, 1, 34, 1, 1, 1, 1, 1, 38, 42, 1}, \
{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 70}, \
{7, 30, 7, 34, 1, 1, 1, 1, 1, 38, 42, 7}, \
{15, 15, 15, 15, 1, 1, 82, 86, 90, 15, 15, 15}, \
{23, 23, 23, 23, 1, 1, 23, 23, 23, 23, 23, 23}, \
{39, 39, 39, 39, 1, 1, 39, 39, 39, 39, 39, 39}, \
{47, 47, 47, 47, 1, 1, 47, 47, 47, 47, 47, 47}, \
{1, 30, 1, 34, 1, 1, 1, 1, 1, 38, 42, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 38, 42, 1}, \
{55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55}, \
{59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59}, \
{1, 1, 1, 1, 1, 50, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 38, 42, 1}, \
{1, 1, 1, 1, 58, 1, 1, 1, 1, 1, 1, 1}, \
{51, 51, 51, 51, 1, 1, 51, 51, 51, 51, 51, 51}, \
{1, 1, 66, 1, 1, 1, 1, 1, 1, 1, 1, 70}, \
{43, 43, 43, 43, 1, 1, 43, 43, 43, 43, 43, 43}, \
{1, 30, 1, 34, 1, 1, 1, 1, 1, 38, 42, 1}, \
{11, 30, 11, 34, 1, 1, 1, 1, 1, 38, 42, 11}, \
{19, 19, 19, 19, 1, 1, 82, 86, 90, 19, 19, 19}, \
{31, 31, 31, 31, 1, 1, 31, 31, 31, 31, 31, 31}, \
{27, 27, 27, 27, 1, 1, 27, 27, 27, 27, 27, 27}, \
{35, 35, 35, 35, 1, 1, 35, 35, 35, 35, 35, 35} };
//==============================
const int RegExpG::RulesToSymbol[15] = { \
0,\
1,\
1,\
2,\
2,\
3,\
3,\
3,\
3,\
4,\
4,\
5,\
5,\
6,\
6 };
//==============================
const int RegExpG::RulesLength[15] = { \
1,\
1,\
3,\
1,\
2,\
1,\
2,\
2,\
2,\
1,\
3,\
1,\
5,\
1,\
1 };
//==============================
const char* const RegExpG::RulesName[15] = { \
"Ep->RegOr ",\
"RegOr->Reg ",\
"RegOr->RegOr Or Reg ",\
"Reg->Complex ",\
"Reg->Reg Complex ",\
"Complex->Node ",\
"Complex->Complex plus ",\
"Complex->Complex star ",\
"Complex->Complex ZeroOrOne ",\
"Node->Range ",\
"Node->left RegOr right ",\
"Range->Char ",\
"Range->begin Char range Char end ",\
"Char->reserved ",\
"Char->CommonChar " };
int RegularExp::set(const char* input)
{
	int error;
	Morpheme eme;
	eme.Build<RegExp>(input);
	//error = fastBuild(eme);
	error = standardBuild(eme);
	return error;
}
const int RegExpG::Implicit[15] = { \
0,\
0,\
0,\
0,\
0,\
0,\
0,\
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0 \
};

void RegularExp::copy(RegularExp& source)
{
	clear();
	tree = source.tree->copy();
}
int RegularExp::fastBuild(Morpheme& input)
{
	/*
terminal-> left | right | begin | end | range | star
| plus | ZeroOrOne | reserved | CommonChar | Or;

Ep->RegOr;
RegOr->Reg | RegOr Or Reg;
Reg->Complex | Reg Complex;
Complex->Node | Complex plus | Complex star | Complex ZeroOrOne;
Node->Range | left RegOr right;
Range->Char | begin Char range Char end;
Char->reserved | CommonChar;
*/

	vector<int> stack;
	vector<RegularExp*> TempTree;
	RegularExp* TreeNow;
	bool DoNext;
	size_t length, head, i, begin_, inputCount_;
	int symbol, top, temp, information;
	int GoFull, GoD, GoI, error;
	RegExpG::type type;
	RegularExp::NodeType TypeTemp;
	stack.append(0);
	head = 0;
	inputCount_ = input.GetCount();
	DoNext = true;
	do
	{
		if (head >= inputCount_)
		{
			error = -1;
			break;
		}
		top = stack.top();
		temp = RegExpG::ACTION[top][input[head].accept];
		information = temp / 4;
		type = (RegExpG::type)(temp % 4);
		//printf( "T = %5d, top = %5d, information = %5d, type = %5d, ", input[head].accept, top, information, (int)type);
		//printf("head = %5zu, lex = %s, \n", head, input.GetWord(head));
		TypeTemp = ZeroOrOne;
		switch (type)
		{
		case RegExpG::accept:
			error = 0;
			DoNext = false;
			//TempTree.top()->Demo(stdout);
			set(TempTree.top());
			break;
		case RegExpG::error:
			error = information;
			DoNext = false;
			break;
		case RegExpG::push:
			//printf("<%8zu, %4d: %4d , %s>\n", head, input[head].category, input[head].accept, input.GetWord(head));
			stack.append(information);
			TreeNow = new RegularExp;
			TreeNow->set(input.GetChar(head));
			TempTree.append(TreeNow);
			head += 1;
			break;
		case RegExpG::reduce:
			symbol = RegExpG::RulesToSymbol[information];
			length = RegExpG::RulesLength[information];
			begin_ = TempTree.count() - length;
			switch (information)
			{
				//No[0], case Ep: prefix: 0, degeneracy: 1
			case 0: //0: Ep, No[0] production rules: Ep->RegOr 
				//No[1], case RegOr: prefix: 1, degeneracy: 2
			case 1: //1: RegOr, No[0] production rules: RegOr->Reg
				TreeNow = TempTree[begin_];
				TempTree[begin_] = NULL;
				break;
			case 2: //1: RegOr, No[1] production rules: RegOr->RegOr Or Reg
				TreeNow = new RegularExp;
				TreeNow->set(TempTree[begin_], TempTree[begin_ + 2], RegularExp::Alternation);
				break;
				//No[2], case Reg: prefix: 3, degeneracy: 2
			case 3: //2: Reg, No[0] production rules: Reg->Complex
				TreeNow = TempTree[begin_];
				TempTree[begin_] = NULL;
				break;
			case 4: //2: Reg, No[1] production rules: Reg->Reg Complex
				TreeNow = new RegularExp;
				TreeNow->set(TempTree[begin_], TempTree[begin_ + 1], RegularExp::Concatenation);
				break;
				//No[3], case Complex: prefix: 5, degeneracy: 4
			case 5: //3: Complex, No[0] production rules: Complex->Node
				TreeNow = TempTree[begin_];
				TempTree[begin_] = NULL;
				break;
			case 6: //3: Complex, No[1] production rules: Complex->Complex plus
				TreeNow = new RegularExp;
				TreeNow->set(TempTree[begin_], RegularExp::OneOrMore);
				break;
			case 7: //3: Complex, No[2] production rules: Complex->Complex star
				TypeTemp = RegularExp::ZeroOrMore;
			case 8: //3: Complex, No[3] production rules: Complex->Complex ZeroOrOne
				TreeNow = new RegularExp;
				TreeNow->set(TempTree[begin_], TypeTemp);
				break;
				//No[4], case Node: prefix: 9, degeneracy: 2
			case 9: //4: Node, No[0] production rules: Node->Range
				TreeNow = TempTree[begin_];
				TempTree[begin_] = NULL;
				break;
			case 10: //4: Node, No[1] production rules: Node->left RegOr right
				TreeNow = TempTree[begin_ + 1];
				TempTree[begin_ + 1] = NULL;
				break;
				//No[5], case Range: prefix: 11, degeneracy: 2
			case 11: //5: Range, No[0] production rules: Range->Char
				TreeNow = TempTree[begin_];
				TempTree[begin_] = NULL;
				break;
			case 12: //5: Range, No[1] production rules: Range->begin Char range Char end
				TreeNow = new RegularExp;
				TreeNow->set(TempTree[begin_ + 1], TempTree[begin_ + 3]);
				break;
				//No[6], case Char: prefix: 13, degeneracy: 2
			case 13: //6: Char, No[0] production rules: Char->reserved
			case 14: //6: Char, No[1] production rules: Char->CommonChar
				TreeNow = TempTree[begin_];
				TempTree[begin_] = NULL;
				break;
			}
			for (i = 0; i < length; i++)
			{
				stack.pop();
				delete TempTree[begin_ + i];
			}
			TempTree.recount(begin_);
			GoFull = RegExpG::GOTO[stack.top()][symbol];
			GoD = GoFull / 4;
			GoI = GoFull % 4;
			stack.append(GoD);
			TempTree.append(TreeNow);
			//TreeNow->Demo(stdout);
			break;
		}

	} while (DoNext);
	for (i = 0; i < TempTree.count(); i++) delete TempTree[i];
	return error;
}
int RegularExp::standardBuild(Morpheme& eme)
{
	GrammarTree Tree;
	int error;
	//eme.Demo(stdout);
	error = Tree.build<RegExpG>(eme);
	//Tree.Demo(stdout, eme, RegExpG::RulesName);
	set(eme, Tree.GT);
	return error;
}
void RegularExp::set(const Morpheme& eme, hyperlex::tree<GrammarTree::TreeInfor>* Tree)
{
	RegularExp* TreeNow;
	RegularExp* Ltemp, * Rtemp;
	size_t site_;
	hyperlex::tree<GrammarTree::TreeInfor>* GT;
	hyperlex::tree<GrammarTree::TreeInfor>::PostIterator iterator;
	iterator.initial(Tree);
	TreeNow = NULL;
	while (iterator.still())
	{
		GT = iterator.target();
		if (iterator.state() != 0)
		{
			//TreeNow = (RegularExp*);
			site_ = GT->root().site;
			if ((RegularExp*)GT->root().rules)
			{
				switch (site_)
				{
					//No[0], case Ep: prefix: 0, degeneracy: 1
				case 0: //0: Ep, No[0] production rules: Ep->RegOr 
					//No[1], case RegOr: prefix: 1, degeneracy: 2
				case 1: //1: RegOr, No[0] production rules: RegOr->Reg
					TreeNow = (RegularExp*)(GT->child(0)->root().infor);
					GT->child(0)->root().infor = NULL;
					break;
				case 2: //1: RegOr, No[1] production rules: RegOr->RegOr Or Reg
					TreeNow = new RegularExp;
					Ltemp = (RegularExp*)(GT->child(0)->root().infor);
					Rtemp = (RegularExp*)(GT->child(2)->root().infor);
					TreeNow->set(Ltemp, Rtemp, RegularExp::Alternation);
					break;
					//No[2], case Reg: prefix: 3, degeneracy: 2
				case 3: //2: Reg, No[0] production rules: Reg->Complex
					TreeNow = (RegularExp*)(GT->child(0)->root().infor);
					GT->child(0)->root().infor = NULL;
					break;
				case 4: //2: Reg, No[1] production rules: Reg->Reg Complex
					TreeNow = new RegularExp;
					Ltemp = (RegularExp*)(GT->child(0)->root().infor);
					Rtemp = (RegularExp*)(GT->child(1)->root().infor);
					TreeNow->set(Ltemp, Rtemp, RegularExp::Concatenation);
					break;
					//No[3], case Complex: prefix: 5, degeneracy: 4
				case 5: //3: Complex, No[0] production rules: Complex->Node
					TreeNow = (RegularExp*)(GT->child(0)->root().infor);
					GT->child(0)->root().infor = NULL;
					break;
				case 6: //3: Complex, No[1] production rules: Complex->Complex plus
					TreeNow = new RegularExp;
					Ltemp = (RegularExp*)(GT->child(0)->root().infor);
					TreeNow->set(Ltemp, RegularExp::OneOrMore);
					break;
				case 7: //3: Complex, No[2] production rules: Complex->Complex star
					TreeNow = new RegularExp;
					Ltemp = (RegularExp*)(GT->child(0)->root().infor);
					TreeNow->set(Ltemp, RegularExp::ZeroOrMore);
					break;
				case 8: //3: Complex, No[3] production rules: Complex->Complex ZeroOrOne
					TreeNow = new RegularExp;
					Ltemp = (RegularExp*)(GT->child(0)->root().infor);
					TreeNow->set(Ltemp, RegularExp::ZeroOrOne);
					break;
					//No[4], case Node: prefix: 9, degeneracy: 2
				case 9: //4: Node, No[0] production rules: Node->Range
					TreeNow = (RegularExp*)(GT->child(0)->root().infor);
					GT->child(0)->root().infor = NULL;
					break;
				case 10: //4: Node, No[1] production rules: Node->left RegOr right
					TreeNow = (RegularExp*)(GT->child(1)->root().infor);
					GT->child(1)->root().infor = NULL;
					break;
					//No[5], case Range: prefix: 11, degeneracy: 2
				case 11: //5: Range, No[0] production rules: Range->Char
					TreeNow = (RegularExp*)(GT->child(0)->root().infor);
					GT->child(0)->root().infor = NULL;
					break;
				case 12: //5: Range, No[1] production rules: Range->begin Char range Char end
					TreeNow = new RegularExp;
					Ltemp = (RegularExp*)(GT->child(1)->root().infor);
					Rtemp = (RegularExp*)(GT->child(3)->root().infor);
					TreeNow->set(Ltemp, Rtemp);
					delete Ltemp;
					delete Rtemp;
					Ltemp = NULL;
					Rtemp = NULL;
					break;
					//No[6], case Char: prefix: 13, degeneracy: 2
				case 13: //6: Char, No[0] production rules: Char->reserved
				case 14: //6: Char, No[1] production rules: Char->CommonChar
					TreeNow = (RegularExp*)(GT->child(0)->root().infor);
					GT->child(0)->root().infor = NULL;
					break;
				}
			}
			else
			{
				TreeNow = new RegularExp;
				TreeNow->set(eme.GetChar(site_));
			}
			GT->root().infor = TreeNow;
		}
		iterator.next();
	}
	TreeNow = (RegularExp*)(Tree->root().infor);
	set(TreeNow);
}

#include<string.h>
FilePath::FilePath()
{
	absolute = false;
	FullPath = NULL;
}
FilePath::~FilePath()
{
	clear();
}
void FilePath::clear()
{
	for (size_t i = 0; i < PathUnit.count(); ++i)
	{
		free(PathUnit[i]);
	}
	PathUnit.clear();
	free(FullPath);
	FullPath = NULL;
}
void FilePath::build(const char* path)
{
	clear();
	if (!path || *path == '\0') return;

	absolute = (path[0] == '/' || path[0] == '\\');
	const char* start = absolute ? path + 1 : path;

	while (*start) {
		while (*start == '/' || *start == '\\') ++start;
		if (!*start) break;

		const char* end = start;
		while (*end && *end != '/' && *end != '\\') ++end;

		char* unit = (char*)malloc(end - start + 1);
		memcpy(unit, start, end - start);
		unit[end - start] = '\0';

		append_copy(unit);
		free(unit);
		

		start = end;
	}

	refresh();
}
void FilePath::operator+=(const FilePath& rhs)
{
	if (rhs.absolute) {
		clear();
		absolute = true;
		for (size_t i = 0; i < rhs.PathUnit.count(); ++i) {
			append_copy(rhs.PathUnit[i]);
		}
	}
	else {
		for (size_t i = 0; i < rhs.PathUnit.count(); ++i) {
			append_copy(rhs.PathUnit[i]);
		}
	}
	refresh();
}
bool FilePath::operator==(const FilePath& rhs) const
{
	// 比较绝对路径标志是否相同
	if (absolute != rhs.absolute)
		return false;

	// 比较路径单元数量是否相同
	if (PathUnit.count() != rhs.PathUnit.count())
		return false;

	// 逐个比较每个路径单元
	for (size_t i = 0; i < PathUnit.count(); ++i) {
		if (strcmp(PathUnit[i], rhs.PathUnit[i]) != 0)
			return false;
	}

	return true;
}
char* FilePath::print(char divider)const
{
	size_t total = absolute ? 1 : 0;
	const size_t count = PathUnit.count();

	for (size_t i = 0; i < count; ++i) {
		total += strlen(PathUnit[i]) + (i != count - 1 ? 1 : 0);
	}

	char* buf = (char*)malloc(total + 1);
	char* ptr = buf;

	if (absolute && count > 0)
	{
		*ptr = divider;
		ptr++;
	}

	for (size_t i = 0; i < count; ++i) {
		const char* unit = PathUnit[i];
		const size_t len = strlen(unit);
		memcpy(ptr, unit, len);
		ptr += len;
		if (i != count - 1) 
		{
			*ptr = divider;
			ptr++;
		}
	}
	*ptr = '\0';

	return buf;
}
const char* FilePath::path(void) const
{
	return FullPath;
}
void FilePath::copy(const FilePath& source)
{
	clear();
	absolute = source.absolute;
	for (size_t i = 0; i < source.PathUnit.count(); i++)
	{
		append_copy(source.PathUnit[i]);
	}
	refresh();
}
void FilePath::demo(FILE* fp)const
{
	fprintf(fp, "[FilePath Demo]");
	fprintf(fp, "-Absolute: %s, ", absolute ? "true" : "false");

	fprintf(fp, "-Path: [%s], ", FullPath);

	fprintf(fp, "-Units: ");
	for (size_t i = 0; i < PathUnit.count(); i++)
	{
		const char* unit = PathUnit[i];
		fprintf(fp, "[%s] ", unit);
	}
	fprintf(fp, "\n");
}
void FilePath::append_copy(const char* str)
{
	if (strcmp(str, ".") == 0)
	{
		if (PathUnit.count() == 0)
		{
			char* copy = (char*)malloc(strlen(str) + 1);
			strcpy(copy, str);
			PathUnit.append(copy);
		}
	}
	else if (strcmp(str, "..") == 0) {
		// 父目录符号
		if (PathUnit.count() > 1) {
			// 移除前一个路径单元（回退一级）
			char* last;
			PathUnit.pop(last);
			free(last);
		}
		else if (PathUnit.count() == 1)
		{
			if (strcmp(PathUnit[0], "..") == 0)
			{
				char* copy = (char*)malloc(strlen("..") + 1);
				strcpy(copy, "..");
				PathUnit.append(copy);
			}
			else if (strcmp(PathUnit[0], ".") == 0)
			{
				char* copy = (char*)malloc(strlen("..") + 1);
				strcpy(copy, "..");
				free(PathUnit[0]);
				PathUnit[0] = copy;
			}
			else
			{
				PathUnit[0][0] = '.';
				PathUnit[0][1] = '\0';
			}
			
		}
		else if (!absolute)
		{
			char* copy = (char*)malloc(strlen(str) + 1);
			strcpy(copy, str);
			PathUnit.append(copy);
		}
		
	}
	else {
		char* copy = (char*)malloc(strlen(str) + 1);
		strcpy(copy, str);
		PathUnit.append(copy);
	}

}
void FilePath::RearCut(void)
{
	if (PathUnit.count() != 0)
	{
		char* out;
		PathUnit.pop(out);
		free(out);
		return;
	}
	else if(absolute)
	{
		absolute = false;
	}
	refresh();
}
void FilePath::clean(void)
{
	if (PathUnit.count() == 0)return;

	vector<char*>temp;
	temp.copy(PathUnit);
	
	for (size_t i = 0; i < temp.count(); i++)
	{
		char* unit = temp[i];
		if (strcmp(unit, "..") == 0)
		{
			long int j;
			for (j = i - 1; j >= 0L; j--)
				if (strcmp(temp[j], ".") != 0) break;
			if (j >= 0L)
			{
				unit[1] = '\0';
				temp[j][0] = '.';
				temp[j][1] = '\0';
			}
			else if(absolute) unit[1] = '\0';
		}
	}
	size_t offset = 0;
	PathUnit[offset] = temp[0];
	offset++;
	for (size_t i = 1; i < temp.count(); i++)
	{
		char* unit = temp[i];
		if (strcmp(unit, ".") == 0)
			free(unit);
		else
		{
			PathUnit[offset] = unit;
			offset++;
		}
	}
	PathUnit.recount(offset);
	refresh();
}
void FilePath::RearCutAppend(const FilePath& rhs)
{
	RearCut();
	if (rhs.absolute) {
		clear();
		absolute = true;
		for (size_t i = 0; i < rhs.PathUnit.count(); ++i) {
			append_copy(rhs.PathUnit[i]);
		}
	}
	else {
		for (size_t i = 0; i < rhs.PathUnit.count(); ++i) {
			append_copy(rhs.PathUnit[i]);
		}
	}
	refresh();
}
void FilePath::RearOnlyAppend(const FilePath& rhs)
{
	size_t i = rhs.PathUnit.count();
	if (i != 0) append_copy(rhs.PathUnit[i - 1]);
	refresh();
}
void FilePath::refresh(void)
{
	free(FullPath);
	FullPath = print();
}

static const char* Copy(const char* input)
{
	char* nnnn;
	size_t length, i;
	for (length = 0; input[length] != '\0'; length++);
	length += 1;
	nnnn = (char*)malloc(sizeof(char) * length);
	for (i = 0; i < length; i++) nnnn[i] = input[i];
	return nnnn;
}








//如果all 没有被定义为词法单元(终结符号)那么all不能出现在文法产生式体内。
//反之all 被定义为词法单元，当all出现在文法产生式体内它会被视为一个终结符号。
//如果void被定义为一个词法单元(终结符号)，那么它将不能表达一个空产生式，
//换言之整个，输入文件系统不能再接受空符号。

InputPanel::InputPanel()
{
	//const char *s_temp;
	GrammarEnclosed = false;
	
	//s_temp = Copy("void");
	//Terminal.append(s_temp);
	RootName = NULL;

	//addVoidGroup();

	errorCode = buildUndone;
	errorInfor1 = 0;
	errorInfor2 = 0;
	errorInfor3 = NULL;
	errorInfor4 = true;
	return;
}
InputPanel::~InputPanel()
{
	clear();
}
void InputPanel::initial(void)
{
	//const char *s_temp;
	GrammarEnclosed = false;

	//s_temp = Copy("void");
	//Terminal.append(s_temp);
	RootName = NULL;

	LexicalSource.append("source");
	addVoidGroup();

	errorCode = buildUndone;
	errorInfor1 = 0;
	errorInfor2 = 0;
	errorInfor3 = NULL;
	errorInfor4 = true;
	return;
}
void InputPanel::clear(void)
{
	size_t i;
	//printf("here!\n");
	for (i = 0; i < RegG.count(); i++) delete RegG[i];
	RegG.clear();
	//for (i = 0; i < RegC.count(); i++) delete RegC[i];
	//printf("here!\n");
	for (i = 0; i < GrammarG.count(); i++) delete GrammarG[i];
	GrammarG.clear();
	//printf("here!\n");
	//for (i = 0; i < rules.count(); i++) delete rules[i];
	rules.clear();

	//printf("here!\n");
	for (i = 0; i < Terminal.count(); i++) free((void*)Terminal[i]);
	Terminal.clear();
	//printf("here!\n");
	for (i = 0; i < NontTerminal.count(); i++) free((void*)NontTerminal[i]);
	NontTerminal.clear();

	rules.clear();

	free(RootName);
	RootName = NULL;
	
	MorphemePre.clear();
	LexicalSource.clear();

	GrammarEnclosed = false;
	errorCode = NoError;
	errorInfor1 = 0;
	errorInfor2 = 0;
	errorInfor3 = NULL;
	errorInfor4 = true;
}

InputPanel::RegGroup::RegGroup()
{
	name = NULL;
	priority = 0;
	prefix = 0;
}
InputPanel::RegGroup::~RegGroup()
{
	size_t i;
	for (i = 0; i < regs.count(); i++) delete regs[i];
}
void InputPanel::RegGroup::SetName(const char* input)
{
	name = Copy(input);
}
InputPanel::RegContent::RegContent()
{
	reg = NULL;
	name = NULL; 
	priority = 0;
	accept = 1;
	group = 0;
}
InputPanel::RegContent::~RegContent()
{
	delete reg;
	free((void*)name);
	name = NULL;
	reg = NULL;
}
void InputPanel::RegContent::SetName(const char* input)
{
	name = Copy(input);
}
InputPanel::Group::Group()
{
	implicit = false;

	name = NULL;
	group = 0;
	offset = 0;
	count = 0;
}
InputPanel::Group::~Group()
{
	size_t i;
	free((void*)name);
	for (i = 0; i < rules.count(); i++)
		delete rules[i];
}
void InputPanel::Group::SetName(const char* input)
{
	name = Copy(input);
}
InputPanel::Rules::Rules()
{
	name = NULL;
	implicit = false;
}
InputPanel::Rules::~Rules()
{
	free((void*)name);
	name = NULL;
}
void InputPanel::Rules::SetName(const char* input)
{
	name = Copy(input);
}
void InputPanel::Rules::demo(FILE* fp, cvccp& N, cvccp& T)
{
	long int index;
	size_t i;
	for (i = 0; i < formula.count(); i++)
	{
		index = formula[i];
		if (index >= 0)
			fprintf(fp, "%s ", N[index]);
		else
			fprintf(fp, "%s ", T[-index - 1]);
	}
}
void InputPanel::demo(FILE* fp) const
{
	if (errorCode != NoError)
	{
		ErrorDemo(fp);
		return;
	}
	demoL(fp);
	if(GrammarEnclosed) demoG(fp);
}


void InputPanel::ErrorDemo(FILE* fp) const
{
	const char* s_temp_1, * s_temp_2; 
	size_t i, j, record;
	switch (errorCode)
	{
	case InputPanel::NoError:
		fprintf(fp, "No Error!\n");
		break;
	case InputPanel::ConflictRegGroupName:
		fprintf(fp, "ConflictRegGroupName: Conflict regular expression's group name\n");
		fprintf(fp, "There has been a group named as: %s\n", RegG[errorInfor1]->name);
		break;
	case InputPanel::repeatRegName:
		s_temp_1 = RegG[errorInfor1]->regs[errorInfor2]->name;
		s_temp_2 = RegG[errorInfor1]->name;
		fprintf(fp, "ConflictRegGroupName: Conflict regular expression's name\n");
		fprintf(fp, "There has been a regular expression as: %s in group: %s\n", s_temp_1, s_temp_2);
		break;
	case InputPanel::missingId:
		fprintf(fp, "missingId: the identifier %s has not been defined\n", errorInfor3);
		break;
	case InputPanel::repeatGGroupName:
		if(errorInfor4) s_temp_1 = GrammarG[errorInfor1]->name;
		else s_temp_1 = NontTerminal[errorInfor1];
		fprintf(fp, "repeatGGroupName: Conflict No.%zu grammar Nonternimal symbol name: %s\n", errorInfor1, s_temp_1);
		break;
	case InputPanel::repeatGName:
		s_temp_1 = GrammarG[errorInfor1]->rules[errorInfor2]->name;
		fprintf(fp, "repeatGName: Conflict grammar rule name: %s\n", s_temp_1);
		break;
	case InputPanel::PretreatLEXICAL:
	{
		fprintf(fp, "PretreatLEXICAL: Error happenned during pretreatment.\n");
		fprintf(fp, "Lexical analysis line: %zu of No.%zu ", errorInfor2, errorInfor1);
		fprintf(fp, "source file %s made a mistake\n", MorphemePre.GetFile(errorInfor1));
		break;
	}	
	case InputPanel::PretreatGRAMMAR:
	{
		fprintf(fp, "PretreatGRAMMAR: Error happenned during pretreatment.\n");
		fprintf(fp, "Grammar analysis of No.%zu ", errorInfor1);
		fprintf(fp, "source file %s made a mistake ", MorphemePre.GetFile(errorInfor1));

		size_t RLine = MorphemePre[errorInfor2].line;
		size_t RFile = MorphemePre[errorInfor2].file;

		fprintf(fp, "in line %zu, file: %zu, unit: %zu\n", RLine, RFile, errorInfor2);

		//fprintf(fp, "%zu\n", record);
		for (i = 0; i < MorphemePre.GetCount(); i++)
		{
			size_t uintTemp1 = MorphemePre[i].line;
			size_t uintTemp2 = MorphemePre[i].file;
			if ((RLine == uintTemp1 || uintTemp1 + 1 == RLine) && uintTemp2 == RFile)
			{
				if (i == errorInfor1)
					fprintf(fp, "| %s |", MorphemePre.GetWord(i));
				else
					fprintf(fp, "%s", MorphemePre.GetWord(i));
			}
		}
		break;
	}
	case InputPanel::PretreatRepeat:
	{
		fprintf(fp, "PretreatRepeat: Error happenned during pretreatment.\n");
		fprintf(fp, "No.%zu ", errorInfor1);
		fprintf(fp, "source file %s repeats with existed ", MorphemePre.GetFile(errorInfor1));
		fprintf(fp, "%zu file %s\n", errorInfor2, MorphemePre.GetFile(errorInfor2));
		break;
	}
	case InputPanel::PretreatOpenfail:
	{
		fprintf(fp, "PretreatOpenfail: Error happenned during pretreatment.\n");
		fprintf(fp, "Open of No.%zu ", errorInfor1);
		fprintf(fp, "source file %s made a mistake\n", MorphemePre.GetFile(errorInfor1));
		break;
	}
	case InputPanel::ErrorNonTernimal:
	{
		fprintf(fp, "ErrorNonTernimal: Non-ternimal symbols in rules' body are conflict with their names.\n");
		fprintf(fp, "Non-ternimal count in body: %zu, in their head: %zu\n", NontTerminal.count(), GrammarG.count());
		
		for (i = 0; i < NontTerminal.count(); i++)
		{
			const char* Lstring = NontTerminal[i];
			for (j = 0; j < GrammarG.count(); j++)
			{
				const char* Rstring = GrammarG[j]->name;
				if (compare(Lstring, Rstring)) break;
			}
			if (j == GrammarG.count())
			{
				fprintf(fp, "NontTerminal[%zu]: %s, unmatched\n", i, NontTerminal[i]);
			}
		}
		for (i = 0; i < GrammarG.count(); i++)
		{
			const char* Lstring = GrammarG[i]->name;
			for (j = 0; j < NontTerminal.count(); j++)
			{
				const char* Rstring = NontTerminal[j];
				if (compare(Lstring, Rstring)) break;
			}
			if (j == NontTerminal.count())
			{
				fprintf(fp, "GrammarG[%zu]->name: %s, unmatched\n", i, NontTerminal[i]);
			}
		}
		fprintf(fp, "//=======================details=======================.\n");
		for (i = 0; i < NontTerminal.count() && i < GrammarG.count(); i++)
			fprintf(fp, "symbol[%zu]: %s, %s\n", i, NontTerminal[i], GrammarG[i]->name);
		for (j = i; j < NontTerminal.count(); j++)
			fprintf(fp, "symbol[%zu]: %s, None\n", j, NontTerminal[j]);
		for (j = i; j < GrammarG.count(); j++)
			fprintf(fp, "symbol[%zu]: None, %s\n", j, GrammarG[j]->name);
		break;
	}
	case InputPanel::WorngRuleBody:
	{
		fprintf(fp, "WorngRuleBody: symbol 'all' can not appear in rule body as a non-ternimal symbol.\n");
		fprintf(fp, "symbol[%zu](%s)", errorInfor1, GrammarG[errorInfor1]->name);
		fprintf(fp, "rule[%zu](%s): ", errorInfor2, (GrammarG[errorInfor1])->rules[errorInfor2]->name);
		GrammarG[errorInfor1]->rules[errorInfor2]->demo(fp, NontTerminal, Terminal);
		fprintf(fp, "\n");
		break;
	}
		
	case InputPanel::missingIdinRegdef:
		fprintf(fp, "missingIdinRegdef: need definition of regular expression: %s.\n", errorInfor3);
		fprintf(fp, "\n");
		break;
	case InputPanel::ErrorinputLEXICAL:
	{
		fprintf(fp, "ErrorinputLEXICAL: \n");
		record = 0;
		for (i = 0; i < LexicalSource.GetCount(); i++)
		{
			if (LexicalSource[i].accept < 0)
			{
				record = LexicalSource[i].line;
				break;
			}
		}
		fprintf(fp, "line[%zu]: ", record + 1);
		for (i = 0; i < LexicalSource.GetCount(); i++)
		{
			size_t  uintTemp = LexicalSource[i].line;
			if (record == uintTemp)
			{
				fprintf(fp, "%s", LexicalSource.GetWord(i));
			}
		}
		break;
	}
	case InputPanel::ErrorinputGrammar:
	{
		fprintf(fp, "ErrorinputGrammar: Something was wrong when parsing of line:");
		size_t RLine = LexicalSource[errorInfor1].line;
		size_t RFile = LexicalSource[errorInfor1].file;
		fprintf(fp, "%zu", RLine);
		fprintf(fp, " of No.%zu file %s\n", RFile, LexicalSource.GetFile(RFile));
		for (i = 0; i < LexicalSource.GetCount(); i++)
		{
			size_t uintTemp1 = LexicalSource[i].line;
			size_t uintTemp2 = LexicalSource[i].file;
			if ((RLine == uintTemp1 || uintTemp1 + 1 == RLine) && uintTemp2 == RFile)
			{
				if (i == errorInfor1)
					fprintf(fp, "| %s |", LexicalSource.GetWord(i));
				else
					fprintf(fp, "%s", LexicalSource.GetWord(i));
			}
		}
		break;
	}
	case InputPanel::regGroupMissing:
	{
		fprintf(fp, "regGroupMissing: There is no corresponding regular expression group as:");
		fprintf(fp, "%s\n", LexicalSource.GetWord(errorInfor1));
		record = LexicalSource[errorInfor1].line;
		fprintf(fp, "line[%zu]: ", record + 1);
		for (i = 0; i < LexicalSource.GetCount(); i++)
		{
			size_t uintTemp = LexicalSource[i].line;
			if (i == errorInfor1)
				fprintf(fp, "| %s |", LexicalSource.GetWord(i));
			else
				fprintf(fp, "%s", LexicalSource.GetWord(i));
		}
		break;
	}
	case InputPanel::buildUndone:
		fprintf(fp, "buildUndone: has not been built.\n");
		fprintf(fp, "\n");
		break;
	default:
		fprintf(fp, "Error Unknown: \n");
	}
}
void InputPanel::demoL(FILE* fp) const
{
	size_t i, j;
	RegGroup* group;
	RegContent* content;
	fprintf(fp, "lexical:{\n");
	for (i = 0; i < RegG.count(); i++)
	{
		group = RegG[i];
		fprintf(fp, "\t%s", group->name ? group->name : "None");
		fprintf(fp, "(%d): \n", group->priority);
		//fprintf(fp, "Prefix: %zu\n", group->prefix);
		fprintf(fp, "\t{\n");
		for (j = 0; j < group->regs.count(); j++)
		{
			content = group->regs[j];
			fprintf(fp, "\t\t[%ld]%s(%ld): ", content->accept, content->name ? content->name : "None", content->priority);
			content->reg->Demo(stdout);
			fprintf(fp, ";\n");
		}
		fprintf(fp, "\t}\n");
	}
	fprintf(fp, "};\n");
	
}
void InputPanel::demoG(FILE* fp) const
{
	// 输出Terminal/NonTerminal符号
	size_t i, j, site;
	Rules* rule;
	Group* g;
	
	
	fprintf(fp, "\n=== Symbols ===\n");
	fprintf(fp, "Terminals (%zu):\n", Terminal.count());
	for (i = 0; i < Terminal.count(); i++)
		fprintf(fp, "\t[%zu]: %s\n", i, Terminal[i]);
	fprintf(fp, "NonTerminals (%zu):\n", NontTerminal.count());
	for (i = 0; i < NontTerminal.count(); i++)
		fprintf(fp, "\t[%zu]: %s\n", i, NontTerminal[i]);

	// 输出Grammar Groups
	fprintf(fp, "all[%5zu]: (%5zu)%s\n", (size_t)0, (size_t)0, RootName ? RootName : "None");
	site = 1;
	for (i = 1; i < GrammarG.count(); i++)
	{
		g = GrammarG[i];
		fprintf(fp, "%s[%5zu]: count: %5zu\n", g->name, i, g->rules.count());
		for (j = 0; j < g->rules.count(); j++)
		{
			rule = g->rules[j];
			fprintf(fp, "\t[%5zu]: %s(%5zu): ", j, rule->name, site);
			site++;
			rule->demo(fp, NontTerminal, Terminal);
			fprintf(fp, "\n");
		}
	}
}
void InputPanel::printGName(FILE* output, FILE* infor, const char* nameG)const
{
	size_t i, j;
	size_t site, length;
	Rules* now;
	Group* GG;
	const char* outstring_;
	bool implicit;
	site = 0;
	fprintf(output, "\tenum rules\n\t{\n");
	for (i = 0; i < GrammarG.count(); i++)
	{
		GG = GrammarG[i];
		for (j = 0; j < GG->rules.count(); j++)
		{
			now = GG->rules[j];
			implicit = now->implicit;
			if (implicit)
			{
				if (site == 0)
					outstring_ = "\t\t//%s_%s_ = %d";
				else
					outstring_ = ",\n\t\t//%s_%s_ = %d";
			}
			else
			{
				if (site == 0)
					outstring_ = "\t\t%s_%s_ = %d";
				else
					outstring_ = ",\n\t\t%s_%s_ = %d";
			}
			fprintf(output, outstring_, GG->name, now->name, (int)site);
			site += 1;
		}
	}
	fprintf(output, "\n\t};\n");


	length = GrammarG.count();
	fprintf(output, "\tenum nonterminal\n\t{\n");
	for (i = 1; i < length; i++)
	{
		if(GrammarG[i - 1]->implicit)
			fprintf(output, "\t\t//_%s_ = %d,\n", GrammarG[i - 1]->name, (int)(i - 1));
		else 
			fprintf(output, "\t\t_%s_ = %d,\n", GrammarG[i - 1]->name, (int)(i - 1));
	}
		
	if (length != 0)
	{
		if (GrammarG[length - 1]->implicit)
		fprintf(output, "\t\t//_%s_ = %d\n", GrammarG[length - 1]->name, (int)(length - 1));
		else 
			fprintf(output, "\t\t_%s_ = %d\n", GrammarG[length - 1]->name, (int)(length - 1));
	}
	fprintf(output, "\t};\n");
}
void InputPanel::printImplicit(FILE* output, FILE* infor, const char* nameG)const
{
	size_t i;
	fprintf(output, "//==============================\n");//
	fprintf(output, "const int %s::Implicit[%zu] = { \\ \n", nameG, rules.count());
	for (i = 1; i < rules.count(); i++)
	{
		if(rules[i - 1]->implicit) fprintf(output, "1, \\\n");
		else fprintf(output, "0, \\\n");
	}
	if (rules[rules.count() - 1]->implicit) fprintf(output, "1};\n");
	else fprintf(output, "0};\n");
}
int InputPanel::printL(FILE* fp, const char* nameL)const
{
	NFA* nfa = NULL;
	sheetDFA* DFAsheet = NULL;
	DFA* DFAgraph = NULL;
	size_t i, No, groupTemp;
	vector<const char*> Name__, Attribute__;
	int error;
	if (errorCode != NoError) return -1;
	error = 0;
	nfa = new NFA(*this);
	DFAsheet = new sheetDFA(*nfa);
	if (DFAsheet->ifDeadAccept()) error = -2;
	DFAsheet->shrink();
	DFAgraph = new DFA(DFAsheet);

	fprintf(fp, "struct %s\n{\n", nameL);
	//nfa = new NFA(*this);
	fprintf(fp, "\tenum regular\n\t{\n");
	//DFAsheet = new sheetDFA(*nfa);
	//fprintf(fp, "\t\tnull = %zu,", 0);
	fprintf(fp, "\t\t_%s_ = %zu", regular[0]->name, (size_t)1);
	//DFAsheet->shrink();
	for (i = 1; i < regular.count(); i++)
	{
		fprintf(fp, ",\n\t\t_%s_ = %ld", regular[i]->name, regular[i]->accept);
	}

	fprintf(fp, "\n\t};\n");
	//DFAgraph = new DFA(DFAsheet);
	fprintf(fp, "\tenum group\n\t{\n");
	if(RegG.count() >= 2)
		fprintf(fp, "\t\t_%s___ = %zu", RegG[1]->name, (size_t)1);
	for (i = 2; i < RegG.count(); i++)
	{
		fprintf(fp, ",\n\t\t_%s___ = %zu", RegG[i]->name, i);
	}
	fprintf(fp, "\n\t};\n");
	fprintf(fp, "\tstatic int next(int state, const char c);\n");
	fprintf(fp, "\tstatic int action(int state);\n");
	fprintf(fp, "\tstatic int GroupGet(int state);\n");
	fprintf(fp, "};\n");

	
	
	
	
	Name__.recount(regular.count());
	Attribute__.recount(regular.count());
	for (i = 0; i < regular.count(); i++)
	{
		groupTemp = regular[i]->group;
		Name__[i] = regular[i]->name; 
		Attribute__[i] = RegG[groupTemp]->name;
	}

	DFAgraph->CppPrint(fp, nameL);
	DFAgraph->CppPrintAccept(fp, nameL, Attribute__.ptr(), Name__.ptr());
	if (nameL != NULL) fprintf(fp, "int %s::GroupGet", nameL);
	else fprintf(fp, "int null::GroupGet");
	fprintf(fp, "(int accept)\n{\n");
	fprintf(fp, "\tswitch (accept)\n\t{\n");
	for (No = 0; No < regular.count(); No++)
	{
		groupTemp = regular[No]->group;
		fprintf(fp, "\tcase %zu:\n", No + 1);
		fprintf(fp, "\t\treturn %zu;", groupTemp);
		fprintf(fp, "//");
		fprintf(fp, "%s: ", RegG[groupTemp]->name);
		fprintf(fp, "%s", regular[No]->name);
		fprintf(fp, "\n");
	}
	fprintf(fp, "\t}\n");
	fprintf(fp, "\treturn 0;\n}\n");

	delete nfa;
	delete DFAsheet;
	delete DFAgraph;
	return error;
}
int InputPanel::printG(FILE* output, FILE* infor, const char* nameG)const
{
	grammerS gs;
	LR0* lr0 = NULL;
	LR1* lr1 = NULL;
	Gsheet Gsheet0, Gsheet1;
	Gsheet::ErrorInfor EI;
	Gsheet::ErrorType ET;
	//printf("???\n");
	if (errorCode != NoError || !GrammarEnclosed) return -1;
	gs.build(*this);
	//printf("???\n");


	lr0 = new LR0(&gs);
	Gsheet0.build(lr0, &gs);
	ET = Gsheet0.ErrorTypeGet();
	lr0->Demo(infor, &gs);
	Gsheet0.Demo(infor);
	if (ET == Gsheet::NoError)
	{
		Gsheet0.CppStructPrint01(nameG, output, &gs);
		printGName(output, infor, nameG);
		Gsheet0.CppStructPrint02(nameG, output, &gs);
		printImplicit(output, infor, nameG);
		delete lr0;
		return 0;
	}

	lr1 = new LR1(&gs);
	Gsheet1.build(lr1, &gs);
	ET = Gsheet1.ErrorTypeGet();
	lr1->Demo(infor, &gs);
	Gsheet1.Demo(infor);
	if (ET == Gsheet::NoError)
	{
		Gsheet1.CppStructPrint01(nameG, output, &gs);
		printGName(output, infor, nameG);
		Gsheet1.CppStructPrint02(nameG, output, &gs);
		printImplicit(output, infor, nameG);
		delete lr1;
		return 1;
	}
	delete lr0;
	delete lr1;
	return 2;
}



struct Reg
{
	enum regular
	{
		_identifier_ = 1,
		_integer_ = 2,
		_CommonChar_ = 3,
		_idChar_ = 4,
		_lexical_ = 5,
		_grammar_ = 6,
		_void_ = 7,
		_all_ = 8,
		_spaces_ = 9,
		_enters_ = 10,
		_tab_ = 11,
		_semicolon_ = 12,
		_colon_ = 13,
		_dot_ = 14,
		_braceL_ = 15,
		_braceR_ = 16,
		_left_ = 17,
		_right_ = 18,
		_squareL_ = 19,
		_squareR_ = 20,
		_angleL_ = 21,
		_angleR_ = 22,
		_anntationS_ = 23,
		_anntationM_ = 24,
		_range_ = 25,
		_star_ = 26,
		_plus_ = 27,
		_question_ = 28,
		_or_ = 29
	};
	enum group
	{
		_identifier___ = 1,
		_const___ = 2,
		_reserved___ = 3,
		_format___ = 4,
		_division___ = 5,
		_braket___ = 6,
		_anntation___ = 7,
		_RegSymbol___ = 8
	};
	static int next(int state, const char c);
	static int action(int state);
	static int GroupGet(int state);
};
struct Panel
{
	enum type
	{
		accept = 0,
		error = 1,
		push = 2,
		reduce = 3
	};
	enum nonterminal
	{
		_all_ = 0,
		_TEXT_ = 1,
		_LEXICAL_ = 2,
		_RegGROUP_ = 3,
		_REGDEFS_ = 4,
		_RegDEF_ = 5,
		_RegGROUPNAME_ = 6,
		_RegNAME_ = 7,
		_RegNAMEHead_ = 8,
		_REGBODY_ = 9,
		_REGEXPRESSor_ = 10,
		_REGEXPRESS_ = 11,
		_RegCOMPLEX_ = 12,
		_RegNODE_ = 13,
		_RegLEAF_ = 14,
		_RegCHAR_ = 15,
		_GRAMMAR_ = 16,
		_GrammerDEF_ = 17,
		_GnameFORMULAS_ = 18,
		_GNameALL_ = 19,
		_GFORMULA_ = 20,
		_GFORMULAUnit_ = 21,
		_END_ = 22,
		_BEGIN_ = 23
	};
	enum rules
	{
		_all_all_ = 0,
		_TEXT_OnlyL_ = 1,
		_TEXT_LG_ = 2,
		_LEXICAL_single_ = 3,
		_LEXICAL_multi_ = 4,
		_RegGROUP_single_ = 5,
		_RegGROUP_multi_ = 6,
		_REGDEFS_single_ = 7,
		_REGDEFS_multi_ = 8,
		_RegDEF_default_ = 9,
		_RegDEF_full_ = 10,
		_RegGROUPNAME_blocks_ = 11,
		_RegGROUPNAME_bare_ = 12,
		_RegGROUPNAME_priority_ = 13,
		_RegNAME_bare_ = 14,
		_RegNAME_priority_ = 15,
		_RegNAMEHead_avoid_ = 16,
		_RegNAMEHead_alexical_ = 17,
		_RegNAMEHead_agrammar_ = 18,
		_RegNAMEHead_aall_ = 19,
		_RegNAMEHead_aid_ = 20,
		_REGBODY_REGBODY_ = 21,
		_REGEXPRESSor_single_ = 22,
		_REGEXPRESSor_ororor_ = 23,
		_REGEXPRESS_single_ = 24,
		_REGEXPRESS_multi_ = 25,
		_RegCOMPLEX_single_ = 26,
		_RegCOMPLEX_multi1_ = 27,
		_RegCOMPLEX_multi2_ = 28,
		_RegCOMPLEX_multi3_ = 29,
		_RegNODE_single_ = 30,
		_RegNODE_braket_ = 31,
		_RegNODE_replace_ = 32,
		_RegLEAF_single_ = 33,
		_RegLEAF_single2_ = 34,
		_RegLEAF_range_ = 35,
		_RegCHAR_commom_ = 36,
		_RegCHAR_id_ = 37,
		_GRAMMAR_single_ = 38,
		_GRAMMAR_multi_ = 39,
		_GrammerDEF_single_ = 40,
		_GrammerDEF_multi_ = 41,
		_GnameFORMULAS_single_ = 42,
		_GnameFORMULAS_multi_ = 43,
		_GNameALL_explicit_ = 44,
		_GNameALL_implicit_ = 45,
		_GFORMULA_unit_ = 46,
		_GFORMULA_more_ = 47,
		_GFORMULA_plus_ = 48,
		_GFORMULA_star_ = 49,
		_GFORMULAUnit_avoid_ = 50,
		_GFORMULAUnit_alexical_ = 51,
		_GFORMULAUnit_agrammar_ = 52,
		_GFORMULAUnit_aall_ = 53,
		_GFORMULAUnit_aid_ = 54,
		_GFORMULAUnit_aImplicit_ = 55,
		_GFORMULAUnit_group_ = 56,
		_END_full_ = 57,
		_END_half_ = 58,
		_BEGIN_BEGIN_ = 59
	};
	static const size_t StateCount;
	static const size_t NonTerminalCount;
	static const size_t TerminalCount;
	static const size_t RulesCount;
	static const int GOTO[107][24];
	static const int ACTION[107][30];
	static const int RulesToSymbol[60];
	static const int RulesLength[60];
	static const char* const RulesName[60];
	static const int Implicit[60];
};
struct PreTreat
{
	enum regular
	{
		_identifier_ = 1,
		_integer_ = 2,
		_CommonChar_ = 3,
		_idChar_ = 4,
		_string_ = 5,
		_include_ = 6,
		_spaces_ = 7,
		_enters_ = 8,
		_tab_ = 9,
		_semicolon_ = 10,
		_colon_ = 11,
		_dot_ = 12,
		_braceL_ = 13,
		_braceR_ = 14,
		_left_ = 15,
		_right_ = 16,
		_squareL_ = 17,
		_squareR_ = 18,
		_angleL_ = 19,
		_angleR_ = 20,
		_anntationS_ = 21,
		_anntationM_ = 22,
		_range_ = 23,
		_star_ = 24,
		_plus_ = 25,
		_question_ = 26,
		_or_ = 27
	};
	enum group
	{
		_identifier___ = 1,
		_const___ = 2,
		_string___ = 3,
		_include___ = 4,
		_format___ = 5,
		_division___ = 6,
		_braket___ = 7,
		_anntation___ = 8,
		_RegSymbol___ = 9
	};
	static int next(int state, const char c);
	static int action(int state);
	static int GroupGet(int state);
};
struct Preparser
{
	enum type
	{
		accept = 0,
		error = 1,
		push = 2,
		reduce = 3
	};
	enum rules
	{
		all_all_ = 0,
		TEXT_meow_ = 1,
		//<DEF*>_first_ = 2,
		//<DEF*>_multi_ = 3,
		DEF_anntation_ = 4,
		DEF_anything_ = 5,
		DEF_include_ = 6,
		//[anntation]_anntationS_ = 7,
		//[anntation]_anntationM_ = 8,
		INCLUDE_include_ = 9,
		INCLUDE_include2_ = 10,
		ANY_identifier_ = 11,
		ANY_const_ = 12,
		ANY_RegSymbol_ = 13,
		ANY_braket_ = 14,
		ANY_division_ = 15,
		ANY_format_ = 16,
		//[identifier]_identifier_ = 17,
		//[const]_integer_ = 18,
		//[const]_CommonChar_ = 19,
		//[const]_idChar_ = 20,
		//[RegSymbol]_range_ = 21,
		//[RegSymbol]_star_ = 22,
		//[RegSymbol]_plus_ = 23,
		//[RegSymbol]_question_ = 24,
		//[RegSymbol]_or_ = 25,
		//[braket]_braceL_ = 26,
		//[braket]_braceR_ = 27,
		//[braket]_left_ = 28,
		//[braket]_right_ = 29,
		//[braket]_squareL_ = 30,
		//[braket]_squareR_ = 31,
		//[braket]_angleL_ = 32,
		//[braket]_angleR_ = 33,
		//[division]_semicolon_ = 34,
		//[division]_colon_ = 35,
		//[division]_dot_ = 36,
		//[format]_spaces_ = 37,
		//[format]_enters_ = 38,
		//[format]_tab_ = 39
	};
	enum nonterminal
	{
		_all_ = 0,
		_TEXT_ = 1,
		//_<DEF*>_ = 2,
		_DEF_ = 3,
		//_[anntation]_ = 4,
		_INCLUDE_ = 5,
		_ANY_ = 6,
		//_[identifier]_ = 7,
		//_[const]_ = 8,
		//_[RegSymbol]_ = 9,
		//_[braket]_ = 10,
		//_[division]_ = 11,
		//_[format]_ = 12
	};
	static const size_t StateCount;
	static const size_t NonTerminalCount;
	static const size_t TerminalCount;
	static const size_t RulesCount;
	static const int GOTO[42][13];
	static const int ACTION[42][28];
	static const int RulesToSymbol[40];
	static const int RulesLength[40];
	static const char* const RulesName[40];
	static const int Implicit[40];
};


int InputPanel::build_v02(const char* file)
{
	int error;
	
	clear();
	initial();
	error = pretreatment(file, MorphemePre);
	if (error != 0) return error;

	error = LexicalSource.Build<Reg>(MorphemePre);
	if (error != 0) return error;
	NeglectNullToken(LexicalSource);
	//eme.Demo(stdout);
	error = buildGanalysis(LexicalSource);
	if (error != 0) return error;
	errorCode = NoError;
	return error;
}


int InputPanel::pretreatment(const char* input, Morpheme& output)
{
	typedef hyperlex::tree<hyperlex::GrammarTree::TreeInfor> GTNode;
	typedef hyperlex::tree<hyperlex::GrammarTree::TreeInfor>::PostIterator GTiterator;
	FILE* fp = fopen(input, "r");
	if (fp == NULL)
	{
		errorCode = PretreatOpenfail;
		return 445624;
	}
	int error = output.Build<PreTreat>(fp);
	output.append(input);
	if (error != 0)
	{
		errorInfor1 = output.FileCount();
		errorCode = PretreatLEXICAL;
		return error;
	}
	bool include;
	do
	{
		include = false;
		size_t begin = 0;
		size_t count = 0;
		size_t file = 0;
		const char* name = NULL;
		hyperlex::GrammarTree Tree;
		error = Tree.build<Preparser>(output);
		if (error != 0)
		{
			errorInfor1 = output.FileCount() - 1;
			errorInfor2 = Tree.error_record01;
			//std::cout << "Tree.error_record01: " << Tree.error_record01 << std::endl;
			//std::cout << "Tree.error_record02: " << Tree.error_record02 << std::endl;
			errorCode = PretreatGRAMMAR;
			return error;
		}
		GTiterator iterator;
		iterator.initial(Tree.GT);
		while (iterator.still())
		{
			GTNode* GT = iterator.target();
			if (iterator.state() == 0)
			{
				size_t infor = GT->root().site;
				
				if (GT->root().rules)
				{
					if (infor == (int)Preparser::INCLUDE_include2_)
					{
						size_t site = GT->child(1)->root().site;
						include = true;
						begin = GT->child(0)->root().site;
						count = 2;
						name = output.GetString(site);
						file = output[site].file;
						break;
					}
					else if (infor == (int)Preparser::INCLUDE_include_)
					{
						size_t site = GT->child(2)->root().site;
						include = true;
						begin = GT->child(0)->root().site;
						count = 3;
						//std::cout << "2begin: " << begin << std::endl;
						name = output.GetString(site);
						//std::cout << "2name: " << name << std::endl;
						file = output[site].file;
						break;
					}
				}
			}
			iterator.next();
		}
		if (include)
		{
			hyperlex::Morpheme eme;
			hyperlex::FilePath left, here;
			here.build(name);
			//std::cout << "file: " << file << std::endl;
			//std::cout << "output.GetFile(file): " << output.GetFile(file) << std::endl;
			left.build(output.GetFile(file));
			//here.demo();
			//left.demo();
			//std::cout << "begin: " << begin << std::endl;
			//std::cout << "name: " << name << std::endl;
			//std::cout << "count: " << count << std::endl;
			left.RearCutAppend(here);

			//left.demo();
			//std::cout << "=============" << std::endl;
			char* newFile = left.print();
			FILE* fp2 = fopen(newFile, "r");
			output.append(newFile);
			free(newFile);
			if (fp2 == NULL)
			{
				errorCode = PretreatOpenfail;
				return 445625;
			}
			int error = eme.Build<PreTreat>(fp2);
			fclose(fp2);
			if (error != 0)
			{
				errorInfor1 = output.FileCount() - 1;
				errorInfor2 = eme[eme.GetCount() - 2].line;
				errorCode = PretreatLEXICAL;
				return error;
			}
			eme.SetFile(output.FileCount() - 1);

			for (size_t i = 0; i + 1 < output.FileCount(); i++)
			{
				hyperlex::FilePath right;
				right.build(output.GetFile(i));
				if (left == right)
				{
					errorInfor1 = output.FileCount() - 1;
					errorInfor2 = i;
					errorCode = PretreatRepeat;
					return 12345678;
				}
			}
			//eme.Demo(stdout);
			//output.Demo(stdout);
			output.insert(begin, count, eme);
			//std::cout << "=============" << std::endl;
			//output.Demo(stdout);
		}
	} while (include);

	return error;
}

int InputPanel::build(FILE* fp)
{
	//Morpheme eme;
	int error;
	BufferChar input;
	input << fp;
	return build(input.ptr());
}
int InputPanel::build(const char* input)
{
	int error;
	clear();
	initial();
	error = LexicalSource.Build<Reg>(input);
	if (error != 0)
	{
		errorCode = ErrorinputLEXICAL;
		return error;
	}
	NeglectNullToken(LexicalSource);
	//eme.Demo(stdout);
	error = buildGanalysis(LexicalSource);
	if (error != 0) return error;
	errorCode = NoError;
	return 0;
}
int InputPanel::buildGanalysis(const Morpheme& eme)
{
	int error;
	GrammarTree Tree;
	error = Tree.build<Panel>(eme);
	if (error != 0)
	{
		errorCode = ErrorinputGrammar;
		errorInfor1 = Tree.error_record01;
		errorInfor2 = Tree.error_record02;
		return error;
	}
	//printf("Here?!\n");
	//Tree.Demo(stdout, eme, Panel::RulesName);
	//printf("Here?!\n");
	error = buildAll(eme, Tree);
	//printf("Here?!:%d\n", error);
	if (error != 0) return error;
	buildLpost();
	//printf("Here?!:%d\n", error);
	return error;
}
void InputPanel::NeglectNullToken(Morpheme& eme) const
{
	size_t i;// , site;
	Reg::regular T;
	Reg::group G;
	//site = 0;
	for (i = 0; i < eme.GetCount(); i++)
	{
		T = (Reg::regular)(eme[i].accept);
		G = (Reg::group)(eme[i].category);
		if (G == Reg::_format___ || G == Reg::_anntation___)
			eme.valid(i) = false;
		//if (G != Reg::_format___ && G != Reg::_annotation___)
		//{
		//	if (i != site) eme.UnitMove(i, site);
		//	site += 1;
		//}
	}
	//eme.CountReset(site);
	//vector<bool> line;
	//line.recount(256);
	//line.value(false);
	//line[(size_t)(Reg::_enters_)] = true;
	//line[(size_t)(Reg::_anntationS_)] = true;
	//eme.SetLine(line.ptr());
	return;
}
int InputPanel::buildAll(const Morpheme& eme, GrammarTree& Tree)
{
	//RegularExp* TreeNow;
	//RegularExp* Ltemp, * Rtemp;
	size_t site_, temp;
	int error;
	hyperlex::tree<GrammarTree::TreeInfor>* GT;
	hyperlex::tree<GrammarTree::TreeInfor>::PostIterator iterator;
	iterator.initial(Tree.GT);
	error = 0;
	while (iterator.still())
	{
		GT = iterator.target();
		if (iterator.state() == 0)
		{
			site_ = GT->root().site;
			if ((RegularExp*)GT->root().rules)
			{
				if (site_ == Panel::_TEXT_OnlyL_)
				{
					error = buildL(eme, GT->child(2));
					break;
				}
				else if(site_ == Panel::_TEXT_LG_)
				{
					error = buildL(eme, GT->child(2));
					if (error != 0) return error;
					if (GT->child(6)->root().rules) return -1;
					temp =  GT->child(6)->root().site;
					RootName = eme.Copy(temp);
					//lexical BEGIN LEXICAL END grammar colon identifier BEGIN GRAMMAR END
					error = buildG(eme, GT->child(8));
					break;
				}
			}
		}
		iterator.next();
	}
	return error;
}



int InputPanel::buildL(const Morpheme& eme, GLTree* Tree)
{
	int error;
	size_t site_, GroupNow;
	size_t RegNow, temp;
	RegContent* Regu;
	GLTree* GT;
	GTIter iterator;
	iterator.initial(Tree);
	error = 0;
	RegNow = 0;
	GroupNow = 0;
	while (iterator.still())
	{
		GT = iterator.target();
		if (iterator.state() == 0)
		{
			site_ = GT->root().site;

			if ((RegularExp*)GT->root().rules){
				//printf("here: %zu\n", site_);
				switch ((Panel::rules)site_)
				{
				//No[3], case RegGROUP: prefix: 5, degeneracy: 4
				case Panel::_RegGROUP_single_: //3: RegGROUP, No[0] production rules: RegGROUP->RegGROUPNAME colon RegDEF
				case Panel::_RegGROUP_multi_: //3: RegGROUP, No[1] production rules: RegGROUP->RegGROUPNAME BEGIN REGDEFS END
					//printf("ghghhere: %zu\n", GT->ChildCount());
					//printf("ghghhere: %zu\n", (size_t)GT->child(0));
					error = RegGroupName(GroupNow, eme, GT->child(0));
					if (error != 0) return error;
					break;
				case Panel::_RegDEF_default_: //5: RegDEF, No[0] production rules: RegDEF->RegNAME semicolon
					error = RegName(RegNow, GroupNow, eme, GT->child(0));
					if (error != 0) return error;
					Regu = RegG[GroupNow]->regs[RegNow];
					Regu->reg = new RegularExp;
					Regu->reg->set(Regu->name);
					//RegG[GroupNow]->regs[RegNow]->reg->Demo(stdout);
					break;
				case Panel::_RegDEF_full_: //5: RegDEF, No[1] production rules: RegDEF->RegNAME colon REGBODY semicolon
					//printf("here\n");
					error = RegName(RegNow, GroupNow, eme, GT->child(0));
					//printf("here: %d\n", error);
					if (error != 0) return error;
					error = RegBuild(RegNow, GroupNow, eme, GT->child(2));
					//printf("here: %d\n", error);
					if (error != 0) return error;
					//RegG[GroupNow]->regs[RegNow]->reg->Demo(stdout);
					break;
					
				}
			}
		}
		iterator.next();
	}
	return error;
}
void InputPanel::addVoidGroup(void)
{
	RegGroup* now;
	now = new RegGroup;
	now->SetName("void");
	RegG.append(now);
}
int InputPanel::RegGroupName(size_t& siteReturn, const Morpheme& eme, GLTree* Tree)
{
	GLTree* GT; 
	RegGroup* now;
	size_t i;
	const char* inputName;
	BufferChar tempString;
	//No[6], case RegNAME: prefix: 13, degeneracy: 2
	//case 13: RegNAME, No[0] production rules: RegNAME->identifier
	//case 14: RegNAME, No[1] production rules: RegNAME->identifier left integar right
	//printf("RegGroupName\n");
	GT = Tree->child(0);
	inputName = eme.GetWord(GT->root().site);
	for (i = 1; i < RegG.count(); i++)
	{
		if (compare(RegG[i]->name, inputName))
		{
			errorCode = ConflictRegGroupName;
			errorInfor1 = i;
			return 5;
		}
	}
	//printf("compare\n");
	if (compare(RegG[0]->name, inputName))
	{
		siteReturn = 0;
		return 0;
	}
	//printf("compare\n");
	now = new RegGroup;
	now->SetName(inputName);
	RegG.append(now);
	siteReturn = RegG.count() - 1;
	if (Tree->root().site == 13)
	{
		GT = Tree->child(2);
		if (GT->root().rules) return -77;
		tempString = eme.GetWord(GT->root().site);
		now->priority = tempString.DequeueInt();
	}
	return 0;
}
int InputPanel::RegName(size_t& siteReturn, size_t group, const Morpheme& eme, GLTree* Tree)
{
	RegGroup* now;
	RegContent* RegNow;
	GLTree* GT;
	size_t i, j; 
	const char* inputName;
	BufferChar tempString;
	GT = Tree->child(0)->child(0);
	inputName = eme.GetWord(GT->root().site);
	now = RegG[group];
	//No[5], case RegDEF: prefix: 9, degeneracy: 2
		//5: RegDEF, No[0] production rules: RegDEF->RegNAME semicolon
		//5: RegDEF, No[1] production rules: RegDEF->RegNAME colon REGBODY semicolon
	//No[7], case RegNAME: prefix: 14, degeneracy: 2
		//7: RegNAME, No[0] production rules: RegNAME->RegNAMEHead
		//case 15 7: RegNAME, No[1] production rules: RegNAME->RegNAMEHead left integar right
	//No[8], case RegNAMEHead: prefix: 16, degeneracy: 5
		//8: RegNAMEHead, No[0] production rules: RegNAMEHead->void
		//8: RegNAMEHead, No[1] production rules: RegNAMEHead->lexical
		//8: RegNAMEHead, No[2] production rules: RegNAMEHead->grammar
		//8: RegNAMEHead, No[3] production rules: RegNAMEHead->all
		// case 20, 8: RegNAMEHead, No[4] production rules: RegNAMEHead->identifier
	if (RegSearch(i, j, inputName))
	{
		errorCode = repeatRegName;
		errorInfor1 = i;
		errorInfor2 = j;
		return 55;
	}
	RegNow = new RegContent;
	RegNow->SetName(inputName);
	now->regs.append(RegNow);
	siteReturn = now->regs.count() - 1;
	if (Tree->root().site == 15)
	{
		GT = Tree->child(2);
		if (GT->root().rules) return -7788;
		tempString = eme.GetWord(GT->root().site);
		RegNow->priority = tempString.DequeueInt();
	}
	else
	{
		RegNow->priority = now->priority;
	}
	return 0;
}
bool InputPanel::RegSearch(size_t& group, size_t& site, const char* name)
{
	size_t i, j;
	RegGroup* now;
	for (i = 0; i < RegG.count(); i++)
	{
		now = RegG[i];
		for (j = 0; j < now->regs.count(); j++)
		{
			if (compare(now->regs[j]->name, name))
			{
				group = i;
				site = j;
				return true;
			}
		}
	}
	return false;
}
int InputPanel::RegBuild(size_t RegNow, size_t group, const Morpheme& eme, GLTree* Tree)
{
	int error;
	//RegContent* RegNow;
	GLTree* GT;
	size_t i, j;
	GTIter iterator;
	RegularExp* TreeNow = NULL;
	RegularExp* Ltemp, * Rtemp;
	size_t site_;
	const char* idName;
	error = 0;
	idName = NULL;
	iterator.initial(Tree);
	while (iterator.still())
	{
		GT = iterator.target();
		if (iterator.state() != 0)
		{
			site_ = GT->root().site;
			if ((RegularExp*)GT->root().rules)
			{
				//printf("rules: %zu   ", site_);
				switch (site_)
				{
				//No[9], case REGBODY: prefix: 21, degeneracy: 1
				case Panel::_REGBODY_REGBODY_: //9: REGBODY, No[0] production rules: REGBODY->REGEXPRESSor
				//No[10], case REGEXPRESSor: prefix: 22, degeneracy: 2
				case Panel::_REGEXPRESSor_single_: //10: REGEXPRESSor, No[0] production rules: REGEXPRESSor->REGEXPRESS
					TreeNow = (RegularExp*)(GT->child(0)->root().infor);
					GT->child(0)->root().infor = NULL;
					break;
				case Panel::_REGEXPRESSor_ororor_: //case 23: 10: REGEXPRESSor, No[1] production rules: REGEXPRESSor->REGEXPRESSor or REGEXPRESS
					TreeNow = new RegularExp;
					Ltemp = (RegularExp*)(GT->child(0)->root().infor);
					Rtemp = (RegularExp*)(GT->child(2)->root().infor);
					TreeNow->set(Ltemp, Rtemp, RegularExp::Alternation);
					break;
					//No[11], case REGEXPRESS: prefix: 24, degeneracy: 2
				case Panel::_REGEXPRESS_single_: //case 24: 11: REGEXPRESS, No[0] production rules: REGEXPRESS->RegCOMPLEX
					TreeNow = (RegularExp*)(GT->child(0)->root().infor);
					GT->child(0)->root().infor = NULL;
					break;
				case Panel::_REGEXPRESS_multi_: //case 25: 11: REGEXPRESS, No[1] production rules: REGEXPRESS->REGEXPRESS RegCOMPLEX
					TreeNow = new RegularExp;
					Ltemp = (RegularExp*)(GT->child(0)->root().infor);
					Rtemp = (RegularExp*)(GT->child(1)->root().infor);
					TreeNow->set(Ltemp, Rtemp, RegularExp::Concatenation);
					break;
					//No[12], case RegCOMPLEX: prefix: 26, degeneracy: 4
				case Panel::_RegCOMPLEX_single_: //case 26:12: RegCOMPLEX, No[0] production rules: RegCOMPLEX->RegNODE
					TreeNow = (RegularExp*)(GT->child(0)->root().infor);
					GT->child(0)->root().infor = NULL;
					break;
				case Panel::_RegCOMPLEX_multi1_: //case 27:12: RegCOMPLEX, No[1] production rules: RegCOMPLEX->RegCOMPLEX plus
					TreeNow = new RegularExp;
					Ltemp = (RegularExp*)(GT->child(0)->root().infor);
					TreeNow->set(Ltemp, RegularExp::OneOrMore);
					break;
				case Panel::_RegCOMPLEX_multi2_: //case 28:12: RegCOMPLEX, No[2] production rules: RegCOMPLEX->RegCOMPLEX star
					TreeNow = new RegularExp;
					Ltemp = (RegularExp*)(GT->child(0)->root().infor);
					TreeNow->set(Ltemp, RegularExp::ZeroOrMore);
					break;
				case Panel::_RegCOMPLEX_multi3_: //case 29:12: RegCOMPLEX, No[3] production rules: RegCOMPLEX->RegCOMPLEX question
					TreeNow = new RegularExp;
					Ltemp = (RegularExp*)(GT->child(0)->root().infor);
					TreeNow->set(Ltemp, RegularExp::ZeroOrOne);
					break;
				//No[13], case RegNODE: prefix: 30, degeneracy: 3
				case Panel::_RegNODE_single_: //case 30:13: RegNODE, No[0] production rules: RegNODE->RegLEAF
					TreeNow = (RegularExp*)(GT->child(0)->root().infor);
					GT->child(0)->root().infor = NULL;
					break;
				case Panel::_RegNODE_braket_: //case 31:13: RegNODE, No[1] production rules: RegNODE->left REGEXPRESSor right
					TreeNow = (RegularExp*)(GT->child(1)->root().infor);
					GT->child(1)->root().infor = NULL;
					break;
				case Panel::_RegNODE_replace_: //case 32:13: RegNODE, No[2] production rules: RegNODE->angleL identifier angleR
					idName = eme.GetWord(GT->child(1)->root().site);
					TreeNow = new RegularExp;
					if (RegSearch(i, j, idName))
					{
						Ltemp = RegG[i]->regs[j]->reg;
						TreeNow->copy(*Ltemp);
					}
					else
					{
						errorCode = missingIdinRegdef;
						errorInfor3 = idName;
						error = -13;
					}
					break;
				//No[14], case RegLEAF: prefix: 33, degeneracy: 3
				case Panel::_RegLEAF_single_: //case 33:14: RegLEAF, No[0] production rules: RegLEAF->RegCHAR
					TreeNow = (RegularExp*)(GT->child(0)->root().infor);
					GT->child(0)->root().infor = NULL;
					break;
				case Panel::_RegLEAF_single2_: //case 34:14: RegLEAF, No[1] production rules: RegLEAF->squareL RegCHAR squareR
					TreeNow = (RegularExp*)(GT->child(1)->root().infor);
					GT->child(1)->root().infor = NULL;
					break;
				case Panel::_RegLEAF_range_: //case 35: 14: RegLEAF, No[2] production rules: RegLEAF->squareL RegCHAR range RegCHAR squareR
					TreeNow = new RegularExp;
					Ltemp = (RegularExp*)(GT->child(1)->root().infor);
					Rtemp = (RegularExp*)(GT->child(3)->root().infor);
					TreeNow->set(Ltemp, Rtemp);
					delete Ltemp;
					delete Rtemp;
					Ltemp = NULL;
					Rtemp = NULL;
					break;
				//No[15], case RegCHAR: prefix: 36, degeneracy: 2
				case Panel::_RegCHAR_commom_: //case 36:15: RegCHAR, No[0] production rules: RegCHAR->CommonChar
				case Panel::_RegCHAR_id_: //case 37:15: RegCHAR, No[1] production rules: RegCHAR->idChar
					TreeNow = (RegularExp*)(GT->child(0)->root().infor);
					GT->child(0)->root().infor = NULL;
					break;

				}
				//TreeNow->Demo(stdout);
				//TreeNow->Demo(stdout);
				//printf("\n");
			}
			else
			{
				TreeNow = new RegularExp;
				TreeNow->set((int)(eme.GetChar(site_)));
			}
			GT->root().infor = TreeNow;
		}
		iterator.next();
	}
	//printf("rules 1: %zu\n", site_);
	TreeNow = (RegularExp*)(Tree->root().infor);
	//printf("rules 2: %zu\n", site_);
	Tree->root().infor = NULL;
	//printf("rules 3: %zu\n", site_);
	RegG[group]->regs[RegNow]->reg = TreeNow;
	//printf("rules 4: %zu\n", site_);
	return error;
}

int InputPanel::buildG(const Morpheme& eme, GLTree* Tree)
{
	int error;
	error = 0;
	
	//printf("here!\n");
	addTerminal();
	addAllGroup();
	//printf("here!\n");
	error = buildRules(eme, Tree);
	//printf("here!\n");
	if (error != 0) return error;
	//printf("here!\n");
	addAllGroup02();
	//printf("here!\n");
	error = NonTerminalSort();
	//printf("here!\n");
	if (error != 0) return error;
	ImplicitAdd();
	GrammarEnclosed = true;
	return error;
}
int InputPanel::buildRules(const Morpheme& eme, GLTree* Tree)
{
	GTIter iterator;
	int error;
	GLTree* GT;
	bool tempBool;
	const char* inputName;
	size_t site_;
	size_t groupNow;
	iterator.initial(Tree);
	error = 0;
	groupNow = 0;
	while (iterator.still())
	{
		GT = iterator.target();
		site_ = GT->root().site;
		tempBool = (RegularExp*)GT->root().rules;
		if (iterator.state() == 0 && tempBool)
		{
			switch (site_) {
				//No[17], case GrammerDEF: prefix: 40, degeneracy: 2
			case Panel::_GrammerDEF_single_: //case 40:17: GrammerDEF, No[0] production rules: GrammerDEF->GNameALL colon GFORMULA semicolon
				error = GrammarGroup(groupNow, eme, GT->child(0));
				if (error != 0) return error;
				inputName = GrammarG[groupNow]->name;
				error = RulesAppend(groupNow, GT->child(0), eme, GT->child(2));
				if (error != 0) return error;
				break;
			case Panel::_GrammerDEF_multi_: //case 41:17: GrammerDEF, No[1] production rules: GrammerDEF->GNameALL BEGIN GnameFORMULAS END
				error = GrammarGroup(groupNow, eme, GT->child(0));
				if (error != 0) return error;
				break;
			}
		}
		else if (tempBool)
		{
			switch (site_)
			{
				//No[18], case GnameFORMULAS: prefix: 42, degeneracy: 2
			case Panel::_GnameFORMULAS_single_: //case 42: 18: GnameFORMULAS, No[0] production rules: GnameFORMULAS->GNameALL colon GFORMULA semicolon
				inputName = eme.GetWord(GT->child(0)->root().site);
				error = RulesAppend(groupNow, GT->child(0), eme, GT->child(2));
				if (error != 0) return error;
				break;
			case Panel::_GnameFORMULAS_multi_: //case 43: 18: GnameFORMULAS, No[1] production rules: GnameFORMULAS->GnameFORMULAS GNameALL colon GFORMULA semicolon
				inputName = eme.GetWord(GT->child(1)->root().site);
				error = RulesAppend(groupNow, GT->child(1), eme, GT->child(3));
				if (error != 0) return error;
				break;
			}
		}
		iterator.next();
	}
	return error;
}
void InputPanel::addAllGroup(void)
{
	Group* GG;
	GG = new Group;
	GG->SetName("all");
	GrammarG.append(GG);
	NontTerminal.append(Copy("all"));
}
void InputPanel::addAllGroup02(void)
{
	Rules* RR;
	RR = new Rules;
	RR->SetName("all");
	RR->formula.append(SymbolAdd(RootName));
	GrammarG[0]->rules.append(RR);
}
int InputPanel::GrammarGroup(size_t&GroupSite, const Morpheme& eme, GLTree* Tree)
{
	int error;
	size_t i; 
	const char* inputName;
	GLTree* GT;
	Group* now;
	bool Implicit;
	BufferChar BC;
	GT = Tree;
	error = 0;
	Implicit = GT->root().site != Panel::_GNameALL_explicit_;
	if (Implicit)
	{
		inputName = eme.GetWord(GT->child(1)->root().site);
		BC = "<";
		BC += inputName;
		BC += ">";
		inputName = BC.ptr();
	}
	else
		inputName = eme.GetWord(GT->child(0)->root().site);
	for (i = 0; i < GrammarG.count(); i++)
	{
		//if(Implicit == GrammarG[i]->implicit)
		if (compare(GrammarG[i]->name, inputName))
		{
			errorCode = repeatGGroupName;
			errorInfor4 = true;
			errorInfor1 = i;
			errorInfor1 = GrammarG.count();
			return 557;
		}
	}
	for (i = 0; i < Terminal.count(); i++)
	{
		if (compare(Terminal[i], inputName))
		{
			errorCode = repeatGGroupName;
			errorInfor4 = false;
			errorInfor1 = i;
			errorInfor2 = GrammarG.count();
			return 558;
		}
	}
	now = new Group;
	now->SetName(inputName);
	now->implicit = Implicit;
	GrammarG.append(now);
	GroupSite = GrammarG.count() - 1;
	return error;
}
int InputPanel::RulesAppend(size_t GroupSite, GLTree* Name, const Morpheme& eme, GLTree* Tree)
{
	const char* inputName;
	BufferChar BC;
	bool Implicit;
	size_t i, site_, middle, length;
	Group* now;
	Rules* RR;
	GTIter iterator;
	GLTree* GT;
	long int unitNum, temp;
	now = GrammarG[GroupSite];
	Implicit = Name->root().site != Panel::_GNameALL_explicit_;
	if (Implicit)
	{
		inputName = eme.GetWord(Name->child(1)->root().site);
		BC = "<";
		BC += inputName;
		BC += ">";
		inputName = BC.ptr();
		//inputName = eme.GetWord(Name->root().site);
	}
	else
		inputName = eme.GetWord(Name->child(0)->root().site);
	for (i = 0; i < now->rules.count(); i++)
	{
		if (compare(now->rules[i]->name, inputName))
		{
			errorCode = repeatGName;
			
			errorInfor1 = GroupSite;
			errorInfor2 = i;
			return 4441;
		}
	}
	
	RR = new Rules;
	RR->SetName(inputName);
	RR->implicit = Implicit;
	now->rules.append(RR);
	iterator.initial(Tree);
	while (iterator.still())
	{
		GT = iterator.target();
		if (iterator.state() == 1)
		{
			site_ = GT->root().site;
			if ((RegularExp*)GT->root().rules) {
				switch ((Panel::rules)site_) {
					//No[19], case GFORMULA: prefix: 44, degeneracy: 2
				case Panel::_GFORMULA_unit_: //case 44:19: GFORMULA, No[0] production rules: GFORMULA->GFORMULAUnit
					//symbolName = eme.GetWord(GT->child(0)->child(0)->root().site);
					unitNum = UnitToSymbol(GT->child(0), eme);
					RR->formula.append(unitNum);
					break;
				case Panel::_GFORMULA_more_: //case 45: 19: GFORMULA, No[1] production rules: GFORMULA->GFORMULA GFORMULAUnit
					//symbolName = eme.GetWord(GT->child(1)->child(0)->root().site);
					unitNum = UnitToSymbol(GT->child(1), eme);
					RR->formula.append(unitNum);
					break;
				case Panel::_GFORMULA_plus_://plus: GFORMULAUnit plus;
					temp = UnitToSymbol(GT->child(0), eme);
					unitNum = WildcardToSymbol(temp, eme, true);
					RR->formula.append(unitNum);
					break;
				case Panel::_GFORMULA_star_:
					temp = UnitToSymbol(GT->child(0), eme);
					unitNum = WildcardToSymbol(temp, eme, false);
					RR->formula.append(unitNum);
					break;
				}
				if (errorCode != buildUndone) return -57347;
			}
		}
		iterator.next();
	}
	//length = RR->formula.count();
	//for (i = 0; i < length / 2; i++)
	//{
	//	middle = RR->formula[i];
	//	RR->formula[i] = RR->formula[length - 1 - i];
	//	RR->formula[length - 1 - i] = middle;
	//}
	return 0;
}
long int InputPanel::UnitToSymbol(GLTree* Tree, const Morpheme& eme)
{
	long int UnitNum;
	const char* symbolName;
	
	Panel::rules RR;
	BufferChar BC;
	RR = (Panel::rules)Tree->root().site;
	UnitNum = 0;
	switch (RR)
	{
	case Panel::_GFORMULAUnit_avoid_:	
	case Panel::_GFORMULAUnit_alexical_:	
	case Panel::_GFORMULAUnit_agrammar_:	
	case Panel::_GFORMULAUnit_aall_:	
	case Panel::_GFORMULAUnit_aid_:
		symbolName = eme.GetWord(Tree->child(0)->root().site);
		UnitNum = SymbolAdd(symbolName);
		break;
	case Panel::_GFORMULAUnit_aImplicit_:
		symbolName = eme.GetWord(Tree->child(1)->root().site);
		BC = "<";
		BC += symbolName;
		BC += ">";
		symbolName = BC.ptr();
		UnitNum = SymbolAdd(symbolName);
		break;
	case Panel::_GFORMULAUnit_group_:
		symbolName = eme.GetWord(Tree->child(1)->root().site);
		errorInfor1 = Tree->child(1)->root().site;
		UnitNum = UnitRegGroupAdd(symbolName);
		if (errorCode == buildUndone) errorInfor1 = 0;
		break;
	}
	return UnitNum;
}
long int InputPanel::WildcardToSymbol(long int symbol, const Morpheme& eme, bool plus)
{
	long int UnitNum;
	const char* Ctemp;
	const char* Symbol;
	BufferChar BC;
	size_t i;
	if (symbol >= 0) Symbol = NontTerminal[symbol];
	else Symbol = Terminal[-symbol - 1];
	BC = "<";
	BC += Symbol;
	if(plus) BC += "+>";
	else BC += "*>";
	Ctemp = BC.ptr();
	for (i = 0; i < NontTerminal.count(); i++)
		if ((compare(Ctemp, NontTerminal[i]))) break;
	UnitNum = (long int)i;
	Group* GG;
	Rules* Arule;
	if (i == NontTerminal.count())
	{
		NontTerminal.append(Copy(Ctemp));
		GG = new Group;
		GG->SetName(Ctemp);
		GG->implicit = true;
		GrammarG.append(GG);

		Arule = new Rules;
		Arule->SetName("first");
		if(!plus) Arule->formula.append((long int)( - 1));
		else Arule->formula.append(symbol);
		GG->rules.append(Arule);

		Arule = new Rules;
		Arule->SetName("multi");
		Arule->formula.append(UnitNum);
		Arule->formula.append(symbol);
		GG->rules.append(Arule);
	}
	return UnitNum;
}
void InputPanel::addTerminal(void)
{
	size_t i, j;
	const char* temp;
	bool voidExist = false;
	Terminal.append();
	for (i = 1; i < RegG.count(); i++)
	{
		for (j = 0; j < RegG[i]->regs.count(); j++)
		{
			temp = Copy(RegG[i]->regs[j]->name);
			Terminal.append(temp);
			if (compare("void", temp)) voidExist = true;
		}
	}
	if (voidExist) Terminal[0] = Copy("*");
	else Terminal[0] = Copy("void");
	temp = Copy("#");
	Terminal.append(temp);
}
long int InputPanel::SymbolAdd(const char* symbol)
{
	long int i;
	const char* temp;
	if (symbol == NULL) return -1;
	for (i = 0; i < Terminal.count(); i++)
		if (compare(symbol, Terminal[i])) return -i - 1;
	for (i = 0; i < NontTerminal.count(); i++)
		if ((compare(symbol, NontTerminal[i]))) return i;
	temp = Copy(symbol);
	NontTerminal.append(temp);
	return NontTerminal.count() - 1;
}
long int InputPanel::UnitRegGroupAdd(const char* symbol)
{
	BufferChar BC;
	size_t i, group_;
	const char * Ctemp;
	long int UnitNum;
	for (group_ = 0; group_ < RegG.count(); group_++)
		if (compare(symbol, RegG[group_]->name)) break;
	if (group_ == RegG.count())
	{
		errorCode = regGroupMissing;
		return 0;
	}
	BC = "[";
	BC += symbol;
	BC += "]";
	Ctemp = BC.ptr();
	//UnitNum = SymbolAdd(Ctemp);
	for (i = 0; i < NontTerminal.count(); i++)
		if ((compare(Ctemp, NontTerminal[i]))) break;
	UnitNum = (long int)i;
	RegGroup* RG;
	RegContent* RC;
	Group* GG;
	Rules* Arule;
	RG = RegG[group_];
	if (i == NontTerminal.count())
	{
		NontTerminal.append(Copy(Ctemp));
		GG = new Group;
		GG->SetName(Ctemp);
		GG->implicit = true;
		GrammarG.append(GG);
		for (i = 0; i < RG->regs.count(); i++)
		{
			RC = RG->regs[i];
			Arule = new Rules;
			Arule->SetName(RC->name);
			Arule->formula.append(SymbolAdd(RC->name));
			GG->rules.append(Arule);
		}
	}
	return UnitNum;
}
int InputPanel::NonTerminalSort(void)
{
	vector<size_t> sort;
	size_t i, j, k; 
	Rules* now;
	long int temp;
	int error = 0;
	sort.recount(NontTerminal.count());
	sort.value(0);
	for (i = 0; i < GrammarG.count(); i++)
	{
		for (j = 0; j < NontTerminal.count(); j++)
			if (compare(GrammarG[i]->name, NontTerminal[j])) sort[j] = i;
	}
	for (i = 1; i < sort.count(); i++)
		if (sort[i] == 0) break;
	if (i < sort.count() || GrammarG.count() != NontTerminal.count())
	{
		errorCode = ErrorNonTernimal;
		return -9990;
	}
	for (j = 1; j < NontTerminal.count(); j++)
	{
		free((void*)NontTerminal[j]);
		NontTerminal[j] = Copy(GrammarG[j]->name);
	}
	for (i = 0; i < GrammarG.count(); i++)
	{
		for (j = 0; j < GrammarG[i]->rules.count(); j++)
		{
			now = GrammarG[i]->rules[j];
			for (k = 0; k < now->formula.count(); k++)
			{
				temp = now->formula[k];
				if (temp == 0)
				{
					error = -9991;
					errorCode = WorngRuleBody;
					errorInfor1 = i;
					errorInfor2 = j;

				}
				now->formula[k] = temp >= 0 ? sort[temp] : temp;
				
			}
		}
	}
	return error;
}
void InputPanel::ImplicitAdd(void)
{
	size_t i, j; 
	Group* GG;
	Rules* Arule;
	rules.clear();
	for (i = 0; i < GrammarG.count(); i++)
	{
		GG = GrammarG[i];
		for (j = 0; j < GG->rules.count(); j++)
		{
			Arule = GG->rules[j];
			Arule->implicit = GG->implicit ? true : Arule->implicit;
			rules.append(Arule);
		}
	}
	
}
void InputPanel::buildLpost(void)
{
	size_t i, j;
	RegContent* now;
	for (i = 0; i < RegG.count(); i++)
	{
		for (j = 0; j < RegG[i]->regs.count(); j++)
			RegG[i]->regs[j]->group = i;
	}
	for (i = 1; i < RegG.count(); i++)
	{
		for (j = 0; j < RegG[i]->regs.count(); j++)
		{
			now = RegG[i]->regs[j];
			regular.append(now);
			now->accept = regular.count();
		}
	}
}


bool Convert::in(const char ele)
{
	return lower <= ele && ele <= upper && !epsilon;
}
bool convert::in(const char ele)
{
	return lower <= (int)ele && (int)ele <= upper;
}
bool convert::in(int ele)
{
	return lower <= ele && ele <= upper;
}

void static RegTree_Demo(FILE* fp, char L, char U)
{
	if (U != L)
	{
		if ((int)L < 32 || (int)L == 127) fprintf(fp, "[\\%d-", (int)L);
		else fprintf(fp, "[%c-", L);
		if ((int)U < 32 || (int)U == 127) fprintf(fp, "\\%d]", (int)U);
		else fprintf(fp, "%c]", U);
	}
	else
	{
		if ((int)L < 32 || (int)L == 127) fprintf(fp, "[\\%d]", (int)L);
		else fputc(L, fp);
	}
}

sNFA::sNFA()
{
	StateAmount = 0;
	accepted = 0;
}
sNFA::~sNFA()
{
}
void sNFA::Demo(FILE* fp) const
{
	size_t i, first, count;
	//size_t To;
	char U, L;
	fprintf(fp, "\nStateAmount = %zu.\n", StateAmount);
	fprintf(fp, "accepted state = %zu.\n", accepted);
	fprintf(fp, "Amount od Arcs = %zu.\n", graph.arcs.count());
	fprintf(fp, "=========================================================\n");
	for (i = 0; i < StateAmount; i++)
	{
		fprintf(fp, "State[%zu], Arc count: %zu\n", i, graph.ArcCount(i));
		count = 0;
		first = graph.vertice[i].first;
		while (first != SizeMax)
		{
			fprintf(fp, "\tNo[%zu], to: %zu, ", count, graph.ArcTo(first));
			if(graph.arcs[first].content.epsilon)
				fprintf(fp, "a default epsilon conversion.\n");
			else
			{
				U = graph.arcs[first].content.upper;
				L = graph.arcs[first].content.lower;
				fprintf(fp, "label: ");
				if (U != L)
				{
					if ((int)L < 32) fprintf(fp, "[\\%d-", (int)L);
					else fprintf(fp, "[%c-", L);
					if ((int)U < 32) fprintf(fp, "\\%d]", (int)U);
					else fprintf(fp, "%c]", U);
				}
				else
				{
					if ((int)L < 32) fprintf(fp, "[\\%d]", (int)L);
					else fputc(L, fp);
				}
				fprintf(fp, "\n");
			}
			count += 1;
			first = graph.arcs[first].next;
		}
	}
	fprintf(fp, "=========================================================\n");
}
void sNFA::refresh(void)
{
	StateAmount = 0;
	graph.refresh();
	accepted = 0;
	//CC.refresh();
}
void sNFA::build(const RegularExp* Reg)
{
	buffer<size_t> output;
	vector<size_t> s;
	vector<sNFA*> nfa;
	BiTree<RegularExp::item>::iterator iterator;
	BiTree<RegularExp::item>* target;
	RegularExp::NodeType T; 
	sNFA* now, * L, * R;
	vector<bool> input;
	size_t j;
	L = NULL;
	R = NULL;
	refresh();
	input.recount(CharSize + 1);
	iterator.initial(Reg->tree);
	while (iterator.still())
	{
		target = iterator.target();
		if (iterator.state() == 2)
		{
			T = target->content().type;
			if (target == Reg->tree) now = this;
			else now = new sNFA;
			switch (T)
			{
			case hyperlex::RegularExp::Concatenation:
			case hyperlex::RegularExp::Alternation:
				L = (sNFA*)target->left()->label();
				target->left()->label() = NULL;
				R = (sNFA*)target->right()->label();
				target->right()->label() = NULL;
				break;
			case hyperlex::RegularExp::ZeroOrMore:
			case hyperlex::RegularExp::OneOrMore:
			case hyperlex::RegularExp::ZeroOrOne:
				L = (sNFA*)target->left()->label();
				target->left()->label() = NULL;
				R = NULL;
				break;
			default:
				L = NULL;
				R = NULL;
				break;
			}
			switch (T)
			{
			case hyperlex::RegularExp::Concatenation:
				now->serial(L, R);
				break;
			case hyperlex::RegularExp::Alternation:
				now->parallel(L, R);
				break;
			case hyperlex::RegularExp::ZeroOrMore:
				now->any(L);
				break;
			case hyperlex::RegularExp::OneOrMore:
				now->plus(L);
				break;
			case hyperlex::RegularExp::ZeroOrOne:
				now->ZeroOrOne(L);
				break;
			case hyperlex::RegularExp::Range:
				for (j = 0; j < CharSize; j++)
					input[j] = (j <= (size_t)target->content().upper && j >= (size_t)target->content().lower);
				now->build(input.ptr());
				break;
			default:
				break;
			}
			target->label() = now;
			delete L;
			delete R;
		}
		iterator.next();
	}
	Reg->tree->label() = NULL;
}

void sNFA::build(void)
{
	//size_t site;
	Convert CC;
	refresh();
	StateAmount = 2;
	graph.refresh(StateAmount);
	//FirstConvert.Malloc(StateAmount);
	//accepted.Malloc(StateAmount);
	//accepted[0] = 0;
	//accepted[1] = 1;
	accepted = 1;
	//FirstConvert[0] = SizeMax;
	//FirstConvert[1] = SizeMax;
	//site = CC.expand();
	//CC[site].from = 0;
	//CC[site].to = 1;
	//CC[site].next = SizeMax;
	CC.epsilon = true;
	CC.lower = 'a' + (char)1;
	CC.upper = 'a';
	graph.append(0, 1, CC);
}
void sNFA::build(const bool input[128])
{
	size_t i;
	Convert CC;
	char state = 0;
	refresh();
	StateAmount = 2;
	graph.refresh(StateAmount);
	//FirstConvert.Malloc(StateAmount);
	//accepted.Malloc(StateAmount);
	//accepted[0] = 0;
	//accepted[1] = 1;
	accepted = 1;
	//FirstConvert[0] = SizeMax;
	//FirstConvert[1] = SizeMax;
	//site = CC.expand();
	//CC[site].from = 0;
	//CC[site].to = 1;
	//CC[site].next = SizeMax;
	for (i = 0; i < 128; i++)
	{
		switch (state)
		{
		case 0:
			if (input[i])
			{
				//site = CC.expand();
				//CC[site].from = 0;
				//CC[site].to = 1;
				//CC[site].next = SizeMax;
				CC.epsilon = false;
				CC.lower = (char)i;
				//graph.append(0, 1, site);
				state = 1;
			}
			break;
		case 1:
			if (!input[i])
			{
				CC.upper = (char)(i - 1);
				graph.append(0, 1, CC);
				//std::cout << "L: " << CC.lower << "U: " << CC.upper << std::endl;
				state = 0;
			}
			break;
		}
	}
	if (state == 1)
	{
		CC.upper = (char)(i - 1);
		graph.append(0, 1, CC);
		state = 0;
	}
	//std::cout << std::endl << "build(const bool input[128])" << std::endl;
	//Demo(stdout);
}
void sNFA::parallel(const sNFA& left, const sNFA& right)
{
	size_t leftOffset, rightOffset;
	Convert CC;
	refresh();
	StateAmount = 2 + left.StateAmount + right.StateAmount;
	graph.refresh(StateAmount);
	accepted = StateAmount - 1;

	leftOffset = 1;
	rightOffset = 1 + left.StateAmount;

	CC.epsilon = true;
	CC.lower = 'a' + (char)1;
	CC.upper = 'a';
	graph.append(0, leftOffset, CC);
	graph.append(0, rightOffset, CC);

	graph.append(left.graph, leftOffset);
	graph.append(right.graph, rightOffset);

	graph.append(leftOffset + left.accepted, accepted, CC);
	graph.append(rightOffset + right.accepted, accepted, CC);
}
void sNFA::serial(const sNFA& left, const  sNFA& right)
{
	size_t leftOffset, rightOffset;
	Convert CC;
	refresh();
	leftOffset = 0;
	rightOffset = left.StateAmount;
	StateAmount = left.StateAmount + right.StateAmount;
	graph.refresh(StateAmount);
	accepted = rightOffset + right.accepted;

	graph.append(left.graph, leftOffset);
	graph.append(right.graph, rightOffset);

	CC.epsilon = true;
	CC.lower = 'a' + (char)1;
	CC.upper = 'a';
	graph.append(left.accepted, rightOffset, CC);
	graph.append(rightOffset, left.accepted, CC);
}
void sNFA::any(const sNFA& right)
{
	size_t rightOffset;
	Convert CC;
	refresh();
	StateAmount = 2 + right.StateAmount;
	graph.refresh(StateAmount);
	accepted = StateAmount - 1;

	rightOffset = 1;
	graph.append(right.graph, rightOffset);

	CC.epsilon = true;
	CC.lower = 'a' + (char)1;
	CC.upper = 'a';
	graph.append(0, 1, CC);
	graph.append(0, accepted, CC);
	graph.append(rightOffset + right.accepted, 1, CC);
	graph.append(rightOffset + right.accepted, accepted, CC);
}
void sNFA::plus(const sNFA& right)
{
	size_t rightOffset, middle;
	Convert CC;
	refresh();
	StateAmount = 2 + 2 * right.StateAmount;
	graph.refresh(StateAmount);
	accepted = StateAmount - 1;

	rightOffset = right.StateAmount + 1;
	middle = right.StateAmount;
	graph.append(right.graph, 0);
	graph.append(right.graph, rightOffset);

	CC.epsilon = true;
	CC.lower = 'a' + (char)1;
	CC.upper = 'a';
	graph.append(right.accepted, middle, CC);
	graph.append(middle, right.accepted, CC);
	//==================================
	graph.append(middle, rightOffset, CC);
	graph.append(middle, accepted, CC);
	graph.append(rightOffset + right.accepted, rightOffset, CC);
	graph.append(rightOffset + right.accepted, accepted, CC);
}
void sNFA::ZeroOrOne(const sNFA& right)
{
	size_t rightOffset;
	Convert CC;
	refresh();
	StateAmount = 2 + right.StateAmount;
	graph.refresh(StateAmount);
	accepted = StateAmount - 1;

	rightOffset = 1;
	graph.append(right.graph, rightOffset);

	CC.epsilon = true;
	CC.lower = 'a' + (char)1;
	CC.upper = 'a';
	graph.append(0, 1, CC);
	graph.append(0, accepted, CC);
	//graph.append(rightOffset + right.accepted, 1, CC);
	graph.append(rightOffset + right.accepted, accepted, CC);
}

void sNFA::parallel(const sNFA* left, const sNFA* right)
{
	size_t leftOffset, rightOffset;
	Convert CC;
	refresh();
	StateAmount = 2 + left->StateAmount + right->StateAmount;
	graph.refresh(StateAmount);
	accepted = StateAmount - 1;

	leftOffset = 1;
	rightOffset = 1 + left->StateAmount;

	CC.epsilon = true;
	CC.lower = 'a' + (char)1;
	CC.upper = 'a';
	graph.append(0, 1, CC);
	graph.append(0, rightOffset, CC);

	graph.append(left->graph, leftOffset);
	graph.append(right->graph, rightOffset);

	graph.append(leftOffset + left->accepted, accepted, CC);
	graph.append(rightOffset + right->accepted, accepted, CC);

	//std::cout << std::endl << "parallel(const sNFA* left, const sNFA* right)" << std::endl;
	//Demo(stdout);
}
void sNFA::serial(const sNFA* left, const  sNFA* right)
{
	size_t leftOffset, rightOffset;
	Convert CC;
	refresh();
	leftOffset = 0;
	rightOffset = left->StateAmount;
	StateAmount = left->StateAmount + right->StateAmount;
	graph.refresh(StateAmount);
	accepted = rightOffset + right->accepted;

	graph.append(left->graph, leftOffset);
	graph.append(right->graph, rightOffset);

	CC.epsilon = true;
	CC.lower = 'a' + (char)1;
	CC.upper = 'a';
	graph.append(left->accepted, rightOffset, CC);
	graph.append(rightOffset, left->accepted, CC);

	//std::cout << std::endl << "void sNFA::serial(const sNFA* left, const  sNFA* right)" << std::endl;
	//Demo(stdout);
	//std::cout << std::endl << "left" << std::endl;
	//left->Demo(stdout);
	//std::cout << std::endl << "right" << std::endl;
	//right->Demo(stdout);
}
void sNFA::any(const sNFA* right)
{
	size_t rightOffset;
	Convert CC;
	refresh();
	StateAmount = 2 + right->StateAmount;
	graph.refresh(StateAmount);
	accepted = StateAmount - 1;

	rightOffset = 1;
	graph.append(right->graph, rightOffset);

	CC.epsilon = true;
	CC.lower = 'a' + (char)1;
	CC.upper = 'a';
	graph.append(0, 1, CC);
	graph.append(0, accepted, CC);
	graph.append(rightOffset + right->accepted, 1, CC);
	graph.append(rightOffset + right->accepted, accepted, CC);
}
void sNFA::plus(const sNFA* right)
{
	size_t rightOffset, middle;
	Convert CC;
	refresh();
	StateAmount = 2 + 2 * right->StateAmount;
	graph.refresh(StateAmount);
	accepted = StateAmount - 1;

	rightOffset = right->StateAmount + 1;
	middle = right->StateAmount;
	graph.append(right->graph, 0);
	graph.append(right->graph, rightOffset);

	CC.epsilon = true;
	CC.lower = 'a' + (char)1;
	CC.upper = 'a';
	graph.append(right->accepted, middle, CC);
	graph.append(middle, right->accepted, CC);
	//==================================
	graph.append(middle, rightOffset, CC);
	graph.append(middle, accepted, CC);
	graph.append(rightOffset + right->accepted, rightOffset, CC);
	graph.append(rightOffset + right->accepted, accepted, CC);

	//std::cout << std::endl << "void sNFA::plus(const sNFA* right)" << std::endl;
	//right->Demo(stdout);
	//std::cout << std::endl << "void sNFA::plus(const sNFA* right)" << std::endl;
	//Demo(stdout);
}
void sNFA::ZeroOrOne(const sNFA* right)
{
	size_t rightOffset;
	Convert CC;
	refresh();
	StateAmount = 2 + right->StateAmount;
	graph.refresh(StateAmount);
	accepted = StateAmount - 1;

	rightOffset = 1;
	graph.append(right->graph, rightOffset);

	CC.epsilon = true;
	CC.lower = 'a' + (char)1;
	CC.upper = 'a';
	graph.append(0, 1, CC);
	graph.append(0, accepted, CC);
	//graph.append(rightOffset + right.accepted, 1, CC);
	graph.append(rightOffset + right->accepted, accepted, CC);
	//std::cout << std::endl << "ZeroOrOne(const sNFA* right)" << std::endl;
	//Demo(stdout);
}

NFA::NFA(const sNFA& single)
{
	size_t i;
	graph.refresh();
	StateAmount = single.StateAmount;
	graph.refresh(StateAmount);
	graph.append(single.graph, 0);
	for (i = 0; i < StateAmount; i++) graph.vertice[i].content = 0;
	graph.vertice[single.accepted].content = 1;
	accepted = 1;

	priority.refresh(accepted + 1);
	for (i = 0; i < 1 + 1; i++) priority[i] = i;
	//stack.reshape(StateAmount);
	//stack.refresh();
}
NFA::NFA(const sNFA*const* multiple, size_t count)
{
	build(multiple, count);
	//stack.reshape(StateAmount);
	//stack.refresh();
}
void NFA::build(const sNFA* const* multiple, size_t count)
{
	size_t i, prefix;
	Convert CC;
	graph.refresh();
	StateAmount = 1;
	accepted = count;
	for (i = 0; i < count; i++) StateAmount += multiple[i]->StateAmount;
	graph.refresh(StateAmount);
	prefix = 1;
	for (i = 0; i < count; i++)
	{
		graph.append(multiple[i]->graph, prefix);
		prefix += multiple[i]->StateAmount;
	}
	for (i = 0; i < StateAmount; i++) graph.vertice[i].content = 0;
	prefix = 1;
	CC.epsilon = true;
	CC.lower = 'a' + (char)1;
	CC.upper = 'a';
	for (i = 0; i < count; i++)
	{
		graph.vertice[prefix + multiple[i]->accepted].content = (int)(i + 1);
		graph.append(0, prefix, CC);
		prefix += multiple[i]->StateAmount;
	}
	priority.refresh(count + 1);
	for (i = 0; i < count + 1; i++) priority[i] = i;
	//stack.reshape(StateAmount);
	//stack.refresh();
}

NFA::NFA(const InputPanel& lP)
{
	vector<sNFA*> all;
	size_t count, i;

	count = lP.regular.count();
	all.recount(count);
	//printf("all.recount(%zu);\n", count);
	for (i = 0; i < count; i++)
	{
		//printf("all[%zu] = new sNFA()\n", i);
		all[i] = new sNFA();
		all[i]->build(lP.regular[i]->reg);
	}
	build(all.ptr(), count);
	for (i = 0; i < count; i++)
		delete all[i];
	for (i = 0; i < count; i++) priority[i + 1] = (size_t)(lP.regular[i]->priority + 1);
	priority[0] = 0;
	
}
NFA::~NFA()
{

}

void NFA::initial(bool* state, list<size_t>& stack) const
{
	size_t i;
	state[0] = true;
	for (i = 1; i < StateAmount; i++) state[i] = false;
	closure(state, stack);
}
void NFA::closure(bool* state, list<size_t>& stack) const
{
	size_t delta;
	size_t Scount;
	size_t i;
	size_t now, arc, temp;
	Scount = 0;
	delta = 0;
	stack.refresh();
	for (i = 0; i < StateAmount; i++)
		if (state[i])
		{
			Scount += 1;
			stack.append(i);
		}
	while (stack.pop(now) != 0)
	{
		arc = graph.vertice[now].first;
		while (arc != SizeMax)
		{
			if (graph.arcs[arc].content.epsilon == true)
			{
				temp = graph.arcs[arc].to;
				if (!state[temp])
				{
					state[temp] = true;
					stack.append(temp);
					delta += 1;
				}
			}
			arc = graph.arcs[arc].next;
		}
	}
}
size_t NFA::move(const bool* from, bool* to, const char ele, list<size_t>& stack)  const
{
	size_t i; 
	size_t Scount;
	size_t now, arc;
	
	/*
	size_t count__ = 0;
	bool flag;
	flag = false;
	for (i = 0; i < StateAmount; i++) count__ += from[i] ? 1 : 0;
	if (count__ == 39 && ele == '\'') flag = true;
	if (count__ == 10 && ele == '\\' && from[150]) flag = true;
	*/

	Scount = 0;
	for (i = 0; i < StateAmount; i++) to[i] = false;
	for (i = 0; i < StateAmount; i++)
		if (from[i])
		{
			//if (flag)
			//{
			//	std::cout << "from[" << i << "]:" << std::endl;
			//}
			arc = graph.vertice[i].first;
			while (arc != SizeMax)
			{
				if (graph.arcs[arc].content.in(ele))
				{
					now = graph.arcs[arc].to;
					Scount += (to[now] ? 0 : 1);
					to[now] = true;
					//if (flag) std::cout << "to: " << now << std::endl;
				}
				arc = graph.arcs[arc].next;
			}
		}
	//sheetDFA::Demo(stdout, to, StateAmount);
	closure(to, stack);
	//sheetDFA::Demo(stdout, to, StateAmount);
	return Scount;
}
size_t NFA::GetAccepted(const bool* state, bool* accept) const
{
	size_t __accpetedCount, i;
	for (i = 0; i < accepted + 1; i++) accept[i] = false;
	for (i = 0; i < StateAmount; i++)
		if (state[i]) accept[graph.vertice[i].content] = true;
	__accpetedCount = 0;
	for (i = 1; i < accepted + 1; i++)
		if (accept[i]) __accpetedCount += 1;
	return __accpetedCount;
}
size_t NFA::FirstAccepted(const bool* accept) const
{
	size_t i, result_;
	result_ = 0;
	for (i = 1; i < accepted + 1; i++)
		if (accept[i])
		{
			result_ = (priority[i] > priority[result_] ? i : result_);
		}
	return result_;
}
void NFA::Demo(FILE* fp) const
{
	size_t i, first, count;
	//size_t To;
	char U, L;
	fprintf(fp, "\nStateAmount = %zu.\n", StateAmount);
	fprintf(fp, "accepted state amount= %zu.\n", accepted);
	fprintf(fp, "Amount od Arcs = %zu.\n", graph.arcs.count());
	fprintf(fp, "=========================================================\n");
	for (i = 0; i < StateAmount; i++)
	{
		fprintf(fp, "State[%zu], Arc count: %zu, ", i, graph.ArcCount(i));
		fprintf(fp, "accepted label: [%d]\n", graph.vertice[i].content);
		count = 0;
		first = graph.vertice[i].first;
		while (first != SizeMax)
		{
			fprintf(fp, "\tNo[%zu], to: %zu, ", count, graph.ArcTo(first));
			if (graph.arcs[first].content.epsilon)
				fprintf(fp, "a default epsilon conversion.\n");
			else
			{
				U = graph.arcs[first].content.upper;
				L = graph.arcs[first].content.lower;
				fprintf(fp, "label: ");
				RegTree_Demo(fp, L, U);
				fprintf(fp, "\n");
			}
			count += 1;
			first = graph.arcs[first].next;
		}
	}
	fprintf(fp, "=========================================================\n");
}

sheetDFA::sheetDFA(const NFA& nfa) : sheet(CharSize)
{
	list<size_t> stack;
	list<size_t> auxiliary;
	matlist<bool> Dstates(nfa.StateAmount);
	size_t now, old;
	bool* state;
	vec<bool> to;
	int ele;
	size_t Scount;
	size_t NewSite_;
	LiveAcceptCount = 0;

	to.Malloc(nfa.StateAmount);
	auxiliary.reshape(nfa.StateAmount);
	auxiliary.refresh();
	state = Dstates.append();
	
	nfa.initial(state, auxiliary);
	//return;
	stack.append(0);
	sheet.append();
	//return;
	while (stack.pop(now) != 0)
	{
		//std::cout << "****************state number = " << now << ";" << std::endl;
		//Demo(stdout, Dstates[now], nfa.StateAmount);
		for (ele = 0; (size_t)ele < CharSize; ele++)
		{
			
			//std::cout << "ele[" << (size_t)ele << "] = " << (char)ele << ";" << std::endl;
			state = Dstates[now];
			Scount = nfa.move(state, to.vector(), (char)ele, auxiliary);
			//if (state[7])
			//{
				//std::cout << "Scount: " << Scount << ", No: " << ele << ", ele: " << (char)ele << std::endl;
				//Demo(stdout, state, nfa.StateAmount);
				//Demo(stdout, to.vector(), nfa.StateAmount);
				//']','Z', '[';
			//}
			//std::cout << "Scount: " << Scount << ");" << std::endl;
			if (Scount == 0) sheet[now][(size_t)ele] = 0;
			else
			{
				//std::cout << "Scount: " << Scount << ", No: " << ele << ", ele: " << (char)ele << std::endl;
				//bool tempKK;
				//tempKK = state[7];
				//if (state[7]) std::cout << "All is well 1 " << std::endl;
				old = Dstates.row();
				//if (state[7]) std::cout << "All is well 2 " << std::endl;
				NewSite_ = Dstates.SearchAdd(to.vector());
				//if (state[7]) std::cout << "All is well 2.5 " << std::endl;
				//else std::cout << "All is wrong 2.5 " << std::endl;
				sheet[now][(size_t)ele] = NewSite_;
				//sheet[now][(size_t)ele] = Dstates.SearchAdd(to.vector());
				//if (tempKK) std::cout << "All is well 3: " << NewSite_ << std::endl;
				if (old != Dstates.row())
				{
					//std::cout << "Scount: " << Scount << ", No: " << ele << ", ele: " << (char)ele << std::endl;
					//std::cout << "To *****new***** state: " << old << " : ";
					//Demo(stdout, to.vector(), nfa.StateAmount);
					sheet.append();
					stack.append(old);
				}
				//else
				//{
					//std::cout << "To old state: " << old << std::endl;
				//}
				//if (state[7]) std::cout << "All is well 4" << std::endl;
				//if (old != Dstates.row()) Demo(stdout, Dstates[old], nfa.StateAmount);
			}
			//if (true)
			//{
			//	std::cout << "=========Scount: " << Scount << ", No: " << ele << ", ele: " << (char)ele << std::endl;
				//Demo(stdout, state, nfa.StateAmount);
			//}
			//std::cout << "Scount: " << Scount << ");" << std::endl;
		}
		//std::cout << "now: (" << now << ");" << std::endl;
	}
	//std::cout << "Dstates.row(): " << Dstates.row() << std::endl;
	//Demo(stdout, Dstates[old], nfa.StateAmount);
	count = Dstates.row();
	acceptSet(Dstates, nfa);
}
sheetDFA::~sheetDFA()
{

}
void sheetDFA::acceptSet(const matlist<bool>& Dstates, const NFA& nfa)
{
	matlist<bool> __AcceptSheet(nfa.accepted + 1);
	list<size_t> __accpetedCount;
	size_t i;
	//size_t to, j;
	bool* __AcceptList;
	//int* SheetTemp;
	//char ele;
	//std::cout << "Dstates.row(): " << Dstates.row() << std::endl;
	AcceptCount = nfa.accepted;
	accept.refresh(Dstates.row());
	__AcceptSheet.refresh(__AcceptSheet.column(), Dstates.row());
	//std::cout << "Dstates.row(): " << Dstates.row() << std::endl;
	for (i = 0; i < Dstates.row(); i++)
	{
		__AcceptList = __AcceptSheet.append();
		//for (j = 0; j < CharSize; j++) accept[i][j] = 0;
		__accpetedCount.append(nfa.GetAccepted(Dstates[i], __AcceptList));
		accept[i] = nfa.FirstAccepted(__AcceptList);
		//std::cout << "__accpetedCount[]: " << __accpetedCount[i] << std::endl;
		//std::cout << "accept[i]: " << accept[i] << std::endl;
		//Demo(stdout, Dstates[i], Dstates.column());
	}
	vector<bool> live;
	live.recount(AcceptCount + 1);
	live.value(false);
	for (i = 0; i < accept.count(); i++)
		live[accept[i]] = true;
	for (i = 0; i < live.count(); i++)
		LiveAcceptCount += live[i] ? 1 : 0;
	LiveAcceptCount -= 1;
	//std::cout << "Dstates.row(): " << Dstates.row() << std::endl;
	//for (i = 0; i < Dstates.row(); i++)
	//{
	//	if (__accpetedCount[i] == 0) continue;
	//	SheetTemp = sheet[i];
	//	for (ele = 0; (size_t)ele < CharSize; ele++)
	//	{
	//		to = SheetTemp[(size_t)ele];
	//		if (__accpetedCount[to] == 0) accept[i][(size_t)ele] = nfa.FirstAccepted(__AcceptSheet[i]);
	//	}
	//}
}
int sheetDFA::next(int state, const char c)const
{
	return sheet[(size_t)state][(size_t)c];
}
int sheetDFA::action(int state)const
{
	return accept[(size_t)state];
}
void sheetDFA::Demo(FILE* fp)const
{
	size_t i, j, arc;
	char U, L;
	int state, to, acc;
	U = '?';
	L = '?';
	to = 0;
	fprintf(fp, "=========================================================\n");
	fprintf(fp, "\nStateAmount = %zu.\n", count);
	fprintf(fp, "accepted state amount= %zu.\n", AcceptCount);
	//fprintf(fp, "Amount od Arcs = %llu.\n", graph.arcs.count());
	fprintf(fp, "=========================================================\n");
	for (i = 0; i < count; i++)
	{
		acc = action(i);
		fprintf(fp, "State[%zu]: acc = %d\n", i, acc);
		state = 0;
		arc = 0;
		for (j = 0; j < CharSize; j++)
		{
			switch (state)
			{
			case 0:
				if (sheet[i][j] != 0)
				{
					L = j;
					to = sheet[i][j];
					state = 1;
				}
				break;
			case 1:
				if (to != sheet[i][j])
				{
					U = j - 1;
					fprintf(fp, "\t Arc[%zu]: ", arc);
					arc += 1;
					RegTree_Demo(fp, L, U);
					
					fprintf(fp, ", to: %d.\n", to);
					if (sheet[i][j] != 0)
					{
						L = j;
						to = sheet[i][j];
					}
					else state = 0;
				}
				break;
			}
		}
		if (state == 1)
		{
			U = (char)(CharSize - 1);
			fprintf(fp, "\t Arc[%zu]: ", arc);
			RegTree_Demo(fp, L, U);
			acc = action((int)i);
			fprintf(fp, ", to: %d, acc = %d.\n", to, acc);
			fprintf(fp, ", to: %d.\n", to);
		}
	}
}
void sheetDFA::Demo(FILE* fp, const bool* state, size_t n)
{
	size_t i, count;
	for (i = 0, count = 0; i < n; i++)
		count += state[i] ? 1 : 0;
	fprintf(fp, "count: %6zu | ", count);
	for (i = 0, count = 0; i < n; i++)
		if(state[i]) fprintf(fp, "%zu ", i);
	fprintf(fp, "\n");
}
bool sheetDFA::ifDeadAccept(void) const
{
	return AcceptCount != LiveAcceptCount;
}
void sheetDFA::shrink(void)
{
	ShrinkList* SL;
	//list<int> state_record;
	size_t i, j, toRecord, now, state, NewS, nextS;
	size_t toNow;
	bool change;
	SL = new ShrinkList(LiveAcceptCount + 1, sheet.row());
	for (i = 0; i < sheet.row(); i++)
		SL->Insert(i, accept[i]);
	//SL->Demo(stdout);
	do
	{
		change = false;
		for (state = 0; state < SL->Statecount; state++)
		{
			for (j = 0; j < CharSize; j++)
			{
				now = SL->Head(state);
				toRecord = SL->group[sheet[now][j]];
				NewS = SizeMax;
				//printf("state: %zu, char:%c", state, (char)j);
				nextS = SL->next[now];
				//printf(", now: %zu, nextS:%zu\n", now, nextS);
				while (nextS != SizeMax)
				{
					toNow = SL->group[sheet[nextS][j]];
					change = change || (toNow != toRecord);
					if (toNow != toRecord)
					{
						//printf("\t\tstate: %zu, char:%c, ", state, (char)j);
						//printf("now: %zu, nextS:%zu, ", now, nextS);
						//printf("toNow: %zu, toRecord:%zu", toNow, toRecord);
						NewS = SL->NewState(NewS);
						SL->DeleteNext(now);
						SL->Insert(nextS, NewS);
						//nextS = SL->next[now];
						//printf(", nextS:%zu\n", nextS);
					}
					else now = nextS;
					nextS = SL->next[now];
					//printf("\tnow: %zu, nextS:%zu\n", now, nextS);
				}
				if (change) break;
			}
			if (change) break;
		}
	} while (change);
	shrink(SL);
	delete SL;
}
void sheetDFA::shrink(const ShrinkList* SL)
{
	//SL->Demo(stdout);
	matlist<int> Newsheet(sheet.column());
	list<int> Newaccept;
	size_t Newcount;
	size_t i, j;
	Newcount = SL->Statecount;
	Newsheet.refresh(sheet.column(), Newcount);
	Newaccept.refresh(Newcount);
	for (i = 0; i < Newcount; i++)
		Newaccept[i] = accept[SL->head[i]];
	for (i = 0; i < Newcount; i++)
	{
		Newsheet.append();
		for (j = 0; j < CharSize; j++)
		{
			Newsheet[i][j] = SL->group[sheet[SL->head[i]][j]];
		}
	}
	sheet.swap(Newsheet);
	accept.swap(Newaccept);
	count = Newcount;
}

DFA::DFA(const sheetDFA* dfa)
{
	//size_t i, j, arc;
	size_t i, j;
	convert CC;
	//char U, L;
	int state, to;
	//int acc;
	//printf("????\n");
	CC.lower = 'c';
	CC.upper = 'c';
	to = 0;
	graph.refresh();
	StateAmount = dfa->sheet.row();
	AcceptCount = dfa->AcceptCount;
	graph.refresh(StateAmount);
	//printf("StateAmount = %llu\n", StateAmount);
	for (i = 0; i < StateAmount; i++)
		graph.vertice[i].content = dfa->accept[i];
	//printf("????\n");
	for (i = 0; i < dfa->count; i++)
	{ 
		state = 0;
		//arc = 0;
		//printf("%llu==\n", i);
		for (j = 0; j < CharSize; j++)
		{
			switch (state)
			{
			case 0:
				if (dfa->sheet[i][j] != 0)
				{
					CC.lower = j;
					to = dfa->sheet[i][j];
					state = 1;
				}
				break;
			case 1:
				if (to != dfa->sheet[i][j])
				{
					CC.upper = j - 1;
					//printf("%llu==%d\n", i, to);
					//graph.Demo(stdout);
					//printf("%llu==%d\n", i, to);
					
					graph.append(i, to, CC);
					//printf("%llu==%d\n", i, to);
					if (dfa->sheet[i][j] != 0)
					{
						CC.lower = j;
						to = dfa->sheet[i][j];
					}
					else state = 0;
				}
				break;
			}
		}
		if (state == 1)
		{
			CC.upper = CharSize - 1;
			graph.append(i, to, CC);
		}
	}
}
DFA::~DFA()
{
}
int DFA::next(int state, const char c)const
{
	size_t from;
	//from = graph.vertice[(size_t)state].first;
	from = graph.FirstArc((size_t)state);
	while (from != SizeMax)
	{
		if (graph.arcs[from].content.in(c))
			return graph.arcs[from].to;
		from = graph.NextArc(from);
	}
	return 0;
}
int DFA::action(int state)const
{
	return graph.vertice[(size_t)state].content;
}
void DFA::Demo(FILE* fp)const
{
	size_t i, arc, No;
	fprintf(fp, "=========================================================\n");
	fprintf(fp, "\nStateAmount = %llu.\n", (long long unsigned int)StateAmount);
	fprintf(fp, "accepted state amount= %llu.\n", (long long unsigned int)AcceptCount);
	//fprintf(fp, "Amount od Arcs = %llu.\n", graph.arcs.count());
	fprintf(fp, "=========================================================\n");
	for (i = 0; i < StateAmount; i++)
	{
		fprintf(fp, "State[%llu]: acc = %d\n", (long long unsigned int)i, (int)graph.vertice[i].content);
		arc = graph.FirstArc((size_t)i);
		No = 0;
		while (arc != SizeMax)
		{
			fprintf(fp, "\t Arc[%llu]: ", (long long unsigned int)No);
			Demo(fp, graph.arcs[arc].content);
			fprintf(fp, ", to: %llu \n", (long long unsigned int)graph.arcs[arc].to);
			arc = graph.NextArc(arc);
			No += 1;
		}
	}
}
void DFA::Demo(FILE* fp, int L, int U)
{
	if (U != L)
	{
		if (L < 32) fprintf(fp, "[\\%d-", L);
		else fprintf(fp, "[%c-", (char)L);
		if (U < 32) fprintf(fp, "\\%d]", U);
		else fprintf(fp, "%c]", (char)U);
	}
	else
	{
		if (L < 32) fprintf(fp, "[\\%d]", L);
		else fputc(L, fp);
	}
}
void DFA::Demo(FILE* fp, convert& CC)
{
	Demo(fp, CC.lower, CC.upper);
}
void DFA::Cprint(FILE* fp, const char* name) const
{
	fprintf(fp, "int next");
	if (name != NULL) fprintf(fp, "_%s", name);
	CprintCore(fp, name);
}
void DFA::CppPrint(FILE* fp, const char* name)const
{
	if (name != NULL) fprintf(fp, "int %s::next", name);
	else fprintf(fp, "int next");
	CprintCore(fp, name);
}
void DFA::CprintCore(FILE* fp, const char* name)const
{
	size_t i, arc, No;
	fprintf(fp, "(int state, const char c)\n{\n");
	fprintf(fp, "\tswitch (state)\n\t{\n");
	for (i = 0; i < StateAmount; i++)
	{
		fprintf(fp, "\tcase %d:\n", (int)i);
		arc = graph.FirstArc((size_t)i);
		No = 0;
		while (arc != SizeMax)
		{
			if (No == 0)
				fprintf(fp, "\t\tif(");
			else
				fprintf(fp, "\t\telse if(");
			Cprint(fp, graph.arcs[arc].content);
			//Demo(fp, graph.arcs[arc].content);
			fprintf(fp, ") return %llu;\n", (long long unsigned int)graph.arcs[arc].to);
			arc = graph.NextArc(arc);
			No += 1;
		}
		if (No == 0)
			fprintf(fp, "\t\treturn 0;\n");
		else
			fprintf(fp, "\t\telse return 0;\n");
	}
	fprintf(fp, "\t}\n");
	fprintf(fp, "\treturn 0;\n}\n");
}
void DFA::CprintAccept(FILE* fp, const char* name, const char** const category, const char** const accept)const
{
	size_t i, No;
	fprintf(fp, "int action");
	if (name != NULL) fprintf(fp, "_%s", name);
	fprintf(fp, "(int state)\n{\n");
	fprintf(fp, "\tswitch (state)\n\t{\n");
	for (i = 0; i < StateAmount; i++)
	{
		No = (size_t)(graph.vertice[i].content);
		if (No == 0) continue;
		fprintf(fp, "\tcase %zu:\n", i);
		fprintf(fp, "\t\treturn %zu;", No);
		if (category != NULL || accept != NULL)
			fprintf(fp, "//");
		if (category != NULL)
			fprintf(fp, "%s: ", category[No - 1]);
		if (accept != NULL)
			fprintf(fp, "%s", accept[No - 1]);
		fprintf(fp, "\n");
	}
	fprintf(fp, "\t}\n");
	fprintf(fp, "\treturn 0;\n}\n");
}
void DFA::CppPrintAccept(FILE* fp, const char* name, const char** const category, const char** const accept)const
{
	if (name != NULL) fprintf(fp, "int %s::action", name);
	else fprintf(fp, "int action");
	CprintAcceptCore(fp, name, category, accept);
}
void DFA::CprintAcceptCore(FILE* fp, const char* name, const char** const category, const char** const accept)const
{
	size_t i, No;
	
	fprintf(fp, "(int state)\n{\n");
	fprintf(fp, "\tswitch (state)\n\t{\n");
	for (i = 0; i < StateAmount; i++)
	{
		No = (size_t)(graph.vertice[i].content);
		if (No == 0) continue;
		fprintf(fp, "\tcase %zu:\n", i);
		fprintf(fp, "\t\treturn %zu;", No);
		if (category != NULL || accept != NULL)
			fprintf(fp, "//");
		if (category != NULL)
			fprintf(fp, "%s: ", category[No - 1]);
		if (accept != NULL)
			fprintf(fp, "%s", accept[No - 1]);
		fprintf(fp, "\n");
	}
	fprintf(fp, "\t}\n");
	fprintf(fp, "\treturn 0;\n}\n");
}
void DFA::Cprint(FILE* fp, convert& CC)
{
	int L; 
	int U;
	L = CC.lower;
	U = CC.upper;
	if (U != L)
	{
		if (L < 32 || L == 127) fprintf(fp, "(char)%d <= c && ", L);
		else if (PostfixSwitch_small(L) == -1) fprintf(fp, "\'%c\' <= c && ", (char)L);
		else fprintf(fp, "\'\\%c\' <= c && ", (char)L);
		if (U < 32 || U == 127) fprintf(fp, "c <= (char)%d", U);
		else if (PostfixSwitch_small(U) == -1) fprintf(fp, "c <= \'%c\'", (char)U);
		else fprintf(fp, "c <= \'\\%c\'", (char)U);
	}
	else
	{
		if (L < 32 || L == 127) fprintf(fp, "c == (char)%d", L);
		else if (PostfixSwitch_small(L) == -1) fprintf(fp, "c == \'%c\'", (char)L);
		else fprintf(fp, "c == \'\\%c\'", (char)L);
	}
	
}
size_t DFA::StateAmountGet(void) const
{
	return StateAmount;
}
int DFA::GraphStateGet(const size_t site) const
{
	return (graph.vertice[site].content);
}

ShrinkList::ShrinkList(size_t AcceptCount, size_t size)
{
	size_t i;
	//size_t j, toRecord, now;
	group.Malloc(size);
	next.Malloc(size);
	Statecount = AcceptCount;
	head.refresh(Statecount);
	rear.refresh(Statecount);
	for (i = 0; i < Statecount; i++)
	{
		head[i] = SizeMax;
		rear[i] = SizeMax;
	}
	for (i = 0; i < size; i++) next[i] = SizeMax;

}
ShrinkList::~ShrinkList()
{
}
void ShrinkList::Insert(size_t site, size_t state)
{
	group[site] = state;
	next[site] = SizeMax;
	if (head[state] == SizeMax) head[state] = site;
	else next[rear[state]] = site;
	rear[state] = site;
}
size_t ShrinkList::NewState(size_t Now)
{
	if (Now != SizeMax) return Now;
	head.append(SizeMax);
	rear.append(SizeMax);
	Statecount += 1;
	return Statecount - 1;
}
size_t ShrinkList::DeleteNext(size_t now)
{
	size_t deleted, Sstate;
	deleted = next[now];
	Sstate = group[deleted];
	next[now] = next[deleted];
	if (deleted == rear[Sstate])  rear[Sstate] = now;
	return deleted;
}
size_t ShrinkList::Head(size_t state)const
{
	return head[state];
}
size_t ShrinkList::Count(void)const
{
	return Statecount;
}
void ShrinkList::Demo(FILE* fp)const
{
	size_t i;
	fprintf(fp, "Statecount = %llu\n", (long long int)Statecount);
	for (i = 0; i < group.size(); i++)
	{
		fprintf(fp, "\tgroup[%llu] = %llu, ", (long long int)i, (long long int)(group[i]));
		fprintf(fp, "next[%llu] = %llu\n", (long long int)i, (long long int)(next[i]));
	}
	fprintf(fp, "\n");
	for (i = 0; i < Statecount; i++)
	{
		fprintf(fp, "\thead[%llu] = %llu, ", (long long int)i, (long long int)(head[i]));
		fprintf(fp, "rear[%llu] = %llu\n", (long long int)i, (long long int)(rear[i]));
	}
}

grammerS::grammerS()
{
	ERROR = undone;
	count = 0;
	TerminalCount = 0;
	
	epsilon = 0;
	end = 0;
}
grammerS::~grammerS()
{
	size_t i;
	for (i = 0; i < name.count(); i++)
	{
		free((void*)name[i]);
	}
	for (i = 0; i < ternimal.count(); i++)
	{
		free((void*)ternimal[i]);
	}
}
int grammerS::build(const InputPanel& input)
{
	//return 0;
	size_t i, prefix_here, j, k, countTemp; 
	production PP;
	const char* Stemp;
	InputPanel::Rules* ruleTemp;
	long int Inttemp;
	for (i = 0; i < input.NontTerminal.count(); i++)
	{
		Stemp = Copy(input.NontTerminal[i]);
		name.append(Stemp);
	}
	for (i = 2; i < input.Terminal.count(); i++)
	{
		Stemp = Copy(input.Terminal[i - 1]);
		ternimal.append(Stemp);
	}
	
	count = name.count();
	TerminalCount = ternimal.count();
	ternimal.append(Copy("epsilon"));
	ternimal.append(Copy("END-EOF"));
	epsilon = (long long int)TerminalCount;
	epsilon = -epsilon - 1;
	end = epsilon - 1;
	degeneracy.refresh(count);//has length of count.
	//degeneracy[i]: rules' count of non-terminal[i]
	prefix.refresh(count);;//has length of count
	prefix_here = 0;
	for (i = 0; i < count; i++)
	{
		degeneracy[i] = input.GrammarG[i]->rules.count();
		prefix[i] = prefix_here;
		prefix_here += degeneracy[i];
	}
	rules.refresh(prefix_here);
	for (i = 0; i < count; i++)
	{
		PP.symbol = i;
		for (j = 0; j < degeneracy[i]; j++)
		{
			PP.begin = all.count();
			ruleTemp = input.GrammarG[i]->rules[j];
			PP.length = ruleTemp->formula.count();
			countTemp = 0;
			for (k = 0; k < PP.length; k++)
			{
				Inttemp = ruleTemp->formula[k];
				if (Inttemp < 0)
				{
					Inttemp = -Inttemp - 1;
					if (Inttemp == 0)
						Inttemp = epsilon;
					else if(Inttemp == (TerminalCount + 1))
						Inttemp = end;
					else 
						Inttemp = -(Inttemp - 1) - 1;
				}
				if (Inttemp != epsilon) countTemp += 1;
				all.append((long long int)Inttemp);
			}
			PP.LengthWithoutVoid = countTemp;
			rules[prefix[i] + j] = PP;
			
		}
	}


	FirstBiuld();
	//return 0;
	FollowBiuld();
	ERROR = done;
	return (int)done;
}
int grammerS::build(FILE* fp)
{
	BufferChar input;
	input << fp;
	return build(input);
}
int grammerS::nextTest(int state, const char c)
{
	switch (state)
	{
	case 0:
		if (c == (char)10) return 3;
		else if (c == (char)13) return 8;
		else if (c == ' ') return 2;
		else if (c == '-') return 7;
		else if (c == ';') return 6;
		else if ('A' <= c && c <= 'Z') return 5;
		else if (c == '_') return 5;
		else if ('a' <= c && c <= 'z') return 5;
		else if (c == '|') return 1;
		else return 0;
	case 1:
		return 0;
	case 2:
		if (c == ' ') return 2;
		else return 0;
	case 3:
		if (c == (char)10) return 3;
		else if (c == (char)13) return 8;
		else return 0;
	case 4:
		return 0;
	case 5:
		if ('0' <= c && c <= '9') return 5;
		else if ('A' <= c && c <= 'Z') return 5;
		else if (c == '_') return 5;
		else if ('a' <= c && c <= 'z') return 5;
		else return 0;
	case 6:
		return 0;
	case 7:
		if (c == '>') return 4;
		else return 0;
	case 8:
		if (c == (char)10) return 3;
		else return 0;
	}
	return 0;
}
int grammerS::actionTest(int state)
{
	switch (state)
	{
	case 1:
		return 1;
	case 2:
		return 2;
	case 3:
		return 3;
	case 4:
		return 4;
	case 5:
		return 5;
	case 6:
		return 6;
	}
	return 0;
	//1 : |
	//2 : space
	//3 : enter
	//4 : ->
	//5: id
	//6: ;
}
bool grammerS::RunBuild(int& accept, BufferChar& result, BufferChar& input, BufferChar &intermediate)
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
		state = nextTest(state, now);
		acc = actionTest(state);
		accept = acc != 0 ? acc : accept;
		switch (action)
		{
		case 0:
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
		case 1:
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
		case 2:
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
int grammerS::build(BufferChar& input)
{
	BufferChar result;
	BufferChar intermediate;
	list<size_t> site, InvSite;
	int accept;
	int state;
	long long int temp;
	const char* Result;
	production PP;
	size_t from, i, j;
	size_t Count_;
	list<const char*> nameTemp;
	PP.symbol = 0;
	PP.LengthWithoutVoid = 0;
	state = 0;
	from = 0;
	Count_ = 0;
	while (RunBuild(accept, result, input, intermediate))
	{
		if (accept == 0)
		{
			char aa;
			input.dequeue(aa);
			continue;
		}
		Result = result.ptr();
		//fprintf(stdout, "<%s, %d>\n", Result, accept);
		//fprintf(stdout, "%s", result.vector());
		//continue;
		switch (state)
		{
		case 0:
			if (compare(Result, "terminal")) state = 1;
			break;
		case 1:
			if (accept == 5)
			{
				if (compare(Result, "epsilon") || compare(Result, "END-EOF"))
				{
					ERROR = unexpectedTernimal;
					return (int)unexpectedTernimal;
				}
				state = 2;
				Tappend(Result);
			}
			break;
		case 2:
			if (accept == 6)
			{
				state = 3;
				TerminalCount = ternimal.count();
				epsilon = TerminalCount;
				epsilon = -epsilon - 1;
				end = epsilon - 1;
				Tappend("epsilon");
				Tappend("END-EOF");
				count = 0;
			}
			else if (accept == 1) state = 1;
			else if (accept == 5)
			{
				ERROR = missingdivision;
				return (int)missingdivision;
			}
			break;
		case 3:
			if (accept == 5) 
			{
				Count_ = 0;
				count += 1;
				site.append(SearchAppend(Result));
				prefix.append(from);
				state = 4;
			}
			break;
		case 4:
			if (accept == 4) state = 5;
			else if (accept == 5)
			{
				ERROR = missingdivision;
				return (int)missingdivision;
			}
			break;
		case 5:
			if (accept == 5)
			{
				Count_ += 1;
				state = 6;
				PP.length = 1;
				PP.begin = all.count();
				temp = SearchTerminal(Result);
				//fprintf(stdout, "temp: %lld, Result: %s\n", temp, Result);
				if (temp == 0) temp = SearchAppend(Result);
				all.append(temp);
			}
			break;
		case 6:
			if (accept == 5)
			{
				PP.length += 1;
				temp = SearchTerminal(Result);
				//fprintf(stdout, "temp: %lld, Result: %s\n", temp, Result);
				if (temp == 0) temp = SearchAppend(Result);
				all.append(temp);
			}
			else if (accept == 1)
			{
				rules.append(PP);
				state = 5;
			}
			else if (accept == 6)
			{
				rules.append(PP);
				from += Count_;
				degeneracy.append(Count_);
				state = 3;
			}
			break;
		}
	}
	if (count != name.count())
	{
		ERROR = missingRulesOrWrongSymbol;
		printf("count: %zu, name.count(): %zu\n", count, name.count());
		for (i = 0; i < name.count(); i++)
		{
			printf("name[%zu]: %s\n", i, name[i]);
		}
		return (int)missingRulesOrWrongSymbol;
	}
	//count = name.count();
	inverse(InvSite, site);
	//for (from = 0; from < site.count(); from++)
	//{
	//	fprintf(stdout, "No[%zu]: site: %zu, inv: %zu, name: %s\n", from, site[from], InvSite[from], name[from]);
	//}
	nameTemp.refresh(count);
	for (i = 0; i < count; i++)
		nameTemp[i] = name[site[i]];
	for (i = 0; i < count; i++) name[i] = nameTemp[i];
	for (i = 0; i < all.count(); i++)
	{
		if (all[i] >= 0) all[i] = (long long int)InvSite[all[i]];
	}
	for (i = 0; i < count; i++)
	{
		for (j = 0; j < degeneracy[i]; j++)
		{
			rules[prefix[i] + j].symbol = i;
		}
	}
	for (i = 0; i < rules.count(); i++)
	{
		Count_ = 0;
		for (j = rules[i].begin; j < rules[i].begin + rules[i].length; j++)
		{
			if (all[j] != epsilon) Count_ += 1;
		}
		rules[i].LengthWithoutVoid = Count_;
	}
	//return 0;
	FirstBiuld();
	//return 0;
	FollowBiuld();
	ERROR = done;
	return (int)done;
}
void grammerS::SetExample(void)
{
	size_t i;
	count = 3; // 非终结符数量
	TerminalCount = 5; // 终结符数量

	// 设置非终结符的名称
	append("E");
	append("T");
	append("F");
	
	// 设置终结符的名称


	Tappend("+");//-1
	Tappend("*");//-2
	Tappend("(");//-3
	Tappend(")");//-4
	Tappend("id");//-5

	Tappend("epsilon");//-6
	Tappend("END-EOF");//-7
	// 设置每个非终结符的 degeneracy

	epsilon = TerminalCount;
	epsilon = -epsilon - 1;
	end = epsilon - 1;

	degeneracy.append(2); 
	degeneracy.append(2);
	degeneracy.append(2);

	size_t pp;
	pp = 0;
	for (i = 0; i < degeneracy.count(); i++)
	{
		prefix.append(pp);
		pp += degeneracy[i];
	}

	// 设置产生式规则
	production PP;
	PP.begin = 0;
	all.append(0);
	all.append(-1);
	all.append(1);
	PP.length = 3;
	rules.append(PP);

}
void grammerS::Demo(FILE* fp) const
{
	size_t i, j, k;
	//size_t site;
	long long int index;
	//long long int  *R;
	production PP;
	fprintf(fp, "error code: %d\n", (int)ERROR);
	fprintf(fp, "count: %zu\n", count);
	fprintf(fp, "TerminalCount: %zu\n", TerminalCount);
	for (i = 0; i < TerminalCount; i++)
	{
		fprintf(fp, "\tNo[%zu] terminal symbol: %s\n", i, ternimal[i]);
	}
	for (i = 0; i < count; i++)
	{
		fprintf(fp, "%s: prefix: %zu, degeneracy: %zu\n", name[i], prefix[i], degeneracy[i]);
		for (j = 0; j < degeneracy[i]; j++)
		{
			PP = rules[prefix[i] + j];
			fprintf(fp, "\tNo[%zu] production rules: %s->", j, name[PP.symbol]);
			//PP = rules[prefix[i] + j];
			for (k = 0; k < PP.length; k++)
			{
				index = all[PP.begin + k]; 
				if (index >= 0)
					fprintf(fp, "%s ", name[index]);
				else
					fprintf(fp, "%s ", ternimal[-index - 1]);
			}
			fprintf(fp, "\n");
		}
	}
	//return;
	for (i = 0; i < count; i++)
	{
		fprintf(fp, "No[%zu] non-terminal symbol: %s.\n\tFIRST: ", i, name[i]);
		for (j = 0; j < TerminalCount + 2; j++)
			if(first[i][j]) 
				fprintf(fp, "%s  ", ternimal[j]);
		fprintf(fp, "\n\t FOLLOW: ");
		for (j = 0; j < TerminalCount + 2; j++)
			if (follow[i][j])
				fprintf(fp, "%s  ", ternimal[j]);
		fprintf(fp, "\n");
	}
}
void grammerS::Cprint(FILE* fp) const
{
	size_t i, j, k;
	//size_t site;
	long long int index;
	//long long int  *R;
	production PP;
	fprintf(fp, "\tswitch(information)\n\t{\n");
	for (i = 0; i < count; i++)//
	{
		fprintf(fp, "\t\t//No[%zu], case %s: prefix: %zu, degeneracy: %zu\n", i, name[i], prefix[i], degeneracy[i]);
		for (j = 0; j < degeneracy[i]; j++)
		{
			PP = rules[prefix[i] + j];
			fprintf(fp, "\t\tcase %zu: //%zu: %s, ", prefix[i] + j, i, name[i]);
			fprintf(fp, "No[%zu] production rules: %s->", j, name[PP.symbol]);
			//PP = rules[prefix[i] + j];
			for (k = 0; k < PP.length; k++)
			{
				index = all[PP.begin + k];
				if (index >= 0)
					fprintf(fp, "%s ", name[index]);
				else
					fprintf(fp, "%s ", ternimal[-index - 1]);
			}
			fprintf(fp, "\n\t\t{\n\t\t}\n\t\tbreak;\n");
		}
	}
	fprintf(fp, "\t}\n");
	//return;
}
void grammerS::Demo(FILE* fp, size_t rule) const
{
	production PP;
	size_t i;
	long long int index;
	PP = rules[rule];
	fprintf(fp, "%s->", name[PP.symbol]);
	for (i = 0; i < PP.length; i++)
	{
		index = all[PP.begin + i];
		if (index >= 0)
			fprintf(fp, "%s ", name[index]);
		else
			fprintf(fp, "%s ", ternimal[-index - 1]);
	}
}
void grammerS::Demo(FILE* fp, size_t rule, size_t dot) const
{
	production PP;
	size_t i;
	long long int index;
	PP = rules[rule];
	fprintf(fp, "%s->",name[PP.symbol]);
	//if ((PP.length == 1) && all[PP.begin] == epsilon)
	//{
	//	fprintf(fp, "@");
	//}
	//else
	{
		for (i = 0; i < PP.length; i++)
		{
			if (i == dot)
				fprintf(fp, "@ ");
			index = all[PP.begin + i];
			if (index >= 0)
				fprintf(fp, "%s ", name[index]);
			else if(index == epsilon)
				fprintf(fp, "(void:null) ");
			else
				fprintf(fp, "%s ", ternimal[-index - 1]);
		}
		if (dot == PP.length)
			fprintf(fp, "@");
	}
}
const char* grammerS::SymbolGet(long long int index) const
{
	if (index >= 0) return name[index];
	else return ternimal[-index - 1];
}
const long long int* grammerS::vector(size_t No)const
{
	return all.vector(rules[No].begin);
}
size_t grammerS::SearchAppend(const char* input)
{
	size_t i;
	for (i = 0; i < name.count(); i++)
		if (compare(input, name[i])) return i;
	append(input);
	return i;
}
long long int grammerS::SearchTerminal(const char* input)
{
	long long int i, length;
	length = ternimal.count();
	for (i = 0; i < length; i++)
		if (compare(input, ternimal[i])) return -i - 1;
	return 0;
}
void grammerS::append(const char* input)
{
	char* nnnn;
	size_t length, i;
	for (length = 0; input[length] != '\0'; length++);
	length += 1;
	nnnn = (char*)malloc(sizeof(char) * length);
	for (i = 0; i < length; i++) nnnn[i] = input[i];
	name.append(nnnn);
}
void grammerS::Tappend(const char* input)
{
	char* nnnn;
	size_t length, i;
	for (length = 0; input[length] != '\0'; length++);
	length += 1;
	nnnn = (char*)malloc(sizeof(char) * length);
	for (i = 0; i < length; i++) nnnn[i] = input[i];
	ternimal.append(nnnn);
}

void grammerS::FirstBiuld(void)
{
	size_t i, j, k;
	//size_t l;
	production PP;
	//long long int temp; 
	long long int * vect;
	bool change;
	first.refresh(count, TerminalCount + 2, count);
	first.value(false);
	//fprintf(stdout, "first.row(): %zu, first.column(): %zu,  first.length(): %zu.\n", first.row(), first.column(), first.length());
	//for (i = 0; i < count; i++)
	//{
	//	for (j = 0; j < degeneracy[i]; j++)
	//	{
	//		k = prefix[i] + j;
	//		PP = rules[k];
	//		temp = all[PP.begin];
			//fprintf(stdout, "i: %zu, j: %zu, k: %zu, temp: %lld, first[i]: %zu\n", i, j, k, temp, (size_t)first[i]);
	//		if (temp < (long long int)0) first[i][(size_t)(-temp - 1)] = true;
	//	}
	//}
	//return;
	do
	{
		change = false;
		for (i = 0; i < count; i++)
		{
			for (j = 0; j < degeneracy[i]; j++)
			{
				k = prefix[i] + j;
				PP = rules[k];
				vect = all.vector(PP.begin);
				change = change || AddFirstBeta(first[i], PP.length, vect);
				//change = change || FirstOr(vect[0], i);
				//change = change || Or(vect[0], first[i]);
				//for (l = 1; l < PP.length; l++)
				//{
				//	if (IfEpsilon(vect[l - 1]))
						//change = change || FirstOr(vect[l], i);
				//		change = change || Or(vect[l], first[i]);
				//	else break;
				//}
				//if (l == PP.length && IfEpsilon(vect[l - 1]))
				//{
				//	change = change || !first[i][TerminalCount];
				//	first[i][TerminalCount] = true;
				//}
			}
		}
	} while (change);

}
bool grammerS::IfEpsilon(long long int site) const
{
	if (site > 0) return first[site][TerminalCount];
	else return (site == epsilon);
}
bool grammerS::FirstOr(long long int symbol, bool* to) const
{
	//size_t i;
	bool change;
	change = false;
	if (symbol < 0)//symbol is terminal
	{
		if (symbol != epsilon)
		{
			change = !to[-symbol - 1];
			to[-symbol - 1] = true;
		}
	}
	else//symbol is non-terminal
		change = Or(first[symbol], to);
	return change;
}
bool grammerS::Or(const bool* from, bool* to) const
{
	size_t i;
	bool change;
	change = false;
	for (i = 0; i < TerminalCount; i++)
	{
		change = change || (from[i] && !to[i]);
		to[i] = to[i] || from[i];
	}
	i = TerminalCount + 1;
	change = change || (from[i] && !to[i]);
	to[i] = to[i] || from[i];
	return change;
}
bool grammerS::FollowOr(const bool* from, size_t to)
{
	size_t i;
	bool change;
	change = false;
	for (i = 0; i < TerminalCount + 2; i++)
	{
		change = change || (from[i] && !follow[to][i]);
		follow[to][i] = follow[to][i] || from[i];
	}
	return change;
}
bool grammerS::AddFirstBeta(bool* result, size_t L, const long long int* in) const
{
	size_t i;
	bool change;
	change = FirstOr(in[0], result);
	for (i = 1; i < L; i++)
	{
		if (IfEpsilon(in[i - 1]))
			change = change || FirstOr(in[i], result);
		else break;
	}
	if (i == L && IfEpsilon(in[i - 1]))
	{
		change = change || !result[TerminalCount];
		result[TerminalCount] = true;
	}
	return change;
}
void grammerS::FirstBeta(bool* result, size_t L, const long long int* in) const
{
	size_t i;
	//bool change;
	for (i = 0; i < TerminalCount + 2; i++) result[i] = false;
	AddFirstBeta(result, L, in);
}
void grammerS::FollowBiuld(void)
{
	size_t i, j, k, l;
	production PP;
	list<bool> FB;
	//long long int temp;
	long long int* vect;
	bool change;

	follow.refresh(count, TerminalCount + 2, count);
	FB.refresh(TerminalCount + 2);
	follow.value(false);
	follow[0][TerminalCount + 1] = true;
	//return;
	do
	{
		change = false;
		for (i = 0; i < count; i++)
		{
			for (j = 0; j < degeneracy[i]; j++)
			{
				k = prefix[i] + j;
				PP = rules[k];
				vect = all.vector(PP.begin);
				if (vect[PP.length - 1] >= 0)
					change = change || FollowOr(follow[i], vect[PP.length - 1]);
				for (l = 0; l < PP.length - 1; l++)
				{
					if (vect[l] < (long long int)0) continue;
					FirstBeta(FB.vector(), PP.length - 1 - l, vect + l + 1);
					if (FB[TerminalCount] == true)
					{
						change = change || FollowOr(follow[i], vect[l]);
					}
					change = change || Or(FB.vector(), follow[vect[l]]);
				}
			}
		}
	} while (change);
}

LR0::LR0(const grammerS* G)
{
	list<size_t> RulesPrefix;
	list<size_t> RulesCount;
	size_t prefix_, count__, length__;
	size_t i, j;
	const long long int* vect;
	//size_t k, l, NonT;
	size_t k, l;
	prefix_ = 0;
	
	for (i = 0; i < G->rules.count(); i++)
	{
		length__ = G->rules[i].length;
		vect = G->vector(i);
		//if (length__ == 1 && vect[0] == G->epsilon)
		//	count__ = 1;
		//else count__ = length__ + 1;
		count__ = length__ + 1;
		RulesPrefix.append(prefix_);
		prefix_ += count__;
		RulesCount.append(count__);
	}
	graph.refresh(prefix_);
	for (i = 0; i < G->rules.count(); i++)
	{
		prefix_ = RulesPrefix[i];
		count__ = RulesCount[i];
		vect = G->vector(i);
		for (j = 0; j + 1 < count__; j++)
		//for (j = 0; j < count__; j++)
		{
			graph[prefix_ + j].dot = j;
			graph[prefix_ + j].rules = i;
			graph.append(prefix_ + j, prefix_ + j + 1, vect[j] == G->epsilon, vect[j]);
			if (vect[j] < (long long int)0) continue;
			//NonT = vect[j];
			for (k = 0; k < G->degeneracy[vect[j]]; k++)
			{
				l = G->prefix[vect[j]] + k;
				graph.append(prefix_ + j, RulesPrefix[l], true, G->epsilon);
				//if(RulesCount[l]==1) 
				//	graph.append(prefix_ + j, prefix_ + j + 1, true, G->epsilon);
			}
		}
		graph[prefix_ + count__ - 1].dot = j;
		graph[prefix_ + count__ - 1].rules = i;
	}


	graph.build(sheet, Dstates, G->count, G->TerminalCount);
	TerminalCount = G->TerminalCount;
	NonTerminalCount = G->count;
}
//void LR0::build(const grammerS* G)
//{
	//item aa;
	//aa.dot = 1;
	//aa.rules = 2;
	//graph.vertice.SearchAppend(aa);
//s}
LR0::~LR0()
{
}
void LR0::Demo(FILE* fp, const grammerS* G)
{
	size_t i, j, now, count;
	item Item;
	long long int index;
	fprintf(fp, "Items count: vertice.count() = %zu\n", graph.vertice.count());
	fprintf(fp, "arcs.count() = %zu\n", graph.arcs.count());
	for (i = 0; i < graph.vertice.count(); i++)
	{
		Item = graph[i];
		fprintf(fp, "No[%5zu] item, rule: %5zu, dot: %5zu, arc count: %zu:", i, Item.rules, Item.dot, graph.ArcCount(i));
		G->Demo(fp, Item.rules, Item.dot);
		fprintf(fp, "\n");
		now = graph.vertice[i].first;
		while (now != SizeMax)
		{
			if (graph.arcs[now].epsilon)
			{
				fprintf(fp, "\t symbol: epsilon, to: %5zu: ", graph.arcs[now].to);
			}
			else
			{
				fprintf(fp, "\t symbol: %s, to: %5zu: ", G->SymbolGet(graph.arcs[now].symbol), graph.arcs[now].to);
			}
			Item = graph[graph.arcs[now].to];
			G->Demo(fp, Item.rules, Item.dot);
			fprintf(fp, "\n");
			now = graph.NextArc(now);
		}
	}

	
	fprintf(fp, "Closure count: %zu, sheet row: %zu\n", Dstates.row(), sheet.row());
	for (i = 0; i < Dstates.row(); i++)
	{
		count = 0;
		for (j = 0; j < Dstates.column(); j++)
			count += Dstates[i][j] ? 1 : 0;
		fprintf(fp, "No[%5zu] closure, items count: %zu, ", i, count);
		count = 0;
		for (j = 0; j < sheet.column(); j++)
			count += sheet[i][j] !=  -1 ? 1 : 0;
		fprintf(fp, "convert count: %zu\n", count);
		count = 0;
		for (j = 0; j < Dstates.column(); j++)
			if (Dstates[i][j])
			{
				fprintf(fp, "\tNo[%5zu] item = %5zu ", count, j);
				Item = graph[j];
				G->Demo(fp, Item.rules, Item.dot);
				fprintf(fp, "\n");
				count += 1;
			}
		count = 0;
		for (j = 0; j < sheet.column(); j++)
			if (sheet[i][j] != -1)
			{
				index =  j < G->count ? j : -(long long int)(j + 1 - G->count);
				fprintf(fp, "\t\tNo[%5zu] index = %5lld, symbol: %s, to: %d", count, index, G->SymbolGet(index), sheet[i][j]);
				fprintf(fp, "\n");
				count += 1;
			}
	}
}

LR1::LR1(const grammerS* G)
{
	list<size_t> stack;
	size_t now, RulesLength_, NewV, i, j;
	item I, To, Default;
	const long long int* List_;
	long long int Symbol__;
	bool IfAppend;

	size_t firstRules, countRules;
	list<bool> symbolT;
	list<long long int> beta__;

	I.dot = 0;
	I.rules = 0;
	I.symbol = G->end;
	now = 0;
	graph.VortexAppend(I);
	stack.append(now);

	symbolT.refresh(G->TerminalCount + 2);

	while (stack.pop(now) != 0)
	{
		//Demo(stdout, now, G);
		I = graph[now];
		RulesLength_ = G->rules[I.rules].length;
		List_ = G->vector(I.rules);
		if (I.dot == RulesLength_) continue;
		//if ((List_[0] == G->epsilon) && (RulesLength_ == 1)) continue;
		To.dot = I.dot + 1;
		To.rules = I.rules;
		To.symbol = I.symbol;
		NewV = SearchAppend(To, IfAppend);
		if (IfAppend) stack.append(NewV);
		Symbol__ = List_[I.dot];
		//graph.append(now, NewV, false, Symbol__);
		graph.append(now, NewV, Symbol__ == G->epsilon, Symbol__);
		//compute FIRST(beta symbol)
		beta__.refresh();
		for (i = I.dot + 1; i < RulesLength_; i++)
			beta__.append(List_[i]);
		beta__.append(I.symbol);
		G->FirstBeta(symbolT.vector(), beta__.count(), beta__.vector());
		//add closure
		if (Symbol__ >= 0)
		{
			firstRules = G->prefix[Symbol__];
			countRules = G->degeneracy[Symbol__];
			for (i = firstRules; i < firstRules + countRules; i++)
			{
				Default.dot = 0;
				Default.rules = i;
				for (j = 0; j < symbolT.count(); j++)
				{
					if (symbolT[j])
					{
						Default.symbol = -(long long int)(j + 1);
						NewV = SearchAppend(Default, IfAppend);
						if (IfAppend) stack.append(NewV);
						graph.append(now, NewV, true, G->epsilon);
					}
				}
				//if (G->IfaRuleEpsilon(i))
				//{
				//	NewV = SearchAppend(To, IfAppend);
				//	graph.append(now, NewV, true, G->epsilon);
				//}
			}
		}
		
	}

	graph.build(sheet, Dstates, G->count, G->TerminalCount);

	TerminalCount = G->TerminalCount;
	NonTerminalCount = G->count;
}
LR1::~LR1()
{

}
void LR1::Demo(FILE* fp, size_t site, const grammerS* G)
{
	item Item;
	Item = graph[site];
	fprintf(fp, "item = %5zu, rule: %5zu, dot: %5zu, ", site, Item.rules, Item.dot);
	fprintf(fp, "symbol: %5lld, arc count: %zu:[", Item.symbol, graph.ArcCount(site));
	G->Demo(fp, Item.rules, Item.dot);
	fprintf(fp, ", %s]\n", G->SymbolGet(Item.symbol));
}
void LR1::Demo(FILE* fp, const grammerS* G)
{
	//size_t i, j, RR, now, count;
	size_t i, j, now, count;
	//item Item;
	long long int index;
	fprintf(fp, "Items count: vertice.count() = %zu\n", graph.vertice.count());
	fprintf(fp, "arcs.count() = %zu\n", graph.arcs.count());
	for (i = 0; i < graph.vertice.count(); i++)
	{
		Demo(fp, i, G);
		now = graph.vertice[i].first;
		while (now != SizeMax)
		{
			if (graph.arcs[now].epsilon)
			{
				fprintf(fp, "\t symbol: epsilon, to: %5zu: ", graph.arcs[now].to);
			}
			else
			{
				fprintf(fp, "\t symbol: %s, to: %5zu: ", G->SymbolGet(graph.arcs[now].symbol), graph.arcs[now].to);
			}
			//Item = graph[graph.arcs[now].to];
			Demo(fp, graph.arcs[now].to, G);
			//G->Demo(fp, Item.rules, Item.dot);
			//fprintf(fp, "\n");
			now = graph.NextArc(now);
		}
		fprintf(fp, "\n");
	}

	fprintf(fp, "Closure count: %zu, sheet row: %zu\n", Dstates.row(), sheet.row());
	for (i = 0; i < Dstates.row(); i++)
	{
		count = 0;
		for (j = 0; j < Dstates.column(); j++)
			count += Dstates[i][j] ? 1 : 0;
		fprintf(fp, "No[%5zu] closure, items count: %zu, ", i, count);
		count = 0;
		for (j = 0; j < sheet.column(); j++)
			count += sheet[i][j] != -1 ? 1 : 0;
		fprintf(fp, "convert count: %zu\n", count);
		count = 0;
		for (j = 0; j < Dstates.column(); j++)
			if (Dstates[i][j])
			{
				fprintf(fp, "\tNo[%5zu] item = %5zu ", count, j);
				Demo(fp, j, G);
				//Item = graph[j];
				//G->Demo(fp, Item.rules, Item.dot);
				//fprintf(fp, "\n");
				count += 1;
			}
		count = 0;
		for (j = 0; j < sheet.column(); j++)
			if (sheet[i][j] != -1)
			{
				index = j < G->count ? j : -(long long int)(j + 1 - G->count);
				fprintf(fp, "\t\tNo[%5zu] index = %5lld, symbol: %s, to: %d", count, index, G->SymbolGet(index), sheet[i][j]);
				fprintf(fp, "\n");
				count += 1;
			}
	}

}
bool LR1::item::operator==(const LR1::item R) const
{
	return (rules == R.rules) && (dot == R.dot) && (symbol == R.symbol);
};
size_t LR1::SearchAppend(const LR1::item & I, bool & IfAppend)
{
	size_t i, length;
	item temp;
	//bool middle;
	length = graph.vertice.count();
	IfAppend = false;
	for (i = 0; i < length; i++)
	{
		temp = graph.vertice[i].content;
		if (temp == I) return i;
	}
	IfAppend = true;
	graph.VortexAppend(I);
	return length;
}

Gsheet::Gsheet()
{
	StateCount = 0;
	TerminalCount = 0;
	NonTerminalCount = 0;
	ACTION = NULL;
	GOTO = NULL;

	ET = uninitialized;

	RulesLength = NULL;
	RulesToSmbol = NULL;

	EI.ActionNew = error;
	EI.ActionOld = error;
	EI.i = (size_t)0;
	EI.j = (size_t)0;
	EI.StateNew = 0;
	EI.StateOld = 0;
}
Gsheet::~Gsheet()
{
	StateCount = 0;
	TerminalCount = 0;
	NonTerminalCount = 0;
	free(ACTION);
	free(GOTO);

	free(RulesLength);
	free(RulesToSmbol);
}
void Gsheet::build(const LR0* lr, const grammerS* G)
{
	size_t i, j, k, column; 
	LR0::item II; 
	grammerS::production PP;
	const matlist<bool>& Dstates = lr->Dstates;
	//const matlist<int>& sheet = lr->sheet;
	const bool* followLine;
	int ErrorTemp;
	bool temp;
	//const LRNFA<LR0::item>& graph = lr->graph;
	//push setting
	//ET = Conflict;
	ErrorTemp = build(lr->sheet, lr->Dstates, G);
	if (ErrorTemp != 0) return;
	//reduce setting
	column = Dstates.column();
	for (i = 0; i < StateCount; i++)
		for (j = 0; j < column; j++)
		{
			if (Dstates[i][j] == true)
			{
				II = lr->graph[j];
				PP = G->rules[II.rules];
				if (II.dot == PP.length)
				{
					if (II.rules == 0 && II.dot == 1)
					{//Sp->S@
						temp = actionAdd(i, TerminalCount, accept, 0);
						if (!temp) return;
					}
					else
					{
						followLine = G->follow[PP.symbol];
						for (k = 0; k < TerminalCount; k++)
						{
							if (followLine[k])
							{
								temp = actionAdd(i, k, reduce, (int)(II.rules));
								if (!temp) return;
							}
						}
						if (followLine[TerminalCount + 1])
						{
							temp = actionAdd(i, TerminalCount, reduce, (int)(II.rules));
							if (!temp) return;
						}
					}
				}
			}
		}
	ET = NoError;

	build();
	return;
}
void Gsheet::build(const LR1* lr, const grammerS* G)
{
	size_t i, j, column;
	LR1::item II;
	grammerS::production PP;
	const matlist<bool>& Dstates = lr->Dstates;
	//const matlist<int>& sheet = lr->sheet;
	//const bool* followLine;
	int ErrorTemp;
	bool temp;
	//const LRNFA<LR1::item>& graph = lr->graph;
	//push setting
	//ET = Conflict;
	ErrorTemp = build(lr->sheet, lr->Dstates, G);
	if (ErrorTemp != 0) return;
	column = Dstates.column();
	for (i = 0; i < StateCount; i++)
		for (j = 0; j < column; j++)
		{
			if (Dstates[i][j] == true)
			{
				II = lr->graph[j];
				PP = G->rules[II.rules];
				if (II.dot == PP.length)
				{
					if (II.symbol == G->end)
					{
						if (II.rules == 0 && II.dot == 1)
						{
							temp = actionAdd(i, TerminalCount, accept, (int)(II.rules));
						}
						else
						{
							temp = actionAdd(i, TerminalCount, reduce, (int)(II.rules));
						}
					}
					else 
					{
						temp = actionAdd(i, (size_t)(-II.symbol - 1), reduce, II.rules);
					}
					if (!temp) return;
				}
			}
		}

	build();

	ET = NoError;
}
int Gsheet::build(const matlist<int>& sheet, const matlist<bool>& Dstates, const grammerS* G)
{
	size_t i, j;
	bool temp;
	TerminalCount = G->TerminalCount;
	NonTerminalCount = G->count;
	
	RulesCount = G->rules.count();
	RulesToSmbol = (int*)malloc(sizeof(int) * RulesCount);
	RulesLength = (int*)malloc(sizeof(int) * RulesCount);
	for (i = 0; i < RulesCount; i++)
	{
		RulesToSmbol[i] = (int)G->rules[i].symbol;
		RulesLength[i] = (int)G->rules[i].length;
	}

	
	StateCount = Dstates.row();

	ACTION = (infor*)malloc(sizeof(infor) * StateCount * (TerminalCount + 1));
	GOTO = (infor*)malloc(sizeof(infor) * StateCount * NonTerminalCount);

	for (i = 0; i < StateCount; i++)
		for (j = 0; j < NonTerminalCount; j++)
		{
			GOTO[i * NonTerminalCount + j].action = error;
			GOTO[i * NonTerminalCount + j].state = -1;
		}
	for (i = 0; i < StateCount * (TerminalCount + 1); i++)
	{
		ACTION[i].action = error;
		ACTION[i].state = -1;
	}
	for (i = 0; i < StateCount; i++)
		for (j = 0; j < NonTerminalCount; j++)
		{
			if (sheet[i][j] != -1)
			{
				temp = gotoAdd(i, j, push, sheet[i][j]);
				if (!temp) return 1;
			}
		}
	for (i = 0; i < StateCount; i++)
		for (j = 0; j < TerminalCount; j++)
		{
			if (sheet[i][j + NonTerminalCount] != -1)
			{
				temp = actionAdd(i, j, push, sheet[i][j + NonTerminalCount]);
				if (!temp) return 2;
			}
		}
	return 0;
}
void Gsheet::build(void)
{
	size_t i, j;
	for (i = 0; i < StateCount; i++)
		for (j = 0; j < NonTerminalCount; j++)
		{
			if(GOTO[i * NonTerminalCount + j].action == error)
				GOTO[i * NonTerminalCount + j].state += 1;
		}
	for (i = 0; i < StateCount * (TerminalCount + 1); i++)
	{
		if(ACTION[i].action == error)
			ACTION[i].state += 1;
	}
	build_singleErrorAdd();
}
void Gsheet::build_singleErrorAdd(void)
{
	size_t i, j, count, record;
	for (i = 0; i < StateCount; i++)
	{
		count = 0; 
		record = 0;
		for (j = 0; j < (TerminalCount + 1); j++)
		{
			if (ACTION[i * (TerminalCount + 1) + j].action == error)
			{
				count += 1;
			}
			else record = j;
		}
		if (record == TerminalCount) record = 0;
		else record = record + 1;
		if(count == TerminalCount)
		{ 
			for (j = 0; j < (TerminalCount + 1); j++)
			{
				if (ACTION[i * (TerminalCount + 1) + j].action == error)
				{
					ACTION[i * (TerminalCount + 1) + j].state = (int)record;
				}
			}

		}
	}
}
bool Gsheet::gotoAdd(size_t i, size_t j, Gsheet::type Action, int State)
{
	bool temp;
	size_t ld;
	ld = NonTerminalCount;
	temp = (GOTO[i * ld + j].action == error);
	temp = temp && (GOTO[i * ld + j].state == -1);
	if (temp)
	{
		GOTO[i * ld + j].action = Action;
		GOTO[i * ld + j].state = State;
	}
	else
	{
		EI.i = i;
		EI.j = j;
		EI.ActionOld = GOTO[i * ld + j].action;
		EI.StateOld = GOTO[i * ld + j].state;
		EI.ActionNew = Action;
		EI.StateNew = State;
		ET = ConflictGOTO;
	}
	return temp;
}
bool Gsheet::actionAdd(size_t i, size_t j, Gsheet::type Action, int State)
{
	bool temp;
	size_t ld;
	ld = (TerminalCount + 1);
	temp = ACTION[i * ld + j].action == error;
	temp = temp && ACTION[i * ld + j].state == -1;
	if (temp)
	{
		ACTION[i * ld + j].action = Action;
		ACTION[i * ld + j].state = State;
	}
	else
	{
		EI.i = i;
		EI.j = j;
		EI.ActionOld = ACTION[i * ld + j].action;
		EI.StateOld = ACTION[i * ld + j].state;
		EI.ActionNew = Action;
		EI.StateNew = State;
		ET = ConflictACTION;
	}
	return temp;
}
bool Gsheet::gotoCheck(size_t i, size_t j)
{
	return (GOTO[i * NonTerminalCount + j].action == error) && (GOTO[i * NonTerminalCount + j].state == -1);
}
bool Gsheet::actionCheck(size_t i, size_t j)
{
	bool temp;
	temp = ACTION[i * (TerminalCount + 1) + j].action == error;
	temp = temp && ACTION[i * (TerminalCount + 1) + j].state == -1;
	return temp;
}
void Gsheet::Demo(FILE* fp, const LR0* lr, const grammerS* G)const
{
	Demo01(fp);
	if (ET == ConflictACTION || ET == ConflictGOTO)
	{
		Demo02(fp);
		return;
	}
	if (ET == NoError)
		Demo03(fp);
}
void Gsheet::Demo(FILE* fp, const LR1* lr, const grammerS* G)const
{
	Demo01(fp);
	if (ET == ConflictACTION || ET == ConflictGOTO)
	{
		Demo02(fp);
		return;
	}
	if (ET == NoError)
		Demo03(fp);
}
void Gsheet::Demo(FILE* fp)const
{
	Demo01(fp);
	if (ET == ConflictACTION || ET == ConflictGOTO)
	{
		Demo02(fp);
		return;
	}
	if (ET == NoError)
		Demo03(fp);
}
void Gsheet::Demo01(FILE* fp)const
{
	fprintf(fp, "TerminalCount: %zu, NonTerminalCount: %zu\n", TerminalCount, NonTerminalCount);
	if (ET == uninitialized)
	{
		fprintf(fp, "Error type: uninitialized\n");
	}
}
void Gsheet::Demo02(FILE* fp)const
{
	if (ET == ConflictACTION) fprintf(fp, "Error type: ACTION Conflict\n");
	if (ET == ConflictGOTO) fprintf(fp, "Error type: GOTO Conflict\n");
	fprintf(fp, "Error site: (%zu, %zu).\n", EI.i, EI.j);
	fprintf(fp, "Former item: (%s, %d).\n", TypeToChar(EI.ActionOld), EI.StateOld);
	fprintf(fp, "New item: (%s, %d).\n", TypeToChar(EI.ActionNew), EI.StateNew);
}
void Gsheet::Demo03(FILE* fp)const
{
	size_t i, j;
	infor InF;
	fprintf(fp, "ACTION: \n");
	for (i = 0; i < StateCount; i++)
	{
		fprintf(fp, "[%4zu]: ", i);
		for (j = 0; j < TerminalCount + 1; j++)
		{
			InF = ACTION[i * (TerminalCount + 1) + j];
			fprintf(fp, "(%6s, %3d) ", TypeToChar(InF.action), InF.state);
		}
		fprintf(fp, "\n");
	}
	fprintf(fp, "GOTO: \n");
	for (i = 0; i < StateCount; i++)
	{
		fprintf(fp, "[%4zu]: ", i);
		for (j = 0; j < NonTerminalCount; j++)
		{
			InF = GOTO[i * NonTerminalCount + j];
			fprintf(fp, "(%6s, %3d) ", TypeToChar(InF.action), InF.state);
		}
		fprintf(fp, "\n");
	}
	for (i = 0; i < RulesCount; i++)
	{
		fprintf(fp, "rules[%4zu]: %d\n", i, RulesToSmbol[i]);
	}
}
const char* Gsheet::TypeToChar(Gsheet::type TT)
{
	switch (TT)
	{
	case hyperlex::Gsheet::accept:
		return "accept";
	case hyperlex::Gsheet::error:
		return "error";
	case hyperlex::Gsheet::push:
		return "push";
	case hyperlex::Gsheet::reduce:
		return "reduce";
	default:
		return "error";
	}
}
Gsheet::ErrorType Gsheet::ErrorTypeGet(void)
{
	return ET;
}
Gsheet::ErrorInfor Gsheet::ErrorInforGet(void)
{
	return EI;
}

Gsheet::infor Gsheet::Goto(size_t state, size_t symbol) const
{
	return GOTO[state * NonTerminalCount + symbol];
}
Gsheet::infor Gsheet::Action(size_t state, size_t symbol) const
{
	return ACTION[state * (TerminalCount + 1) + symbol];
}
int Gsheet::GetSymbol(size_t rules) const
{
	return RulesToSmbol[rules];
}
void Gsheet::Cprint(const char* name, FILE* fp)const
{
	size_t i, j;
	//size_t No;
	int infor;
	fprintf(fp, "int GOTO");
	if (name != NULL) fprintf(fp, "_%s", name);
	fprintf(fp, "(int* list)\n{\n");
	for (i = 0; i < StateCount; i++)
		for (j = 0; j < NonTerminalCount; j++)
		{
			infor = 4 * GOTO[i * NonTerminalCount + j].state;
			infor += (int)GOTO[i * NonTerminalCount + j].action;
			fprintf(fp, "\tlist[%zu] = %d;\n", i * NonTerminalCount + j, infor);
		}
	fprintf(fp, "}\n");
	//====================================================
	fprintf(fp, "int ACTION");
	if (name != NULL) fprintf(fp, "_%s", name);
	fprintf(fp, "(int* list)\n{\n");
	for (i = 0; i < StateCount; i++)
	{//put END-EOF on the first column, the remaining symbols are shifted backward by one position.
		infor = 4 * ACTION[i * (TerminalCount + 1) + TerminalCount].state;
		infor += (int)ACTION[i * (TerminalCount + 1) + TerminalCount].action;
		fprintf(fp, "\tlist[%zu] = %d;\n", i * (TerminalCount + 1), infor);
		for (j = 0; j < (TerminalCount + 0); j++)
		{
			infor = 4 * ACTION[i * (TerminalCount + 1) + j].state;
			infor += (int)ACTION[i * (TerminalCount + 1) + j].action;
			fprintf(fp, "\tlist[%zu] = %d;\n", i * (TerminalCount + 1) + j + 1, infor);
		}
		
	}
	fprintf(fp, "}\n");
	//====================================================
	fprintf(fp, "int RulesToSymbol");
	if (name != NULL) fprintf(fp, "_%s", name);
	fprintf(fp, "(int* list)\n{\n");
	for (i = 0; i < RulesCount; i++)
	{
		fprintf(fp, "\tlist[%zu] = %d;\n", i, RulesToSmbol[i]);
	}
	fprintf(fp, "}\n");
}
void Gsheet::CppPrint(const char* name, FILE* fp)const
{
	size_t i, j;
	//size_t No;
	int infor;
	fprintf(fp, "void %s::build()\n{\n", name);

	fprintf(fp, "\tStateCount = %zu;\n", StateCount);
	fprintf(fp, "\tNonTerminalCount = %zu;\n", NonTerminalCount);
	fprintf(fp, "\tTerminalCount = %zu;\n", TerminalCount);
	fprintf(fp, "\tRulesCount = %zu;\n", RulesCount);

	fprintf(fp, "\tGOTO = (int*)malloc(sizeof(int) * StateCount * NonTerminalCount);\n");
	for (i = 0; i < StateCount; i++)
		for (j = 0; j < NonTerminalCount; j++)
		{
			infor = 4 * GOTO[i * NonTerminalCount + j].state;
			infor += (int)GOTO[i * NonTerminalCount + j].action;
			fprintf(fp, "\tGOTO[%zu] = %d;\n", i * NonTerminalCount + j, infor);
		}
	fprintf(fp, "\t//==============================\n");//
	//====================================================
	fprintf(fp, "\tACTION = (int*)malloc(sizeof(int) * StateCount * (TerminalCount + 1));\n");
	for (i = 0; i < StateCount * (TerminalCount + 1); i++)
	{
		infor = 4 * ACTION[i].state;
		infor += (int)ACTION[i].action;
		fprintf(fp, "\tACTION[%zu] = %d;\n", i, infor);
	}
	fprintf(fp, "\t//==============================\n");//
	//====================================================
	fprintf(fp, "\tRulesToSymbol = (int*)malloc(sizeof(int) * RulesCount);\n");
	fprintf(fp, "\tRulesLength = (int*)malloc(sizeof(int) * RulesCount);\n");
	for (i = 0; i < RulesCount; i++)
	{
		fprintf(fp, "\tRulesToSymbol[%zu] = %d;\n", i, RulesToSmbol[i]);
		fprintf(fp, "\tRulesLength[%zu] = %d;\n", i, RulesLength[i]);
	}
	fprintf(fp, "}\n");
}
void Gsheet::CppStructPrint(const char* name, FILE* fp, const grammerS* grammer)const
{
	CppStructPrint01(name, fp, grammer);
	CppStructPrint02(name, fp, grammer);
}
void Gsheet::CppStructPrint01(const char* name, FILE* fp, const grammerS* grammer)const
{
	size_t i;
	size_t length;
	fprintf(fp, "struct %s\n{\n", name);

	fprintf(fp, "\tenum type\n\t{\n");
	fprintf(fp, "\t\taccept = %d,\n", (int)accept);
	fprintf(fp, "\t\terror = %d,\n", (int)error);
	fprintf(fp, "\t\tpush = %d,\n", (int)push);
	fprintf(fp, "\t\treduce = %d\n\t};\n", (int)reduce);

	//length = grammer->name.count();
	//fprintf(fp, "\tenum nonterminal\n\t{\n");
	//for (i = 1; i < length; i++)
	//	fprintf(fp, "\t\t_%s_ = %d,\n", grammer->name[i - 1], (int)(i - 1));
	//if (length != 0) fprintf(fp, "\t\t_%s_ = %d\n", grammer->name[length - 1], (int)(length - 1));
	//fprintf(fp, "\t};\n");
}
void Gsheet::CppStructPrint02(const char* name, FILE* fp, const grammerS* grammer)const
{
	size_t i, j;
	int infor;

	fprintf(fp, "\tstatic const size_t StateCount;\n");
	fprintf(fp, "\tstatic const size_t NonTerminalCount;\n");
	fprintf(fp, "\tstatic const size_t TerminalCount;\n");
	fprintf(fp, "\tstatic const size_t RulesCount;\n");

	fprintf(fp, "\tstatic const int GOTO[%zu][%zu];\n", StateCount, NonTerminalCount);
	fprintf(fp, "\tstatic const int ACTION[%zu][%zu];\n", StateCount, (TerminalCount + 1));
	fprintf(fp, "\tstatic const int RulesToSymbol[%zu];\n", RulesCount);
	fprintf(fp, "\tstatic const int RulesLength[%zu];\n", RulesCount);
	fprintf(fp, "\tstatic const char* const RulesName[%zu];\n", RulesCount);
	fprintf(fp, "\tstatic const int Implicit[%zu];\n", RulesCount);

	fprintf(fp, "};\n");

	fprintf(fp, "const size_t %s::StateCount = %zu;\n", name, StateCount);
	fprintf(fp, "const size_t %s::NonTerminalCount = %zu;\n", name, NonTerminalCount);
	fprintf(fp, "const size_t %s::TerminalCount = %zu;\n", name, TerminalCount);
	fprintf(fp, "const size_t %s::RulesCount = %zu;\n", name, RulesCount);

	fprintf(fp, "const int %s::GOTO[%zu][%zu] = { \\ \n", name, StateCount, NonTerminalCount);

	//fprintf(fp, "\tGOTO = (int*)malloc(sizeof(int) * StateCount * NonTerminalCount);\n");
	for (i = 0; i < StateCount; i++)
	{
		infor = 4 * GOTO[i * NonTerminalCount].state;
		infor += (int)GOTO[i * NonTerminalCount].action;
		fprintf(fp, "{%d", infor);
		for (j = 1; j < NonTerminalCount; j++)
		{
			infor = 4 * GOTO[i * NonTerminalCount + j].state;
			infor += (int)GOTO[i * NonTerminalCount + j].action;
			fprintf(fp, ", %d", infor);
		}
		if (i + 1 < StateCount) fprintf(fp, "}, \\\n");
		else  fprintf(fp, "}};\n");
	}

	fprintf(fp, "//==============================\n");//
	//====================================================
	fprintf(fp, "const int %s::ACTION[%zu][%zu] = { \\ \n", name, StateCount, (TerminalCount + 1));
	//fprintf(fp, "\tACTION = (int*)malloc(sizeof(int) * StateCount * (TerminalCount + 1));\n");
	for (i = 0; i < StateCount; i++)
	{
		infor = 4 * ACTION[i * (TerminalCount + 1) + TerminalCount].state;
		infor += (int)ACTION[i * (TerminalCount + 1) + TerminalCount].action;
		fprintf(fp, "{%d", infor);
		for (j = 0; j < (TerminalCount + 0); j++)
		{
			infor = 4 * ACTION[i * (TerminalCount + 1) + j].state;
			infor += (int)ACTION[i * (TerminalCount + 1) + j].action;
			fprintf(fp, ", %d", infor);
		}
		if (i + 1 < StateCount) fprintf(fp, "}, \\\n");
		else  fprintf(fp, "}};\n");
	}
	fprintf(fp, "//==============================\n");//
	//====================================================
	fprintf(fp, "const int %s::RulesToSymbol[%zu] = { \\ \n", name, RulesCount);
	for (i = 0; i < RulesCount; i++)
	{
		if (i + 1 < RulesCount) fprintf(fp, "%d,\\\n", RulesToSmbol[i]);
		else fprintf(fp, "%d};\n", RulesToSmbol[i]);
	}
	fprintf(fp, "//==============================\n");//

	fprintf(fp, "const int %s::RulesLength[%zu] = { \\ \n", name, RulesCount);
	for (i = 0; i < RulesCount; i++)
	{
		if (i + 1 < RulesCount) fprintf(fp, "%zu,\\\n", grammer->rules[i].LengthWithoutVoid);
		else fprintf(fp, "%zu};\n", grammer->rules[i].LengthWithoutVoid);
	}
	fprintf(fp, "//==============================\n");//
	fprintf(fp, "const char* const %s::RulesName[%zu] = { \\ \n\"", name, RulesCount);
	for (i = 1; i < RulesCount; i++)
	{
		grammer->Demo(fp, i - 1);
		fprintf(fp, "\",\\\n\"");
	}
	grammer->Demo(fp, RulesCount - 1);
	fprintf(fp, "\"};\n");
}


static char* CopyMalloc(const char* s)
{
	char* v;
	size_t size;
	size = strlength(s);
	v = (char*)malloc(sizeof(char) * (size + 4));
	for (size_t i = 0; i < size; i++) v[i] = s[i];
	v[size] = '\0';
	return v;
}
static void CopyNc(const char* src, char* dst, size_t length)
{
	for (size_t i = 0; i < length; i++) dst[i] = src[i];
}
static bool compare(const char* str1, const char* str2)
{
	size_t i;
	for (i = 0; (str1[i] != '\0') && (str1[i] == str2[i]); i++);
	return str1[i] == str2[i];
}
static void write_escaped_string(const char* str, FILE* file)
{
	if (!str || !file) return;

	fputc('\"', file);  // 字符常量起始引号

	for (int i = 0; str[i] != '\0'; i++) {
		switch (str[i]) {
			// 处理必须转义的特殊字符
		case '\"': fputs("\\\"", file); break;   // 双引号[6,8](@ref)
		case '\\': fputs("\\\\", file); break;   // 反斜杠[6,7](@ref)
		case '\n': fputs("\\n", file); break;    // 换行符[1,6](@ref)
		case '\t': fputs("\\t", file); break;    // 制表符[1,8](@ref)
		case '\v': fputs("\\v", file); break;    // 制表符[1,8](@ref)
		case '\r': fputs("\\r", file); break;    // 回车符[6,8](@ref)
		case '\b': fputs("\\b", file); break;    // 退格符[7,8](@ref)
		case '\f': fputs("\\f", file); break;    // 换页符[8](@ref)
		case '\a': fputs("\\a", file); break;    // 响铃符[8](@ref)
			// 其他不可打印字符用十六进制转义
		default:
			if (str[i] < 32 || str[i] > 126) {  // 非ASCII可打印字符
				fprintf(file, "\\x%02X", (unsigned char)str[i]);  // 十六进制转义[6](@ref)
			}
			else {
				fputc(str[i], file);  // 直接写入可打印字符
			}
		}
	}
	fputc('\"', file);  // 字符常量结束引号
}
static size_t strlength(const char* str)
{
	size_t i;
	for (i = 0; str[i] != '\0'; i++);
	return i;
}

static void strfree(const char** strs, size_t length)
{
	size_t i;
	for (i = 0; i < length; i++)
	{
		free((void*)strs[i]);
	}
	free(strs);
}
static void inverse(list<size_t>& out, const list<size_t>& in)
{
	size_t i, length;
	length = in.count();
	out.refresh(length);
	for (i = 0; i < length; i++)
		out[in[i]] = i;
}
static int PostfixSwitch_small(char c)
{
	switch (c)
	{
	/*case 'a':
		return 7;
	case 'b':
		return 8;
	case 'f':
		return 12;
	case 'n':
		return 10;
	case 'r':
		return 13;
	case 't':
		return 9;
	case 'v':
		return 11;*/
	case '\\':
		return 92;
	case '\'':
		return 39;
	case '\"':
		return 34;
	case '\?':
		return 63;
	case '\0':
		return 0;
	default:
		return -1;
	}
}



int hyperlex::PostfixSwitch(char c)
{
	switch (c)
	{
	case 'a':
		return 7;
	case 'b':
		return 8;
	case 'f':
		return 12;
	case 'n':
		return 10;
	case 'r':
		return 13;
	case 't':
		return 9;
	case 'v':
		return 11;
	case '\\':
		return 92;
	case '\'':
		return 39;
	case '\"':
		return 34;
	case '?':
		return 63;
	case '\0':
		return 0;
	default:
		return -1;
	}
}
bool hyperlex::IfHexa(char c)
{
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}
int hyperlex::SwitchHexa(char c)
{
	int e;
	if (c >= '0' && c <= '9')
		c -= '0';
	else if (c >= 'a' && c <= 'f')
		c = c - 'a' + (char)10;
	else
		c = c - 'A' + (char)10;
	e = (int)c;
	return e;
}
int hyperlex::SwitchOcta(char c)
{
	return c - '0';
}
char hyperlex::dequeue(const char* list, size_t end, size_t& head)
{
	char c;
	if (head < end)
	{
		c = list[head];
		head++;
	}
	else c = EOF;
	return c;
}
char hyperlex::CharGet(int& error, const char* list, size_t end, size_t& head)
{
	char c, result;
	int temp;
	//int state;
	char c1, c2;
	c = dequeue(list, end, head);
	error = 0;
	if (c != '\\') return c;
	//state = 0;
	c = dequeue(list, end, head);
	if (c == 'x' || c == 'X')
	{
		temp = -1;
		c1 = dequeue(list, end, head);
		if (IfHexa(c1))
		{
			temp = SwitchHexa(c1);
			c2 = dequeue(list, end, head);
			if (IfHexa(c2)) temp = 16 * temp + SwitchHexa(c2);
		}
		else
			error = -2;
	}
	else if('0' <= c && c <= '8')
	{
		temp = SwitchOcta(c);
		c1 = dequeue(list, end, head);
		if ('0' <= c1 && c1 <= '8')
		{
			temp = 8 * temp + SwitchOcta(c1);
			c2 = dequeue(list, end, head);
			if ('0' <= c2 && c2 <= '8') temp = 8 * temp + SwitchOcta(c2);
		}
	}
	else
	{
		temp = PostfixSwitch(c);
		if (temp == -1) error = -1;
	}
	result = (char)temp;
	return result;
}

long int hyperlex::IntGet(const char* list, size_t end, size_t& head)
{
	char c, state;
	long int number;
	long int Sign = 1;
	number = 0;
	state = 0;
	do
	{
		c = dequeue(list, end, head);
		switch (state)
		{
		case 0:
			if (c >= '0' && c <= '9') number = c - '0';
			else if (c == '-') Sign = -1;
			state = 1;
			break;
		case 1:
			if (c >= '0' && c <= '9') number = number * 10 + c - '0';
			else state = 2;
			break;
		}
	} while (state != 2);
	return number * Sign;
}
double hyperlex::RealGet(int& state, const char* list, size_t end, size_t& head)
{
	char c;
	double number, digit;
	int num_sign = 1;
	int index;
	int index_sign;
	number = 0.0;
	digit = 1.0;
	num_sign = 1.0;
	index_sign = 1;
	index = 0;
	state = 0;
	do
	{
		c = dequeue(list, end, head);
		switch (state)
		{
		case 0:
			if (c == '-') { num_sign = 0; state = 1; }
			else if (c == '+') { num_sign = 1; state = 1; }
			else if (c == '.') state = 2;
			else if (c >= '0' && c <= '9') { number = (double)(c - '0'); state = 3; }
			else state = -1;
			break;
		case 1:
			if (c >= '0' && c <= '9') { number = (double)(c - '0'); state = 3; }
			else if (c == '.') state = 2;
			else state = -1;
			break;
		case 2:
			if (c >= '0' && c <= '9')
			{
				digit *= 0.1;
				number = number + digit * (double)(c - '0');
				state = 4;
			}
			else if (c == 'e' || c == 'E') state = 5;
			else state = -1;
			break;
		case 3:
			if (c >= '0' && c <= '9') { number = number * 10.0 + (double)(c - '0'); state = 3; }
			else if (c == '.') state = 2;
			else state = -1;
			break;
		case 4:
			if (c >= '0' && c <= '9')
			{
				digit *= 0.1;
				number = number + digit * (double)(c - '0');
				state = 4;
			}
			else if (c == 'e' || c == 'E') state = 5;
			else state = -1;
			break;
		case 5:
			if (c == '-') { index_sign = -1; state = 6; }
			else if (c == '+') { index_sign = 1; state = 6; }
			else if (c >= '0' && c <= '9') { index = (int)(c - '0'); state = 7; }
			else state = -1;
			break;
		case 6:
			if (c >= '0' && c <= '9') { index = (int)(c - '0'); state = 7; }
			else state = -1;
			break;
		case 7:
			if (c >= '0' && c <= '9') { index = index * 10 + (int)(c - '0'); state = 7; }
			else state = -1;
			break;
		}
	} while (state != -1);
	number = num_sign ? number : -number;
	digit = (index_sign == 1 ? 10.0 : 0.1);
	while (index != 0)
	{
		index--;
		number *= digit;
	}
	return number;
}
char hyperlex::CharGet(const char* list, size_t end, size_t& head)
{
	char c, result;
	int error;
	result = EOF;
	c = dequeue(list, end, head);
	error = 10;
	if (c == '\'') result = CharGet(error, list, end, head);
	else return EOF;
	c = dequeue(list, end, head);
	if (c == '\'') return result;
	else return EOF;
}

static bool isIdBegin(char c)
{
	return (c >= 'a' && c <= 'z') || c == '_' || (c >= 'A' && c <= 'Z');
}
static bool isIdNumber(char c)
{
	return c >= '0' && c <= '9';
}

int Reg::next(int state, const char c)
{
	switch (state)
	{
	case 0:
		if (c == (char)9) return 11;
		else if (c == (char)10) return 10;
		else if (c == (char)13) return 30;
		else if (c == ' ') return 9;
		else if (c == '\'') return 45;
		else if (c == '(') return 17;
		else if (c == ')') return 18;
		else if (c == '*') return 26;
		else if (c == '+') return 27;
		else if (c == '-') return 25;
		else if (c == '.') return 14;
		else if (c == '/') return 53;
		else if ('0' <= c && c <= '9') return 4;
		else if (c == ':') return 13;
		else if (c == ';') return 12;
		else if (c == '<') return 21;
		else if (c == '>') return 22;
		else if (c == '\?') return 28;
		else if ('A' <= c && c <= 'Z') return 43;
		else if (c == '[') return 19;
		else if (c == ']') return 20;
		else if (c == '_') return 43;
		else if (c == 'a') return 48;
		else if ('b' <= c && c <= 'f') return 43;
		else if (c == 'g') return 50;
		else if ('h' <= c && c <= 'k') return 43;
		else if (c == 'l') return 47;
		else if ('m' <= c && c <= 'u') return 43;
		else if (c == 'v') return 49;
		else if ('w' <= c && c <= 'z') return 43;
		else if (c == '{') return 15;
		else if (c == '|') return 29;
		else if (c == '}') return 16;
		else return 0;
	case 1:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'z') return 1;
		else return 0;
	case 2:
		if ('0' <= c && c <= '9') return 2;
		else return 0;
	case 3:
		return 0;
	case 4:
		return 0;
	case 5:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'z') return 1;
		else return 0;
	case 6:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'z') return 1;
		else return 0;
	case 7:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'z') return 1;
		else return 0;
	case 8:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'z') return 1;
		else return 0;
	case 9:
		if (c == ' ') return 9;
		else return 0;
	case 10:
		if (c == (char)10) return 10;
		else if (c == (char)13) return 30;
		else return 0;
	case 11:
		return 0;
	case 12:
		return 0;
	case 13:
		return 0;
	case 14:
		return 0;
	case 15:
		return 0;
	case 16:
		return 0;
	case 17:
		return 0;
	case 18:
		return 0;
	case 19:
		return 0;
	case 20:
		return 0;
	case 21:
		return 0;
	case 22:
		return 0;
	case 23:
		if ((char)0 <= c && c <= (char)9) return 23;
		else if ((char)11 <= c && c <= (char)127) return 23;
		else return 0;
	case 24:
		return 0;
	case 25:
		return 0;
	case 26:
		return 0;
	case 27:
		if ('0' <= c && c <= '9') return 2;
		else return 0;
	case 28:
		return 0;
	case 29:
		return 0;
	case 30:
		if (c == (char)10) return 10;
		else return 0;
	case 31:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'c') return 1;
		else if (c == 'd') return 7;
		else if ('e' <= c && c <= 'z') return 1;
		else return 0;
	case 32:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'h') return 1;
		else if (c == 'i') return 31;
		else if ('j' <= c && c <= 'z') return 1;
		else return 0;
	case 33:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'k') return 1;
		else if (c == 'l') return 5;
		else if ('m' <= c && c <= 'z') return 1;
		else return 0;
	case 34:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if (c == 'a') return 33;
		else if ('b' <= c && c <= 'z') return 1;
		else return 0;
	case 35:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'b') return 1;
		else if (c == 'c') return 34;
		else if ('d' <= c && c <= 'z') return 1;
		else return 0;
	case 36:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'h') return 1;
		else if (c == 'i') return 35;
		else if ('j' <= c && c <= 'z') return 1;
		else return 0;
	case 37:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'q') return 1;
		else if (c == 'r') return 6;
		else if ('s' <= c && c <= 'z') return 1;
		else return 0;
	case 38:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if (c == 'a') return 37;
		else if ('b' <= c && c <= 'z') return 1;
		else return 0;
	case 39:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'l') return 1;
		else if (c == 'm') return 38;
		else if ('n' <= c && c <= 'z') return 1;
		else return 0;
	case 40:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'l') return 1;
		else if (c == 'm') return 39;
		else if ('n' <= c && c <= 'z') return 1;
		else return 0;
	case 41:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if (c == 'a') return 40;
		else if ('b' <= c && c <= 'z') return 1;
		else return 0;
	case 42:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'w') return 1;
		else if (c == 'x') return 36;
		else if ('y' <= c && c <= 'z') return 1;
		else return 0;
	case 43:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'z') return 1;
		else return 0;
	case 44:
		if ((char)0 <= c && c <= ')') return 44;
		else if (c == '*') return 52;
		else if ('+' <= c && c <= (char)127) return 44;
		else return 0;
	case 45:
		if (' ' <= c && c <= '!') return 54;
		else if ('#' <= c && c <= '&') return 54;
		else if ('(' <= c && c <= '[') return 54;
		else if (c == '\\') return 51;
		else if (']' <= c && c <= '~') return 54;
		else return 0;
	case 46:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'k') return 1;
		else if (c == 'l') return 8;
		else if ('m' <= c && c <= 'z') return 1;
		else return 0;
	case 47:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'd') return 1;
		else if (c == 'e') return 42;
		else if ('f' <= c && c <= 'z') return 1;
		else return 0;
	case 48:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'k') return 1;
		else if (c == 'l') return 46;
		else if ('m' <= c && c <= 'z') return 1;
		else return 0;
	case 49:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'n') return 1;
		else if (c == 'o') return 32;
		else if ('p' <= c && c <= 'z') return 1;
		else return 0;
	case 50:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'q') return 1;
		else if (c == 'r') return 41;
		else if ('s' <= c && c <= 'z') return 1;
		else return 0;
	case 51:
		if (c == (char)0) return 54;
		else if (c == '\"') return 54;
		else if (c == '\'') return 54;
		else if ('0' <= c && c <= '7') return 56;
		else if (c == '\?') return 54;
		else if (c == 'X') return 55;
		else if (c == '\\') return 54;
		else if ('a' <= c && c <= 'b') return 54;
		else if (c == 'f') return 54;
		else if (c == 'n') return 54;
		else if (c == 'r') return 54;
		else if (c == 't') return 54;
		else if (c == 'v') return 54;
		else if (c == 'x') return 55;
		else return 0;
	case 52:
		if ((char)0 <= c && c <= ')') return 44;
		else if (c == '*') return 52;
		else if ('+' <= c && c <= '.') return 44;
		else if (c == '/') return 24;
		else if ('0' <= c && c <= (char)127) return 44;
		else return 0;
	case 53:
		if (c == '*') return 44;
		else if (c == '/') return 23;
		else return 0;
	case 54:
		if (c == '\'') return 3;
		else return 0;
	case 55:
		if ('0' <= c && c <= '9') return 57;
		else if ('A' <= c && c <= 'F') return 57;
		else if ('a' <= c && c <= 'f') return 57;
		else return 0;
	case 56:
		if (c == '\'') return 3;
		else if ('0' <= c && c <= '7') return 58;
		else return 0;
	case 57:
		if (c == '\'') return 3;
		else if ('0' <= c && c <= '9') return 54;
		else if ('A' <= c && c <= 'F') return 54;
		else if ('a' <= c && c <= 'f') return 54;
		else return 0;
	case 58:
		if (c == '\'') return 3;
		else if ('0' <= c && c <= '7') return 54;
		else return 0;
	}
	return 0;
}
int Reg::action(int state)
{
	switch (state)
	{
	case 1:
		return 1;//identifier: identifier
	case 2:
		return 2;//const: integer
	case 3:
		return 3;//const: CommonChar
	case 4:
		return 4;//const: idChar
	case 5:
		return 5;//reserved: lexical
	case 6:
		return 6;//reserved: grammar
	case 7:
		return 7;//reserved: void
	case 8:
		return 8;//reserved: all
	case 9:
		return 9;//format: spaces
	case 10:
		return 10;//format: enters
	case 11:
		return 11;//format: tab
	case 12:
		return 12;//division: semicolon
	case 13:
		return 13;//division: colon
	case 14:
		return 14;//division: dot
	case 15:
		return 15;//braket: braceL
	case 16:
		return 16;//braket: braceR
	case 17:
		return 17;//braket: left
	case 18:
		return 18;//braket: right
	case 19:
		return 19;//braket: squareL
	case 20:
		return 20;//braket: squareR
	case 21:
		return 21;//braket: angleL
	case 22:
		return 22;//braket: angleR
	case 23:
		return 23;//anntation: anntationS
	case 24:
		return 24;//anntation: anntationM
	case 25:
		return 25;//RegSymbol: range
	case 26:
		return 26;//RegSymbol: star
	case 27:
		return 27;//RegSymbol: plus
	case 28:
		return 28;//RegSymbol: question
	case 29:
		return 29;//RegSymbol: or
	case 31:
		return 1;//identifier: identifier
	case 32:
		return 1;//identifier: identifier
	case 33:
		return 1;//identifier: identifier
	case 34:
		return 1;//identifier: identifier
	case 35:
		return 1;//identifier: identifier
	case 36:
		return 1;//identifier: identifier
	case 37:
		return 1;//identifier: identifier
	case 38:
		return 1;//identifier: identifier
	case 39:
		return 1;//identifier: identifier
	case 40:
		return 1;//identifier: identifier
	case 41:
		return 1;//identifier: identifier
	case 42:
		return 1;//identifier: identifier
	case 43:
		return 4;//const: idChar
	case 46:
		return 1;//identifier: identifier
	case 47:
		return 4;//const: idChar
	case 48:
		return 4;//const: idChar
	case 49:
		return 4;//const: idChar
	case 50:
		return 4;//const: idChar
	}
	return 0;
}
int Reg::GroupGet(int accept)
{
	switch (accept)
	{
	case 1:
		return 1;//identifier: identifier
	case 2:
		return 2;//const: integer
	case 3:
		return 2;//const: CommonChar
	case 4:
		return 2;//const: idChar
	case 5:
		return 3;//reserved: lexical
	case 6:
		return 3;//reserved: grammar
	case 7:
		return 3;//reserved: void
	case 8:
		return 3;//reserved: all
	case 9:
		return 4;//format: spaces
	case 10:
		return 4;//format: enters
	case 11:
		return 4;//format: tab
	case 12:
		return 5;//division: semicolon
	case 13:
		return 5;//division: colon
	case 14:
		return 5;//division: dot
	case 15:
		return 6;//braket: braceL
	case 16:
		return 6;//braket: braceR
	case 17:
		return 6;//braket: left
	case 18:
		return 6;//braket: right
	case 19:
		return 6;//braket: squareL
	case 20:
		return 6;//braket: squareR
	case 21:
		return 6;//braket: angleL
	case 22:
		return 6;//braket: angleR
	case 23:
		return 7;//anntation: anntationS
	case 24:
		return 7;//anntation: anntationM
	case 25:
		return 8;//RegSymbol: range
	case 26:
		return 8;//RegSymbol: star
	case 27:
		return 8;//RegSymbol: plus
	case 28:
		return 8;//RegSymbol: question
	case 29:
		return 8;//RegSymbol: or
	}
	return 0;
}


const size_t Panel::StateCount = 107;
const size_t Panel::NonTerminalCount = 24;
const size_t Panel::TerminalCount = 29;
const size_t Panel::RulesCount = 60;
const int Panel::GOTO[107][24] = { \
{1, 6, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 14}, \
{1, 1, 26, 30, 1, 1, 34, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 254, 1, 1, 34, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 258, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 58}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 230, 234, 1, 70, 74, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 66, 1, 70, 74, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 118, 122, 126, 130, 134, 138, 142, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 210, 134, 138, 142, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 194, 126, 130, 134, 138, 142, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 174, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 186, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 206, 130, 134, 138, 142, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 210, 134, 138, 142, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 238, 1, 70, 74, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 242, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 274}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 278, 282, 1, 286, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 422, 1, 286, 1, 1, 426, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 306}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 382, 386, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 314, 318, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 374, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 402, 1, 1, 406, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 394, 318, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 374, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 414, 318, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 374, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1} };
//==============================
const int Panel::ACTION[107][30] = { \
{1, 1, 1, 1, 1, 10, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 18, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 38, 1, 1, 1, 1, 1, 42, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 22, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 239, 1, 1, 1, 239, 239, 239, 239, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 239, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 38, 1, 1, 1, 1, 1, 42, 1, 1, 1, 1, 1, 1, 1, 1, 246, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 15, 1, 1, 1, 1, 1, 15, 1, 1, 1, 1, 1, 1, 1, 1, 15, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 62, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 51, 1, 1, 1, 46, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 47, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 50, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 54, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 55, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 78, 1, 1, 1, 82, 86, 90, 94, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 78, 1, 1, 1, 82, 86, 90, 94, 1, 1, 1, 1, 1, 1, 22, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 23, 1, 1, 1, 1, 1, 23, 1, 1, 1, 1, 1, 1, 1, 1, 23, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 110, 114, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 59, 59, 1, 1, 1, 98, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 83, 83, 1, 1, 1, 83, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 71, 71, 1, 1, 1, 71, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 75, 75, 1, 1, 1, 75, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 67, 67, 1, 1, 1, 67, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 79, 79, 1, 1, 1, 79, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 102, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 106, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 63, 63, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 39, 1, 1, 1, 39, 39, 39, 39, 1, 1, 1, 1, 1, 1, 1, 39, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 146, 150, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 154, 1, 158, 1, 162, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 226, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 87, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 202}, \
{1, 1, 1, 146, 150, 1, 1, 1, 1, 1, 1, 1, 91, 1, 1, 1, 1, 154, 91, 158, 1, 162, 1, 1, 1, 1, 1, 1, 1, 91}, \
{1, 1, 1, 99, 99, 1, 1, 1, 1, 1, 1, 1, 99, 1, 1, 1, 1, 99, 99, 99, 1, 99, 1, 1, 1, 1, 214, 218, 222, 99}, \
{1, 1, 1, 107, 107, 1, 1, 1, 1, 1, 1, 1, 107, 1, 1, 1, 1, 107, 107, 107, 1, 107, 1, 1, 1, 1, 107, 107, 107, 107}, \
{1, 1, 1, 123, 123, 1, 1, 1, 1, 1, 1, 1, 123, 1, 1, 1, 1, 123, 123, 123, 1, 123, 1, 1, 1, 1, 123, 123, 123, 123}, \
{1, 1, 1, 135, 135, 1, 1, 1, 1, 1, 1, 1, 135, 1, 1, 1, 1, 135, 135, 135, 1, 135, 1, 1, 1, 1, 135, 135, 135, 135}, \
{1, 1, 1, 147, 147, 1, 1, 1, 1, 1, 1, 1, 147, 1, 1, 1, 1, 147, 147, 147, 147, 147, 1, 1, 1, 147, 147, 147, 147, 147}, \
{1, 1, 1, 151, 151, 1, 1, 1, 1, 1, 1, 1, 151, 1, 1, 1, 1, 151, 151, 151, 151, 151, 1, 1, 1, 151, 151, 151, 151, 151}, \
{1, 1, 1, 146, 150, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 154, 1, 158, 1, 162, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 146, 150, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 166, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 170, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 131, 131, 1, 1, 1, 1, 1, 1, 1, 131, 1, 1, 1, 1, 131, 131, 131, 1, 131, 1, 1, 1, 1, 131, 131, 131, 131}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 178, 1, 1, 1, 1, 182, 1, 1, 1, 1}, \
{1, 1, 1, 139, 139, 1, 1, 1, 1, 1, 1, 1, 139, 1, 1, 1, 1, 139, 139, 139, 1, 139, 1, 1, 1, 1, 139, 139, 139, 139}, \
{1, 1, 1, 146, 150, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 190, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 143, 143, 1, 1, 1, 1, 1, 1, 1, 143, 1, 1, 1, 1, 143, 143, 143, 1, 143, 1, 1, 1, 1, 143, 143, 143, 143}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 198, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 202}, \
{1, 1, 1, 127, 127, 1, 1, 1, 1, 1, 1, 1, 127, 1, 1, 1, 1, 127, 127, 127, 1, 127, 1, 1, 1, 1, 127, 127, 127, 127}, \
{1, 1, 1, 146, 150, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 154, 1, 158, 1, 162, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 146, 150, 1, 1, 1, 1, 1, 1, 1, 95, 1, 1, 1, 1, 154, 95, 158, 1, 162, 1, 1, 1, 1, 1, 1, 1, 95}, \
{1, 1, 1, 103, 103, 1, 1, 1, 1, 1, 1, 1, 103, 1, 1, 1, 1, 103, 103, 103, 1, 103, 1, 1, 1, 1, 214, 218, 222, 103}, \
{1, 1, 1, 115, 115, 1, 1, 1, 1, 1, 1, 1, 115, 1, 1, 1, 1, 115, 115, 115, 1, 115, 1, 1, 1, 1, 115, 115, 115, 115}, \
{1, 1, 1, 111, 111, 1, 1, 1, 1, 1, 1, 1, 111, 1, 1, 1, 1, 111, 111, 111, 1, 111, 1, 1, 1, 1, 111, 111, 111, 111}, \
{1, 1, 1, 119, 119, 1, 1, 1, 1, 1, 1, 1, 119, 1, 1, 1, 1, 119, 119, 119, 1, 119, 1, 1, 1, 1, 119, 119, 119, 119}, \
{1, 43, 1, 1, 1, 43, 43, 43, 43, 1, 1, 1, 1, 1, 1, 1, 43, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 78, 1, 1, 1, 82, 86, 90, 94, 1, 1, 1, 1, 1, 1, 1, 246, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 31, 1, 1, 1, 31, 31, 31, 31, 1, 1, 1, 1, 1, 1, 1, 31, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 35, 1, 1, 1, 35, 35, 35, 35, 1, 1, 1, 1, 1, 1, 1, 35, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 27, 1, 1, 1, 1, 1, 27, 1, 1, 1, 1, 1, 1, 1, 1, 27, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{235, 235, 1, 1, 1, 1, 235, 235, 1, 1, 1, 1, 250, 1, 1, 1, 235, 1, 1, 1, 1, 235, 1, 1, 1, 1, 1, 1, 1, 1}, \
{231, 231, 1, 1, 1, 1, 231, 231, 1, 1, 1, 1, 1, 1, 1, 1, 231, 1, 1, 1, 1, 231, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 19, 1, 1, 1, 1, 1, 19, 1, 1, 1, 1, 1, 1, 1, 1, 19, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{7, 1, 1, 1, 1, 1, 262, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 266, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 270, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 18, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 290, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 294, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 290, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 246, 1, 1, 1, 1, 294, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 155, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 155, 1, 1, 1, 1, 155, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 310, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 179, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 298, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 302, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 183, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 290, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 294, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 322, 1, 1, 1, 326, 330, 334, 338, 1, 1, 1, 1, 1, 1, 22, 1, 1, 1, 342, 1, 346, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 322, 1, 1, 1, 326, 330, 334, 338, 1, 1, 1, 378, 1, 1, 1, 1, 1, 1, 342, 1, 346, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 187, 1, 1, 1, 187, 187, 187, 187, 1, 1, 1, 187, 1, 1, 1, 1, 1, 1, 187, 1, 187, 1, 1, 1, 1, 366, 370, 1, 1}, \
{1, 219, 1, 1, 1, 219, 219, 219, 219, 1, 1, 1, 219, 1, 1, 1, 1, 1, 1, 219, 1, 219, 1, 1, 1, 1, 219, 219, 1, 1}, \
{1, 207, 1, 1, 1, 207, 207, 207, 207, 1, 1, 1, 207, 1, 1, 1, 1, 1, 1, 207, 1, 207, 1, 1, 1, 1, 207, 207, 1, 1}, \
{1, 211, 1, 1, 1, 211, 211, 211, 211, 1, 1, 1, 211, 1, 1, 1, 1, 1, 1, 211, 1, 211, 1, 1, 1, 1, 211, 211, 1, 1}, \
{1, 203, 1, 1, 1, 203, 203, 203, 203, 1, 1, 1, 203, 1, 1, 1, 1, 1, 1, 203, 1, 203, 1, 1, 1, 1, 203, 203, 1, 1}, \
{1, 215, 1, 1, 1, 215, 215, 215, 215, 1, 1, 1, 215, 1, 1, 1, 1, 1, 1, 215, 1, 215, 1, 1, 1, 1, 215, 215, 1, 1}, \
{1, 358, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 350, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 354, 1, 1, 1, 1, 1, 1, 1}, \
{1, 223, 1, 1, 1, 223, 223, 223, 223, 1, 1, 1, 223, 1, 1, 1, 1, 1, 1, 223, 1, 223, 1, 1, 1, 1, 223, 223, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 362, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 227, 1, 1, 1, 227, 227, 227, 227, 1, 1, 1, 227, 1, 1, 1, 1, 1, 1, 227, 1, 227, 1, 1, 1, 1, 227, 227, 1, 1}, \
{1, 199, 1, 1, 1, 199, 199, 199, 199, 1, 1, 1, 199, 1, 1, 1, 1, 1, 1, 199, 1, 199, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 195, 1, 1, 1, 195, 195, 195, 195, 1, 1, 1, 195, 1, 1, 1, 1, 1, 1, 195, 1, 195, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 191, 1, 1, 1, 191, 191, 191, 191, 1, 1, 1, 191, 1, 1, 1, 1, 1, 1, 191, 1, 191, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 163, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 163, 1, 1, 1, 1, 163, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 290, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 246, 1, 1, 1, 1, 294, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 390, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 322, 1, 1, 1, 326, 330, 334, 338, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 342, 1, 346, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 322, 1, 1, 1, 326, 330, 334, 338, 1, 1, 1, 398, 1, 1, 1, 1, 1, 1, 342, 1, 346, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 171, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 171, 1, 1, 1, 1, 171, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 410, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 167, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 167, 1, 1, 1, 1, 167, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 322, 1, 1, 1, 326, 330, 334, 338, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 342, 1, 346, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 322, 1, 1, 1, 326, 330, 334, 338, 1, 1, 1, 418, 1, 1, 1, 1, 1, 1, 342, 1, 346, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 175, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 175, 1, 1, 1, 1, 175, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 159, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 159, 1, 1, 1, 1, 159, 1, 1, 1, 1, 1, 1, 1, 1}, \
{11, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1} };
//==============================
const int Panel::RulesToSymbol[60] = { \
0,\
1,\
1,\
2,\
2,\
3,\
3,\
4,\
4,\
5,\
5,\
6,\
6,\
6,\
7,\
7,\
8,\
8,\
8,\
8,\
8,\
9,\
10,\
10,\
11,\
11,\
12,\
12,\
12,\
12,\
13,\
13,\
13,\
14,\
14,\
14,\
15,\
15,\
16,\
16,\
17,\
17,\
18,\
18,\
19,\
19,\
20,\
20,\
20,\
20,\
21,\
21,\
21,\
21,\
21,\
21,\
21,\
22,\
22,\
23 };
//==============================
const int Panel::RulesLength[60] = { \
1,\
4,\
10,\
1,\
2,\
3,\
4,\
1,\
2,\
2,\
4,\
1,\
1,\
4,\
1,\
4,\
1,\
1,\
1,\
1,\
1,\
1,\
1,\
3,\
1,\
2,\
1,\
2,\
2,\
2,\
1,\
3,\
3,\
1,\
3,\
5,\
1,\
1,\
1,\
2,\
4,\
4,\
4,\
5,\
1,\
3,\
1,\
2,\
2,\
2,\
1,\
1,\
1,\
1,\
1,\
3,\
3,\
2,\
1,\
2 };
//==============================
const char* const Panel::RulesName[60] = { \
"all->TEXT ",\
"TEXT->lexical BEGIN LEXICAL END ",\
"TEXT->lexical BEGIN LEXICAL END grammar colon identifier BEGIN GRAMMAR END ",\
"LEXICAL->RegGROUP ",\
"LEXICAL->LEXICAL RegGROUP ",\
"RegGROUP->RegGROUPNAME colon RegDEF ",\
"RegGROUP->RegGROUPNAME BEGIN REGDEFS END ",\
"REGDEFS->RegDEF ",\
"REGDEFS->REGDEFS RegDEF ",\
"RegDEF->RegNAME semicolon ",\
"RegDEF->RegNAME colon REGBODY semicolon ",\
"RegGROUPNAME->void ",\
"RegGROUPNAME->identifier ",\
"RegGROUPNAME->identifier left integer right ",\
"RegNAME->RegNAMEHead ",\
"RegNAME->RegNAMEHead left integer right ",\
"RegNAMEHead->void ",\
"RegNAMEHead->lexical ",\
"RegNAMEHead->grammar ",\
"RegNAMEHead->all ",\
"RegNAMEHead->identifier ",\
"REGBODY->REGEXPRESSor ",\
"REGEXPRESSor->REGEXPRESS ",\
"REGEXPRESSor->REGEXPRESSor or REGEXPRESS ",\
"REGEXPRESS->RegCOMPLEX ",\
"REGEXPRESS->REGEXPRESS RegCOMPLEX ",\
"RegCOMPLEX->RegNODE ",\
"RegCOMPLEX->RegCOMPLEX plus ",\
"RegCOMPLEX->RegCOMPLEX star ",\
"RegCOMPLEX->RegCOMPLEX question ",\
"RegNODE->RegLEAF ",\
"RegNODE->left REGEXPRESSor right ",\
"RegNODE->angleL identifier angleR ",\
"RegLEAF->RegCHAR ",\
"RegLEAF->squareL RegCHAR squareR ",\
"RegLEAF->squareL RegCHAR range RegCHAR squareR ",\
"RegCHAR->CommonChar ",\
"RegCHAR->idChar ",\
"GRAMMAR->GrammerDEF ",\
"GRAMMAR->GRAMMAR GrammerDEF ",\
"GrammerDEF->GNameALL colon GFORMULA semicolon ",\
"GrammerDEF->GNameALL BEGIN GnameFORMULAS END ",\
"GnameFORMULAS->GNameALL colon GFORMULA semicolon ",\
"GnameFORMULAS->GnameFORMULAS GNameALL colon GFORMULA semicolon ",\
"GNameALL->identifier ",\
"GNameALL->angleL identifier angleR ",\
"GFORMULA->GFORMULAUnit ",\
"GFORMULA->GFORMULA GFORMULAUnit ",\
"GFORMULA->GFORMULAUnit plus ",\
"GFORMULA->GFORMULAUnit star ",\
"GFORMULAUnit->void ",\
"GFORMULAUnit->lexical ",\
"GFORMULAUnit->grammar ",\
"GFORMULAUnit->all ",\
"GFORMULAUnit->identifier ",\
"GFORMULAUnit->angleL identifier angleR ",\
"GFORMULAUnit->squareL identifier squareR ",\
"END->braceR semicolon ",\
"END->braceR ",\
"BEGIN->colon braceL " };
//==============================
const int Panel::Implicit[60] = { \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0 };



int PreTreat::next(int state, const char c)
{
	switch (state)
	{
	case 0:
		if (c == (char)9) return 9;
		else if (c == (char)10) return 8;
		else if (c == (char)13) return 28;
		else if (c == ' ') return 7;
		else if (c == '\"') return 31;
		else if (c == '#') return 34;
		else if (c == '\'') return 35;
		else if (c == '(') return 15;
		else if (c == ')') return 16;
		else if (c == '*') return 24;
		else if (c == '+') return 25;
		else if (c == '-') return 23;
		else if (c == '.') return 12;
		else if (c == '/') return 38;
		else if ('0' <= c && c <= '9') return 4;
		else if (c == ':') return 11;
		else if (c == ';') return 10;
		else if (c == '<') return 19;
		else if (c == '>') return 20;
		else if (c == '\?') return 26;
		else if ('A' <= c && c <= 'Z') return 29;
		else if (c == '[') return 17;
		else if (c == ']') return 18;
		else if (c == '_') return 29;
		else if ('a' <= c && c <= 'z') return 29;
		else if (c == '{') return 13;
		else if (c == '|') return 27;
		else if (c == '}') return 14;
		else return 0;
	case 1:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'z') return 1;
		else return 0;
	case 2:
		if ('0' <= c && c <= '9') return 2;
		else return 0;
	case 3:
		return 0;
	case 4:
		return 0;
	case 5:
		return 0;
	case 6:
		return 0;
	case 7:
		if (c == ' ') return 7;
		else return 0;
	case 8:
		if (c == (char)10) return 8;
		else if (c == (char)13) return 28;
		else return 0;
	case 9:
		return 0;
	case 10:
		return 0;
	case 11:
		return 0;
	case 12:
		return 0;
	case 13:
		return 0;
	case 14:
		return 0;
	case 15:
		return 0;
	case 16:
		return 0;
	case 17:
		return 0;
	case 18:
		return 0;
	case 19:
		return 0;
	case 20:
		return 0;
	case 21:
		if ((char)0 <= c && c <= (char)9) return 21;
		else if ((char)11 <= c && c <= (char)127) return 21;
		else return 0;
	case 22:
		return 0;
	case 23:
		return 0;
	case 24:
		return 0;
	case 25:
		if ('0' <= c && c <= '9') return 2;
		else return 0;
	case 26:
		return 0;
	case 27:
		return 0;
	case 28:
		if (c == (char)10) return 8;
		else return 0;
	case 29:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'z') return 1;
		else return 0;
	case 30:
		if ((char)0 <= c && c <= ')') return 30;
		else if (c == '*') return 33;
		else if ('+' <= c && c <= (char)127) return 30;
		else return 0;
	case 31:
		if (' ' <= c && c <= '!') return 31;
		else if (c == '\"') return 5;
		else if ('#' <= c && c <= '[') return 31;
		else if (c == '\\') return 36;
		else if (']' <= c && c <= (char)127) return 31;
		else return 0;
	case 32:
		if (c == (char)0) return 37;
		else if (c == '\"') return 37;
		else if (c == '\'') return 37;
		else if ('0' <= c && c <= '7') return 44;
		else if (c == '\?') return 37;
		else if (c == 'X') return 39;
		else if (c == '\\') return 37;
		else if ('a' <= c && c <= 'b') return 37;
		else if (c == 'f') return 37;
		else if (c == 'n') return 37;
		else if (c == 'r') return 37;
		else if (c == 't') return 37;
		else if (c == 'v') return 37;
		else if (c == 'x') return 39;
		else return 0;
	case 33:
		if ((char)0 <= c && c <= ')') return 30;
		else if (c == '*') return 33;
		else if ('+' <= c && c <= '.') return 30;
		else if (c == '/') return 22;
		else if ('0' <= c && c <= (char)127) return 30;
		else return 0;
	case 34:
		if (c == 'i') return 43;
		else return 0;
	case 35:
		if (' ' <= c && c <= '!') return 37;
		else if ('#' <= c && c <= '&') return 37;
		else if ('(' <= c && c <= '[') return 37;
		else if (c == '\\') return 32;
		else if (']' <= c && c <= '~') return 37;
		else return 0;
	case 36:
		if (c == (char)0) return 31;
		else if (c == '\"') return 31;
		else if (c == '\'') return 31;
		else if ('0' <= c && c <= '7') return 31;
		else if (c == '\?') return 31;
		else if (c == 'X') return 45;
		else if (c == '\\') return 31;
		else if ('a' <= c && c <= 'b') return 31;
		else if (c == 'f') return 31;
		else if (c == 'n') return 31;
		else if (c == 'r') return 31;
		else if (c == 't') return 31;
		else if (c == 'v') return 31;
		else if (c == 'x') return 45;
		else return 0;
	case 37:
		if (c == '\'') return 3;
		else return 0;
	case 38:
		if (c == '*') return 30;
		else if (c == '/') return 21;
		else return 0;
	case 39:
		if ('0' <= c && c <= '9') return 48;
		else if ('A' <= c && c <= 'F') return 48;
		else if ('a' <= c && c <= 'f') return 48;
		else return 0;
	case 40:
		if (c == 'c') return 46;
		else return 0;
	case 41:
		if (c == 'd') return 42;
		else return 0;
	case 42:
		if (c == 'e') return 6;
		else return 0;
	case 43:
		if (c == 'n') return 40;
		else return 0;
	case 44:
		if (c == '\'') return 3;
		else if ('0' <= c && c <= '7') return 49;
		else return 0;
	case 45:
		if ('0' <= c && c <= '9') return 31;
		else if ('A' <= c && c <= 'F') return 31;
		else if ('a' <= c && c <= 'f') return 31;
		else return 0;
	case 46:
		if (c == 'l') return 47;
		else return 0;
	case 47:
		if (c == 'u') return 41;
		else return 0;
	case 48:
		if (c == '\'') return 3;
		else if ('0' <= c && c <= '9') return 37;
		else if ('A' <= c && c <= 'F') return 37;
		else if ('a' <= c && c <= 'f') return 37;
		else return 0;
	case 49:
		if (c == '\'') return 3;
		else if ('0' <= c && c <= '7') return 37;
		else return 0;
	}
	return 0;
}
int PreTreat::action(int state)
{
	switch (state)
	{
	case 1:
		return 1;//identifier: identifier
	case 2:
		return 2;//const: integer
	case 3:
		return 3;//const: CommonChar
	case 4:
		return 4;//const: idChar
	case 5:
		return 5;//string: string
	case 6:
		return 6;//include: include
	case 7:
		return 7;//format: spaces
	case 8:
		return 8;//format: enters
	case 9:
		return 9;//format: tab
	case 10:
		return 10;//division: semicolon
	case 11:
		return 11;//division: colon
	case 12:
		return 12;//division: dot
	case 13:
		return 13;//braket: braceL
	case 14:
		return 14;//braket: braceR
	case 15:
		return 15;//braket: left
	case 16:
		return 16;//braket: right
	case 17:
		return 17;//braket: squareL
	case 18:
		return 18;//braket: squareR
	case 19:
		return 19;//braket: angleL
	case 20:
		return 20;//braket: angleR
	case 21:
		return 21;//anntation: anntationS
	case 22:
		return 22;//anntation: anntationM
	case 23:
		return 23;//RegSymbol: range
	case 24:
		return 24;//RegSymbol: star
	case 25:
		return 25;//RegSymbol: plus
	case 26:
		return 26;//RegSymbol: question
	case 27:
		return 27;//RegSymbol: or
	case 29:
		return 4;//const: idChar
	}
	return 0;
}
int PreTreat::GroupGet(int accept)
{
	switch (accept)
	{
	case 1:
		return 1;//identifier: identifier
	case 2:
		return 2;//const: integer
	case 3:
		return 2;//const: CommonChar
	case 4:
		return 2;//const: idChar
	case 5:
		return 3;//string: string
	case 6:
		return 4;//include: include
	case 7:
		return 5;//format: spaces
	case 8:
		return 5;//format: enters
	case 9:
		return 5;//format: tab
	case 10:
		return 6;//division: semicolon
	case 11:
		return 6;//division: colon
	case 12:
		return 6;//division: dot
	case 13:
		return 7;//braket: braceL
	case 14:
		return 7;//braket: braceR
	case 15:
		return 7;//braket: left
	case 16:
		return 7;//braket: right
	case 17:
		return 7;//braket: squareL
	case 18:
		return 7;//braket: squareR
	case 19:
		return 7;//braket: angleL
	case 20:
		return 7;//braket: angleR
	case 21:
		return 8;//anntation: anntationS
	case 22:
		return 8;//anntation: anntationM
	case 23:
		return 9;//RegSymbol: range
	case 24:
		return 9;//RegSymbol: star
	case 25:
		return 9;//RegSymbol: plus
	case 26:
		return 9;//RegSymbol: question
	case 27:
		return 9;//RegSymbol: or
	}
	return 0;
}



//Preparser

const size_t Preparser::StateCount = 42;
const size_t Preparser::NonTerminalCount = 13;
const size_t Preparser::TerminalCount = 27;
const size_t Preparser::RulesCount = 40;
const int Preparser::GOTO[42][13] = { \
{1, 6, 10, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 14, 18, 22, 26, 30, 34, 38, 42, 46, 50}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1} };
//==============================
const int Preparser::ACTION[42][28] = { \
{11, 11, 11, 11, 11, 1, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11}, \
{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{7, 54, 58, 62, 66, 1, 70, 74, 78, 82, 86, 90, 94, 98, 102, 106, 110, 114, 118, 122, 126, 130, 134, 138, 142, 146, 150, 154}, \
{15, 15, 15, 15, 15, 1, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15}, \
{19, 19, 19, 19, 19, 1, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19}, \
{27, 27, 27, 27, 27, 1, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27}, \
{23, 23, 23, 23, 23, 1, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23}, \
{47, 47, 47, 47, 47, 1, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47}, \
{51, 51, 51, 51, 51, 1, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51}, \
{55, 55, 55, 55, 55, 1, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55}, \
{59, 59, 59, 59, 59, 1, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59}, \
{63, 63, 63, 63, 63, 1, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63}, \
{67, 67, 67, 67, 67, 1, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67, 67}, \
{71, 71, 71, 71, 71, 1, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71}, \
{75, 75, 75, 75, 75, 1, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75}, \
{79, 79, 79, 79, 79, 1, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79}, \
{83, 83, 83, 83, 83, 1, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83}, \
{1, 1, 1, 1, 1, 158, 1, 162, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{151, 151, 151, 151, 151, 1, 151, 151, 151, 151, 151, 151, 151, 151, 151, 151, 151, 151, 151, 151, 151, 151, 151, 151, 151, 151, 151, 151}, \
{155, 155, 155, 155, 155, 1, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155}, \
{159, 159, 159, 159, 159, 1, 159, 159, 159, 159, 159, 159, 159, 159, 159, 159, 159, 159, 159, 159, 159, 159, 159, 159, 159, 159, 159, 159}, \
{139, 139, 139, 139, 139, 1, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139, 139}, \
{143, 143, 143, 143, 143, 1, 143, 143, 143, 143, 143, 143, 143, 143, 143, 143, 143, 143, 143, 143, 143, 143, 143, 143, 143, 143, 143, 143}, \
{147, 147, 147, 147, 147, 1, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147, 147}, \
{107, 107, 107, 107, 107, 1, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107}, \
{111, 111, 111, 111, 111, 1, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111, 111}, \
{115, 115, 115, 115, 115, 1, 115, 115, 115, 115, 115, 115, 115, 115, 115, 115, 115, 115, 115, 115, 115, 115, 115, 115, 115, 115, 115, 115}, \
{119, 119, 119, 119, 119, 1, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119, 119}, \
{123, 123, 123, 123, 123, 1, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123, 123}, \
{127, 127, 127, 127, 127, 1, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127}, \
{131, 131, 131, 131, 131, 1, 131, 131, 131, 131, 131, 131, 131, 131, 131, 131, 131, 131, 131, 131, 131, 131, 131, 131, 131, 131, 131, 131}, \
{135, 135, 135, 135, 135, 1, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135, 135}, \
{31, 31, 31, 31, 31, 1, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31}, \
{35, 35, 35, 35, 35, 1, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35}, \
{87, 87, 87, 87, 87, 1, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87}, \
{91, 91, 91, 91, 91, 1, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91}, \
{95, 95, 95, 95, 95, 1, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95}, \
{99, 99, 99, 99, 99, 1, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99}, \
{103, 103, 103, 103, 103, 1, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103}, \
{43, 43, 43, 43, 43, 1, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43}, \
{21, 21, 21, 21, 21, 166, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21}, \
{39, 39, 39, 39, 39, 1, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39} };
//==============================
const int Preparser::RulesToSymbol[40] = { \
0,\
1,\
2,\
2,\
3,\
3,\
3,\
4,\
4,\
5,\
5,\
6,\
6,\
6,\
6,\
6,\
6,\
7,\
8,\
8,\
8,\
9,\
9,\
9,\
9,\
9,\
10,\
10,\
10,\
10,\
10,\
10,\
10,\
10,\
11,\
11,\
11,\
12,\
12,\
12 };
//==============================
const int Preparser::RulesLength[40] = { \
1,\
1,\
0,\
2,\
1,\
1,\
1,\
1,\
1,\
3,\
2,\
1,\
1,\
1,\
1,\
1,\
1,\
1,\
1,\
1,\
1,\
1,\
1,\
1,\
1,\
1,\
1,\
1,\
1,\
1,\
1,\
1,\
1,\
1,\
1,\
1,\
1,\
1,\
1,\
1 };
//==============================
const char* const Preparser::RulesName[40] = { \
"all->TEXT ",\
"TEXT-><DEF*> ",\
"<DEF*>->epsilon ",\
"<DEF*>-><DEF*> DEF ",\
"DEF->[anntation] ",\
"DEF->ANY ",\
"DEF->INCLUDE ",\
"[anntation]->anntationS ",\
"[anntation]->anntationM ",\
"INCLUDE->include spaces string ",\
"INCLUDE->include string ",\
"ANY->[identifier] ",\
"ANY->[const] ",\
"ANY->[RegSymbol] ",\
"ANY->[braket] ",\
"ANY->[division] ",\
"ANY->[format] ",\
"[identifier]->identifier ",\
"[const]->integer ",\
"[const]->CommonChar ",\
"[const]->idChar ",\
"[RegSymbol]->range ",\
"[RegSymbol]->star ",\
"[RegSymbol]->plus ",\
"[RegSymbol]->question ",\
"[RegSymbol]->or ",\
"[braket]->braceL ",\
"[braket]->braceR ",\
"[braket]->left ",\
"[braket]->right ",\
"[braket]->squareL ",\
"[braket]->squareR ",\
"[braket]->angleL ",\
"[braket]->angleR ",\
"[division]->semicolon ",\
"[division]->colon ",\
"[division]->dot ",\
"[format]->spaces ",\
"[format]->enters ",\
"[format]->tab " };
//==============================
const int Preparser::Implicit[40] = { \
0, \
0, \
1, \
1, \
0, \
0, \
0, \
1, \
1, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
0, \
1, \
1, \
1, \
1, \
1, \
1, \
1, \
1, \
1, \
1, \
1, \
1, \
1, \
1, \
1, \
1, \
1, \
1, \
1, \
1, \
1, \
1, \
1 };








