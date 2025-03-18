#include"FIanalyzer.h"
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<string.h> 
using namespace FIanalyzer;
static char* CopyMalloc(const char* s);

xyz::xyz()
{
}
xyz::~xyz()
{
	size_t i;
	for (i = 0; i < name.count(); i++)
		delete name[i];
}
void xyz::Print(double* mat) const
{
	size_t i;
	size_t dim;
	dim = (Force ? 6 : 3);
	for (i = 0; i < count; i++)
	{
		mat[i * 3 + 0] = XYZ[i * dim + 0];
		mat[i * 3 + 1] = XYZ[i * dim + 1];
		mat[i * 3 + 2] = XYZ[i * dim + 2];
	}
	mat[count * 3] = E;
}
void xyz::PrintBondlength(double* mat) const
{
	size_t i, j, site;
	size_t dim;
	double x_1, y_1, z_1, x_2, y_2, z_2;
	double r, temp;
	dim = (Force ? 6 : 3);
	site = 0;
	for (i = 0; i < count; i++)
	{
		x_1 = XYZ[i * dim + 0];
		y_1 = XYZ[i * dim + 1];
		z_1 = XYZ[i * dim + 2];
		for (j = i + 1; j < count; j++)
		{
			x_2 = XYZ[j * dim + 0];
			y_2 = XYZ[j * dim + 1];
			z_2 = XYZ[j * dim + 2];
			r = 0.0;
			temp = x_2 - x_1;
			r += temp * temp;
			temp = y_2 - y_1;
			r += temp * temp;
			temp = z_2 - z_1;
			r += temp * temp;
			mat[site] = r;
			site += 1;
		}
	}
	mat[site] = E;
}
void xyz::demo(FILE* fp) const
{
	size_t i, j;
	size_t dim;
	dim = (Force ? 6 : 3);
	fprintf(fp, "%zu\n", count);
	fprintf(fp, "%+25.16E\n", E);
	for (i = 0; i < name.count(); i++)
	{
		fprintf(fp, "%s", name[i]);
		for (j = 0; j < dim; j++)
		{
			fprintf(fp, "    %+25.16E", XYZ[i * dim + j]);
		}
		fprintf(fp, "\n");
	}
}
bool xyz::operator==(const xyz& right) const
{
	return compare(right);
}
bool xyz::operator!=(const xyz& right) const
{
	return !compare(right);
}
bool xyz::compare(const xyz& right) const
{
	size_t i;
	int result;
	if (count != right.count) return false;
	if (Force != right.Force) return false;
	if (name.count() != right.name.count()) return false;
	if (XYZ.count() != right.XYZ.count()) return false;
	for (i = 0; i < name.count(); i++)
	{
		result = strcmp(name[i], right.name[i]);
		if (result != 0) return false;
	}
	return true;
}
int xyz::build(double Energy, const vector<char*>& Name, const vector<double>& xyzSource)
{
	E = Energy;
	if (3 * Name.count() == xyzSource.count()) Force = false;
	else if (6 * Name.count() == xyzSource.count()) Force = true;
	else return 1;
	name.copy(Name);
	XYZ.copy(xyzSource);
	count = name.count();
	return 0;
}
int xyz::build(const xyz& source)
{
	size_t i;
	E = source.E;
	count = source.count;
	XYZ.copy(source.XYZ);
	name.recount(source.name.count());
	for (i = 0; i < name.count(); i++)
		name[i] = CopyMalloc(source.name[i]);
	return 0;
}
void xyz::permutation(const vector<size_t>& group)
{
	vector<char*> tempN;
	vector<double> tempXYZ;
	tempN.recount(name.count());
	tempXYZ.recount(XYZ.count());
	if (group.count() != name.count())return;
	permutation(group, tempN, tempXYZ);
}
void xyz::permutation(const vector<size_t>& group, vector<char*>& tempN, vector<double>& tempXYZ)
{
	size_t i, dim, Src, j;
	dim = (Force ? 6 : 3);
	for (i = 0; i < name.count(); i++) tempN[i] = name[i];
	for (i = 0; i < XYZ.count(); i++) tempXYZ[i] = XYZ[i];
	for (i = 0; i < group.count(); i++) 
	{
		Src = group[i];
		name[i] = tempN[Src];
		for (j = 0; j < dim; j++)
			XYZ[i * dim + j] = tempXYZ[Src * dim + j];
	}
}
size_t xyz::AtomCount(void)const
{
	return count;
}


