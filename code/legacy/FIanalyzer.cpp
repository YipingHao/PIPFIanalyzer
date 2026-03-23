#include"FIanalyzer.h"
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
using namespace FIanalyzer;
// 0 gt
// 1 lt
// 2 eq
// FI: x_1 > x_2 > x_3 > …… > x_n 单项式降序排列，所有多项式升序排列
//=====================================================================
static int* MallocZero(size_t length);
static int* MallocDfiDr(const int* List, const int Order, const int Item, const int R_i, int* Length);
static int* MallocCopy(size_t length, const int* List);
static void Copy(size_t length, const int* from, int* to);
//=====================================================================
FI_DFA::FI_DFA()
{
	state = 0;
	FormerState = 0;
	should = wait;
}
FI_DFA::~FI_DFA()
{
	state = 0;
	FormerState = 0;
	should = wait;
}
int FI_DFA::DFAstate(void)
{
	return state;
}
int FI_DFA::DFAformerState(void)
{
	return FormerState;
}
enum FI_DFA::action FI_DFA::DFAswicth(char in)
{
	FormerState = state;
	switch (state)
	{
	case 0:
		switch (in)
		{
		case '(':
			state = 1;
			break;
		case '[':
			state = 2;
			break;
		case '{':
			state = 3;
			break;
		case ')':
			state = 4;
			break;
		case ']':
			state = 5;
			break;
		case '}':
			state = 6;
			break;
		case '+':
			state = 7;
			break;
		case '=':
			state = 8;
			break;
		case '*':
			state = 9;
			break;
		case '^':
			state = 10;
			break;
			//==============================================
		default:
			if ((in >= 0 && in <= 31) || in == 127)
				state = 0;
			else if (in >= '0' && in <= '9')
				state = 11;
			else if (in == '_' || (in >= 'a' && in <= 'z') || (in >= 'A' && in <= 'Z'))
				state = 12;
			else
				state = 0;
			break;
		}
		break;
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 10:
		state = 0;
		break;
	case 9:
		if (in == '*') state = 10;
		else state = 0;
		break;
	case 11:
		if (in >= '0' && in <= '9') state = 11;
		else state = 0;
		break;
	case 12:
		if ((in >= '0' && in <= '9') || in == '_' || (in >= 'a' && in <= 'z') || (in >= 'A' && in <= 'Z')) state = 12;
		else state = 0;
		break;
	default:
		state = 0;
		break;
	}
	if (FormerState == 0)
	{
		if (state == 0)
			should = wait;
		else
			should = add;
	}
	else if (FormerState >= 1 && FormerState <= 12)
	{
		if (state == 0)
			should = retrieve;
	}
	else
	{
		if (state <= 0)
			should = halt;
	}
	return should;
}
enum FIlexicalUnit FI_DFA::DFAresult(void)
{
	switch (FormerState)
	{
	case 1:
	case 2:
	case 3:
		return Left;
	case 4:
	case 5:
	case 6:
		return Right;
	case 7:
		return Add;
	case 8:
		return Assignment;
	case 9:
		return Multi;
	case 10:
		return Power;
	case 11:
		return Integer;
	case 12:
		return Identifier;
	default:
		return ERROR;
	}
	return Assignment;
}
//=================================================================
FIgrammerDFA::FIgrammerDFA(/* args */)
{
    state = 0;
    No = 0;
    highest_order = 0;
    order_now = 0;
    item = 0;
}
FIgrammerDFA::~FIgrammerDFA()
{
    return;
}
void FIgrammerDFA::refresh(void)
{
    state = 0;
    No = 0;
    highest_order = 0;
    order_now = 0;
    item = 0;
    return;
}
int FIgrammerDFA::Site(void)
{
    return No;
}
int FIgrammerDFA::DFAstate(void)
{
    return state;
}
int FIgrammerDFA::Order(void)
{
    return highest_order;
}
int FIgrammerDFA::Item(void)
{
    return item;
}
int FIgrammerDFA::DFAswicth(enum FIlexicalUnit in, int num)
{
    switch (state)
    {
    case 0:
        if (in == Integer)
        {
            No = num;
            state = 1;
            item = 0;
        }
        else state = 0;
        break;
    case 1:
        if (in == Assignment)
        {
            state = 2;
            item = 1;
            order_now = 0;
        }
        else state = 1;
        break;
    case 2:
        if (in == Identifier)
        {
            if (num == 0) state = -1;
            else if (num == -1) state = -1;
            else
            {
                state = 3;
                order_now += 1;
            }
        }
        else state = 2;
        break;
    case 3:
        if (in == Identifier || in == Add || in == Multi || in == Power || in == -1) state = -2;
        else if (in == Integer) state = 4;
        else state = 3;
        break;
    case 4:
        if (in == Integer) state = -13;
        else if (in == Identifier)
        {
            if (num == -1)
            {
                state = 0;
                if (highest_order == 0) highest_order = order_now;
                else if (highest_order != order_now) state = -14;
            }
            else state = -15;
        }
        else if (in == Power) state = 5;
        else if (in == Add)
        {
            state = 2;
            if (highest_order == 0) highest_order = order_now;
            else if (highest_order != order_now) state = -4;
            item++;
            order_now = 0;
        }
        else if (in == Multi) state = 2;
        else state = 4;
        break;
    case 5:
        if (in == Integer)
        {
            order_now += (num - 1);
            state = 4;
        }
        else state = -5;
        break;
    default:
        break;
    }
    return state;
}




