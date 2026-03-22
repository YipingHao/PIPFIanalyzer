#include"dictionary.h"
using namespace hyperlex;

#define SizeMax ((size_t)0xffffffffffffffff)
#define CharSize ((size_t)(1 << (sizeof(char) * 8 - 1)))

static bool compare(const char* str1, const char* str2);
static void write_escaped_string(const char* str, FILE* file);
static char* Copy(const char* input);

dictionary::dictionary()
{
	errorCode = NoError;
	errorInfor1 = 0;
	errorInfor2 = 0;
}
dictionary::~dictionary()
{
	clear();
}
void dictionary::clear(void)
{
	size_t i;
	for (i = 0; i < Content.count(); i++)
	{
		Content[i].ruin();
	}
	Content.clear();
	LexicalSource.clear();

	errorCode = NoError;
	errorInfor1 = 0;
	errorInfor2 = 0;
}
dictionary::KV::KV()
{
	initial();
}
dictionary::KV::~KV()
{
	ruin();
	free(key);
}
void dictionary::KV::copy(const KV& src)
{
	// 复制基本字段
	T = src.T;
	
	// 深拷贝 key
	if (src.key != NULL) 
	{
		setName(src.key);
	}
	else 
	{
		key = NULL;
	}
	recapacity(src.Capacity);
	Count = src.Count;
	switch (T)
	{
	case hyperlex::dictionary::null_:
	case hyperlex::dictionary::int_:
	{
		for (size_t i = 0; i < Count; ++i)
			Content[i].ii = src.Content[i].ii;
		break;
	}
	case hyperlex::dictionary::float_:
	{
		for (size_t i = 0; i < Count; ++i)
			Content[i].ff = src.Content[i].ff;
		break;
	}
	case hyperlex::dictionary::string_:
	{
		for (size_t i = 0; i < Count; ++i)
		{
			if(src.Content[i].ss != NULL)
			{
				Content[i].ss = Copy(src.Content[i].ss); 
			}
			else 
			{
				Content[i].ss = NULL;
			}
		}
		break;
	}
	case hyperlex::dictionary::bool_:
	{
		for (size_t i = 0; i < Count; ++i)
			Content[i].bb = src.Content[i].bb;
		break;
	}
	case hyperlex::dictionary::dictionary_:
	{
		for (size_t i = 0; i < Count; ++i)
		{
			Content[i].dd = new dictionary;
			Content[i].dd->copy(*src.Content[i].dd);
		}
		break;
	}
	default:
		break;
	}

}
void dictionary::KV::ruin(void)
{
	clear();
	free(Content);
	Content = NULL;
	free(key);
	key = NULL;
}
void dictionary::KV::clear(void)
{
	size_t i;
	for (i = 0; i < Count; ++i)
	{
		switch (T)
		{
		case string_: free(Content[i].ss); break;
		case dictionary_: delete Content[i].dd; break;
		default: break;
		}
	}
	Count = 0;
}
void dictionary::KV::initial(void)
{
	key = NULL;
	T = null_;
	Content = NULL;
	Capacity = 0;
	Count = 0;
}
void dictionary::KV::recapacity(size_t NewSize)
{
	size_t i;
	element* newContent = (element*)malloc(sizeof(element) * NewSize);
	if (NewSize <= Count)
	{
		for (i = NewSize; i < Count; ++i)
		{
			switch (T) {
			case string_:
				free(Content[i].ss);
				break;
			case dictionary_:
				delete Content[i].dd;
				break;
			}
		}
		Count = NewSize;
	}
	else
	{
		for (i = 0; i < Count; ++i) {
			newContent[i] = Content[i];
		}
	}
	free(Content);
	Content = newContent;
	Capacity = NewSize;
}
void dictionary::KV::append(const element& e)
{
	if (Count >= Capacity) recapacity(Capacity ? (Capacity + Capacity / 4 + 4) : 1);
	Content[Count] = e;
	Count++;
}
void dictionary::KV::setType(Ktype newType)
{
	if (T != newType) {
		clear();
		T = newType;
	}
}
void dictionary::KV::setName(const char* newKey)
{
	size_t i;
	free(key);
	key = NULL;
	if (!newKey) return;

	size_t len = 0;
	while (newKey[len] != '\0') ++len;

	key = (char*)(malloc((len + 1) * sizeof(char)));
	for (i = 0; i <= len; ++i)
	{
		key[i] = newKey[i];
	}
}

size_t dictionary::KV::capacity() const { return Capacity; }
size_t dictionary::KV::count() const { return Count; }
dictionary::element& dictionary::KV::operator[](size_t i) { return Content[i]; }
dictionary::element const & dictionary::KV::operator[](size_t i) const { return Content[i]; }

struct complex
{
	enum regular
	{
		_id_ = 1,
		_arrow_ = 2,
		_colon_ = 3,
		_dot_ = 4,
		_colon2_ = 5,
		_slash_ = 6,
		_backslash_ = 7
	};
	enum group
	{
		_id___ = 1,
		_division___ = 2
	};
	static int next(int state, const char c);
	static int action(int state);
	static int GroupGet(int state);
};

