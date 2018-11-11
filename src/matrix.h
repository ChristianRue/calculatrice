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

// matrix.h

#ifndef MATRIX_H
#define MATRIX_H 

#include <ostream>
#include <fstream>
#include <complex>

#include "nrtype.h"

enum MTYPE
{
	None, L, R, D
};

class ARRAY2D
{
public:
	int _m, _n;
	CRTYPE **_value;

	ARRAY2D(int m, int n);
	ARRAY2D(int m, int n, CRTYPE **value);
	ARRAY2D(const ARRAY2D& other);
	~ARRAY2D();
private:
	int _ref;
	ARRAY2D *_parent;

	void setval(int m, int n, CRTYPE **value);
	void resize(int m, int n);

	friend class MATRIX;
};

class MATRIX
{
public:
	MATRIX();
	MATRIX(NRTYPE value);
	MATRIX(CRTYPE value);
	MATRIX(const MATRIX& other);
	MATRIX(int m, int n, CRTYPE **value);
	MATRIX(int m, int n);
	MATRIX(const char *MA);
	MATRIX(std::ifstream &ifstr);
	void resize(int m, int n);
	~MATRIX();

	MATRIX clone() const;
	CRTYPE& value(int i, int j);
	MATRIX sub(int i, int j, int m, int n);
	int m() const;
	int n() const;
	void zero();
	void eye();

	CRTYPE norm(double p) const;
	void transpose();
	void factorial();

	int mr(MATRIX *M);				// triangularisation (with pivoting), returns count of permutations
	bool rev_sub();					// reverse substitution
	bool lgs() { mr(0); return rev_sub();}
	CRTYPE det();
	CRTYPE trace();
	MATRIX cofactor(int i, int j);

  bool lrc(MATRIX *L, MATRIX *P);
	bool lr();						// lr-decomposition (without pivoting)
	bool plr(MATRIX *L, MATRIX *P);	// lr-decomposition with pivoting
	bool qr(MATRIX *Q, MATRIX *R);	// qr-decomposition (householder)
	MATRIX eig(NRTYPE TOL) const;

	void setD();
	void setL();
	void setR();
	void setNone();
	MTYPE getMType();

	//MATRIX reverse();
	bool invert();
	void inv_r1(int last = 0);
	void expo(int n);
	void permutate(int i, int j);
	void perm_c(int i, int j);
	void round(NRTYPE d);

	bool add(const MATRIX& other);
	bool subtract(const MATRIX& other);
	bool mult(const MATRIX& other, MATRIX& result);	
	bool mult(const CRTYPE& other);
	bool div(const CRTYPE& other);

	MATRIX operator +=(const MATRIX& other);
	MATRIX operator -=(const MATRIX& other);
	MATRIX operator %=(const MATRIX& other);
	MATRIX operator %=(const NRTYPE& other);
	MATRIX operator *=(const MATRIX& other);
	MATRIX operator *=(const CRTYPE& other);
	MATRIX operator /=(const CRTYPE& other);
	
	MATRIX operator +(const MATRIX& other) const;
	MATRIX operator -(const MATRIX& other) const;
	MATRIX operator -() const;

	MATRIX operator *(const MATRIX& other) const;
	MATRIX operator *(const CRTYPE& other) const;
	MATRIX operator /(const MATRIX& other) const;
	MATRIX operator /(const CRTYPE& other) const;

	MATRIX operator ^(const MATRIX& other) const;

	MATRIX operator =(const MATRIX &other);
private:
	void delarrval();
	MTYPE _mtype;

	ARRAY2D *_arrval;

	friend void operator <<(std::ofstream &ofstr, MATRIX &M);
};

std::ostream& operator <<(std::ostream &ostr, MATRIX &M);
void operator <<(std::ofstream &ofstr, MATRIX &M);

#endif