xyzFile::xyzFile()
{
}
xyzFile::~xyzFile()
{
	size_t i;
	for (i = 0; i < content.count(); i++)
		delete content[i];
}
int xyzFile::append(xyz* element)
{
	//if (content.count() != 0)
	//{
	//	if ((*(content[content.count() - 1])) != (*element)) return 1;
	//}
	content.append(element);
	atom = element->count;
	return 0;
}
size_t xyzFile::check(void)const
{
	size_t i;
	for (i = 1; i < content.count(); i++)
	{
		if ((*(content[i])) != (*(content[0]))) return i;
	}
	return 0;
}
size_t xyzFile::count(void) const
{
	return content.count();
}
double* xyzFile::Print(void) const
{
	double* mat;
	size_t i, dim;
	dim = (atom * 3 + 1);
	mat = (double*)malloc(sizeof(double) * dim * content.count());
	for (i = 0; i < content.count(); i++)
		content[i]->Print(mat + i * dim);
	return mat;
}
void xyzFile::PrintR(vector<double>& output) const
{
	size_t i, dim;
	dim = atom * (atom - 1);
	dim = dim / 2 + 1;
	output.recount(dim * content.count());
	for (i = 0; i < content.count(); i++)
		content[i]->PrintBondlength(output.array(i * dim));
}
void xyzFile::demo(FILE* fp) const
{
	demo(fp, 0, content.count());
}
void xyzFile::demo(FILE* fp, size_t lower, size_t upper) const
{
	size_t i;
	for (i = lower; i < upper && i < content.count(); i++)
	{
		content[i]->demo(fp);
	}
}
void xyzFile::tail(FILE* fp, size_t N) const
{
	size_t lower;
	size_t i;
	lower = (N < content.count() ? content.count() - N : 0);
	fprintf(fp, "count: %zu\n", content.count());
	for (i = lower; i < N + lower && i < content.count(); i++)
	{
		content[i]->demo(fp);
	}
}
int xyzFile::permutation(const vector<size_t>& group)
{
	vector<char*> tempN;
	vector<double> tempXYZ;
	size_t i;

	if (group.count() != content[0]->name.count())return 1;
	tempN.recount(content[0]->name.count());
	tempXYZ.recount(content[0]->XYZ.count());
	for (i = 0; i < content.count(); i++)
	{
		content[i]->permutation(group, tempN, tempXYZ);
	}
	return 0;
}
double xyzFile::EnergyUpper(void) const
{
	double upper;
	size_t i;
	if (content.count() != 0) upper = content[0]->E;
	else upper = 0.0;
	for (i = 1; i < content.count(); i++)
	{
		upper = (upper > content[i]->E ? upper : content[i]->E);
	}
	return upper;
}
double xyzFile::EnerguLower(void) const
{
	double lower;
	size_t i;
	if (content.count() != 0) lower = content[0]->E;
	else lower = 0.0;
	for (i = 1; i < content.count(); i++)
	{
		lower = (lower < content[i]->E ? lower : content[i]->E);
	}
	return lower;
}
void xyzFile::OutputErange(vector<double>&output) const
{
	size_t i;
	double lower;
	output.recount(content.count());
	lower = EnerguLower();
	for (i = 0; i < content.count(); i++)
	{
		output[i] = content[i]->E - lower;
	}
}

xyz*& xyzFile::operator[](size_t target)
{
	return content[target];
}


static char* CopyMalloc(const char* s)
{
	char* v;
	size_t size;
	size = strlen(s);
	v = (char*)malloc(sizeof(char) * (size + 4));
	strcpy(v, s);
	return v;
}