dictionary::element dictionary::search(dictionary::Ktype& T, const char* key) const
{
	size_t i, j;
	element E;
	hyperlex::Morpheme eme;
	vector<const char*> storage;
	const dictionary* target;

	eme.Build<complex>(key);
	target = this;
	for (i = 0; i < eme.GetCount(); i++)
	{
		if (eme[i].category == complex::_id___)
			storage.append(eme.GetWord(i));
	}
	E.dd = NULL;
	T = null_;
	for (i = 1; i < storage.count(); i++)
	{
		for (j = 0; j < target->Content.count(); j++)
		{
			if (compare(storage[i - 1], target->Content[j].key))
			{
				if (target->Content[j].T == dictionary_)
				{
					target = target->Content[j][0].dd;
					break;
				}
				else return E;
			}
		}
		if (j == target->Content.count()) return E;
	}
	for (j = 0; j < target->Content.count(); j++)
	{
		if (compare(storage[i - 1], target->Content[j].key))
		{
			T = target->Content[j].T;
			E = target->Content[j][0];
			return E;
		}
	}
	return E;
}
dictionary::element* dictionary::search(size_t& count, dictionary::Ktype& T, const char* key) const
{
	size_t i, j;
	element*E;
	hyperlex::Morpheme eme;
	vector<const char*> storage;
	const dictionary* target;

	target = this;
	count = 0;
	E = NULL;
	T = null_;
	eme.Build<complex>(key);
	for (i = 0; i < eme.GetCount(); i++)
	{
		if (eme[i].category == complex::_id___)
			storage.append(eme.GetWord(i));
	}
	
	for (i = 1; i < storage.count(); i++)
	{
		for (j = 0; j < target->Content.count(); j++)
		{
			if (compare(storage[i - 1], target->Content[j].key))
			{
				if (target->Content[j].T == dictionary_)
				{
					target = target->Content[j][0].dd;
					break;
				}
				else return E;
			}
		}
		if (j == target->Content.count()) return E;
	}
	for (j = 0; j < target->Content.count(); j++)
	{
		if (compare(storage[i - 1], target->Content[j].key))
		{
			T = target->Content[j].T;
			E = target->Content[j].Content;
			count = target->Content[j].count();
			return E;
		}
	}
	return E;
}
const char* dictionary::search(const char* Default_, const char* key) const
{
	element E;
	Ktype T;
	E = search(T, key);
	if (T == string_) return E.ss;
	else return Default_;
}
bool dictionary::search(bool Default_, const char* key) const
{
	element E;
	Ktype T;
	E = search(T, key);
	if (T == bool_) return E.bb;
	else return Default_;
} 
long int dictionary::search(long int Default_, const char* key) const
{
	element E;
	Ktype T;
	E = search(T, key);
	if (T == int_) return E.ii;
	else return Default_;
}
double dictionary::search(double Default_, const char* key) const
{
	element E;
	Ktype T;
	E = search(T, key);
	if (T == float_) return E.ff;
	else return Default_;
}


size_t dictionary::append(const char* key, dictionary::element value, dictionary::Ktype T)
{
	size_t i, j;
	size_t site;
	hyperlex::Morpheme eme;
	vector<const char*> storage;
	dictionary* target;
	KV kv;
	eme.Build<complex>(key);
	target = this;
	site = SizeMax;
	for (i = 0; i < eme.GetCount(); i++)
	{
		if (eme[i].category == complex::_id___)
			storage.append(eme.GetWord(i));
	}
	for (i = 1; i < storage.count(); i++)
	{
		for (j = 0; j < target->Content.count(); j++)
		{
			if (compare(storage[i - 1], target->Content[j].key))
			{
				if (target->Content[j].T == dictionary_)
				{
					target = target->Content[j][0].dd;
					break;
				}
				else return site;
			}
		}
		if(j == target->Content.count()) return site;
	}
	for (j = 0; j < target->Content.count(); j++)
		if (compare(storage[i - 1], target->Content[j].key))
		{
			if (target->Content[j].T == T)
			{
				target->Content[j].append(value);
				return site;
			}
			else return SizeMax;
		}
	site = target->Content.count();
	kv.key = (char*)Copy(storage[i - 1]);
	kv.setType(T);
	kv.append(value);
	target->Content.append(kv);
	kv.Count = 0;
	kv.Content = NULL;
	kv.key = NULL;
	return site;
}
size_t dictionary::append(const char* key, const char* value)
{
	element VV;
	Ktype T;
	T = string_;
	VV.ss = (char*)Copy(value);
	return append(key, VV, T);
}
size_t dictionary::append(const char* key, double value)
{
	element VV;
	Ktype T;
	T = float_;
	VV.ff = value;
	return append(key, VV, T);
}
size_t dictionary::append(const char* key, long int value)
{
	element VV;
	Ktype T;
	T = int_;
	VV.ii = value;
	return append(key, VV, T);
}
size_t dictionary::append(const char* key, int value)
{
	return append(key, (long int)value);
}
size_t dictionary::append(const char* key, size_t value)
{
	return append(key, (long int)value);
}
size_t dictionary::append(const char* key, bool value)
{
	element VV;
	Ktype T;
	T = bool_;
	VV.bb = value;
	return append(key, VV, T);
}
size_t dictionary::append(const char* key, dictionary* value)
{
	element VV;
	Ktype T;
	T = dictionary_;
	VV.dd = value;
	return append(key, VV, T);
}