//=================================================================
//=====================================================================
FIunit::FIunit()
{
    length = 128;
    ItemLength = 32;
    FormulaAmount = 0;
    next = 0;
    list = (int*)malloc(sizeof(int) * length);
    fi_formula = (enum FIlexicalUnit*)malloc(sizeof(enum FIlexicalUnit) * length);



    formula_beign = (size_t*)malloc(sizeof(size_t) * ItemLength);
    item = (int*)malloc(sizeof(int) * ItemLength);
    order = (int*)malloc(sizeof(int) * ItemLength);
    site = (int*)malloc(sizeof(int) * ItemLength);
    head = 0;
}
FIunit::~FIunit()
{
    length = 0;
    ItemLength = 0;
    next = 0;
    FormulaAmount = 0;
    free((void*)list);
    free((void*)fi_formula);

    free((void*)formula_beign);
    free((void*)item);
    free((void*)order);
    list = NULL;
    fi_formula = NULL;
    formula_beign = NULL;
    item = NULL;
    order = NULL;
    head = 0;
}
void FIunit::build(hyperlex::CharBuffer & in)
{
    FI_DFA dfa;
    hyperlex::CharBuffer temp, now;
    enum FI_DFA::action Ac;
    FIgrammerDFA gra_dfa;
    char c;
    int state;
    temp << in;
    fi = "";
    bondlength = "";
    //cout << ":::::" << endl;
    //temp.print(stdout, 128);
    //cout << "aaaa" << endl;
    c = temp.Dequeue();
    for (; c != (char)EOF; )
    {
        Ac = dfa.DFAswicth(c);
        if (Ac == FI_DFA::retrieve)
        {
            //if (fi == "") now.print(stdout, 128);
            append(now, dfa.DFAresult());
            //cout << "e" << endl;
            now.Clear();
            if (list[next - 1] == 0 && fi_formula[next - 1] == Identifier)
            {
                //cout << "new formula" << endl;
                if (bondlength != "")
                {
                    if ((state = gra_dfa.DFAswicth(Identifier, -1)) < 0)
                        fprintf(stderr, "ERROR:build:%d!!!!\n", state);
                    item_add(gra_dfa);
                }
                gra_dfa.refresh();
                item_add();
            }
            else
            {
                if ((state = gra_dfa.DFAswicth(fi_formula[next - 1], list[next - 1])) < 0)
                {
                    fprintf(stderr, "ERROR:%d, %d!!!!\n", state, list[next - 1]);
                }
            }
        }
        else if (Ac == FI_DFA::add)
        {
            //if (fi == "") {cout<<"add" << endl;now.print(stdout, 128);}
            now << c;
            c = temp.Dequeue();
        }
        else c = temp.Dequeue();
    }
    //cout << "last_one" << endl;
    if (gra_dfa.DFAswicth(Identifier, -1) < 0)
        fprintf(stderr, "ERROR:!!!!\n");
    item_add(gra_dfa);
    correctness_check();
}
void FIunit::correctness_check(void)
{
    int min, max, i, gap;
    int* temp, * new_site;
    min = site[0];
    max = site[0];
    for (i = 1; i < FormulaAmount; i++)
    {
        if (min > site[i]) min = site[i];
        if (max < site[i]) max = site[i];
    }
    //for (i = 0; i < FormulaAmount; i++) site[i] -= min;
    gap = max - min + 1;
    temp = (int*)malloc(sizeof(int) * gap);
    new_site = (int*)malloc(sizeof(int) * gap);
    for (i = 0; i < gap; i++) temp[i] = 0;
    for (i = 0; i < FormulaAmount; i++)
    {
        temp[((site[i]) - min)] += 1;
        if (temp[((site[i]) - min)] > 1)
        {
            fprintf(stderr, "ERROR: Two polynomials have ths same index. Exit with error code 10.\n");
            exit(10);
        }
    }
    for (i = 1, new_site[0] = 0; i < gap; i++)
        new_site[i] = new_site[i - 1] + temp[i - 1];
    for (i = 0; i < FormulaAmount; i++)
        site[i] = new_site[site[i] - min];
    free((void*)temp);
    free((void*)new_site);
    return;
}
void FIunit::append(hyperlex::CharBuffer & now, enum FIlexicalUnit FI)
{
    size_t new_length;
    if (next >= length)
    {
        new_length = length + length / 4 + 8;
        list = (int*)realloc(list, new_length * sizeof(int));
        fi_formula = (enum FIlexicalUnit*)realloc(fi_formula, new_length * sizeof(enum FIlexicalUnit));
        length = new_length;
    }
    fi_formula[next] = FI;
    if (FI == Integer)
        list[next] = now.DequeueInt();//}
    else if (FI == Identifier)
    {
        if (fi == "")
        {
            //now.print(stdout);
            //cout << "fi " << now.queue_string() << endl;
            //now.print(stdout);
            fi = now.DequeueId();
            list[next] = 0;
            //item_amount++;
        }
        else if (bondlength == "")
        {
            //cout << "bondlength " << now.queue_string() << endl;
            bondlength = now.DequeueId();
            list[next] = 1;
        }
        else
        {
            if (bondlength == now.DequeueId()) list[next] = 1;
            else
            {
                //item_amount++;
                list[next] = 0;
            }
        }
    }
    else
        list[next] = -1;
    next++;
}
void FIunit::item_add(FIgrammerDFA& gra_dfa)
{
    item[FormulaAmount] = gra_dfa.Item();
    order[FormulaAmount] = gra_dfa.Order();
    site[FormulaAmount] = gra_dfa.Site();
    FormulaAmount++;
}
void FIunit::item_add(void)
{
    size_t new_length;
    if (FormulaAmount >= ItemLength)
    {
        new_length = ItemLength + ItemLength / 4 + 8;
        formula_beign = (size_t*)realloc(formula_beign, new_length * sizeof(size_t));
        item = (int*)realloc(item, new_length * sizeof(int));
        order = (int*)realloc(order, new_length * sizeof(int));
        site = (int*)realloc(site, new_length * sizeof(int));
        ItemLength = new_length;
    }
    formula_beign[FormulaAmount] = next;
    return;
}
void FIunit::print(FILE* fp)
{
    size_t i, j;
    for (i = 0, j = 0; i < next; i++, j++)
    {
        if (j % 31 == 0) fprintf(fp, "\n");
        switch (fi_formula[i])
        {
        case Identifier:
            if (list[i] == 0)
            {
                fprintf(fp, "\n\n<Id:%s>", fi.c_str());
                j = 0;
            }
            else fprintf(fp, "<Id:%s>", bondlength.c_str());
            break;// [a-z_A-Z][a-z_A-Z0-9]*
        case Integer:
            fprintf(fp, "(%4d)", list[i]);
            break;
        case Assignment:
            fprintf(fp, "< = >");
            break;
        case Add:
            fprintf(fp, "< + >");
            break;
        case Multi:
            fprintf(fp, "< * >");
            break;
        case Power:
            fprintf(fp, "< ^ >");
            break; // ** ^
        case Left:
            fprintf(fp, "< [ >");
            break;// ( [ {
        case Right:
            fprintf(fp, "< ] >");
            break; // ) ] }:

        }
    }
    fprintf(fp, "\nTotol formula amount:%4d\n", FormulaAmount);
}
void FIunit::print(FILE* fp, int count)
{
    size_t i, j;
    for (i = 0, j = 0; i < next && (count >= 0); i++, j++)
    {
        if (j % 31 == 0) fprintf(fp, "\n");
        switch (fi_formula[i])
        {
        case Identifier:
            if (list[i] == 0)
            {
                count--;
                j = 0;
                if (count >= 0 && i == 0) fprintf(fp, "<Id:%s>", fi.c_str());
                else if (count >= 0) fprintf(fp, "\n\n<Id:%s>", fi.c_str());
            }
            else fprintf(fp, "<Id:%s>", bondlength.c_str());
            break;// [a-z_A-Z][a-z_A-Z0-9]*
        case Integer:
            fprintf(fp, "(%4d)", list[i]);
            break;
        case Assignment:
            fprintf(fp, "< = >");
            break;
        case Add:
            fprintf(fp, "< + >");
            break;
        case Multi:
            fprintf(fp, "< * >");
            break;
        case Power:
            fprintf(fp, "< ^ >");
            break; // ** ^
        case Left:
            fprintf(fp, "< [ >");
            break;// ( [ {
        case Right:
            fprintf(fp, "< ] >");
            break; // ) ] }:
        }
    }
    fprintf(fp, "\nTotol formula amount:%4d\n", FormulaAmount);
    for (i = 0, j = 0; i < FormulaAmount; i++, j++)
        fprintf(fp, "No[%4d] formula is No[%4d] fi. Its order is %4d, it has %4d items.\n", (int)i, site[i], order[i], item[i]);
}


enum FIlexicalUnit FIunit::queue(void)
{
    if (head >= next) return Identifier;
    return (fi_formula[head]);
}
int FIunit::dequeue(void)
{
    if (head >= next) return -2;
    head++;
    return list[head - 1];
}
void FIunit::reset(void)
{
    head = 0;
}
int FIunit::if_begin(void)
{
    if (head >= next) return -1;
    return (fi_formula[head] == Identifier) && (list[head] == 0);
}
void FIunit::reset(int No)
{
    No = No % FormulaAmount;
    head = formula_beign[No];
    return;
}

int FIunit::if_still(int No)
{
    return No;
}

int FIunit::Formula_amount(void)
{
    return FormulaAmount;
}
int FIunit::Order(int No)
{
    return order[No];
}
int FIunit::Item(int No)
{
    return item[No];
}
int FIunit::Site(int No)
{
    return site[No];
}
void FIunit::formula_give(int* target, int No, int length)
{
    int bond, count, writing_site, state, i;
    enum FIlexicalUnit in;
    int num;
    writing_site = 0;
    num = 0;
    count = 0;
    state = 0;
    bond = 0;
    head = formula_beign[No];
    while (writing_site < length)
    {
        //cout << head << list[head];
        in = fi_formula[head];
        num = list[head];
        switch (state)
        {
        case 0:
            if (in == Identifier && num == 1) state = 1;
            else state = 0;
            head++;
            break;
        case 1:
            if (in == Integer)
            {
                state = 2;
                bond = num;
                count = 1;
            }
            else state = 1;
            head++;
            break;
        case 2:
            if (in == Power) state = 3;
            else if (in == Add || in == Multi || (length - writing_site) == count)
            {
                state = 0;
                for (; count > 0; count--, writing_site++)
                {
                    if (writing_site >= length)
                        fprintf(stderr, "ERROR form {void FIunit::formula_give(int*target, int No, int length):} code 20\n");
                    target[writing_site] = bond;
                }
            }
            else state = 2;
            head++;
            break;
        case 3:
            if (in == Integer)
            {
                state = 2;
                count += num - 1;
            }
            else state = 3;
            head++;
            break;
        }
    }
}


