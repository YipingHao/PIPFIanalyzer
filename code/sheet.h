#include <stdlib.h>
namespace sheet
{
    struct FIL
{
	enum regular
	{
		_identifier_ = 1,
		_integer_ = 2,
		_CommonChar_ = 3,
		_idChar_ = 4,
		_spaces_ = 5,
		_enters_ = 6,
		_tab_ = 7,
		_semicolon_ = 8,
		_colon_ = 9,
		_dot_ = 10,
		_braceL_ = 11,
		_braceR_ = 12,
		_left_ = 13,
		_right_ = 14,
		_squareL_ = 15,
		_squareR_ = 16,
		_angleL_ = 17,
		_angleR_ = 18,
		_anntationS_ = 19,
		_anntationM_ = 20,
		_multi_ = 21,
		_add_ = 22,
		_assign_ = 23
	};
	enum group
	{
		_identifier___ = 1,
		_const___ = 2,
		_format___ = 3,
		_division___ = 4,
		_braket___ = 5,
		_anntation___ = 6,
		_operation___ = 7,
		_assign___ = 8
	};
	static int next(int state, const char c);
	static int action(int state);
	static int GroupGet(int state);
};


}


namespace sheet
{

    struct FIG
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
		FIS_single_ = 1,
		FIS_multiple_ = 2,
		FI_formula_ = 3,
		TOKEN_braket_ = 4,
		TOKEN_brace_ = 5,
		Lvalue_only_ = 6,
		Rvalue_only_ = 7,
		ITEMS_single_ = 8,
		ITEMS_multiple_ = 9,
		ITEM_single_ = 10,
		ITEM_multiple_ = 11
	};
	enum nonterminal
	{
		_all_ = 0,
		_FIS_ = 1,
		_FI_ = 2,
		_TOKEN_ = 3,
		_Lvalue_ = 4,
		_Rvalue_ = 5,
		_ITEMS_ = 6,
		_ITEM_ = 7
	};
	static const size_t StateCount;
	static const size_t NonTerminalCount;
	static const size_t TerminalCount;
	static const size_t RulesCount;
	static const int GOTO[22][8];
	static const int ACTION[22][24];
	static const int RulesToSymbol[12];
	static const int RulesLength[12];
	static const char* const RulesName[12];
	static const int Implicit[12];
};
}