size_t dictionary::assign(const char* key, element value, Ktype T)
{
	size_t i, j;
	size_t site;
	hyperlex::Morpheme eme;
	vector<const char*> storage;
	dictionary* target;
	KV kv;
	eme.Build<complex>(key);
	target = this;
	site = SizeMax;
	for (i = 0; i < eme.GetCount(); i++)
	{
		if (eme[i].category == complex::_id___)
			storage.append(eme.GetWord(i));
	}
	for (i = 1; i < storage.count(); i++)
	{
		for (j = 0; j < target->Content.count(); j++)
		{
			if (compare(storage[i - 1], target->Content[j].key))
			{
				if (target->Content[j].T == dictionary_)
				{
					target = target->Content[j][0].dd;
					break;
				}
				else return site;
			}
		}
		if (j == target->Content.count()) return site;
	}
	for (j = 0; j < target->Content.count(); j++)
		if (compare(storage[i - 1], target->Content[j].key))
		{
			target->Content[j].clear();
			target->Content[j].T = T;
			target->Content[j].append(value);
			return site;
		}
	site = target->Content.count();
	kv.key = (char*)Copy(storage[i - 1]);
	kv.setType(T);
	kv.append(value);
	target->Content.append(kv);
	kv.Count = 0;
	kv.Content = NULL;
	kv.key = NULL;
	return site;
}
size_t dictionary::assign(const char* key, const char* value)
{
	element VV;
	Ktype T;
	T = string_;
	VV.ss = (char*)Copy(value);
	return assign(key, VV, T);
}

void dictionary::copy(const dictionary& source)
{
	if (source.errorCode != NoError) {
		return; // 错误不为 NoError，不做任何操作
	}

	// 清空当前的内容以避免重复数据
	clear();
	
	for (size_t i = 0; i < source.Content.count(); i++)
	{
		KV * temp = new KV;
		temp->copy(source.Content[i]);
		move(temp);
	}

}


void dictionary::print(FILE* fp) const
{
	if (errorCode != NoError)
	{
		ErrorDemo(fp);
		return;
	}
	print(fp, 0);
	fprintf(fp, "\n");
}
void dictionary::ErrorDemo(FILE* fp) const
{
	//const char* s_temp_1, * s_temp_2;
	size_t i, record;
	switch (errorCode)
	{
	case hyperlex::dictionary::NoError:
		fprintf(fp, "No Error!\n");
		break;
	case dictionary::ErrorinputLEXICAL:
		fprintf(fp, "ErrorinputLEXICAL: \n");
		break;
	case dictionary::ErrorinputGrammar:
		fprintf(fp, "ErrorinputGrammar: Something was wrong when parsing of line:");
		record = LexicalSource[errorInfor1].line;
		fprintf(fp, "%zu\n", record);
		for (i = 0; i < LexicalSource.GetCount(); i++)
		{
			if (record == LexicalSource[i].line)
			{
				if (i == errorInfor1)
					fprintf(fp, "| %s |", LexicalSource.GetWord(i));
				else
					fprintf(fp, "%s", LexicalSource.GetWord(i));
			}
		}
		break;
	case hyperlex::dictionary::buildUndone:
		fprintf(fp, "buildUndone!\n");
		break;
	default:
		break;
	}
}
void dictionary::PrintTab(FILE* fp, size_t count) const
{
	size_t i;
	for (i = 0; i < count; ++i)
	{
		fprintf(fp, "\t");
	}
}
void dictionary::print(FILE* fp, size_t indent) const
{
	fprintf(fp, "{\n");

	for (size_t i = 0; i < Content.count(); ++i)
	{
		const KV& kv = Content[i];
		PrintTab(fp, indent + 1);
		fprintf(fp, "%s: ", kv.key ? kv.key : "(null)");
		//continue;
		// 根据类型打印内容
		if (kv.Count != 1)
		{
			PrintArray(fp, kv, indent + 1);          // 基础类型紧凑打印
		}
		else
		{
			switch (kv.T) {
			case int_:
				fprintf(fp, "%ld", kv.Content[0].ii);
				break;
			case float_:
				fprintf(fp, "%g", kv.Content[0].ff);
				break;
			case string_:
				write_escaped_string(kv.Content[0].ss ? kv.Content[0].ss : "", fp);
				break;
			case bool_:
				fprintf(fp, "%s", kv.Content[0].bb ? "true" : "false");
				break;
			case dictionary_:
				if (kv.Content[0].dd) {
					kv.Content[0].dd->print(fp, indent + 1);
				}
				else {
					fprintf(fp, "{}");
				}
				break;
			default:
				fprintf(fp, "null");
			}
		}
		fprintf(fp, ";\n");
	}

	PrintTab(fp, indent);
	fprintf(fp, "}");
}
void dictionary::PrintArray(FILE* fp, const KV& kv, size_t indent) const
{
	const bool isComplexType = (kv.T == dictionary_ || kv.T == string_);
	//fprintf(fp, "%zu, indent: %zu", kv.Count, indent);
	fprintf(fp, "[");
	//return;
	for (size_t j = 0; j < kv.Count; ++j)
	{
		//fprintf(fp, "%zu, indent: %zu\n", j, kv.Count);

		//fprintf(fp, "===%zu, indent: %zu\n", j, kv.Count);
		//continue;
		// 打印单个元素
		switch (kv.T) {
		case int_:
			fprintf(fp, "%ld", kv.Content[j].ii);
			break;
		case float_:
			fprintf(fp, "%g", kv.Content[j].ff);
			break;
		case string_:
			fprintf(fp, "\"%s\"", kv.Content[j].ss ? kv.Content[j].ss : "");
			break;
		case bool_:
			fprintf(fp, "%s", kv.Content[j].bb ? "true" : "false");
			break;
		case dictionary_:
			if (kv.Content[j].dd) {
				kv.Content[j].dd->print(fp, indent);
			}
			else {
				fprintf(fp, "{}");
			}
			break;
		default:
			fprintf(fp, "null");
		}

		// 元素分隔符
		if (j != kv.Count - 1) {
			if (isComplexType)
			{
				fprintf(fp, ",\n");
				PrintTab(fp, indent + 1);
			}
			else
				fprintf(fp, ", ");
		}
	}
	fprintf(fp, "]");
}

