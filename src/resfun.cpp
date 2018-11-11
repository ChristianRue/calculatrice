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

// resfun.cpp

#include <iostream>
#include <stdarg.h>
#include <cstdlib>
#include <complex>
#include <string.h>

#include "utilities.h"
#include "nrtype.h"
#include "matrix.h"
#include "term.h"
#include "function.h"
#include "resfun.h"

using namespace std;

RESFUNCTION::RESFUNCTION(int argnr, const char *name, const char *desc, ...)
{
	int i;
	_argnr = argnr;

	_name = new char[strlen(name)+1];
	for (i = 0; name[i] != 0; i++) _name[i] = name[i];
	_name[i] = 0;

	_desc = new char[strlen(desc)+1];
	for (i = 0; desc[i] != 0; i++) _desc[i] = desc[i];
	_desc[i] = 0;

	char *argname;
	_argname = new char *[argnr];
    va_list params;
    va_start(params, argnr);
		for (i = 0; i < argnr; i++)
		{
			argname = va_arg(params, char *);
			_argname[i] = new char[strlen(argname)+1];
      int j;
			for (j = 0; argname[j] != 0; j++) _argname[i][j] = argname[j];
			_argname[i][j] = 0;
		}
    va_end(params);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MATRIX RFNR1::value (ENODE **eargs, MATRIX **args)
{
	MATRIX m(eargs[0]->value(args));
	if (m.n() == 1 && m.m() == 1) return MATRIX(resfun(m.value(0,0)));
	cout << "Error: Function" << _name << " does not accept matrix arguments!" << endl;
	return MATRIX(0.);
}

MATRIX RFMAT1::value (ENODE **eargs, MATRIX **args)
{
	return resfun(eargs[0]->value(args)); 
}

MATRIX RFMAT2::value (ENODE **eargs, MATRIX **args)
{
	return resfun(eargs[0]->value(args), eargs[1]->value(args)); 
}

MATRIX RFMAT3::value (ENODE **eargs, MATRIX **args)
{
	return resfun(eargs[0]->value(args), eargs[1]->value(args), eargs[2]->value(args)); 
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MATRIX RFRND::value (ENODE **eargs, MATRIX **args)
{
	double nr;
	unsigned char *i = (unsigned char *)&nr;
	unsigned short j;
	for (j = 0; j < sizeof(double); j++) i[j] = (unsigned char)(rand() % 256);
	for (j = 1022; j > 0 && rand() % 2; j--);
	i[6] = (0x000F & j) << 4 | 0x0F & i[6];
	i[7] = (0x03F0 & j) >> 4 | 0x80 & i[7];
	return MATRIX(nr); 
}

MATRIX RFSUM::value (ENODE **eargs, MATRIX **args)
{
	char *iname = eargs[0]->name();
	if (iname) {
		if ((iname[0] >= 'a' && iname[0] <= 'z') || (iname[0] >= 'A' && iname[0] <= 'Z')) {
			MATRIX mstart(eargs[1]->value(args));
			int start = (int)real(mstart.value(0,0));
			if (mstart.m() != 1 || mstart.n() != 1 || mstart.value(0,0) != (NRTYPE)start) {
				cout << "Error: Function \"sum\" expects an integer as second argument!" << endl;
				return MATRIX(0.);
			}
			MATRIX mend(eargs[2]->value(args));
			int end = (int)real(mend.value(0,0));
			if (mend.m() != 1 || mend.n() != 1 || mend.value(0,0) != (NRTYPE)end) {
				cout << "Error: Function \"sum\" expects an integer as third argument!" << endl;
				return MATRIX(0.);
			}
			CRTYPE& it = eargs[0]->data().value(0,0);
			it = mstart.value(0,0);
			MATRIX m = eargs[3]->value(args).clone();
			for (int i = start+1; i <= end; i++) {
				it = (CRTYPE)i;
				m += eargs[3]->value(args);
			}
			return m;
		}
	}
	cout << "Error: Function \"sum\" expects a variable name as first argument!" << endl;
	return MATRIX(0.);
}

MATRIX RFPROD::value (ENODE **eargs, MATRIX **args)
{
	char *iname = eargs[0]->name();
	if (iname) {
		if ((iname[0] >= 'a' && iname[0] <= 'z') || (iname[0] >= 'A' && iname[0] <= 'Z')) {
			MATRIX mstart(eargs[1]->value(args));
			int start = (int)real(mstart.value(0,0));
			if (mstart.m() != 1 || mstart.n() != 1 || mstart.value(0,0) != (NRTYPE)start) {
				cout << "Error: Function \"prod\" expects an integer as second argument!" << endl;
				return MATRIX(0.);
			}
			MATRIX mend(eargs[2]->value(args));
			int end = (int)real(mend.value(0,0));
			if (mend.m() != 1 || mend.n() != 1 || mend.value(0,0) != (NRTYPE)end) {
				cout << "Error: Function \"prod\" expects an integer as third argument!" << endl;
				return MATRIX(0.);
			}
			CRTYPE& it = eargs[0]->data().value(0,0);
			it = mstart.value(0,0);
			MATRIX m = eargs[3]->value(args).clone();
			for (int i = start+1; i <= end; i++) {
				it = (CRTYPE)i;
				m *= eargs[3]->value(args);
			}
			return m;
		}
	}
	cout << "Error: Function \"prod\" expects a variable name as first argument!" << endl;
	return MATRIX(0.);
}

MATRIX RFWAV::value(ENODE **eargs, MATRIX **args)
{
	char *iname = eargs[1]->name();
	if (iname) {
		if ((iname[0] >= 'a' && iname[0] <= 'z') || (iname[0] >= 'A' && iname[0] <= 'Z')) {
			USERFUNCTION f(0, 1, &iname, eargs[2]);

			int in[4];
			for (int i = 0; i < 4; i++) {
				MATRIX m = eargs[i+3]->value(args);
				in[i] = (int)real(m.value(0,0));
				if (m.m() != 1 && m.n() != 1 && imag(m.value(0,0)) != 0. && (NRTYPE)in[i] != real(m.value(0,0))) {
					cout << "Error: Function \"wav\" expects an integer as " << i+4 << "th argument!" << endl;
					return MATRIX(0.);
				}
			}

			return MATRIX((NRTYPE)writewav(eargs[0]->name(), eargs[1], eargs[2], in[0], in[1], in[2], in[3]));
		}
	}
	cout << "Error: Function \"wav\" expects a variable name as second argument!" << endl;
	return MATRIX(0.);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CRTYPE crsin(const CRTYPE& x)
{
	if (imag(x) == 0.) return sin(real(x));
	return sin(x);
}

CRTYPE crcos(const CRTYPE& x)
{
	if (imag(x) == 0.) return cos(real(x));
	return cos(x);
}

CRTYPE crtan(const CRTYPE& x)
{
	if (imag(x) == 0.) return tan(real(x));
	return sin(x)/cos(x);
}

CRTYPE crasin(const CRTYPE& x)
{
	if (imag(x) == 0.) return asin(real(x));
	CRTYPE s(-imag(x), real(x));
	s = log(s + sqrt(1.+s*s));
	return CRTYPE(imag(s), -real(x));
}

CRTYPE cracos(const CRTYPE& x)
{
	if (imag(x) == 0.) return acos(real(x));
	CRTYPE s(sqrt(1.-x*x));
	s = log(x + CRTYPE(-imag(s), real(s)));
	return CRTYPE(imag(s), -real(x));
}

CRTYPE cratan(const CRTYPE& x)
{
	if (imag(x) == 0.) return atan(real(x));
	CRTYPE s(-imag(x), real(x));
	s = .5*log((1.+s)/(1.-s));
	return CRTYPE(imag(s), -real(x));
}

CRTYPE crceil(const CRTYPE& x)
{
	return CRTYPE(ceil(real(x)), ceil(imag(x)));
}

CRTYPE crfloor(const CRTYPE& x)
{
	return CRTYPE(floor(real(x)), floor(imag(x)));
}

CRTYPE signum(const CRTYPE& x)
{
	if (imag(x) == 0.) {
		if (real(x) < 0.) return -1.;
		return 1.;
	}
	cout << "Error: " << "Function \"sign\" got invalid arguments!" << endl;
	return 0.;
}

CRTYPE crabs(const CRTYPE& x)
{
	return abs(x);
}

MATRIX minimum(MATRIX M)
{
	if (M.n() > 0 && M.m() == 1) {
		for (int j = 0; j < M.n(); j++) {
			if (imag(M.value(0,j)) != 0.) {
				NRTYPE m = abs(M.value(0,0));
				for (int i = 0; i < M.n(); i++)
					if (abs(M.value(0,i)) < m) m = abs(M.value(0,i));
				return MATRIX(m);
			}
		}
		NRTYPE m = real(M.value(0,0));
		for (int i = 0; i < M.n(); i++)
			if (real(M.value(0,i)) < m) m = real(M.value(0,i));
		return MATRIX(m);
	}
	cout << "Error: Function \"min\" got invalid arguments!" << endl;
	return MATRIX(0.);
}

MATRIX maximum(MATRIX M)
{
	if (M.n() > 0 && M.m() == 1) {
		for (int j = 0; j < M.n(); j++) {
			if (imag(M.value(0,j)) != 0.) {
				NRTYPE m = abs(M.value(0,0));
				for (int i = 0; i < M.n(); i++)
					if (abs(M.value(0,i)) > m) m = abs(M.value(0,i));
				return MATRIX(m);
			}
		}
		NRTYPE m = real(M.value(0,0));
		for (int i = 0; i < M.n(); i++)
			if (real(M.value(0,i)) > m) m = real(M.value(0,i));
		return MATRIX(m);
	}
	cout << "Error: Function \"max\" got invalid arguments!" << endl;
	return MATRIX(0.);
}

MATRIX eye(MATRIX M)
{
	if (M.n() == 1 && M.m() == 1 && imag(M.value(0,0)) == 0.) {
		MATRIX m((int)real(M.value(0,0)), (int)real(M.value(0,0)));
		m.eye();
		return m;
	}
	cout << "Error: Function \"eye\" got invalid arguments!" << endl;
	return MATRIX(0.);
}

MATRIX zero(MATRIX x, MATRIX y)
{
	if (x.n() == 1 && x.m() == 1 && y.n() == 1 && y.m() == 1 && imag(x.value(0,0)) == 0. && imag(y.value(0,0)) == 0.) {
		MATRIX m((int)real(x.value(0,0)), (int)real(y.value(0,0)));
		m.zero();
		return m;
	}
	cout << "Error: Function \"zero\" got invalid arguments!" << endl;
	return MATRIX(0.);
}

MATRIX diag(MATRIX M)
{
	if (M.n() > 0 && M.m() == 1) {
		MATRIX m(M.n(), M.n());
		m.zero();
		for (int i = 0; i < M.n(); i++) m.value(i,i) = M.value(0,i);
		return m;
	}
	cout << "Error: Function \"diag\" got invalid arguments!" << endl;
	return MATRIX(0.);
}

MATRIX dim(MATRIX M)
{
	if (M.m() == 1 && M.n() > 0)
		return M.n();
	else if (M.m() > 0 && M.n() == 1) 
		return M.m();
	cout << "Error: Function \"dim\" got invalid arguments!" << endl;
	return MATRIX(0.);
}

MATRIX size(MATRIX M)
{
	MATRIX m(1,2);
	m.value(0,0) = M.m();
	m.value(0,1) = M.n();
	return m;
}

MATRIX norm(MATRIX M, MATRIX p)
{
	if (p.m() == 1 && p.n() == 1 && imag(p.value(0,0)) == 0.)
		return MATRIX(M.norm(real(p.value(0,0))));
	cout << "Error: Function \"norm\" got invalid arguments!" << endl;
	return MATRIX(0.);

}

MATRIX eig(MATRIX M)
{
	return M.eig(1.e-12);
}

MATRIX lrl(MATRIX M)
{
	MATRIX L(M.m(), M.m());
	M.plr(&L,0);
	return L;
}

MATRIX lrr(MATRIX M)
{
	M.plr(0, 0);
	return M;
}

MATRIX qrq(MATRIX M)
{
	MATRIX Q(M.m(), M.m());
	M.qr(&Q, 0);
	return Q;
}

MATRIX qrr(MATRIX M)
{
	MATRIX R(M.m(), M.n());
	M.qr(0, &R);
	return R;
}

MATRIX mrr(MATRIX M)
{
	MATRIX R = M.clone();
	R.mr(0);
	return R;
}

MATRIX mrm(MATRIX M)
{
	MATRIX R = M.clone();
	MATRIX N;
	R.mr(&N);
	return N;
}

MATRIX lgs(MATRIX M)
{
	MATRIX R = M.clone();
	if (R.lgs())
		return R;
	else {
		cout << "LGS has no solution!" << endl;
		return MATRIX(0.);
	}
}

MATRIX ker(MATRIX M)
{
	MATRIX R = MATRIX(M.m(), M.n()+1);
	for (int i = 0; i < M.m(); i++) {
		for (int j = 0; j < M.n(); j++) R.value(i,j) = M.value(i,j);
		R.value(i,M.n()) = 0.;
	}
	if (R.lgs())
		return R;
	else {
		cout << "LGS has no solution!" << endl;
		return MATRIX(0.);
	}
}

MATRIX det(MATRIX M)
{
	if (M.m() == M.n())	return M.det();
	cout << "Error: Matrix not quadratic (determinant)!" << endl;
	return MATRIX(0.);
}

MATRIX trc(MATRIX M)
{
	return M.trace();
}

MATRIX cofactor(MATRIX M, MATRIX i, MATRIX j)
{
	if (i.n() == 1 && i.m() == 1 && j.n() == 1 && j.m() == 1 && imag(i.value(0,0)) == 0. && imag(j.value(0,0)) == 0.)
		return M.cofactor((int)real(i.value(0,0)), (int)real(j.value(0,0)));
	cout << "Error: Function \"cofactor\" got invalid arguments!" << endl;
	return MATRIX(0.);
}