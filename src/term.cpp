/* Copyright 2011 Christian Rüschoff

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

// term.cpp

#include <iostream>
#include <sstream>
#include <cmath>
#include <complex>

#include "utilities.h"
#include "nrtype.h"
#include "matrix.h"
#include "term.h"
#include "function.h"
#include "resfun.h"
#include "memory.h"

//#undef min
//#undef max

using namespace std;

SNODE::SNODE(char*& data)
{
  int i;
	for (i = 0; data[i] && data[i] != '\"'; i++);
	_data = new char[i];
  char* tmp;
	for (tmp = _data; *data && *data != '\"'; ) *tmp++ = *data++;
	*tmp = 0;
}

TNODE::TNODE(char name) : _argnr(0)
{
	_name = new char;
	*_name = name;
	_args = 0;
}

TNODE::TNODE(char name, int x)
{
	_name = new char[sizeof(int)];
	_name[0] = name;
	char *c = (char *)&x;
	for (int i = 0; i < sizeof(int); i++) _name[i+1] = c[i];
}

TNODE::TNODE(char name, ELIST& lst)
{
	_name = new char;
	*_name = name;
	_argnr = lst.count();
	_args = lst.makearray();
}

TNODE::TNODE(char name, ENODE *t) : _argnr(1)
{
	_name = new char;
	*_name = name;
	_args = new ENODE*;
	*_args = t;
}

TNODE::TNODE(char name, int argnr, ENODE **args) : _argnr(argnr), _args(args)
{
	_name = new char;
	*_name = name;
}

TNODE::TNODE(EELIST *lst)
{
	int m, n;
	int i,j;
	ELIST *f;
	ENODE *ff;
	bool error = false;

	m = lst->count();
	if (m < 1)
		error = true;
	else {
		f = lst->pop();

		n = f->count();
		if (n < 1)
			error = true;
		else {
			_argnr = m*n;
			_args = new ENODE*[_argnr];

			for(i = m-1; i >= 0; i--) {
				if (f->count() != n) break;
				ff = f->pop();
				for(j = n-1; j >= 0; j--) {
					_args[i*n+j] = ff;
					ff = f->pop();
				}
				delete f;
				f = lst->pop();
			}
			if (f) {
				for (j = (i+1)*n; j < m*n; j++) delete _args[j];
				delete[] _args;
				error = true;
			}
		}
	}
	delete lst;

	if (error) {
		cout << "Error: Matrix!" << endl;
		_name = new char;
		*_name = '?';
		return;
	}

	_name = new char[sizeof(int)+1];
	_name[0] = '[';
	char *c = (char *)&m;
	for (i = 0; i < sizeof(int); i++) _name[i+1] = c[i];
}

TNODE::TNODE(FUNCTION **f, ENODE **args) : _args(args), _argnr((**f)._argnr)
{
	_name = new char[sizeof(FUNCTION **)+1];
	_name[0] = '§';
	char *c = (char *)f;
	for (int i = 0; i < sizeof(FUNCTION **); i++) _name[i+1] = c[i];
}

MATRIX TNODE::makematrix(MATRIX **args)
{
	int i,j, ii, jj;
	int mi, nj;
	int mm = *(int *)&_name[1];
	int nn = _argnr/mm;
	int m = 0;
	int n = 0;

	MATRIX *MM = new MATRIX[mm*nn];

	for(ii = 0; ii < mm; ii++) {
	for(jj = 0; jj < nn; jj++) {
		MM[ii*nn+jj] = _args[ii*nn+jj]->value(args);
		if (MM[ii*nn+jj].m() != MM[ii*nn].m() || MM[ii*nn+jj].n() != MM[jj].n()) {
			cout << "Error: Invalid matrix entry!" << endl;
			delete[] MM;
			return MATRIX(0.);
		}
	}
	}

	for (ii = 0; ii < mm; ii++) m += MM[ii*nn].m();
	for (jj = 0; jj < nn; jj++) n += MM[jj].n();

	MATRIX M(m,n);

	mi = 0; 
	for(ii = 0; ii < mm; ii++) {
		nj = 0;
		for(jj = 0; jj < nn; jj++) {
			for(i = 0; i < MM[ii*nn+jj].m(); i++) {
			for(j = 0; j < MM[ii*nn+jj].n(); j++) {
				M.value(mi+i, nj+j) = MM[ii*nn+jj].value(i,j);
			}
			}
			nj += MM[jj].n();
		}
		mi += MM[ii*nn].m();
	}
	delete[] MM;
	return M;
}

MATRIX TNODE::makevector(MATRIX **args)
{
	MATRIX a[3];
	int i;
	for (i = 0; i < _argnr; i++) {
		a[i] = _args[i]->value(args);
		if (a[i].n() != 1 && a[i].m() != 1) {
			cout << "Error: Vector does not accept matrix arguments!" << endl;
			return MATRIX(0.);
		}
	}

	NRTYPE d;
	NRTYPE start = real(a[0].value(0,0));
	NRTYPE end = real(a[_argnr-1].value(0,0));
	int size;
	if (_argnr == 2) 
		d = end - start;
	else if (_argnr == 3)
		d = (end - start) / abs(a[1].value(0,0));
	if (fabs(d) > MAXINT) {
		cout << "Error: Vector too big!" << endl;
		return MATRIX(0.);
	}
	size = fabs(d)+1;
	MATRIX m(1,size);
	d = (end-start)/d;
	for (i = 0; i < size; i++) {
		m.value(0,i) = start + (NRTYPE)i*d;
	}
	return m;
}

MATRIX TNODE::value(MATRIX **args)
{
	if (*_name == '+') {
		MATRIX m = _args[0]->value(args).clone();
		for (int i = 1; i < _argnr; i++) m += _args[i]->value(args);
		return m;
	} else if (*_name == '%') {
		MATRIX m = _args[0]->value(args).clone();
		for (int i = 1; i < _argnr; i++) m %= _args[i]->value(args);
		return m;
	} else if (*_name == '*') {
		MATRIX m = _args[0]->value(args).clone();
		for (int i = 1; i < _argnr; i++) m *= _args[i]->value(args);
		return m;
	} else if (*_name == '^') {
		MATRIX m = _args[0]->value(args).clone();
		for (int i = 1; i < _argnr; i++) m = m ^ _args[i]->value(args);
		return m;
	} else if (*_name == '!') {
		MATRIX m = _args[0]->value(args).clone();
		m.factorial();
		return m;
	} else if (*_name == 'i') {
		MATRIX m = _args[0]->value(args).clone();
		m *= CRTYPE(0.,1.);
		return m;
	} else if (*_name == '\'') {
		MATRIX m = _args[0]->value(args).clone();
		m.transpose();
		return m;
	} else if (*_name == '-') {
		return -_args[0]->value(args);
	} else if (*_name == '/') {
		MATRIX m = _args[0]->value(args).clone();
		m.invert();
		return m;
	} else if (*_name == '#') {
		return *args[*((int *)&_name[1])];
	} else if (*_name == '[') {
		return makematrix(args);
	} else if (*_name == ':') {
		return makevector(args);
	} else if (*_name == ']') {
		MATRIX m = _args[0]->value(args).clone();
		int xy1[2], xy2[2];

		for (int i = 0; i < 2; i++) {
			int s;
			if (i == 0)
				s = m.m();
			else
				s = m.n();
			if (_args[2*i+1]) {
				MATRIX n = _args[2*i+1]->value(args).clone();
				xy1[i] = (int)real(n.value(0,0));
				if (n.m() != 1 || n.n() != 1 || (double)xy1[i] != n.value(0,0) || xy1[i] < 0 || xy1[i] >= s || imag(n.value(0,0)) != 0.) {
					cout << "Error: Invalid sub matrix index!" << endl;
					return MATRIX(0.);
				}
				if (_args[2*i+2]) {
					n = _args[2*i+2]->value(args).clone();
					xy2[i] = (int)real(n.value(0,0));
					if (n.m() != 1 || n.n() != 1 || (double)xy2[i] != n.value(0,0) || xy2[i] < 0 || xy2[i] >= s || imag(n.value(0,0)) != 0.) {
						cout << "Error: Invalid sub matrix index!" << endl;
						return MATRIX(0.);
					}
				} else
					xy2[i] = xy1[i];
			} else {
				xy1[i] = 0;
				xy2[i] = s-1;
			}
		}
		return m.sub(xy1[0],xy1[1], xy2[0]-xy1[0]+1,xy2[1]-xy1[1]+1);
	} else if (*_name == '§') {
		FUNCTION **f = (FUNCTION **)&_name[1];
		if (!f) {
			cout << "Error: Function!";
			return MATRIX(0.);
		}
		return (*f)->value(_args, args);
	}
	return MATRIX(0.);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int level(char op)
{
	if (op == ':' || op == '=') return 0;
	if (op == '+') return 1;
	if (op == '-') return 2;
	if (op == '%') return 3;
	if (op == '*') return 4;
	if (op == '/') return 5;
	if (op == '!') return 6;
	if (op == '\'') return 7;
	if (op == '^') return 8;
	if (op == 'i') return 9;
	if (op == ']') return 10;
	if (op == '#' || op == '$' || op == '§' || op == '?' || op == '[') return 11;
	return -1;
}

void SNODE::print(ostream &ostr, char **argname, int oplevel)
{
	ostr << "\"" << _data << "\"";
	return;
}

void CNODE::print(ostream &ostr, char **argname, int oplevel)
{
	int i,j;
	if (_data->m() == 1 && _data->n() == 1)
		ostr << _data->value(0,0);
	else {
		ostr << "[";
		for (i = 0; i < _data->m(); i++) {
		for (j = 0; j < _data->n(); j++) {
			ostr << _data->value(i,j);
			if (j < _data->n()-1) 
				ostr << ", ";
			else if (i < _data->m()-1)
				ostr << "; ";
		}
		}
		ostr << "]";
	}
	return;
}

void VNODE::print(ostream &ostr, char **argname, int oplevel)
{
	ostr << _name;
	return;
}

void TNODE::print(ostream &ostr, char **argname, int oplevel)
{
	int i;

	if (level(*_name) < oplevel) ostr << "(";

	if (*_name == '+' || *_name == '*' || *_name == '^' || *_name == '%' || *_name == ':') {
		for (i = 0; i < _argnr-1; i++) {
			_args[i]->print(ostr, argname, level(*_name));
			ostr << " " << *_name << " ";
		}
		if (_argnr > 0)	_args[_argnr-1]->print(ostr, argname, level(*_name));
	} else if (*_name == '!' || *_name == '\'') {
		_args[0]->print(ostr, argname, level(*_name));
		ostr << *_name;
	} else if (*_name == 'i') {
		_args[0]->print(ostr, argname, level(*_name));
		ostr << " i";
	} else if (*_name == '-' || *_name == '/') {
		ostr << *_name;
		_args[0]->print(ostr, argname, level(*_name));
	} else if (*_name == '#') {
		if (argname)
			ostr << argname[*(int *)&_name[1]];
		else
			ostr << *_name << *(int *)&_name[1];
	} else if (*_name == '[') {
		int n = _argnr / *(int *)&_name[1];
		ostr << "[";
		_args[0]->print(ostr, argname, 0);
		for (i = 1; i < _argnr; i++) {
			if (i % n == 0)
				ostr << "; ";
			else
				ostr << ", ";
			_args[i]->print(ostr, argname, 0);
		}
		ostr << "]";
	} else if (*_name == '§') {
		FUNCTION **f = (FUNCTION **)&_name[1];
		if (f)
			ostr << (*f)->_name;
		else
			ostr << "?function?";

		ostr << "(";
		if (_argnr > 0) _args[0]->print(ostr, argname, 0);
		for (i = 1; i < _argnr; i++) {
			ostr << ", ";
			_args[i]->print(ostr, argname, 0);
		}
		ostr << ")";
	} else if (*_name == ']') {
		_args[0]->print(ostr, argname, level(*_name));
		ostr << "[";
		if (!_args[1] && _args[3])
			_args[3]->print(ostr, argname, 0);
		else {
			for (i = 0; i < 2; i++) {
				if (_args[2*i+1]) {
					_args[2*i+1]->print(ostr, argname, 0);
					if (_args[2*i+2]) {
						ostr << " : ";
						_args[2*i+2]->print(ostr, argname, 0);
					}
				} else
					ostr << " : ";
				if (i == 0) ostr << ", ";
			}
		}
		ostr << "]";
	} else if (*_name == '?')
		ostr << "?";

	if (level(*_name) < oplevel) ostr << ")";
	return;
}

ostream& operator <<(ostream &ostr, ENODE &v)
{
	if (&v) 
		v.print(ostr, 0, 0);
	else {
		ostr << "?";
	}
	return ostr;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