FIstruct::FIstruct()
{
    FIamount = 0;
    differential_amount = 0;
    length = NULL;
    order = NULL;
    ItemAmount = NULL;
    formula = NULL;

    if_differential = NULL;
    differential_count = NULL;
    differential_offset = NULL;
    differ_count_per_fi = NULL;

    differential_length = NULL;
    differential = NULL;

    highest_order = 0;
    order_amount = NULL;
    workload = NULL;
}
FIstruct::~FIstruct()
{
    int i;
#ifdef under_debug_1
    cout << "free((void*)length);" << endl;
#endif // under_debug_1
    free((void*)length);
    free((void*)order);
    free((void*)ItemAmount);
    for (i = 0; i < FIamount; i++)
        free((void*)(*(formula + i)));
    free((void*)formula);

    length = NULL;
    order = NULL;
    ItemAmount = NULL;
    formula = NULL;

#ifdef under_debug_1
    cout << "if_differential;" << endl;
#endif // under_debug_1

    free((void*)if_differential);
    free((void*)differential_count);
    free((void*)differ_count_per_fi);
    differ_count_per_fi = NULL;
    free((void*)differential_offset);
    differential_offset = NULL;

#ifdef under_debug_1
    cout << "differential;" << endl;
#endif // under_debug_1

    for (i = 0; i < differential_amount; i++)
        free(differential[i]);
    free((void*)differential);
    free((void*)differential_length);
    differential_length = NULL;
    differential = NULL;

    //differential_amount = 0;

#ifdef under_debug_1
    cout << "free((void*)order_amount);" << endl;
#endif // under_debug_1

    highest_order = 0;
    if (order_amount != NULL) free((void*)order_amount);
    order_amount = NULL;
    if (workload != NULL) free((void*)workload);
    workload = NULL;
}
void FIstruct::build(FILE* fp)
{
    FIunit FI;
    hyperlex::CharBuffer fi_buffer;
    fi_buffer << fp;
    FI.build(fi_buffer);
    //cout << "???" << endl;
    build(FI);
    return;
}
void FIstruct::build(FIunit& FI)
{
#ifdef under_debug_1
    cout << "Build00Initiate(FI);" << endl;
#endif // under_debug_1
    Build00Initiate(FI);
#ifdef under_debug_1
    cout << "Build01BondAmount();" << endl;
#endif // under_debug_1
    Build01BondAmount();
#ifdef under_debug_1
    cout << "Build02DifferentialInfro();" << endl;
#endif // under_debug_1
    Build02DifferentialInfro();
#ifdef under_debug_1
    cout << "Build03OrderInfro();" << endl;
#endif // under_debug_1
    Build03OrderInfro();
#ifdef under_debug_1
    cout << "Build04Differential();" << endl;
#endif // under_debug_1
    Build04Differential();
#ifdef under_debug_1
    cout << "Build05Workload();" << endl;
#endif // under_debug_1
    Build05Workload();
#ifdef under_debug_1
    cout << "return;" << endl;
#endif // under_debug_1
    return;
}
void FIstruct::Build00Initiate(FIunit& FI)
{
    int i;
    FIamount = FI.Formula_amount();
    length = (int*)malloc(sizeof(int) * FIamount);
    order = (int*)malloc(sizeof(int) * FIamount);
    ItemAmount = (int*)malloc(sizeof(int) * FIamount);
    formula = (int**)malloc(sizeof(int*) * FIamount);
    for (i = 0; i < FIamount; i++)
        formula[i] = NULL;
    for (i = 0; i < FIamount; i++)
    {
        order[FI.Site(i)] = FI.Order(i);
        ItemAmount[FI.Site(i)] = FI.Item(i);
        //cout << FI.Order(i) << " " << FI.Item(i) << endl;
        length[FI.Site(i)] = ItemAmount[FI.Site(i)] * order[FI.Site(i)];
        formula[FI.Site(i)] = (int*)malloc(sizeof(int) * length[FI.Site(i)]);
        //cout << FI.Order(i) << " " << FI.Item(i) << endl;
        FI.formula_give(formula[FI.Site(i)], i, length[FI.Site(i)]);
        //cout << FI.Order(i) << " " << FI.Item(i) << endl;
    }
    for (i = 0; i < FIamount; i++)
        if (formula[i] == NULL)
            fprintf(stderr, "ERROR:{void fi_formula::build(fi_unit & FI)}\n");
    return;
}
void FIstruct::Build01BondAmount(void)
{
    int min, max;
    int i, j, * list;
    int* index;
    differential_amount = 0;
    min = formula[0][0];
    max = min;
    for (i = 0; i < FIamount; i++)
    {
        list = formula[i];
        for (j = 0; j < length[i]; j++)
        {
            if (max < list[j]) max = list[j];
            if (min > list[j]) min = list[j];
        }
    }
    BondLengthAmount = max - min + 1;
    index = (int*)malloc(sizeof(int) * BondLengthAmount);
    for (i = 0; i < BondLengthAmount; i++) index[i] = 0;
    for (i = 0; i < FIamount; i++)
    {
        list = formula[i];
        for (j = 0; j < length[i]; j++) index[list[j] - min] = 1;
    }
    for (i = 0; i < BondLengthAmount; i++)
        if (index[i] == 0)
        {
            fprintf(stderr, "ERROR, some bond lengths are missing from fi input file.\n");
            exit(3);
        }
    for (i = 0; i < FIamount; i++)
    {
        list = formula[i];
        for (j = 0; j < length[i]; j++) list[j] -= min;
    }
    free((void*)index);
}
void FIstruct::Build02DifferentialInfro(void)
{
    int i, j, * list, site;
    int* index;
    differential_offset = MallocZero(FIamount * BondLengthAmount);
    if_differential = MallocZero(FIamount * BondLengthAmount);
    differential_count = MallocZero(BondLengthAmount);
    differ_count_per_fi = MallocZero(FIamount);
    for (i = 0; i < FIamount; i++)
    {
        list = formula[i];
        index = if_differential + i * BondLengthAmount;
        for (j = 0; j < length[i]; j++) index[list[j]] = 1;
    }
    for (i = 0; i < FIamount; i++)
        for (j = 0; j < BondLengthAmount; j++)
        {
            differ_count_per_fi[i] += if_differential[i * BondLengthAmount + j];
            differential_count[j] += if_differential[i * BondLengthAmount + j];
        }

    for (j = 0; j < BondLengthAmount; j++) differential_amount += differential_count[j];
    for (i = 0, site = 0; i < FIamount * BondLengthAmount; i++)
    {
        differential_offset[i] = site;
        site += if_differential[i];
    }
}
void FIstruct::Build03OrderInfro(void)
{
    int i;
    for (i = 0; i < FIamount; i++)
        if (highest_order < order[i]) highest_order = order[i];
    order_amount = (int*)malloc(sizeof(int) * (highest_order + 1));
    for (i = 0; i <= highest_order; i++) order_amount[i] = 0;
    for (i = 0; i < FIamount; i++) order_amount[order[i]] += 1;
}
void FIstruct::Build04Differential(void)
{
    int* List;
    int Order;
    int Item;
    int R_i;
    int* Length;
    int i, j, site;
    int judge;
    differential = (int**)malloc(sizeof(int*) * differential_amount);
    differential_length = (int*)malloc(sizeof(int) * differential_amount);
    for (i = 0; i < FIamount; i++)
        for (j = 0; j < BondLengthAmount; j++)
        {
            judge = if_differential[i * BondLengthAmount + j];
            if (!judge) continue;
            site = differential_offset[i * BondLengthAmount + j];
            List = formula[i];
            Order = order[i];
            Item = ItemAmount[i];
            R_i = j;
            Length = differential_length + site;
            differential[site] = MallocDfiDr(List, Order, Item, R_i, Length);
        }
#ifdef under_debug_1
    cout << "site = " << site << "  differential_amount = " << differential_amount << endl;
#endif // under_debug_1
}
void FIstruct::Build05Workload(void)
{
    unsigned int i, j, load, site;
    workload = (int*)malloc(sizeof(int) * FIamount);
    for (j = 0; j < BondLengthAmount; j++)
    {
        load = 0;
        for (i = 0; i < FIamount; i++)
        {
            if (if_differential[i * BondLengthAmount + j])
            {
                site = differential_offset[i * BondLengthAmount + j];
                load += differential_length[site];
            }
        }
        workload[j] = load;
    }
}
int FIstruct::dfi_scale(int No_fi)
{
    return 0;
}
int FIstruct::dfi_dr_scale(unsigned int fi_No, unsigned int R_No)
{
    int site;
    site = differential_offset[fi_No * BondLengthAmount + R_No];
    return differential_length[site];
}
int* FIstruct::dfi_dr(unsigned int fi_No, unsigned int R_No)
{
    int site;
    site = differential_offset[fi_No * BondLengthAmount + R_No];
    return differential[site];
}
void FIstruct::print(FILE* fp)
{
    int i, j;
    int total_count;
    total_count = 0;
    for (i = 0; i < FIamount; i++)
    {
        fprintf(fp, "No.%5d: order:%5d, item:%5d, length:%5d\n", i, order[i], ItemAmount[i], length[i]);
        total_count += length[i];
        for (j = 0; j < length[i]; j++)
        {
            fprintf(fp, "%5d", formula[i][j]);
            if (j % order[i] == (order[i] - 1))
                fprintf(fp, "   ");
            if (j % (8 * order[i]) == ((8 * order[i]) - 1))
                fprintf(fp, "\n");
        }
        if (j % (8 * order[i]) != 0)
            fprintf(fp, "\n");
    }
    fprintf(fp, "total %5d fundamental invariants formula.\n", total_count);
    fprintf(fp, "total %5d fundamental invariants' differential versus bondlength.\n", differential_amount);
}
void FIstruct::print(void)
{
    int i, j;
    int total_count;
    total_count = 0;
    printf("total %5d fundamental invariants formula.\n", FIamount);
    printf("total %5d fundamental invariants' differential versus bondlength.\n", differential_amount);
    for (i = 0; i <= highest_order; i++)
        printf("There are %5d order %5d FI.\n", order_amount[i], i);
    for (i = 0; i < FIamount; i++) total_count += workload[i];
    printf("There are about %7d floating-point calculations per PES referring to FI.\n", total_count);
    for (i = 0; i < FIamount; i++) total_count -= length[i];
    printf("%d of them are in the process of computation of fundamental invariants' differential versus bondlength.\n", total_count);
}


