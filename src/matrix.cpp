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

// matrix.cpp

#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <complex>
#include <cstring>

#include "utilities.h"
#include "nrtype.h"
#include "matrix.h"

#undef max
#undef min
using namespace std;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CRTYPE **newarray(int m, int n)
{
	CRTYPE **value;
	if (m > 0 && n > 0) {
		value = new CRTYPE*[m];
		for (int i = 0; i < m; i++)
			value[i] = new CRTYPE[n];
	} else
		value = 0;
	return value;
}

void delarray(CRTYPE **value, int m, int n)
{
	if (m > 1) {
		if (n > 1) {
			for (int i = 0; i < m; i++) 
				delete[] value[i];
		} else if (n > 0) {
			for (int i = 0; i < m; i++) 
				delete value[i];
		}
		delete[] value;
	} else if (m > 0)
		delete value;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ARRAY2D::ARRAY2D(int m, int n)					: _m(m), _n(n), _ref(1), _parent(0), _value(newarray(m, n)) {}
ARRAY2D::ARRAY2D(int m, int n, CRTYPE **value)	: _m(m), _n(n), _ref(1), _parent(0), _value(value) {}

ARRAY2D::ARRAY2D(const ARRAY2D& other)			: _m(other._m), _n(other._n), _ref(1), _parent(0)
{
	_value = newarray(_m, _n);
	for (int i = 0; i < _m; i++)
	for (int j = 0; j < _n; j++)
		_value[i][j] = other._value[i][j];
}

ARRAY2D::~ARRAY2D()
{
	if (_parent)
		delete _parent;
	else
		delarray(_value, _m, _n);
}

void ARRAY2D::setval(int m, int n, CRTYPE **value)
{
	_m = m;
	_n = n;
	_value = value;
	return;
}

void ARRAY2D::resize(int m, int n)
{
	_m = m;
	_n = n;
	_value = newarray(_m, _n);
	return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// CONSTRUCTOR:

MATRIX::MATRIX()								: _arrval(new ARRAY2D(0, 0)), _mtype(None) {}
MATRIX::MATRIX(int m, int n)					: _arrval(new ARRAY2D(m, n)), _mtype(None) {}
MATRIX::MATRIX(int m, int n, CRTYPE **value)	: _arrval(new ARRAY2D(m, n, value)), _mtype(None) {}

MATRIX::MATRIX(NRTYPE value) : _arrval(new ARRAY2D(1, 1)), _mtype(None)
{
	_arrval->_value[0][0] = CRTYPE(value, 0.);
}

MATRIX::MATRIX(CRTYPE value) : _arrval(new ARRAY2D(1, 1)), _mtype(None)
{
	_arrval->_value[0][0] = value;
}

MATRIX::MATRIX(const MATRIX& other) : _arrval(other._arrval), _mtype(None)
{
	_arrval->_ref++;
	return;
}

MATRIX::MATRIX(ifstream &ifstr) : _mtype(None)
{
	int m, n;
	ifstr.read((char *)&m,sizeof(int));
	ifstr.read((char *)&n,sizeof(int));

	if (m < 0) m = 0;
	if (n < 0) n = 0;

	if (m > 1 << 8) m = 1 << 8;
	if (n > 1 << 8) n = 1 << 8;

	CRTYPE **arr = new CRTYPE*[m];
	for (int i = 0 ; i < m; i++) {
		arr[i] = new CRTYPE[n];
		if (ifstr.eof()) return;
		ifstr.read((char *)arr[i], n*sizeof(CRTYPE));
	}
	_arrval = new ARRAY2D(m, n, arr);
}

MATRIX::MATRIX(const char *MA) : _mtype(None)
{
	stringstream ss;
	char c;
	int i, j;
	int m, n;

	ss << MA << endl;
	ss >> c;
	if (c != '[') {
		cout << "Error: '[' expected!" << endl;
		return;
	}

	m = 1;
	n = 0;
	for (i = 0; i < strlen(MA); i++) {
		if		(MA[i] == ',') n++;
		else if (MA[i] == ';') m++;
	}
	n = n/m + 1;
	_arrval = new ARRAY2D(m, n);

	i = 0;
	j = 0;
	while(true) {
		ss >> _arrval->_value[i][j];
		ss >> c;
		if (c == ',') {
			j++;
			if (j == n) {
				cout << "Error: ';' expected!" << endl;
				return;
			} 
		} else if (c == ';') {
			i++;
			if (j < n-1) {
				cout << "Error: ',' expected!" << endl;
				return;
			}
			j = 0;
		} else if (c == ']') {
			if (j < n-1) {
				cout << "Error: ',' expected!" << endl;
				return;
			}			
			break;
		}
	}
	ss >> c;
}

void MATRIX::resize(int mm, int nn)
{
	if (n() != nn || _arrval->_parent || _arrval->_ref > 1) {
		ARRAY2D *narrval = new ARRAY2D(mm,nn);
		
		for (int i = 0; i < min(mm,m()); i++) {
			for (int j = 0; j < min(nn,n()); j++) {
				narrval->_value[i][j] = _arrval->_value[i][j];
			}
		}
		
		delarrval();
		_arrval = narrval;
		return;
	}

	CRTYPE **oval = _arrval->_value;
	_arrval->_value = new CRTYPE*[mm];
  int i;
	for (i = 0; i < min(m(),mm); i++) {
		_arrval->_value[i] = oval[i];
	}
	for (i = mm; i < m(); i++)
		delete[] oval[i];
	for (i = m(); i < mm; i++)
		_arrval->_value[i] = new CRTYPE[nn];
	delete oval;
	_arrval->_m = mm;
}

// DESTRUCTOR:

void MATRIX::delarrval()
{
	ARRAY2D *tmp = _arrval;
	tmp->_ref--;
	while (tmp->_ref == 0) {
		if (tmp->_parent) 
			tmp = tmp->_parent;
		else {
			delete tmp;
		}
		tmp->_ref--;
	}
}

MATRIX::~MATRIX()
{
	delarrval();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MATRIX MATRIX::clone() const
{
	MATRIX M(m(), n());

	for (int i = 0; i < m(); i++)
	for (int j = 0; j < n(); j++)
		M._arrval->_value[i][j] = _arrval->_value[i][j];
	return M;
}

inline
CRTYPE& MATRIX::value(int i, int j)
{
	if (_arrval) return _arrval->_value[i][j];
}

MATRIX MATRIX::sub(int ii, int jj, int mm, int nn)
{
	if (ii < 0 || jj < 0 || ii + mm > m() || jj + nn > n()) {
		cout << "Error: Indices exceed the limits (Sub-Matrix)!" << endl;
		return MATRIX();
	}
	CRTYPE **x = new CRTYPE*[mm];
	for (int i = 0; i < mm; i++) 
		x[i] = &_arrval->_value[ii + i][jj];

	MATRIX M(mm, nn, x);
	M._arrval->_parent = _arrval;
	_arrval->_ref++;
	return M;
}

inline
int MATRIX::m() const
{
	return _arrval->_m;
}

inline
int MATRIX::n() const
{
	return _arrval->_n;
}

void MATRIX::zero()
{
	for (int i = 0; i < m(); i++)
	for (int j = 0; j < n(); j++)
		_arrval->_value[i][j] = 0.0;
	return;
}

void MATRIX::eye()
{
	int i,j;
	for (i = 0; i < m(); i++) {
		for (j = 0; j < n(); j++)
			_arrval->_value[i][j] = 0.0;
	}
	for (i = 0; i < min(m(), n()); i++)
		_arrval->_value[i][i] = 1.0;
	return;
}

void MATRIX::transpose()
{
	int i,j;
	CRTYPE **tmp = newarray(n(), m());

	for (i = 0; i < m(); i++)
		for (j = 0; j < n(); j++)
			tmp[j][i] = _arrval->_value[i][j];

	_arrval->setval(n(), m(), tmp);
	return;
}

void MATRIX::factorial()
{
	if (m() != 1 || n() != 1 || imag(value(0,0)) != 0.) {
		cout << "Error: Positive integer expected (Factorial)!" << endl;
		return;
	}

	NRTYPE rv = real(value(0,0));
	static const NRTYPE maxulong = pow(2.0, (NRTYPE)(sizeof(unsigned long)*8));
	NRTYPE stirling = sqrt(2.*PI*rv)*pow(rv / EULER, rv);

	if (rv < 0)
		cout << "Error: Positive integer expected  (Factorial)!" << endl;
	else if (stirling > maxulong)
		value(0,0) = stirling;
	else {
		unsigned long x = (unsigned long)rv;
		if ((NRTYPE)x != rv) {
			cout << "Error: Positive integer expected (Factorial)!" << endl;
			return;
		}

		unsigned long f = 1;
		for (unsigned long i = 2; i <= x; i++)
			f *= i;

		value(0,0) = (CRTYPE)f;
	}
	return;
}

void MATRIX::permutate(int i, int j)
{
	CRTYPE *tmp = _arrval->_value[i];
	_arrval->_value[i] = _arrval->_value[j];
	_arrval->_value[j] = tmp;
	return;
}

void MATRIX::perm_c(int i, int j)
{
	CRTYPE tmp;

	for (int k = 0; k < m(); k++) {
		tmp = _arrval->_value[k][j];
		_arrval->_value[k][j] = _arrval->_value[k][i];
		_arrval->_value[k][i] = tmp;
	}
	return;
}

bool MATRIX::lr()
{
	if (m() > n()){
		cout << "Error: n > m (LR-Decomposition)!" << endl;
		return false; // MATRIX(m, m);
	}

	int i, j, ii;

	for (i = 0; i < m(); i++) {
		if (_arrval->_value[i][i] == 0.0) {
			cout << "Error: LR-Decomposition (without pivoting) not possible!" << endl;
			return false; // MATRIX(m, m);
		}

		for (ii = i+1; ii < m(); ii++) {
			if (_arrval->_value[ii][i] != 0.0) {
				_arrval->_value[ii][i] = _arrval->_value[ii][i]/_arrval->_value[i][i];
				for (j = i+1; j < n(); j++)
					_arrval->_value[ii][j] -= _arrval->_value[ii][i]*_arrval->_value[i][j];
			}
		}
	}
	return true;
}

bool MATRIX::plr(MATRIX *L, MATRIX *P)
{
	if (m() > n()){
		cout << "Error: n > m (LR-Decomposition)!" << endl;
		return false; // MATRIX(m, m);
	}

	int i, j, ii;
	if (L) L->eye();
	if (P && P != L) P->eye();


	for (i = 0; i < m(); i++) {
		// Pivoting:
		for (j = i+1; j < m(); j++) {
			if (abs(_arrval->_value[j][i]) > abs(_arrval->_value[i][i])) {
				permutate(i, j);
				if (P) P->permutate(i, j);
			}
		}

		if (_arrval->_value[i][i] == 0.0) {
			cout << "Error: LR-Decomposition (with pivoting) not possible!" << endl;
			return false; // MATRIX(m, m);
		}

		for (ii = i+1; ii < m(); ii++) {
			if (_arrval->_value[ii][i] != 0.0) {
				_arrval->_value[ii][i] = _arrval->_value[ii][i]/_arrval->_value[i][i];
				if (L) L->_arrval->_value[ii][i] = _arrval->_value[ii][i];
				for (j = i+1; j < n(); j++)
					_arrval->_value[ii][j] -= _arrval->_value[ii][i]*_arrval->_value[i][j];
				_arrval->_value[ii][i] = 0.0;
			}
		}

		/*
		for (j = i+1; j < m(); j++) {
			if (_arrval->_value[j][i] != 0.0) {
				for (jj = i+1; jj < n(); jj++) {
					s = _arrval->_value[j][i]/_arrval->_value[i][i];
					_arrval->_value[j][jj] -= s*_arrval->_value[i][jj];
					if (L) L->_arrval->_value[j][jj] -= s*L->_arrval->_value[i][jj];
				}
				_arrval->_value[j][i] = 0.0;
			}
		}*/
	}
	return true;
}

bool MATRIX::lrc(MATRIX *L, MATRIX *P)
{
	if (m() > n()){
		cout << "Error: n > m (LR-Decomposition)!" << endl;
		return false; // MATRIX(m, m);
	}

	int i, j, ii;
	if (L) L->eye();
	if (P && P != L) P->eye();


	for (i = 0; i < m(); i++) {
		/*
		// Pivoting:
		for (j = i+1; j < m(); j++) {
			if (abs(_arrval->_value[j][i]) > abs(_arrval->_value[i][i])) {
				permutate(i, j);
				if (P) P->permutate(i, j);
			}
		}
		*/

		if (_arrval->_value[i][i] == 0.0) {
			cout << "Error: LR-Decomposition (without pivoting) not possible!" << endl;
			return false; // MATRIX(m, m);
		}

		for (ii = i+1; ii < m(); ii++) {
			if (_arrval->_value[ii][i] != 0.0) {
				_arrval->_value[ii][i] = _arrval->_value[ii][i]/_arrval->_value[i][i];
				if (L) L->_arrval->_value[ii][i] = _arrval->_value[ii][i];
				for (j = i+1; j < n(); j++)
					_arrval->_value[ii][j] -= _arrval->_value[ii][i]*_arrval->_value[i][j];
				_arrval->_value[ii][i] = 0.0;
			}
		}

		/*
		for (j = i+1; j < m(); j++) {
			if (_arrval->_value[j][i] != 0.0) {
				for (jj = i+1; jj < n(); jj++) {
					s = _arrval->_value[j][i]/_arrval->_value[i][i];
					_arrval->_value[j][jj] -= s*_arrval->_value[i][jj];
					if (L) L->_arrval->_value[j][jj] -= s*L->_arrval->_value[i][jj];
				}
				_arrval->_value[j][i] = 0.0;
			}
		}*/
	}
	return true;
}

int MATRIX::mr(MATRIX *M)
{
	int i, j, ii, z;
	int perm_count = 0;

	if (M) {
		*M = MATRIX(m(),m());
		M->eye();
	}

	z = 0;
	for (i = 0; i < m(); i++) {
		// Pivoting:
		for (ii = z+1; ii < m(); ii++) {
			if (abs(_arrval->_value[ii][i]) > abs(_arrval->_value[z][i])) {
				permutate(z, ii);
				if (M) M->perm_c(z, ii);
				perm_count++;
			}
		}

		if (_arrval->_value[z][i] == 0.0) continue;

		for (ii = z+1; ii < m(); ii++) {
			if (_arrval->_value[ii][i] != 0.0) {
				_arrval->_value[ii][i] /= _arrval->_value[z][i];
				for (j = i+1; j < n(); j++)
					_arrval->_value[ii][j] -= _arrval->_value[ii][i]*_arrval->_value[z][j];
				for (j = 0; j < m() && M; j++)
					M->_arrval->_value[j][z] += _arrval->_value[ii][i]*M->_arrval->_value[j][ii];
				_arrval->_value[ii][i] = 0.0;
			}
		}
		z++;
	}
	if (z < m()) {
		for (i = 0; i < m(); ) {
			for (j = i; j < n() && _arrval->_value[i][j] == 0.; j++);
			if (j != i && j != n()) {
				permutate(i, j);
				if (M) M->perm_c(i, j);
				perm_count++;
			} else
				i++;
		}
	}
	round(1.e-14);
	return perm_count;
}

bool MATRIX::rev_sub()
{
	int i, j;

	// no solution?
	for (i = 0; i < m(); i++) {
		if (value(i,n()-1) != 0.) {
			for (j = i; j < n()-1 && value(i,j) == 0.; j++);
			if (j >= n()-1) return false;
		}
	}

	// scale lines so that diagonal-elt. is 1
	for (i = 0; i < m(); i++) {
		if (value(i,i) != 0.) {
			for (j = i+1; j < n(); j++)
				value(i,j) /= value(i,i);
			value(i,i) = 1.;
		}
	}

	// resize if needed
	if (m() != n() - 1) {
		int om = m();
		resize(n() - 1, n());
		for (i = om; i < m(); i++) {
			for (j = 0; j < n(); j++)
				value(i,j) = 0.;
		}
	}

	// calc solution:
	inv_r1(1);
	for (j = 0; j < n() - 1; j++) {
		if (value(j,j) != 0.) {
			for (i = 0; i < m(); i++)
				value(i,j) = 0.;
		} else
			value(j,j) = 1.;
	}
	return true;
}

CRTYPE MATRIX::det()
{
	MATRIX M = clone();
	CRTYPE det;

	if (M.mr(0) & 1)
		det = -1.;
	else
		det = 1.;

	for (int i = 0; i < min(m(), n()); i++)
		det *= M.value(i,i);

	return det;
}

CRTYPE MATRIX::trace()
{
	CRTYPE tr = 0;

	for (int i = 0; i < min(m(), n()); i++)
		tr += value(i,i);

	return tr;
}

MATRIX MATRIX::cofactor(int i, int j)
{
	if (m() < 1) return MATRIX(0.);

	int cm = m();
	int cn = n();

	if (i < m()) cm--;
	if (j < n()) cn--;

	CRTYPE **arr = new CRTYPE*[cm];
	CRTYPE **itmp = arr;
	CRTYPE *jtmp;
	for (int ii = 0; ii < m(); ii++){
		if (ii == i) continue;
		*itmp = new CRTYPE[cn];
		jtmp = *itmp;
		itmp++;
		for (int jj = 0; jj < n(); jj++) {
			if (jj == j) continue;
			*jtmp = value(ii,jj);
			jtmp++;			
		}
	}
	return MATRIX(cm, cn, arr);
}

bool MATRIX::qr(MATRIX *Q, MATRIX *R)
{
	if (m() < n()){
		cout << "Error: m < n (QR-Decomposition)!" << endl;
		return false; // MATRIX(m, m);
	}

	int i, j, xi, yi;
	MATRIX qtmp;
	MATRIX rtmp;
	if (!Q) Q = &qtmp;
	if (!R) R = &rtmp;
	
	MATRIX S(m(), m());
	*R = clone();
	Q->eye();
	MATRIX v;
	CRTYPE nrm, nrm2;

	for (i = 0; i < n(); i++) {
		// Norm des i-ten Spaltenvektors v:
		nrm = 0.;
		for (j = i; j < m(); j++) {
			nrm += R->value(j, i)*R->value(j, i);
		}
		nrm = sqrt(nrm);

		// v += sign(v1)*||v||*e_i:
		v = R->sub(i,i, m()-i, 1);
		v.value(0,0) += sign(real(R->value(i,i))) * nrm;

		// nrm2 = v'*v:
		nrm2 = 0.;
		for (j = 0; j < v.m(); j++) {
			nrm2 += v.value(j, 0)*v.value(j, 0);
		}

		// T = (-2/nrm2)*v*v' + I:
		S.eye();
		MATRIX T = S.sub(i,i,v.m(),v.m());
		for (xi = 0; xi < v.m(); xi++) {
			for (yi = 0; yi < v.m(); yi++) {
				T.value(xi, yi) -= 2.0/nrm2 * v.value(xi, 0) * v.value(yi, 0);
			}
		}

		// Q, R aktualisieren:
		if (R) {
			MATRIX RS = R->sub(i,i+1, v.m(), n()-i-1);
			RS = T * RS;

			R->value(i,i) = -sign(real(R->value(i,i)))*nrm;
			for (j = i+1; j < m(); j++) 
				R->value(j,i) = 0.0;
		}
		if (Q) *Q *= S;
	}
	return true;
}

MATRIX MATRIX::eig(NRTYPE TOL) const
{
	int d = m();
	if (d != n()) {
		cout << "Error: QR-Method not possible! Matrix not quadratic!" << endl;
		return MATRIX(0.);
	}
	MATRIX v(1, d);

	MATRIX A(clone());
	MATRIX Q(d,d);
	MATRIX R(d,d);
	int i;
	CRTYPE last;

	for (i = 0; i < 1000000/(d*d*d); i++) {
		if (!A.qr(&Q, &R)) break;
		last = A.value(d-1,d-1);
		A = R*Q;
		last -= A.value(d-1,d-1);
		if (abs(last) < TOL) break;
	}
	for (i = 0; i < d; i++)
		v.value(0, i) = A.value(i,i);
	return v;
}

void MATRIX::setD()
{
	_mtype = D;
}

void MATRIX::setL()
{
	_mtype = L;
}

void MATRIX::setR()
{
	_mtype = R;
}

void MATRIX::setNone()
{
	_mtype = None;
}

MTYPE MATRIX::getMType()
{
	return _mtype;
}

/*
MATRIX MATRIX::reverse()
{
	int i, j;
	if (m()+1 != n()) {
		cout << "Error: m+1 != n (Reverse Substitution)!" << endl;
		return MATRIX();
	}
	MATRIX x = MATRIX(m(), 1);
	for (i = m()-1; i >= 0; i--) {
		if (_arrval->_value[i][i] == 0.) {
			cout << "Error: Matrix not invertible (Reverse Substitution)!" << endl;
			return MATRIX();
		}
		x._arrval->_value[i][0] = _arrval->_value[i][m()];
		for (j = i+1; j < m(); j++)
			x._arrval->_value[i][0] -= _arrval->_value[i][j]*x._arrval->_value[j][0];
		x._arrval->_value[i][0] /= _arrval->_value[i][i];
	}
	return x;
}
*/

bool MATRIX::invert()
{
	if (m() == 1 && n() == 1) {
		if (_arrval->_value[0][0] != 0.)
			_arrval->_value[0][0] = 1.0/_arrval->_value[0][0];
		else
			cout << "Error: Division by 0!" << endl;
		return false;
	}

	if (m() != n()){
		cout << "Error: n != m (Inversion)!" << endl;
		return false; // MATRIX(m, m);
	}

	int i, j, ii;
	MATRIX I(m(),m());
	I.eye();

	for (i = 0; i < m(); i++) {
		// Pivoting:
		for (j = i+1; j < m(); j++) {
			if (abs(_arrval->_value[j][i]) > abs(_arrval->_value[i][i])) {
				permutate(i, j);
				I.permutate(i, j);
			}
		}

		if (_arrval->_value[i][i] == 0.0) {
			cout << "Error: LR-Decomposition (with pivoting) not possible (Inversion)!" << endl;
			return false; // MATRIX(m, m);
		}

		for (ii = i+1; ii < m(); ii++) {
			if (_arrval->_value[ii][i] != 0.0) {
				_arrval->_value[ii][i] = _arrval->_value[ii][i]/_arrval->_value[i][i];
				for (j = i+1; j < n(); j++)	_arrval->_value[ii][j] -= _arrval->_value[ii][i]*_arrval->_value[i][j];
				for (j = 0; j < n(); j++) I._arrval->_value[ii][j] -= _arrval->_value[ii][i]*I._arrval->_value[i][j];
			}
		}
	}
	for (i = 0; i < m(); i++) {
		for (j = i+1; j < n(); j++) _arrval->_value[i][j] /= _arrval->_value[i][i];
		for (j = 0; j < n(); j++) I._arrval->_value[i][j] /= _arrval->_value[i][i];
	}
	for (i = m()-2; i >= 0; i--) {
		for (ii = i+1; ii < n(); ii++) 
			for (j = 0; j < n(); j++) I._arrval->_value[i][j] -= I._arrval->_value[ii][j]*_arrval->_value[i][ii];
	}
	swap(_arrval, I._arrval);
	return true;
}

void MATRIX::inv_r1(int last)
{
	for (int i = 0; i < m(); i++) {
		for (int j = i+1; j < n() - last; j++) {
			value(i,j) = -value(i,j);
			for (int jj = j+1; jj < n(); jj++) {
				value(i,jj) += value(i,j) * value(j,jj);
			}
		}
	}
}

void MATRIX::expo(int n)
{
	if (n % 2 == 0) {
		this->expo(n/2);
		*this = *this * *this;
	} else if (n > 1) {
		MATRIX mat = clone();
		mat.expo(n/2);
		*this = *this * mat * mat;
	}
	return;
}

CRTYPE MATRIX::norm(double p) const
{
	int i, j;
	double norm = 0.;
	double max;
	if (m() == 1 && n() > 0) {
		if (p > 0.) {
			for (j = 0; j < n(); j++) 
				norm += pow(abs(_arrval->_value[0][j]), p);
			return pow(norm, 1/p);
		} else if (p == -1.) {
			NRTYPE smax = abs(_arrval->_value[0][0]);
			for (j = 1; j < n(); j++)
				if (abs(_arrval->_value[0][j]) > smax) smax = abs(_arrval->_value[0][j]);
			return smax;
		}
	}
	if (m() > 0 && n() == 1) {
		if (p > 0.) {
			for (i = 0; i < m(); i++) 
				norm += pow(abs(_arrval->_value[i][0]), p);
			return pow(norm, 1/p);
		} else if (p == -1.) {
			NRTYPE smax = abs(_arrval->_value[0][0]);
			for (i = 1; i < m(); i++)
				if (abs(_arrval->_value[i][0]) > smax) smax = abs(_arrval->_value[i][0]);
			return smax;
		}
	}
	if (m() <= 0 && n() <= 0) {
		cout << "Error: Invalid Matrix (Norm)!" << endl;
		return 0.;
	}

	int ip = (int)p;
	if (p == (double)ip && (ip == 1 || ip == 2 || ip == -1)) {
		if (p == 1) {
			for (i = 0; i < m(); i++) {
				max = 0.;
				for (j = 0; j < n(); j++) max += abs(_arrval->_value[i][j]);
        if (max > norm) norm = max;
			}
		} else if (p == -1) {
			for (j = 0; j < n(); j++) {
				max = 0.;
				for (i = 0; i < m(); i++) max += abs(_arrval->_value[i][j]);
				if (max > norm) norm = max;
			}
		} else if (p == 2) {
			MATRIX M = clone();
			M.transpose();
			M *= *this;
			MATRIX e = M.eig(1.e-12);

			NRTYPE smax = abs(e.value(0,0));
			for (int i = 1; i < e.n(); i++)
				if (abs(M.value(0,i)) > smax) smax = abs(e.value(0,i));

			return sqrt(smax);
		}
		return norm;
	}
	cout << "Error: p = " << p << " not in {1, 2, -1}!" << endl;
	return 0.;
}

void MATRIX::round(NRTYPE d)
{
	for (int i = 0; i < m(); i++) {
		for (int j = 0;  j < n(); j++) {
			if (abs(value(i,j)) < d) value(i,j) = 0.;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MATRIX::add(const MATRIX& other)
{
	if (m() == other.m() && n() == other.n()) {
		for (int i = 0; i < m(); i++) 
		for (int j = 0; j < n(); j++) 
			_arrval->_value[i][j] += other._arrval->_value[i][j];
		return true;
	} else 
		return false;
}

bool MATRIX::subtract(const MATRIX& other)
{
	if (m() == other.m() && n() == other.n()) {
		for (int i = 0; i < m(); i++) 
		for (int j = 0; j < n(); j++) 
			_arrval->_value[i][j] -= other._arrval->_value[i][j];
		return true;
	} 
	return false;
}

bool MATRIX::mult(const MATRIX& other, MATRIX& result)
{
	if (n() == 1 && m() == 1) {
		if (result.m() != other.m() || result.n() != other.n())
			return false;
		for (int i = 0; i < other.m(); i++) 
		for (int j = 0; j < other.n(); j++) 
			result._arrval->_value[i][j] = _arrval->_value[0][0] * other._arrval->_value[i][j];
	} else if (other.n() == 1 && other.m() == 1) {
		if (result.m() != m() || result.n() != n())
			return false;
		for (int i = 0; i < m(); i++) 
		for (int j = 0; j < n(); j++) 
			result._arrval->_value[i][j] = other._arrval->_value[0][0] * _arrval->_value[i][j];
	} else 	if (n() == other.m()) {
		if (result.m() != m() || result.n() != other.n())
			return false;
		result.zero();
		for (int k = 0; k < m(); k++)
		for (int l = 0; l < other.n(); l++)
			for (int j = 0; j < n(); j++)
				result._arrval->_value[k][l] += _arrval->_value[k][j] * other._arrval->_value[j][l];
	} else
		return false;

	return true;
}

bool MATRIX::mult(const CRTYPE& other)
{
	for (int i = 0; i < m(); i++) {
		for (int j = 0; j < n(); j++)
			_arrval->_value[i][j] *= other;
	}
	return true;
}

bool MATRIX::div(const CRTYPE& other)
{
	for (int i = 0; i < m(); i++) {
		for (int j = 0; j < n(); j++)
			_arrval->_value[i][j] /= other;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MATRIX MATRIX::operator +=(const MATRIX& other)
{
	if (add(other))	return *this;
	cout << "Error: Different dimensions (Addition)!" << endl;
	return MATRIX();
}

MATRIX MATRIX::operator -=(const MATRIX& other)
{
	if (subtract(other)) return *this;
	cout << "Error: Different dimensions (Subtraction)!" << endl;
	return MATRIX();
}

MATRIX MATRIX::operator %=(const NRTYPE& other)
{
	if (m() == 1 && n() == 1 && imag(value(0,0)) == 0.) {
		NRTYPE d = real(value(0,0));
		value(0,0) = fmod(real(value(0,0)), other);
		return *this;
	}
	cout << "Error: Different dimensions (Modulo-Division)!" << endl;
	return MATRIX();
}

MATRIX MATRIX::operator %=(const MATRIX& other)
{
	if (m() == 1 && n() == 1 && imag(value(0,0)) == 0. && other.m() == 1 && other.n() == 1 && imag(other._arrval->_value[0][0]) == 0.) {
		NRTYPE d = real(value(0,0));
		value(0,0) = fmod(real(value(0,0)),real(other._arrval->_value[0][0]));
		return *this;
	}
	cout << "Error: Real number expected (Modulo-Division)!" << endl;
	return MATRIX();
}

MATRIX MATRIX::operator *=(const MATRIX& other)
{
	if (n() == other.m()) {
		MATRIX res(m(), other.n());
		if (mult(other, res)) {
			*this = res;
			return *this;
		}
	} else if (n() == 1 && m() == 1) {
		MATRIX res(other.m(), other.n());
		if (mult(other, res)) {
			*this = res;
			return *this;
		}
	} else if (other.n() == 1 && other.m() == 1) {
		return mult(other._arrval->_value[0][0]);
	}
	cout << "Error: Different dimensions (Multiplication)!" << endl;
	return MATRIX();
}

MATRIX MATRIX::operator *=(const CRTYPE& other)
{
	if (mult(other)) return *this;
	cout << "Error: Different dimensions (Multiplication)!" << endl;
	return MATRIX();
}

MATRIX MATRIX::operator /=(const CRTYPE& other)
{
	if (div(other))	return *this;
	cout << "Error: Different dimensions (Division)!" << endl;
	return MATRIX();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MATRIX MATRIX::operator +(const MATRIX& other) const
{
	MATRIX mat = MATRIX(m(), n());

	if (m() == other.m() && n() == other.n()) 
		for (int i = 0; i < m(); i++) 
		for (int j = 0; j < n(); j++) 
			mat._arrval->_value[i][j] = _arrval->_value[i][j] + other._arrval->_value[i][j];
	else
		cout << "Error: Different dimensions (Addition)!" << endl;

	return mat;
}

MATRIX MATRIX::operator -(const MATRIX& other) const
{
	MATRIX mat = MATRIX(m(), n());

	if (m() == other.m() && n() == other.n()) 
		for (int i = 0; i < m(); i++) 
		for (int j = 0; j < n(); j++) 
			mat._arrval->_value[i][j] = _arrval->_value[i][j] - other._arrval->_value[i][j];
	else
		cout << "Error: Different dimensions (Subtraction)!" << endl;

	return mat;
}

MATRIX MATRIX::operator -() const
{
	MATRIX mat = MATRIX(m(), n());
	for (int i = 0; i < m(); i++) 
	for (int j = 0; j < n(); j++) 
		mat._arrval->_value[i][j] = -_arrval->_value[i][j];
	return mat;
}

MATRIX MATRIX::operator *(const MATRIX &other) const
{
	MATRIX mat;
	if (n() == 1 && m() == 1) {
		mat = MATRIX(other.m(), other.n());
		for (int i = 0; i < other.m(); i++) 
		for (int j = 0; j < other.n(); j++) 
			mat._arrval->_value[i][j] = _arrval->_value[0][0] * other._arrval->_value[i][j];
	} else if (other.n() == 1 && other.m() == 1) {
		mat = MATRIX(m(), n());
		for (int i = 0; i < m(); i++) 
		for (int j = 0; j < n(); j++) 
			mat._arrval->_value[i][j] = other._arrval->_value[0][0] * _arrval->_value[i][j];
	} else 	if (n() == other.m()) {
		mat = MATRIX(m(), other.n());
		mat.zero();
		for (int k = 0; k < m(); k++)
		for (int l = 0; l < other.n(); l++)
			for (int j = 0; j < n(); j++)
				mat._arrval->_value[k][l] += _arrval->_value[k][j] * other._arrval->_value[j][l];
	} else
		cout << "Error: Different dimensions (Multiplication)!" << endl;

	return mat;
}

MATRIX MATRIX::operator *(const CRTYPE& other) const
{
	MATRIX mat = MATRIX(m(), n());
	for (int i = 0; i < m(); i++) {
		for (int j = 0; j < n(); j++)
			mat._arrval->_value[i][j] = _arrval->_value[i][j] * other;
	}
	return mat;
}

MATRIX MATRIX::operator /(const MATRIX& other) const
{
	if (m() == 1 && n() == 1)
		return (_arrval->_value[0][0] / other._arrval->_value[0][0]);

	MATRIX otherinv = other.clone();
	otherinv.invert();

	return (*this * otherinv);
}

MATRIX MATRIX::operator /(const CRTYPE& other) const
{
	MATRIX mat = MATRIX(m(), n());
	for (int i = 0; i < m(); i++) {
		for (int j = 0; j < n(); j++)
			mat._arrval->_value[i][j] = _arrval->_value[i][j] / other;
	}
	return mat;
}

MATRIX MATRIX::operator ^(const MATRIX& other) const
{
	MATRIX mat;
	if (m() == n()) {
		if (other.m() == 1 && other.n() == 1) { //  && imag(other._arrval->_value[0][0]) == 0.
			if (m() == 1 && n() == 1)
				return pow(_arrval->_value[0][0], other._arrval->_value[0][0]);

			int o = (int)real(other._arrval->_value[0][0]);
			if ((CRTYPE)o == other._arrval->_value[0][0]) {
				int i;
				if (o < 0) {
					mat = clone();
					mat.invert();
					mat.expo(-o);
					return mat;
				} else if (o == 0) {
					mat = MATRIX(m(), n());
					for (i = 0; i < min(m(),n()); i++) mat._arrval->_value[i][i] = 1.0;
					return mat;
				} else if (o > 0) {
					mat = clone();
					mat.expo(o);
					return mat;
				}
			}
			cout << "Error: Value must be an integer (Power)!" << endl;
			return mat;
		}
		cout << "Error: Exponent as matrix not valid (Power)!" << endl;
	}
	cout << "Error: Matrix not quadratic (Power)!" << endl;
	return mat;
}


MATRIX MATRIX::operator =(const MATRIX& other)
{
	int i,j;
	if (m() != other.m() || n() != other.n()) {
		ARRAY2D *tmp = _arrval;
		delarrval();
		_arrval = new ARRAY2D(*other._arrval);
	} else {
		switch (other._mtype) {
			case None:
				for (i = 0; i < m(); i++)
				for (j = 0; j < n(); j++)
					_arrval->_value[i][j] = other._arrval->_value[i][j];
				break;
			case D:
				for (i = 0; i < m(); i++)
				for (j = 0; j < n(); j++)
					_arrval->_value[i][j] = 0.0;
				for (i = 0; j < min(m(), n()); i++)
					_arrval->_value[i][i] = other._arrval->_value[i][i];
				break;
			case L:
				for (i = 0; i < m(); i++) {
					for (j = 0; j < i; j++)
						_arrval->_value[i][j] = other._arrval->_value[i][j];
					_arrval->_value[i][i] = 1.0;
					for (j = i+1; j < n(); j++)
						_arrval->_value[i][j] = 0.0;
				}
				break;
			case R:
				for (i = 0; i < m(); i++) {
					for (j = 0; j < i; j++)
						_arrval->_value[i][j] = 0;
					for (j = i; j < n(); j++)
						_arrval->_value[i][j] = other._arrval->_value[i][j];
				}
				break;
		}
	}

	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<(ostream &ostr, MATRIX &M)
{
	int i,j;
	if (M.n() == 1)
		ostr.precision(16);
	else
		ostr.precision(3);
	switch (M.getMType()) {
		case None:
			for (i = 0; i < M.m(); i++) {
				for (j = 0; j < M.n(); j++) {
					cout << "\t" << M.value(i, j);
				}
				ostr << endl;
			}
			break;
		case L:
			for (i = 0; i < M.m(); i++) {
				for (j = 0; j < M.n(); j++) {
					if (j < i)
						cout << "\t" << M.value(i, j);
					else if (j == i)
						cout << "\t" << 1.0;
					else
						cout << "\t" << 0.0;
				}
				ostr << endl;
			}
			break;
		case R:
			for (i = 0; i < M.m(); i++) {
				for (j = 0; j < M.n(); j++) {
					if (j >= i)
						cout << "\t" << M.value(i, j);
					else
						cout << "\t" << 0.0;
				}
				ostr << endl;
			}
			break;
		case D:
			for (i = 0; i < M.m(); i++) {
				for (j = 0; j < M.n(); j++) {
					if (j == i)
						cout << "\t" << M.value(i, j);
					else
						cout << "\t" << 0.0;
				}
				ostr << endl;
			}
			break;
	}
	return ostr;
}

void operator <<(ofstream &ofstr, MATRIX &M)
{
	ofstr.write((const char *)&M._arrval->_m, sizeof(int));
	ofstr.write((const char *)&M._arrval->_n, sizeof(int));
	for (int i = 0; i < M.m(); i++)
		ofstr.write((const char*)M._arrval->_value[i], M.n()*sizeof(CRTYPE));
}