struct DictReg
{
	enum regular
	{
		_id_ = 1,
		_integer_ = 2,
		_real_ = 3,
		_string_ = 4,
		_false_ = 5,
		_true_ = 6,
		_null_ = 7,
		_NULL_ = 8,
		_semicolon_ = 9,
		_colon_ = 10,
		_dot_ = 11,
		_comma_ = 12,
		_value_ = 13,
		_braceL_ = 14,
		_braceR_ = 15,
		_left_ = 16,
		_right_ = 17,
		_squareL_ = 18,
		_squareR_ = 19,
		_anntationS_ = 20,
		_anntationM_ = 21,
		_spaces_ = 22,
		_enters_ = 23,
		_tab_ = 24
	};
	enum group
	{
		_id___ = 1,
		_number___ = 2,
		_string___ = 3,
		_reserved___ = 4,
		_division___ = 5,
		_value___ = 6,
		_braket___ = 7,
		_anntation___ = 8,
		_format___ = 9
	};
	static int next(int state, const char c);
	static int action(int state);
	static int GroupGet(int state);
};




struct DictPraser
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
		DICTIONARY_DICTIONARY_ = 1,
		KVS_single_ = 2,
		KVS_multi_ = 3,
		KV_value_ = 4,
		KV_colon_ = 5,
		VALUE_nul_ = 6,
		VALUE_single_ = 7,
		VALUE_multi_ = 8,
		UNITS_single_ = 9,
		UNITS_multi_ = 10,
		UNIT_dictionary_ = 11,
		UNIT_false_ = 12,
		UNIT_true_ = 13,
		UNIT_null_ = 14,
		UNIT_NULL_ = 15,
		UNIT_real_ = 16,
		UNIT_integer_ = 17,
		UNIT_string_ = 18
	};
	enum nonterminal
	{
		_all_ = 0,
		_DICTIONARY_ = 1,
		_KVS_ = 2,
		_KV_ = 3,
		_VALUE_ = 4,
		_UNITS_ = 5,
		_UNIT_ = 6
	};
	static const size_t StateCount;
	static const size_t NonTerminalCount;
	static const size_t TerminalCount;
	static const size_t RulesCount;
	static const int GOTO[30][7];
	static const int ACTION[30][25];
	static const int RulesToSymbol[19];
	static const int RulesLength[19];
	static const char* const RulesName[19];
	static const int Implicit[19];
};






int dictionary::build(FILE* fp)
{
	//Morpheme eme;
	int error;
	clear();
	errorCode = buildUndone;
	error = LexicalSource.Build<DictReg>(fp);
	if (error != 0)
	{
		//errorCode = ErrorinputLEXICAL;
		return error;
	}
	NeglectNullToken(LexicalSource);
	//LexicalSource.Demo(stdout);
	error = buildGanalysis(LexicalSource);
	if (error != 0) return error;
	errorCode = NoError;
	LexicalSource.clear();
	return 0;
}
int dictionary::build(const char* input)
{
	//Morpheme eme;
	int error;
	clear();
	errorCode = buildUndone;
	//initial();
	error = LexicalSource.Build<DictReg>(input);
	if (error != 0)
	{
		//errorCode = ErrorinputLEXICAL;
		return error;
	}
	NeglectNullToken(LexicalSource);
	//LexicalSource.Demo(stdout);
	error = buildGanalysis(LexicalSource);
	if (error != 0)
	{

		return error;
	}
	errorCode = NoError;
	LexicalSource.clear();
	return 0;
}
void dictionary::move(dictionary* source)
{
	size_t i;
	clear();
	Content.recount(source->Content.count());
	for (i = 0; i < source->Content.count(); i++)
	{
		Content[i] = source->Content[i];
	}
	source->Content.clear();
}
void dictionary::move(KV*& source)
{
	Content.append(*source);
	source->Count = 0;
	source->Content = NULL;
	source->key = NULL;
	delete source;
	source = NULL;
}