FIformula::FIformula()
{
    FIamount = 0;
    //differential_amount = 0;
    length = NULL;
    order = NULL;
    ItemAmount = NULL;
    formula = NULL;

    //if_differential = NULL;
    //differential_count = NULL;
    //differential_offset = NULL;
    //differ_count_per_fi = NULL;

   //differential_length = NULL;
    //differential = NULL;

    //highest_order = 0;
   //order_amount = NULL;
    //workload = NULL;
}
FIformula::~FIformula()
{
    int i;
#ifdef under_debug_1
    cout << "free((void*)length);" << endl;
#endif // under_debug_1
    free((void*)length);
    free((void*)order);
    free((void*)ItemAmount);
    for (i = 0; i < FIamount; i++)
        free((void*)(*(formula + i)));
    free((void*)formula);

    length = NULL;
    order = NULL;
    ItemAmount = NULL;
    formula = NULL;

#ifdef under_debug_1
    cout << "if_differential;" << endl;
#endif // under_debug_1

    //free((void*)if_differential);
    //free((void*)differential_count);
    //free((void*)differ_count_per_fi);
    //differ_count_per_fi = NULL;
    //free((void*)differential_offset);
    //differential_offset = NULL;

#ifdef under_debug_1
    cout << "differential;" << endl;
#endif // under_debug_1

    //for (i = 0; i < differential_amount; i++)
     //   free(differential[i]);
    //free((void*)differential);
    //free((void*)differential_length);
    //differential_length = NULL;
    //differential = NULL;

    //differential_amount = 0;

#ifdef under_debug_1
    cout << "free((void*)order_amount);" << endl;
#endif // under_debug_1

    //highest_order = 0;
    //if (order_amount != NULL) free((void*)order_amount);
    //order_amount = NULL;
    //if (workload != NULL) free((void*)workload);
    //workload = NULL;
}
void FIformula::build(void)
{
    BondSort();
}
void FIformula::input(FIformula& old)
{
    int i;
    FIamount = old.FIamount;
    length = (int*)malloc(sizeof(int) * FIamount);
    order = (int*)malloc(sizeof(int) * FIamount);
    ItemAmount = (int*)malloc(sizeof(int) * FIamount);
    formula = (int**)malloc(sizeof(int*) * FIamount);
    Copy(FIamount, old.length, length);
    Copy(FIamount, old.order, order);
    Copy(FIamount, old.ItemAmount, ItemAmount);
    for (i = 0; i < FIamount; i++)
        formula[i] = MallocCopy(old.length[i], old.formula[i]);
}
void FIformula::input(const char* file)
{
    hyperlex::CFile CF;
    FILE* fp;
    fp = CF.OpenRead(file);
    input(fp);
    fclose(fp);
}
void FIformula::input(FILE* fp)
{
    FIunit FI;
    hyperlex::CharBuffer fi_buffer;
    std::cout << "fi_buffer << fp;" << std::endl;
    fi_buffer << fp;
    std::cout << "FI.build(fi_buffer);" << std::endl;
    FI.build(fi_buffer);
    std::cout << "input(FI);" << std::endl;
    input(FI);
    std::cout << "return;" << std::endl;
    return;
}
void FIformula::input(FIunit & FI)
{
#ifdef under_debug_1
    cout << "Build00Initiate(FI);" << endl;
#endif // under_debug_1
    Build00Initiate(FI);
#ifdef under_debug_1
    cout << "Build01BondAmount();" << endl;
#endif // under_debug_1
    BondSort();
#ifdef under_debug_1
    cout << "Build02DifferentialInfro();" << endl;
#endif // under_debug_1
    //Build02DifferentialInfro();
#ifdef under_debug_1
    cout << "Build03OrderInfro();" << endl;
#endif // under_debug_1
    //Build03OrderInfro();
#ifdef under_debug_1
    cout << "Build04Differential();" << endl;
#endif // under_debug_1
    //Build04Differential();
#ifdef under_debug_1
    cout << "Build05Workload();" << endl;
#endif // under_debug_1
    //Build05Workload();
#ifdef under_debug_1
    cout << "return;" << endl;
#endif // under_debug_1
    return;
}
void FIformula::Build00Initiate(FIunit& FI)
{
    int i;
    FIamount = FI.Formula_amount();
    length = (int*)malloc(sizeof(int) * FIamount);
    order = (int*)malloc(sizeof(int) * FIamount);
    ItemAmount = (int*)malloc(sizeof(int) * FIamount);
    formula = (int**)malloc(sizeof(int*) * FIamount);
    for (i = 0; i < FIamount; i++)
        formula[i] = NULL;
    for (i = 0; i < FIamount; i++)
    {
        order[FI.Site(i)] = FI.Order(i);
        ItemAmount[FI.Site(i)] = FI.Item(i);
        //cout << FI.Order(i) << " " << FI.Item(i) << endl;
        length[FI.Site(i)] = ItemAmount[FI.Site(i)] * order[FI.Site(i)];
        formula[FI.Site(i)] = (int*)malloc(sizeof(int) * length[FI.Site(i)]);
        //cout << FI.Order(i) << " " << FI.Item(i) << endl;
        FI.formula_give(formula[FI.Site(i)], i, length[FI.Site(i)]);
        //cout << FI.Order(i) << " " << FI.Item(i) << endl;
    }
    for (i = 0; i < FIamount; i++)
        if (formula[i] == NULL)
            fprintf(stderr, "ERROR:{void fi_formula::build(fi_unit & FI)}\n");

    return;
}
void FIformula::BondSort(void)
{
    int min, max;
    int i, j, * list;
    int* index;
    int* prefix;
    int BondCount;
    //differential_amount = 0;
    min = formula[0][0];
    max = min;
    for (i = 0; i < FIamount; i++)
    {
        list = formula[i];
        for (j = 0; j < length[i]; j++)
        {
            if (max < list[j]) max = list[j];
            if (min > list[j]) min = list[j];
        }
    }
    BondCount = max - min + 1;
    index = (int*)malloc(sizeof(int) * BondCount);
    prefix = (int*)malloc(sizeof(int) * BondCount);
    for (i = 0; i < BondCount; i++) index[i] = 0;
    for (i = 0; i < FIamount; i++)
    {
        list = formula[i];
        for (j = 0; j < length[i]; j++) index[list[j] - min] = 1;
    }
    prefix[0] = 0;
    for (i = 1; i < BondCount; i++) prefix[i] = prefix[i - 1] + index[i - 1];
    //for (i = 0; i < BondLengthAmount; i++)
    //    if (index[i] == 0)
    //    {
    //        fprintf(stderr, "ERROR, some bond lengths are missing from fi input file.\n");
    //        exit(3);
    //    }
    for (i = 0; i < FIamount; i++)
    {
        list = formula[i];
        //for (j = 0; j < length[i]; j++) list[j] = prefix[list[j] - min];
        for (j = 0; j < length[i]; j++) list[j] -= min;
    }
    //BondLengthAmount = prefix[BondCount - 1] + index[BondCount - 1];
    BondLengthAmount = BondCount;
    free((void*)index);
    free((void*)prefix);
}

