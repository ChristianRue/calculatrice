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

// resfun.h

#ifndef RESFUN_H
#define RESFUN_H

class MATRIX;
class FUNCTION;

#include <complex>

class RESFUNCTION : public FUNCTION
{
public:
	RESFUNCTION(int argnr, const char *name, const char *desc, ...);

	virtual MATRIX value (ENODE **eargs, MATRIX **args) = 0;
	void print (std::ostream& ostr) { ostr << _desc; }

	char *_desc;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RFNR1 : public RESFUNCTION
{
public:
	RFNR1(const char *name, std::complex<NRTYPE> (*function)(const std::complex<NRTYPE>& x), const char *desc, char *argname) 
		: RESFUNCTION(1, name, desc, argname), resfun(function) {}
	virtual MATRIX value (ENODE **eargs, MATRIX **args);
	std::complex<NRTYPE> (*resfun)(const std::complex<NRTYPE>& x);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RFMAT1 : public RESFUNCTION
{
public:
	RFMAT1(const char *name, MATRIX (*function)(MATRIX x), const char *desc, char *argname) 
		: RESFUNCTION(1, name, desc, argname), resfun(function) {}
	virtual MATRIX value (ENODE **eargs, MATRIX **args);
	MATRIX (*resfun)(MATRIX x);
};

class RFMAT2 : public RESFUNCTION
{
public:
	RFMAT2(const char *name, MATRIX (*function)(MATRIX x, MATRIX y), const char *desc, char *argname1, char *argname2) 
		: RESFUNCTION(2, name, desc, argname1, argname2), resfun(function) {}
	virtual MATRIX value (ENODE **eargs, MATRIX **args);
	MATRIX (*resfun)(MATRIX x, MATRIX y);
};

class RFMAT3 : public RESFUNCTION
{
public:
	RFMAT3(const char *name, MATRIX (*function)(MATRIX x, MATRIX y, MATRIX z), const char *desc, char *argname1, char *argname2, char *argname3) 
		: RESFUNCTION(3, name, desc, argname1, argname2, argname3), resfun(function) {}
	virtual MATRIX value (ENODE **eargs, MATRIX **args);
	MATRIX (*resfun)(MATRIX x, MATRIX y, MATRIX z);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RFRND : public RESFUNCTION
{
public:
	RFRND() : RESFUNCTION(0, "rnd", "a random real number in [-1, 1]") {}
	virtual MATRIX value (ENODE **eargs, MATRIX **args);
};

class RFSUM : public RESFUNCTION
{
public:
	RFSUM() : RESFUNCTION(4, "sum", "function(start) + function(start + 1) + ... + function(end)", "iterator (a defined variable)", "start (in Z)", "end (in Z > start)", "function(iterator)") {}
	virtual MATRIX value (ENODE **eargs, MATRIX **args);
};

class RFPROD : public RESFUNCTION
{
public:
	RFPROD() : RESFUNCTION(4, "prod", "function(start) * function(start + 1) * ... * function(end)", "iterator (a defined variable)", "start (in Z)", "end (in Z > start)", "function(iterator)") {}
	virtual MATRIX value (ENODE **eargs, MATRIX **args);
};

class RFWAV : public RESFUNCTION
{
public:
	RFWAV() : RESFUNCTION(7, "wav", "writes a wave-file", "file name (a string)", "iterator (a defined variable)", "function(iterator)", 
		"rate (samples/second, e.g. 44100Hz)", 
		"depth (byte/sample, e.g. 2 for 16bit)", 
		"length (in seconds)", 
		"channels (1 = mono, 2 = stereo)") {}
	virtual MATRIX value (ENODE **eargs, MATRIX **args);
};

CRTYPE crsin(const CRTYPE& x);
CRTYPE crcos(const CRTYPE& x);
CRTYPE crtan(const CRTYPE& x);
CRTYPE crasin(const CRTYPE& x);
CRTYPE cracos(const CRTYPE& x);
CRTYPE cratan(const CRTYPE& x);
CRTYPE crceil(const CRTYPE& x);
CRTYPE crfloor(const CRTYPE& x);
CRTYPE signum(const CRTYPE& m);
CRTYPE crabs(const CRTYPE& x);
MATRIX minimum(MATRIX m);
MATRIX maximum(MATRIX m);
MATRIX eye(MATRIX M);
MATRIX zero(MATRIX x, MATRIX y);
MATRIX diag(MATRIX M);
MATRIX dim(MATRIX M);
MATRIX size(MATRIX M);
MATRIX norm(MATRIX M, MATRIX p);
MATRIX eig(MATRIX M);

MATRIX lrl(MATRIX M);
MATRIX lrr(MATRIX M);
MATRIX qrq(MATRIX M);
MATRIX qrr(MATRIX M);

MATRIX mrm(MATRIX M);
MATRIX mrr(MATRIX M);
MATRIX lgs(MATRIX M);
MATRIX ker(MATRIX M);
MATRIX det(MATRIX M);
MATRIX trc(MATRIX M);
MATRIX cofactor(MATRIX M, MATRIX i, MATRIX j);

#endif