void dictionary::NeglectNullToken(Morpheme& eme) const
{
	size_t i;
	DictReg::regular T;
	DictReg::group G;
	for (i = 0; i < eme.GetCount(); i++)
	{
		T = (DictReg::regular)(eme[i].accept);
		G = (DictReg::group)(eme[i].category);
		if (G == DictReg::_format___ || G == DictReg::_anntation___)
		{
			eme.valid(i) = false;
		}
	}
	return;
}
int dictionary::buildGanalysis(const Morpheme& eme)
{
	int error;
	GrammarTree Tree;
	error = Tree.build<DictPraser>(eme);
	if (error != 0)
	{
		errorCode = ErrorinputGrammar;
		errorInfor1 = Tree.error_record01;
		errorInfor2 = Tree.error_record02;
		return error;
	}
	//printf("Here?!\n");
	//Tree.Demo(stdout, eme, DictPraser::RulesName);
	//printf("Here?!\n");
	error = buildAll(eme, Tree);
	//printf("Here?!:%d\n", error);
	//Tree.Demo(stdout, eme, DictPraser::RulesName);
	//printf("Here?!\n");
	if (error != 0) return error;
	//printf("Here?!:%d\n", error);
	return error;
}
int dictionary::buildAll(const Morpheme& eme, GrammarTree& Tree)
{
	int error;
	struct tempInfor
	{
		Ktype T;
		element E;
	}*tempI;
	Ktype T;
	GLTree* GT;
	KV* KVtemp;
	GTIter iterator;
	DictPraser::rules RR;
	size_t site_;
	const char* Ctemp;
	dictionary* Dtemp;
	error = 0;
	iterator.initial(Tree.GT);
	while (iterator.still())
	{
		GT = iterator.target();
		site_ = GT->root().site;
		if (iterator.state() != 0 && GT->root().rules)
		{
			//printf("label:%zu, %zu\n", GT->root().label, (size_t)GT);
			RR = (DictPraser::rules)GT->root().site;
			switch (RR)
			{
			case DictPraser::all_all_:
				Dtemp = (dictionary*)GT->child(0)->root().infor;
				//printf("%zu:%zu\n", Content.count(), Dtemp->Content.count());
				move(Dtemp);
				//printf("%zu:%zu\n", Content.count(), Dtemp->Content.count());
				delete Dtemp;
				GT->child(0)->root().infor = NULL;
				break;
			case DictPraser::DICTIONARY_DICTIONARY_:
				GT->root().infor = GT->child(1)->root().infor;
				GT->child(1)->root().infor = NULL;
				Dtemp = (dictionary*)GT->root().infor;
				//printf("%zu\n", Dtemp->Content.count());
				break;
			case DictPraser::KVS_multi_:
				Dtemp = (dictionary*)(GT->child(0)->root().infor);
				KVtemp = (KV*)(GT->child(1)->root().infor);
				//printf("???\n");
				Dtemp->move(KVtemp);
				//printf("???\n");
				GT->child(0)->root().infor = NULL;
				GT->child(1)->root().infor = NULL;
				GT->root().infor = (void*)Dtemp;
				//printf("%zu\n", Dtemp->Content.count());
				break;
			case DictPraser::KVS_single_:
				Dtemp = new dictionary;
				KVtemp = (KV*)(GT->child(0)->root().infor);

				Dtemp->move(KVtemp);
				//printf("???\n");
				GT->child(0)->root().infor = NULL;
				GT->root().infor = (void*)Dtemp;
				//printf("%zu\n", Dtemp->Content.count());
				break;
			case DictPraser::KV_colon_:
			case DictPraser::KV_value_:
				KVtemp = (KV*)(GT->child(2)->root().infor);
				//printf("label:%zu, child(2)%zu", GT->child(2)->root().label, (size_t)GT->child(2));
				//printf("KVtemp:%zu, %zu, ", (size_t)KVtemp, KVtemp->Count);
				//if (KVtemp->T == string_) printf("_KV_value_%s\n", KVtemp->Content[0].ss);
				Ctemp = eme.GetWord(GT->child(0)->root().site);
				KVtemp->setName(Ctemp);
				GT->root().infor = (void*)KVtemp;
				GT->child(2)->root().infor = NULL;
				//printf("KVtemp%zu, <%s> \n", KVtemp->Count, Ctemp);
				break;
			case DictPraser::VALUE_multi_:
				GT->root().infor = GT->child(1)->root().infor;
				GT->child(1)->root().infor = NULL;
				break;
			case DictPraser::VALUE_single_:
				KVtemp = new KV();
				tempI = (tempInfor*)(GT->child(0)->root().infor);
				KVtemp->setType(tempI->T);
				KVtemp->append(tempI->E);
				//if (tempI->T == string_) printf("_VALUE_single_%s\n", tempI->E.ss);
				//printf("label: %zu, KVtemp(single):%zu, ", GT->root().label, (size_t)KVtemp);
				free(tempI);
				GT->root().infor = (void*)KVtemp;
				//printf("KVtemp(single):%zu, %zu\n", KVtemp->Count, (size_t)GT->root().infor);
				break;
			case DictPraser::VALUE_nul_:
				KVtemp = new KV();
				KVtemp->setType(null_);
				GT->root().infor = (void*)KVtemp;
				break;
			case DictPraser::UNITS_multi_:
				KVtemp = (KV*)(GT->child(0)->root().infor);
				GT->child(0)->root().infor = NULL;
				tempI = (tempInfor*)(GT->child(2)->root().infor);
				if (tempI->T != KVtemp->T) return -233;
				KVtemp->append(tempI->E);
				free(tempI);
				GT->root().infor = (void*)KVtemp;
				break;
			case DictPraser::UNITS_single_:
				KVtemp = new KV();
				tempI = (tempInfor*)(GT->child(0)->root().infor);
				KVtemp->setType(tempI->T);
				KVtemp->append(tempI->E);
				free(tempI);
				GT->root().infor = (void*)KVtemp;
				break;
			case DictPraser::UNIT_string_:
			case DictPraser::UNIT_integer_:
			case DictPraser::UNIT_real_:
			case DictPraser::UNIT_NULL_:
			case DictPraser::UNIT_null_:
			case DictPraser::UNIT_true_:
			case DictPraser::UNIT_false_:
			case DictPraser::UNIT_dictionary_:
				tempI = (tempInfor*)malloc(sizeof(tempInfor));
				tempI->E = buildUnit(T, eme, GT);
				tempI->T = T;
				//if (T == string_) printf("KL%s\n", tempI->E.ss);
				GT->root().infor = (void*)tempI;
				break;
			default:
				error = -7;
				return error;
				break;
			}
		}
		iterator.next();
	}
	return error;
}
dictionary::element dictionary::buildUnit(dictionary::Ktype& T, const Morpheme& eme, GLTree* GT)
{
	element E;
	DictPraser::rules RR;
	BufferChar BC;
	E.bb = true;
	RR = (DictPraser::rules)GT->root().site;
	switch (RR)
	{
	case DictPraser::UNIT_string_:
		T = string_;
		BC = eme.GetWord(GT->child(0)->root().site);
		E.ss = BC.DequeueString();
		break;
	case DictPraser::UNIT_integer_:
		T = int_;
		BC = eme.GetWord(GT->child(0)->root().site);
		E.ii = BC.DequeueInt();
		break;
	case DictPraser::UNIT_real_:
		T = float_;
		BC = eme.GetWord(GT->child(0)->root().site);
		E.ff = BC.DequeueReal();
		break;
	case DictPraser::UNIT_NULL_:
	case DictPraser::UNIT_null_:
		T = null_;
		break;
	case DictPraser::UNIT_true_:
		T = bool_;
		E.bb = true;
		break;
	case DictPraser::UNIT_false_:
		T = bool_;
		E.bb = false;
		break;
	case DictPraser::UNIT_dictionary_:
		T = dictionary_;
		E.dd = (dictionary*)(GT->child(0)->root().infor);
		GT->child(0)->root().infor = NULL;
		break;
	default:
		break;
	}
	return E;
}