size_t FIformula::BondCount(void)
{
    return BondLengthAmount;
}
size_t FIformula::FICount(void)
{
    return FIamount;
}
int FIformula::OrderGet(size_t No)
{
    return order[No];
}

void FIformula::print(FILE* fp)
{
    int i, j;
    int total_count;
    total_count = 0;
    for (i = 0; i < FIamount; i++)
    {
        fprintf(fp, "No.%5d: order:%5d, item:%5d, length:%5d\n", i, order[i], ItemAmount[i], length[i]);
        total_count += length[i];
        print(fp, i, formula[i], order[i], ItemAmount[i]);
        
    }
    fprintf(fp, "total %d fundamental invariants formula.\n", total_count);
    //fprintf(fp, "total %5d fundamental invariants' differential versus bondlength.\n", differential_amount);
}
void FIformula::print(FILE* fp, int No, int* poly, int Order, int Item)
{
    int i, j;
    fprintf(fp, "P[%d] = r[%d]", No, poly[0]);
    for (j = 1; j < Order; j++)
        fprintf(fp, " * r[%d]", poly[j]);
    for (i = 1; i < Item; i++)
    {
        fprintf(fp, " + r[%d]", poly[i * Order]);
        for (j = 1; j < Order; j++)
            fprintf(fp, " * r[%d]", poly[i * Order + j]);
    }
    fprintf(fp, "\n");
}
void FIformula::print(void)
{
    int i, j;
    int total_count;
    total_count = 0;
    printf("total %5d fundamental invariants formula.\n", FIamount);
    //printf("total %5d fundamental invariants' differential versus bondlength.\n", differential_amount);
    //for (i = 0; i <= highest_order; i++)
    //    printf("There are %5d order %5d FI.\n", order_amount[i], i);
    //for (i = 0; i < FIamount; i++) total_count += workload[i];
   // printf("There are about %7d floating-point calculations per PES referring to FI.\n", total_count);
    //for (i = 0; i < FIamount; i++) total_count -= length[i];
    //printf("%d of them are in the process of computation of fundamental invariants' differential versus bondlength.\n", total_count);
}

void FIformula::Append(FIformula& FI)
{
    size_t NewAmount, i;
    NewAmount = FIamount + FI.FIamount;
    length = (int*)realloc(length, sizeof(int) * NewAmount);
    order = (int*)realloc(order, sizeof(int) * NewAmount);
    ItemAmount = (int*)realloc(ItemAmount, sizeof(int) * NewAmount);
    formula = (int**)realloc(formula, sizeof(int*) * NewAmount);
    Copy(FI.FIamount, FI.length, length + FIamount);
    Copy(FI.FIamount, FI.order, order + FIamount);
    Copy(FI.FIamount, FI.ItemAmount, ItemAmount + FIamount);
    for (i = 0; i < FI.FIamount; i++)
        formula[i + FIamount] = MallocCopy(FI.length[i], FI.formula[i]);
    FIamount = NewAmount;
}
void FIformula::Sort(void)
{
    // 0 gt
    // 1 lt
    // 2 eq
    int i, j, k;
    for (i = 0; i < FIamount; i++)
        sortFI(formula[i], length[i], order[i], ItemAmount[i]);
    for (i = FIamount - 1; i != 0; i--)
    {
        k = 0;
        for (j = 1; j <= i; j++) if (compare(k, j) == 1) k = j;
        if (k != i) SwapPoly(i, k);
    }
}
void FIformula::sortFI(int* poly, int L, int Order, int Item)
{
    int i, j, k;
    if (Order > 1)
    {
        for (i = 0; i < Item; i++)
            SortItem(poly + i * Order, Order);
    }
    for (i = Item - 1; i != 0; i--)
    {
        k = 0;
        for (j = 1; j <= i; j++)
            if (CompareItem(poly + Order * k, poly + Order * j, Order) == 0) k = j;  
        if (k != i)
            SwapItem(poly, Order, i, k);
    }
}
int FIformula::CompareItem(const int* left, const int* right, int Order)
{
    int i;
    for (i = 0; i < Order; i++)
        if (left[i] != right[i]) return left[i] > right[i];
    return 2;
}
void FIformula::SortItem(int* mono, int Order)
{
    int i, j, temp, k;
    for (i = Order - 1; i != 0; i--)
    {
        k = 0;
        for (j = 1; j <= i; j++)
            if (mono[j] > mono[k]) k = j;
        if (k != i)
        {
            temp = mono[k];
            mono[k] = mono[i];
            mono[i] = temp;
        }
    }
}
void FIformula::SwapItem(int* poly, int Order, int i, int j)
{
    int temp;
    int k;
    for (k = 0; k < Order; k++)
    {
        temp = poly[i * Order + k];
        poly[i * Order + k] = poly[j * Order + k];
        poly[j * Order + k] = temp;
    }
}
void FIformula::SwapPoly(int i, int j)
{
    int* temp;
    int middle;
    temp = formula[i];
    formula[i] = formula[j];
    formula[j] = temp;
    //========================
    middle = length[i];
    length[i] = length[j];
    length[j] = middle;
    //========================
    middle = order[i];
    order[i] = order[j];
    order[j] = middle;
    //========================
    middle = ItemAmount[i];
    ItemAmount[i] = ItemAmount[j];
    ItemAmount[j] = middle;
}
int FIformula::compare(int i, int j)
{
    int* left;
    int* right;
    int L, k;
    if (order[i] != order[j])
        return order[i] < order[j];
    left = formula[i];
    right = formula[j];
    if (length[i] == length[j])
    {
        L = length[i];
        for (k = 0; k < L; k++) 
            if (left[k] != right[k]) break;
        if (k == L) return 2;
    }
    return CompareItem(left, right, order[i]);
}

