#include "sheet.h"


namespace sheet
{


int FIL::next(int state, const char c)
{
	switch (state)
	{
	case 0:
		if(c == (char)9) return 6;
		else if(c == (char)10) return 5;
		else if(c == (char)13) return 23;
		else if(c == ' ') return 4;
		else if(c == '\'') return 25;
		else if(c == '(') return 12;
		else if(c == ')') return 13;
		else if(c == '*') return 20;
		else if(c == '+') return 21;
		else if(c == '-') return 28;
		else if(c == '.') return 9;
		else if(c == '/') return 30;
		else if('0' <= c && c <= '9') return 2;
		else if(c == ':') return 8;
		else if(c == ';') return 7;
		else if(c == '<') return 16;
		else if(c == '=') return 22;
		else if(c == '>') return 17;
		else if('A' <= c && c <= 'Z') return 1;
		else if(c == '[') return 14;
		else if(c == ']') return 15;
		else if(c == '_') return 1;
		else if('a' <= c && c <= 'z') return 1;
		else if(c == '{') return 10;
		else if(c == '}') return 11;
		else return 0;
	case 1:
		if('0' <= c && c <= '9') return 1;
		else if('A' <= c && c <= 'Z') return 1;
		else if(c == '_') return 1;
		else if('a' <= c && c <= 'z') return 1;
		else return 0;
	case 2:
		if('0' <= c && c <= '9') return 2;
		else return 0;
	case 3:
		return 0;
	case 4:
		if(c == ' ') return 4;
		else return 0;
	case 5:
		if(c == (char)10) return 5;
		else if(c == (char)13) return 23;
		else return 0;
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
		if((char)0 <= c && c <= (char)9) return 18;
		else if((char)11 <= c && c <= (char)127) return 18;
		else return 0;
	case 19:
		return 0;
	case 20:
		return 0;
	case 21:
		if('0' <= c && c <= '9') return 2;
		else return 0;
	case 22:
		return 0;
	case 23:
		if(c == (char)10) return 5;
		else return 0;
	case 24:
		if((char)0 <= c && c <= ')') return 24;
		else if(c == '*') return 27;
		else if('+' <= c && c <= (char)127) return 24;
		else return 0;
	case 25:
		if(' ' <= c && c <= '!') return 29;
		else if('#' <= c && c <= '&') return 29;
		else if('(' <= c && c <= '[') return 29;
		else if(c == '\\') return 26;
		else if(']' <= c && c <= '~') return 29;
		else return 0;
	case 26:
		if(c == (char)0) return 29;
		else if(c == '\"') return 29;
		else if(c == '\'') return 29;
		else if('0' <= c && c <= '7') return 32;
		else if(c == '\?') return 29;
		else if(c == 'X') return 31;
		else if(c == '\\') return 29;
		else if('a' <= c && c <= 'b') return 29;
		else if(c == 'f') return 29;
		else if(c == 'n') return 29;
		else if(c == 'r') return 29;
		else if(c == 't') return 29;
		else if(c == 'v') return 29;
		else if(c == 'x') return 31;
		else return 0;
	case 27:
		if((char)0 <= c && c <= ')') return 24;
		else if(c == '*') return 27;
		else if('+' <= c && c <= '.') return 24;
		else if(c == '/') return 19;
		else if('0' <= c && c <= (char)127) return 24;
		else return 0;
	case 28:
		if('0' <= c && c <= '9') return 2;
		else return 0;
	case 29:
		if(c == '\'') return 3;
		else return 0;
	case 30:
		if(c == '*') return 24;
		else if(c == '/') return 18;
		else return 0;
	case 31:
		if('0' <= c && c <= '9') return 33;
		else if('A' <= c && c <= 'F') return 33;
		else if('a' <= c && c <= 'f') return 33;
		else return 0;
	case 32:
		if(c == '\'') return 3;
		else if('0' <= c && c <= '7') return 34;
		else return 0;
	case 33:
		if(c == '\'') return 3;
		else if('0' <= c && c <= '9') return 29;
		else if('A' <= c && c <= 'F') return 29;
		else if('a' <= c && c <= 'f') return 29;
		else return 0;
	case 34:
		if(c == '\'') return 3;
		else if('0' <= c && c <= '7') return 29;
		else return 0;
	}
	return 0;
}
int FIL::action(int state)
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
		return 4;//format: spaces
	case 5:
		return 5;//format: enters
	case 6:
		return 6;//format: tab
	case 7:
		return 7;//division: semicolon
	case 8:
		return 8;//division: colon
	case 9:
		return 9;//division: dot
	case 10:
		return 10;//braket: braceL
	case 11:
		return 11;//braket: braceR
	case 12:
		return 12;//braket: left
	case 13:
		return 13;//braket: right
	case 14:
		return 14;//braket: squareL
	case 15:
		return 15;//braket: squareR
	case 16:
		return 16;//braket: angleL
	case 17:
		return 17;//braket: angleR
	case 18:
		return 18;//anntation: anntationS
	case 19:
		return 19;//anntation: anntationM
	case 20:
		return 20;//operation: multi
	case 21:
		return 21;//operation: add
	case 22:
		return 22;//assign: assign
	}
	return 0;
}
int FIL::GroupGet(int accept)
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
		return 3;//format: spaces
	case 5:
		return 3;//format: enters
	case 6:
		return 3;//format: tab
	case 7:
		return 4;//division: semicolon
	case 8:
		return 4;//division: colon
	case 9:
		return 4;//division: dot
	case 10:
		return 5;//braket: braceL
	case 11:
		return 5;//braket: braceR
	case 12:
		return 5;//braket: left
	case 13:
		return 5;//braket: right
	case 14:
		return 5;//braket: squareL
	case 15:
		return 5;//braket: squareR
	case 16:
		return 5;//braket: angleL
	case 17:
		return 5;//braket: angleR
	case 18:
		return 6;//anntation: anntationS
	case 19:
		return 6;//anntation: anntationM
	case 20:
		return 7;//operation: multi
	case 21:
		return 7;//operation: add
	case 22:
		return 8;//assign: assign
	}
	return 0;
}


}