int DictReg::next(int state, const char c)
{
	switch (state)
	{
	case 0:
		if (c == (char)9) return 24;
		else if (c == (char)10) return 23;
		else if (c == (char)13) return 25;
		else if (c == ' ') return 22;
		else if (c == '\"') return 39;
		else if (c == '(') return 16;
		else if (c == ')') return 17;
		else if (c == '+') return 45;
		else if (c == ',') return 12;
		else if (c == '-') return 45;
		else if (c == '.') return 11;
		else if (c == '/') return 46;
		else if ('0' <= c && c <= '9') return 2;
		else if (c == ':') return 10;
		else if (c == ';') return 9;
		else if (c == '=') return 13;
		else if ('A' <= c && c <= 'M') return 1;
		else if (c == 'N') return 28;
		else if ('O' <= c && c <= 'Z') return 1;
		else if (c == '[') return 18;
		else if (c == ']') return 19;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'e') return 1;
		else if (c == 'f') return 34;
		else if ('g' <= c && c <= 'm') return 1;
		else if (c == 'n') return 35;
		else if ('o' <= c && c <= 's') return 1;
		else if (c == 't') return 36;
		else if ('u' <= c && c <= 'z') return 1;
		else if (c == '{') return 14;
		else if (c == '}') return 15;
		else return 0;
	case 1:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'z') return 1;
		else return 0;
	case 2:
		if (c == '.') return 48;
		else if ('0' <= c && c <= '9') return 2;
		else return 0;
	case 3:
		if ('0' <= c && c <= '9') return 3;
		else if (c == 'E') return 47;
		else if (c == 'e') return 47;
		else return 0;
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
		return 0;
	case 22:
		if (c == ' ') return 22;
		else return 0;
	case 23:
		if (c == (char)10) return 23;
		else if (c == (char)13) return 25;
		else return 0;
	case 24:
		return 0;
	case 25:
		if (c == (char)10) return 23;
		else return 0;
	case 26:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'K') return 1;
		else if (c == 'L') return 8;
		else if ('M' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'z') return 1;
		else return 0;
	case 27:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'K') return 1;
		else if (c == 'L') return 26;
		else if ('M' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'z') return 1;
		else return 0;
	case 28:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'T') return 1;
		else if (c == 'U') return 27;
		else if ('V' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'z') return 1;
		else return 0;
	case 29:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'd') return 1;
		else if (c == 'e') return 6;
		else if ('f' <= c && c <= 'z') return 1;
		else return 0;
	case 30:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'k') return 1;
		else if (c == 'l') return 7;
		else if ('m' <= c && c <= 'z') return 1;
		else return 0;
	case 31:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'k') return 1;
		else if (c == 'l') return 30;
		else if ('m' <= c && c <= 'z') return 1;
		else return 0;
	case 32:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'r') return 1;
		else if (c == 's') return 40;
		else if ('t' <= c && c <= 'z') return 1;
		else return 0;
	case 33:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'k') return 1;
		else if (c == 'l') return 32;
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
		else if ('a' <= c && c <= 't') return 1;
		else if (c == 'u') return 31;
		else if ('v' <= c && c <= 'z') return 1;
		else return 0;
	case 36:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'q') return 1;
		else if (c == 'r') return 41;
		else if ('s' <= c && c <= 'z') return 1;
		else return 0;
	case 37:
		if ('0' <= c && c <= '9') return 37;
		else return 0;
	case 38:
		if ((char)0 <= c && c <= ')') return 38;
		else if (c == '*') return 44;
		else if ('+' <= c && c <= (char)127) return 38;
		else return 0;
	case 39:
		if (' ' <= c && c <= '!') return 39;
		else if (c == '\"') return 4;
		else if ('#' <= c && c <= '[') return 39;
		else if (c == '\\') return 42;
		else if (']' <= c && c <= (char)127) return 39;
		else return 0;
	case 40:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'd') return 1;
		else if (c == 'e') return 5;
		else if ('f' <= c && c <= 'z') return 1;
		else return 0;
	case 41:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 't') return 1;
		else if (c == 'u') return 29;
		else if ('v' <= c && c <= 'z') return 1;
		else return 0;
	case 42:
		if (c == (char)0) return 39;
		else if (c == '\"') return 39;
		else if (c == '\'') return 39;
		else if ('0' <= c && c <= '7') return 39;
		else if (c == '\?') return 39;
		else if (c == 'X') return 50;
		else if (c == '\\') return 39;
		else if ('a' <= c && c <= 'b') return 39;
		else if (c == 'f') return 39;
		else if (c == 'n') return 39;
		else if (c == 'r') return 39;
		else if (c == 't') return 39;
		else if (c == 'v') return 39;
		else if (c == 'x') return 50;
		else return 0;
	case 43:
		if ((char)0 <= c && c <= (char)9) return 43;
		else if (c == (char)10) return 20;
		else if ((char)11 <= c && c <= (char)127) return 43;
		else return 0;
	case 44:
		if ((char)0 <= c && c <= ')') return 38;
		else if (c == '*') return 44;
		else if ('+' <= c && c <= '.') return 38;
		else if (c == '/') return 21;
		else if ('0' <= c && c <= (char)127) return 38;
		else return 0;
	case 45:
		if ('0' <= c && c <= '9') return 2;
		else return 0;
	case 46:
		if (c == '*') return 38;
		else if (c == '/') return 43;
		else return 0;
	case 47:
		if (c == '+') return 49;
		else if (c == '-') return 49;
		else if ('0' <= c && c <= '9') return 37;
		else return 0;
	case 48:
		if ('0' <= c && c <= '9') return 3;
		else return 0;
	case 49:
		if ('0' <= c && c <= '9') return 37;
		else return 0;
	case 50:
		if ('0' <= c && c <= '9') return 39;
		else if ('A' <= c && c <= 'F') return 39;
		else if ('a' <= c && c <= 'f') return 39;
		else return 0;
	}
	return 0;
}
int DictReg::action(int state)
{
	switch (state)
	{
	case 1:
		return 1;//id: id
	case 2:
		return 2;//number: integer
	case 3:
		return 3;//number: real
	case 4:
		return 4;//string: string
	case 5:
		return 5;//reserved: false
	case 6:
		return 6;//reserved: true
	case 7:
		return 7;//reserved: null
	case 8:
		return 8;//reserved: NULL
	case 9:
		return 9;//division: semicolon
	case 10:
		return 10;//division: colon
	case 11:
		return 11;//division: dot
	case 12:
		return 12;//division: comma
	case 13:
		return 13;//value: value
	case 14:
		return 14;//braket: braceL
	case 15:
		return 15;//braket: braceR
	case 16:
		return 16;//braket: left
	case 17:
		return 17;//braket: right
	case 18:
		return 18;//braket: squareL
	case 19:
		return 19;//braket: squareR
	case 20:
		return 20;//anntation: anntationS
	case 21:
		return 21;//anntation: anntationM
	case 22:
		return 22;//format: spaces
	case 23:
		return 23;//format: enters
	case 24:
		return 24;//format: tab
	case 26:
		return 1;//id: id
	case 27:
		return 1;//id: id
	case 28:
		return 1;//id: id
	case 29:
		return 1;//id: id
	case 30:
		return 1;//id: id
	case 31:
		return 1;//id: id
	case 32:
		return 1;//id: id
	case 33:
		return 1;//id: id
	case 34:
		return 1;//id: id
	case 35:
		return 1;//id: id
	case 36:
		return 1;//id: id
	case 37:
		return 3;//number: real
	case 40:
		return 1;//id: id
	case 41:
		return 1;//id: id
	}
	return 0;
}
int DictReg::GroupGet(int accept)
{
	switch (accept)
	{
	case 1:
		return 1;//id: id
	case 2:
		return 2;//number: integer
	case 3:
		return 2;//number: real
	case 4:
		return 3;//string: string
	case 5:
		return 4;//reserved: false
	case 6:
		return 4;//reserved: true
	case 7:
		return 4;//reserved: null
	case 8:
		return 4;//reserved: NULL
	case 9:
		return 5;//division: semicolon
	case 10:
		return 5;//division: colon
	case 11:
		return 5;//division: dot
	case 12:
		return 5;//division: comma
	case 13:
		return 6;//value: value
	case 14:
		return 7;//braket: braceL
	case 15:
		return 7;//braket: braceR
	case 16:
		return 7;//braket: left
	case 17:
		return 7;//braket: right
	case 18:
		return 7;//braket: squareL
	case 19:
		return 7;//braket: squareR
	case 20:
		return 8;//anntation: anntationS
	case 21:
		return 8;//anntation: anntationM
	case 22:
		return 9;//format: spaces
	case 23:
		return 9;//format: enters
	case 24:
		return 9;//format: tab
	}
	return 0;
}