int FIformula::compare(const int* left, int OrderL, int ItemL, const int* right, int OrderR, int ItemR)
{
    int k;
    if (OrderL != OrderR)
        return OrderL < OrderR;
    if (ItemL == ItemR)
    {
        for (k = 0; k < OrderL * ItemL; k++)
            if (left[k] != right[k]) break;
        if (k == OrderL * ItemL) return 2;
    }
    return CompareItem(left, right, OrderL);
}
void FIformula::compare(FILE* fp, FIformula& background)
{
    int i, j, count, cc;
    fprintf(fp, "total %d fundamental invariants formula.\n", FIamount);
    for (i = 0; i < FIamount; i++)
    {
        count = 0;
        fprintf(fp, "P[%d] = ", i);
        for (j = 0; j < background.FIamount; j++)
        {
            cc = compare(formula[i], order[i], ItemAmount[i], background.formula[j], background.order[j], background.ItemAmount[j]);
            if (cc == 2)
            {
                fprintf(fp, " B[%d] ", j);
                count += 1;
            }
        }
        fprintf(fp, "\n");
    }
}
double FIformula::compute(size_t No, const double* R)
{
    double F, II;
    int i, j, *ff, Order;
    F = 0.0;
    ff = formula[No];
    Order = order[No];
    for (i = 0; i < ItemAmount[No]; i++)
    {
        II = 1.0;
        for (j = 0; j < Order; j++)
            II *= R[ff[i * Order + j]];
        F += II;
    }
    return F;
}
std::string FIformula::CSingleConfig(size_t No, size_t TAB, const char* RR, const char* FI, const char* middle)
{
    double F, II;
    int i, j, * ff, Order;
    std::string output;
    std::string tab;
    F = 0.0;
    ff = formula[No];
    Order = order[No];
    for (i = 0; i < TAB; i++) tab += '\t';
    for (i = 0; i < ItemAmount[No]; i++)
    {
        output += tab;
        output += middle;
        output += " = ";
        output += RR;
        output += '[';
        output += std::to_string(ff[i * Order]);
        output += "];\n";
        for (j = 1; j < Order; j++)
        {
            output += tab;
            output += middle;
            output += " *= ";
            output += RR;
            output += '[';
            output += std::to_string(ff[i * Order + j]);
            output += "];\n";
        }
        output += tab;
        output += FI;
        if (i == 0)
        {
            output += " = ";
        }
        else
        {
            output += " += ";
        }
        output += middle;
        output += ";\n";
    }
    return output;
}
std::string FIformula::CSingleConfig(const char* funcName, const char* RR, const char* FI)
{
    size_t i;
    std::string output;
    output += "void ";
    output += funcName;
    output += "(const double*";
    output += RR;
    output += ", double*";
    output += FI;
    output += ")\n{\n";
    output += "\tdouble temp;\n";
    output += "\tdouble FIreg;\n";
    for (i = 0; i < FIamount; i++)
    {
        output += CSingleConfig(i, 1, RR, "FIreg", "temp");
        output += "\t";
        output += FI;
        output += "[";
        output += std::to_string(i);
        output += "] = FIreg;\n\n";
    }

    output += "\treturn;\n}\n";
    return output;
}
std::string FIformula::CSingleConfig(size_t thread, const char* funcName, const char* RR, const char* FI)
{
    size_t i, j, Min;
    std::string output;
    size_t* WorkLoad;
    size_t* partitions;
    output += "void ";
    output += funcName;
    output += "(int thread, const double*";
    output += RR;
    output += ", double*";
    output += FI;
    output += ")// thread count";
    output += std::to_string(thread);
    output += "\n{\n";
    output += "\tdouble temp;\n";
    output += "\tdouble FIreg;\n";
    WorkLoad = (size_t*)malloc(sizeof(size_t) * thread);
    partitions = (size_t*)malloc(sizeof(size_t) * FIamount);
    for (i = 0; i < thread; i++) WorkLoad[i] = 0;
    for (i = 0; i < FIamount; i++)
    {
        Min = 0;
        for (j = 1; j < thread; j++)
            if (WorkLoad[j] < WorkLoad[Min]) Min = j;
        partitions[i] = Min;
        WorkLoad[Min] += length[i];
    }
    output += "\tswitch(thread)\n\t{\n";
    for (i = 0; i < thread; i++)
    {
        output += "\tcase ";
        output += std::to_string(i);
        output += ":\n";
        for (j = 0; j < FIamount; j++)
        {
            if (partitions[j] != i)continue;
            output += "\t\t//=============================FI[";
            output += std::to_string(j);
            output += "]\n";
            output += CSingleConfig(j, 2, RR, "FIreg", "temp");
            output += "\t\t";
            output += FI;
            output += "[";
            output += std::to_string(i);
            output += "] = FIreg;\n\n";
        }
        output += "\t\tbreak;\n\n";
    }
    output += "\t}\n";
    output += "\treturn;\n}\n";
    free(WorkLoad);
    free(partitions);
    return output;
}





static int* MallocZero(size_t length)
{
    int* list_out;
    size_t i;
    list_out = (int*)malloc(sizeof(int) * length);
    for (i = 0; i < length; i++) list_out[i] = 0;
    return list_out;
}
static int* MallocDfiDr(const int* List, const int Order, const int Item, const int R_i, int* Length)
{
    int site, i, j, item, length;
    int* amount, * list;
    int state, now;
    item = 0;
    //order = Order - 1;
    amount = (int*)malloc(sizeof(int) * Item);
    for (i = 0; i < Item; i++)
    {
        amount[i] = 0;
        for (j = 0; j < Order; j++)
            if (List[i * Order + j] == R_i) amount[i] += 1;
        if (amount[i] != 0) item++;
    }
    length = Order * item;
    *Length = length;
    if (item != 0)
    {
        list = (int*)malloc(sizeof(int) * length);
        for (i = 0, site = 0; i < Item; i++)
        {
            if (amount[i] != 0)
            {
                list[site * Order] = amount[i];
                state = 1;
                for (j = 0; j < Order; j++)
                {
                    now = List[i * Order + j];
                    state = (state && (state != 2)) * ((now == R_i) + 1);
                    /*Actually is: (state = 0, 1, 2)
                    if (state == 1)
                        state = (now == R_i ? 2 : 1);// (now == R_i) + 1
                    else if (state == 2)
                        state = 0;
                    else if (state == 0) state = 0;
                    */
                    //===========================
                    if (state != 2)
                        list[site * Order + state + j] = now;
                }
                //for (j = 0; j < Order && List[i * Order + j] != R_i; j++) list[site * Order + 1 + j] = List[i * Order + j];
                //for (j += 1; j < Order; j++) list[site * Order + j] = List[i * Order + j];
                site++;
            }
        }
    }
    else
        list = NULL;
    free(amount);
    return list;
}
static int* MallocCopy(size_t length, const int* List)
{
    int* NewList;
    size_t i;
    NewList = (int*)malloc(sizeof(int) * length);
    for (i = 0; i < length; i++) NewList[i] = List[i];
    return NewList;
}
static void Copy(size_t length, const int* from, int* to)
{
    size_t i;
    for (i = 0; i < length; i++) to[i] = from[i];
}


#include <time.h>
void FIanalyzer::FItransform(const char* FIname, const char* DataName, const char* OutputName)
{
    FILE* FIfile;
    FILE* DataFile;
    FILE* OutputFile;
    hyperlex::CFile CF;
    FIfile = CF.OpenRead(FIname);
    DataFile = CF.OpenRead(DataName);
    OutputFile = CF.OpenWriteAtRear(OutputName);
    FItransform(FIfile, DataFile, OutputFile);
    fclose(FIfile);
    fclose(DataFile);
    fclose(OutputFile);
}
void FIanalyzer::FItransformCsingle(size_t thread, const char* FIname, const char* funcName, const char* OutputName)
{
    FILE* FIfile;
    FILE* OutputFile;
    hyperlex::CFile CF;
    FIfile = CF.OpenRead(FIname);
    OutputFile = CF.OpenWritePlus(OutputName);
    FItransformCsingle(thread, FIfile, funcName, OutputFile);
    fclose(FIfile);
    fclose(OutputFile);
}

#include<cmath>