namespace sheet
{

const size_t FIG::StateCount = 22;
const size_t FIG::NonTerminalCount = 8;
const size_t FIG::TerminalCount = 22;
const size_t FIG::RulesCount = 12;
const int FIG::GOTO[22][8] = { \
{1, 6, 10, 14, 18, 1, 1, 1}, \
{1, 1, 86, 14, 18, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 54, 1, 58, 62, 66}, \
{1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 74, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 54, 1, 1, 1, 82}, \
{1, 1, 1, 1, 1, 1, 1, 1}, \
{1, 1, 1, 1, 1, 1, 1, 1}};
//==============================
const int FIG::ACTION[22][23] = { \
{5, 22, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5}, \
{0, 22, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{7, 7, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 27}, \
{89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 50}, \
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 26, 1, 30, 1, 1, 1, 1, 1, 1, 1, 1}, \
{9, 9, 42, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9}, \
{9, 9, 34, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9}, \
{61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 38, 61, 61, 61, 61, 61, 61, 61}, \
{19, 19, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 19, 19, 19}, \
{53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 46, 53, 53, 53, 53, 53, 53, 53, 53, 53}, \
{23, 23, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 23, 23, 23}, \
{5, 22, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5}, \
{43, 43, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 43, 43, 1}, \
{15, 15, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, \
{31, 31, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 78, 1}, \
{35, 35, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 70, 35, 1}, \
{5, 22, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5}, \
{47, 47, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 47, 47, 1}, \
{5, 22, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5}, \
{39, 39, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 70, 39, 1}, \
{11, 11, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};
//==============================
const int FIG::RulesToSymbol[12] = { \
0,\
1,\
1,\
2,\
3,\
3,\
4,\
5,\
6,\
6,\
7,\
7};
//==============================
const int FIG::RulesLength[12] = { \
1,\
1,\
2,\
3,\
4,\
4,\
1,\
1,\
1,\
3,\
1,\
3};
//==============================
const char* const FIG::RulesName[12] = { \
"all->FIS ",\
"FIS->FI ",\
"FIS->FIS FI ",\
"FI->Lvalue assign Rvalue ",\
"TOKEN->identifier squareL integer squareR ",\
"TOKEN->identifier left integer right ",\
"Lvalue->TOKEN ",\
"Rvalue->ITEMS ",\
"ITEMS->ITEM ",\
"ITEMS->ITEMS add ITEM ",\
"ITEM->TOKEN ",\
"ITEM->ITEM multi TOKEN "};
//==============================
const int FIG::Implicit[12] = { \
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
0};

}