const size_t DictPraser::StateCount = 30;
const size_t DictPraser::NonTerminalCount = 7;
const size_t DictPraser::TerminalCount = 24;
const size_t DictPraser::RulesCount = 19;
const int DictPraser::GOTO[30][7] = { \
{1, 6, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 14, 18, 1, 1, 1}, \
{1, 1, 1, 114, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 34, 1, 1, 106, 1, 42}, \
{1, 34, 1, 1, 38, 1, 42}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 34, 1, 1, 1, 78, 82}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 34, 1, 1, 1, 1, 98}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1} };
//==============================
const int DictPraser::ACTION[30][25] = { \
{57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 10, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57}, \
{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{5, 22, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5}, \
{1, 22, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 118, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 11, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 11, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 26, 1, 1, 30, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 46, 50, 54, 58, 62, 66, 70, 1, 1, 1, 1, 1, 10, 1, 1, 1, 74, 1, 1, 1, 1, 1, 1}, \
{1, 1, 46, 50, 54, 58, 62, 66, 70, 1, 1, 1, 1, 1, 10, 1, 1, 1, 74, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 47, 1, 1, 47, 1, 1, 1, 1, 1, 1, 47, 1, 1, 1, 1, 1}, \
{37, 37, 37, 37, 37, 37, 37, 37, 37, 102, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37}, \
{37, 37, 37, 37, 37, 37, 37, 37, 37, 31, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 71, 1, 1, 71, 1, 1, 1, 1, 1, 1, 71, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 67, 1, 1, 67, 1, 1, 1, 1, 1, 1, 67, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 75, 1, 1, 75, 1, 1, 1, 1, 1, 1, 75, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 51, 1, 1, 51, 1, 1, 1, 1, 1, 1, 51, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 55, 1, 1, 55, 1, 1, 1, 1, 1, 1, 55, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 59, 1, 1, 59, 1, 1, 1, 1, 1, 1, 59, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 63, 1, 1, 63, 1, 1, 1, 1, 1, 1, 63, 1, 1, 1, 1, 1}, \
{1, 1, 46, 50, 54, 58, 62, 66, 70, 1, 1, 1, 1, 1, 10, 1, 1, 1, 1, 86, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 90, 1, 1, 1, 1, 1, 1, 94, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 39, 1, 1, 1, 1, 1, 1, 39, 1, 1, 1, 1, 1}, \
{37, 37, 37, 37, 37, 37, 37, 37, 37, 27, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37}, \
{1, 1, 46, 50, 54, 58, 62, 66, 70, 1, 1, 1, 1, 1, 10, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{37, 37, 37, 37, 37, 37, 37, 37, 37, 35, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 43, 1, 1, 1, 1, 1, 1, 43, 1, 1, 1, 1, 1}, \
{1, 19, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 19, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{37, 37, 37, 37, 37, 37, 37, 37, 37, 110, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37}, \
{1, 23, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 23, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 15, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 15, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{7, 1, 1, 1, 1, 1, 1, 1, 1, 7, 1, 1, 7, 1, 1, 1, 1, 1, 1, 7, 1, 1, 1, 1, 1} };
//==============================
const int DictPraser::RulesToSymbol[19] = { \
0,\
1,\
2,\
2,\
3,\
3,\
4,\
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
6,\
6 };
//==============================
const int DictPraser::RulesLength[19] = { \
1,\
3,\
1,\
2,\
4,\
4,\
2,\
1,\
3,\
1,\
3,\
1,\
1,\
1,\
1,\
1,\
1,\
1,\
1 };
//==============================
const char* const DictPraser::RulesName[19] = { \
"all->DICTIONARY ",\
"DICTIONARY->braceL KVS braceR ",\
"KVS->KV ",\
"KVS->KVS KV ",\
"KV->id value VALUE semicolon ",\
"KV->id colon VALUE semicolon ",\
"VALUE->squareL squareR ",\
"VALUE->UNIT ",\
"VALUE->squareL UNITS squareR ",\
"UNITS->UNIT ",\
"UNITS->UNITS comma UNIT ",\
"UNIT->DICTIONARY ",\
"UNIT->false ",\
"UNIT->true ",\
"UNIT->null ",\
"UNIT->NULL ",\
"UNIT->real ",\
"UNIT->integer ",\
"UNIT->string " };
//==============================
const int DictPraser::Implicit[19] = { \
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

//==============================
int complex::next(int state, const char c)
{
	switch (state)
	{
	case 0:
		if (c == '-') return 8;
		else if (c == '.') return 4;
		else if (c == '/') return 6;
		else if (c == ':') return 3;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '\\') return 7;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'z') return 1;
		else return 0;
	case 1:
		if ('0' <= c && c <= '9') return 1;
		else if ('A' <= c && c <= 'Z') return 1;
		else if (c == '_') return 1;
		else if ('a' <= c && c <= 'z') return 1;
		else return 0;
	case 2:
		return 0;
	case 3:
		if (c == ':') return 5;
		else return 0;
	case 4:
		return 0;
	case 5:
		return 0;
	case 6:
		return 0;
	case 7:
		return 0;
	case 8:
		if (c == '>') return 2;
		else return 0;
	}
	return 0;
}
int complex::action(int state)
{
	switch (state)
	{
	case 1:
		return 1;//id: id
	case 2:
		return 2;//division: arrow
	case 3:
		return 3;//division: colon
	case 4:
		return 4;//division: dot
	case 5:
		return 5;//division: colon2
	case 6:
		return 6;//division: slash
	case 7:
		return 7;//division: backslash
	}
	return 0;
}
int complex::GroupGet(int accept)
{
	switch (accept)
	{
	case 1:
		return 1;//id: id
	case 2:
		return 2;//division: arrow
	case 3:
		return 2;//division: colon
	case 4:
		return 2;//division: dot
	case 5:
		return 2;//division: colon2
	case 6:
		return 2;//division: slash
	case 7:
		return 2;//division: backslash
	}
	return 0;
}

static bool compare(const char* str1, const char* str2)
{
	size_t i;
	for (i = 0; (str1[i] != '\0') && (str1[i] == str2[i]); i++);
	return str1[i] == str2[i];
}
static char* Copy(const char* input)
{
	char* nnnn;
	size_t length, i;
	for (length = 0; input[length] != '\0'; length++);
	length += 1;
	nnnn = (char*)malloc(sizeof(char) * length);
	for (i = 0; i < length; i++) nnnn[i] = input[i];
	return nnnn;
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