void FIanalyzer::FItransformCsingle(size_t thread, FILE* FIfile, const char* funcName, FILE* OutputFile)
{
    FIformula formula;
    time_t t;
    time(&t);
    fprintf(stdout, "Analysis begin: %s", ctime(&t));
    formula.input(FIfile);
    formula.BondSort();
    time(&t);
    fprintf(stdout, "Transform begin: %s", ctime(&t));
    std::string output;
    if (thread == 0 || thread == 1)
    {
        fprintf(stdout, "single thread: %zu\n", thread);
        output = formula.CSingleConfig(funcName, "RR", "FI");
    }
    else
    {
        output = formula.CSingleConfig(thread, funcName, "RR", "FI");
        fprintf(stdout, "many threads: %zu\n", thread);
    }
        
    fprintf(stdout, "Transform end: %s", ctime(&t));
    fprintf(OutputFile, "%s", output.c_str());
    return;
}

void FIanalyzer::FItransform(FILE* FIfile, FILE* DataFile, FILE* OutputFile)
{
    FIformula formula;
    hyperlex::MatFile MF;
    double* DataOut;
    double* mat;
    double* R, FIvalue;
    double* RR;
    size_t row, column, i, j, FIdim;
    int error, FIorder;
    time_t t;
    time(&t);
    fprintf(stdout, "Analysis begin: %s", ctime(&t));
    DataOut = NULL;
    formula.input(FIfile);
    formula.BondSort();
    error = MF.MatrixGet(&mat, &row, &column, DataFile);
    if (error == 0) fprintf(stderr, "ERROR!\n");
    if (error == 0) return;
    if (column != formula.BondCount()) fprintf(stderr, "WARNING!(column != formula.BondCount())\n");
    FIdim = formula.FICount();
    DataOut = (double*)malloc(sizeof(double) * row * FIdim);
    fprintf(stdout, "input dim, row: %llu, column: %llu\n", row, column);
    fprintf(stdout, "output dim, row: %llu, column: %llu\n", row, FIdim);
    RR = (double*)malloc(sizeof(double) * column * column);
    double alpha, beta, bond, delta;
    size_t k, site, L;
    L = column;

    
    time(&t);
    fprintf(stdout, "Computation begin: %s", ctime(&t));
    //L = (column - 1) * column / 2;



    for (i = 0; i < row; i++)
    {
        R = mat + i * column;
        site = 0;
        /*for (j = 0; j < column / 3; j++)
        {
            for (k = j + 1; k < column/3; k++)
            {
                bond = 0.0;
                delta = R[3 * j] - R[3 * k];
                bond += delta * delta;
                delta = R[3 * j + 1] - R[3 * k + 1];
                bond += delta * delta;
                delta = R[3 * j + 2] - R[3 * k + 2];
                bond += delta * delta;
                bond = std::sqrt(bond);
                RR[site] = bond;
                site++;
            }
        }*/
        for (j = 0; j < L; j++)
        {
            RR[j] = R[j];
        }
        for (j = 0; j < L; j++)
        {
            //RR[j] = 1.0 / RR[j];
        }
        for (j = 0; j < FIdim; j++)
        {
            FIvalue = formula.compute(j, RR);
            FIorder = formula.OrderGet(j);
            //FIvalue = std::pow(FIvalue, FIorder);
            FIvalue = std::pow(FIvalue, 1);
            DataOut[i * FIdim + j] = FIvalue;
        }
    }
    time(&t);
    fprintf(stdout, "File writing begin: %s", ctime(&t));
    MF.MatrixSet(DataOut, FIdim, row, FIdim, OutputFile);
    fprintf(stdout, "File writing end: %s", ctime(&t));
    free(DataOut);
    free(RR);
    time(&t);
    
}



struct ComputeInput
{
    size_t RowFrom;
    size_t RowTo;
    const double* mat;
    size_t column;
    double* DataOut;
    size_t FIdim;
    FIformula* formula;
    int NO;
};
#include<pthread.h>
class PthreadHandle
{
private:
    pthread_t* handle;
    void* (*f)(void*);
    //unsigned int No;
    unsigned int amount;
    void** arg;
    // ret_val_p = NULL;
    int type;
public:
    //pthread_handle(unsigned int thread_amount, void* (*F)(void*));
    PthreadHandle(unsigned int thread_amount, void* (*F)(void*), void** Arg);
    PthreadHandle(unsigned int thread_amount, void* (*F)(void*), void* Arg, size_t sizeof_arg);
    ~PthreadHandle();
    //void arg_set(void *Arg, unsigned int No__);
    void launch(void);
    size_t DataDivideFrom(size_t DataAmount, size_t ThreadAmount, size_t ThreadNO);
    size_t DataDivideTo(size_t DataAmount, size_t ThreadAmount, size_t ThreadNO);
};
static void* FIcomputethread(void* input);
static void FIcompute(size_t RowFrom, size_t RowTo, const double* mat, size_t column, double* DataOut, size_t FIdim, FIformula* formula);




void FIanalyzer::FItransformPthread(int thread, const char* FIname, const char* DataName, const char* OutputName)
{
    FILE* FIfile;
    FILE* DataFile;
    FILE* OutputFile;
    hyperlex::CFile CF;
    FIfile = CF.OpenRead(FIname);
    DataFile = CF.OpenRead(DataName);
    OutputFile = CF.OpenWriteAtRear(OutputName);
    FItransformPthread(thread, FIfile, DataFile, OutputFile);
    fclose(FIfile);
    fclose(DataFile);
    fclose(OutputFile);
}
void FIanalyzer::FItransformPthread(int thread, FILE* FIfile, FILE* DataFile, FILE* OutputFile)
{
    FIformula formula;
    hyperlex::MatFile MF;
    double* DataOut;
    double* mat;
    double* R, FIvalue;
    double* RR;
    size_t row, column, i, j, FIdim;
    int error, FIorder;
    time_t t;
    time(&t);
    fprintf(stdout, "Analysis begin: %s, thread: %d", ctime(&t), thread);
    DataOut = NULL;
    formula.input(FIfile);
    formula.BondSort();
    error = MF.MatrixGet(&mat, &row, &column, DataFile);
    if (error == 0) fprintf(stderr, "ERROR!\n");
    if (error == 0) return;
    if (column != formula.BondCount()) fprintf(stderr, "WARNING!(column != formula.BondCount())\n");
    FIdim = formula.FICount();
    DataOut = (double*)malloc(sizeof(double) * row * FIdim);
    fprintf(stdout, "input dim, row: %llu, column: %llu\n", row, column);
    fprintf(stdout, "output dim, row: %llu, column: %llu\n", row, FIdim);
    RR = (double*)malloc(sizeof(double) * column * column);
    double alpha, beta, bond, delta;
    size_t k, site, L;
    L = column;


    time(&t);
    fprintf(stdout, "Computation begin: %s", ctime(&t));
    //L = (column - 1) * column / 2;

    PthreadHandle* Ph; 
    ComputeInput* input;
    input = (ComputeInput*)malloc(sizeof(ComputeInput) * thread);
    Ph = new PthreadHandle(thread, FIcomputethread,(void*)input, sizeof(ComputeInput));
    for (i = 0; i < thread; i++)
    {
        input[i].NO = i;
        input[i].RowFrom = Ph->DataDivideFrom(row, thread, i);
        input[i].RowTo = Ph->DataDivideTo(row, thread, i);
        input[i].mat = mat;
        input[i].column = column;
        input[i].DataOut = DataOut;
        input[i].FIdim = FIdim;
        input[i].formula = &formula;
    }
    Ph->launch();
    delete Ph;
    
    {
        time(&t);
        fprintf(stdout, "File writing begin: %s", ctime(&t));
        MF.MatrixSet(DataOut, FIdim, row, FIdim, OutputFile);
        fprintf(stdout, "File writing end: %s", ctime(&t));
        free(DataOut);
        free(RR);
        time(&t);
    }
}

PthreadHandle::PthreadHandle(unsigned int thread_amount, void* (*F)(void*), void** Arg)
{
    type = 0;
    handle = (pthread_t*)malloc(sizeof(pthread_t) * thread_amount);
    f = F;
    amount = thread_amount;
    arg = Arg;
    //No = No__;
}
PthreadHandle::PthreadHandle(unsigned int thread_amount, void* (*F)(void*), void* Arg, size_t sizeof_arg)
{
    int i;
    type = 1;
    handle = (pthread_t*)malloc(sizeof(pthread_t) * thread_amount);
    f = F;
    amount = thread_amount;
    arg = (void**)malloc(sizeof(void*) * thread_amount);
    for (i = 0; i < thread_amount; i++)
        arg[i] = Arg + (size_t)i * sizeof_arg;

}
PthreadHandle::~PthreadHandle()
{
    unsigned int i;
    for (i = 0; i < amount; i++)
        pthread_join(handle[i], NULL);
    free((void*)handle);
    if (type != 0) free(arg);
}
void PthreadHandle::launch(void)
{
    unsigned int i;
    for (i = 0; i < amount; i++)
        pthread_create(handle + i, NULL, f, arg[i]);

}
size_t PthreadHandle::DataDivideFrom(size_t DataAmount, size_t ThreadAmount, size_t ThreadNO)
{
    size_t begin, l, S;
    S = DataAmount / ThreadAmount;
    l = DataAmount - S * ThreadAmount;
    begin = ThreadNO * S;
    begin += (ThreadNO > l ? l : ThreadNO);
    return begin;
}
size_t PthreadHandle::DataDivideTo(size_t DataAmount, size_t ThreadAmount, size_t ThreadNO)
{
     size_t end, l, S, No;
    S = DataAmount / ThreadAmount;
    l = DataAmount - S * ThreadAmount;
    No = ThreadNO + 1;
    end = No * S;
    end += (No > l ? l : No);
    return end;
}


static void* FIcomputethread(void* input)
{
    ComputeInput* In;

    size_t RowFrom;
    size_t RowTo;
    const double* mat;
    size_t column;
    double* DataOut;
    size_t FIdim;
    FIformula* formula;

    In = (ComputeInput*)input;

    RowFrom = In->RowFrom;
    RowTo = In->RowTo;
    mat = In->mat;
    column = In->column;
    DataOut = In->DataOut;
    FIdim = In->FIdim;
    formula = In->formula;

    fprintf(stdout, "Hello world from thread %d, from: %llu, to: %llu\n", In->NO, RowFrom, RowTo);

    FIcompute(RowFrom, RowTo, mat, column, DataOut, FIdim, formula);

    return NULL;
}
static void FIcompute(size_t RowFrom, size_t RowTo, const double* mat, size_t column, double* DataOut, size_t FIdim, FIformula * formula)
{
    size_t i, j, L, site;
    const double* R;
    double* RR;
    double FIvalue;
    int FIorder;
    RR = (double*)malloc(sizeof(double) * column * column);
    L = column;
    for (i = RowFrom; i < RowTo; i++)
    {
        R = mat + i * column;
        site = 0;
        /*for (j = 0; j < column / 3; j++)
        {
            for (k = j + 1; k < column/3; k++)
            {
                bond = 0.0;
                delta = R[3 * j] - R[3 * k];
                bond += delta * delta;
                delta = R[3 * j + 1] - R[3 * k + 1];
                bond += delta * delta;
                delta = R[3 * j + 2] - R[3 * k + 2];
                bond += delta * delta;
                bond = std::sqrt(bond);
                RR[site] = bond;
                site++;
            }
        }*/
        for (j = 0; j < L; j++)
        {
            RR[j] = R[j];
        }
        for (j = 0; j < L; j++)
        {
            //RR[j] = 1.0 / RR[j];
        }
        for (j = 0; j < FIdim; j++)
        {
            FIvalue = formula->compute(j, RR);
            FIorder = formula->OrderGet(j);
            FIvalue = std::pow(FIvalue, 1.0 / (double)FIorder);
            //FIvalue = std::pow(FIvalue, 1);
            DataOut[i * FIdim + j] = FIvalue;
        }
    }
    free(RR);
}


struct ComputeInputV02
{
    size_t RowFrom;
    size_t RowTo;
    const double* mat;
    size_t column;
    size_t ld;
    double* DataOut;
    size_t FIdim;
    FIformula* formula;
    int NO;
};
static void* FIcomputethreadV02(void* input);
static void FIcomputeV02(size_t RowFrom, size_t RowTo, size_t column, const double* mat, size_t ld, double* DataOut, size_t FIdim, FIformula* formula);


int FIanalyzer::FItransformPthread(int thread, size_t row, size_t column, vector<double>& bondlength, size_t ld, vector<double>& output, FIformula*formula)
{
    PthreadHandle* Ph;
    ComputeInputV02* input;
    size_t i, FIdim;
    

    FIdim = formula->FICount();
    if (column != formula->BondCount())
    {
        fprintf(stderr, "WARNING!(column != formula.BondCount())\n");
        return 1;
    }
    if (bondlength.count() != (ld * row)) fprintf(stderr, "WARNING!bondlength.count() != (ld * row)\n");
    if (bondlength.count() != (ld * row)) return 2;
    output.recount(row * (FIdim + ld - column));
    input = (ComputeInputV02*)malloc(sizeof(ComputeInputV02) * thread);
    Ph = new PthreadHandle(thread, FIcomputethreadV02, (void*)input, sizeof(ComputeInputV02));
    for (i = 0; i < (size_t)thread; i++)
    {
        input[i].NO = i;
        input[i].RowFrom = Ph->DataDivideFrom(row, thread, i);
        input[i].RowTo = Ph->DataDivideTo(row, thread, i);
        input[i].mat = bondlength.array();
        input[i].column = column;
        input[i].ld = ld;
        input[i].DataOut = output.array();
        input[i].FIdim = FIdim;
        input[i].formula = formula;
    }
    Ph->launch();
    delete Ph;
    return 0;
}
static void* FIcomputethreadV02(void* input)
{
    ComputeInputV02* In;

    size_t RowFrom;
    size_t RowTo;
    const double* mat;
    size_t column;
    size_t ld;
    double* DataOut;
    size_t FIdim;
    FIformula* formula;

    In = (ComputeInputV02*)input;

    RowFrom = In->RowFrom;
    RowTo = In->RowTo;
    mat = In->mat;
    column = In->column;
    ld = In->ld;
    DataOut = In->DataOut;
    FIdim = In->FIdim;
    formula = In->formula;

    fprintf(stdout, "Hello world from thread %d, from: %llu, to: %llu\n", In->NO, RowFrom, RowTo);

    FIcomputeV02(RowFrom, RowTo, column, mat, ld, DataOut, FIdim, formula);

    return NULL;
}
static void FIcomputeV02(size_t RowFrom, size_t RowTo, size_t column, const double* mat, size_t ld, double* DataOut, size_t FIdim, FIformula* formula)
{
    size_t i, j, L, site;
    size_t ldF;
    const double* R;
    double* RR;
    double FIvalue;
    int FIorder;
    RR = (double*)malloc(sizeof(double) * column * column);
    L = column;
    ldF = ld - column + FIdim;
    for (i = RowFrom; i < RowTo; i++)
    {
        R = mat + i * ld;
        for (j = 0; j < L; j++)
        {
            RR[j] = 1.0 / R[j];
        }
        for (j = 0; j < FIdim; j++)
        {
            FIvalue = formula->compute(j, RR);
            FIorder = formula->OrderGet(j);
            FIvalue = std::pow(FIvalue, 1.0 / (double)FIorder);
            //FIvalue = std::pow(FIvalue, 1);
            DataOut[i * ldF + j] = FIvalue;
        }
        for (j = 0; j < ld - column; j++)
        {
            DataOut[i * ldF + FIdim + j] = R[column + j];
        }
    }
    free(RR);
}
/*
for (i = 0; i < row; i++)
    {
        R = mat + i * column;
        site = 0;
for (j = 0; j < L; j++)
{
    RR[j] = R[j];
}
for (j = 0; j < L; j++)
{
    //RR[j] = 1.0 / RR[j];
}
for (j = 0; j < FIdim; j++)
{
    FIvalue = formula.compute(j, RR);
    FIorder = formula.OrderGet(j);
    //FIvalue = std::pow(FIvalue, FIorder);
    FIvalue = std::pow(FIvalue, 1);
    DataOut[i * FIdim + j] = FIvalue;
}
    }
*/




