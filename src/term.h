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

// term.h

#ifndef TERM_H
#define TERM_H 

#include <iostream>

// EXPRESSION-NODE: base node class
class ENODE {
public:
	virtual MATRIX value(MATRIX **args) = 0;
	virtual void print(std::ostream &ostr, char **argname, int oplevel) = 0;
	virtual char *name() = 0;
	virtual MATRIX& data() = 0;
};

// STRING-NODE
class SNODE : public ENODE {
public:
	SNODE(char*& data);
	~SNODE()	{	delete _data;	}

	virtual MATRIX value(MATRIX **args) { return MATRIX(0.); }
	virtual void print(std::ostream &ostr, char **argname, int oplevel);
	virtual char *name() { return _data; };
	virtual MATRIX& data() { return *(MATRIX *)_data; }

	char *_data;
};

// CONSTANT-NODE
class CNODE : public ENODE {
public:
	CNODE(MATRIX *data) : _data(data) {}
	~CNODE()	{	delete _data;	}

	virtual MATRIX value(MATRIX **args) { return *_data; }
	virtual void print(std::ostream &ostr, char **argname, int oplevel);
	virtual char *name() { return 0; };
	virtual MATRIX& data() { return *_data; }

	MATRIX *_data;
};

// VARIABLE-NODE
class VNODE : public ENODE {
public:
	VNODE(MATRIX **data, char *name) : _data(data), _name(name) {}

	virtual MATRIX value(MATRIX **args) { return **_data; }
	virtual void print(std::ostream &ostr, char **argname, int oplevel);
	virtual char *name() { return _name; };
	virtual MATRIX& data() { return **_data; }

	char *_name;
	MATRIX **_data;
};

class FUNCTION;

typedef LIST<ENODE> ELIST;
typedef LIST<ELIST>	EELIST;

class TNODE : public ENODE {
public:
	TNODE(char name);
	TNODE(char name, int x);
	TNODE(char name, ELIST& lst);		// +,*,^
	TNODE(char name, ENODE *t);			// -,/,!,'
	TNODE(char name, int argnr, ENODE **t);		// :
	TNODE(EELIST *lst);					// Matrix
	TNODE(FUNCTION **f, ENODE **args);	// Function

	MATRIX makevector(MATRIX **args);
	MATRIX makematrix(MATRIX **args);

	virtual MATRIX value(MATRIX **args);
	virtual void print(std::ostream &ostr, char **argname, int oplevel);
	virtual char *name() { return _name; };
	virtual MATRIX& data() { return *(MATRIX *)_name; }

	char *_name;
	ENODE **_args;
	int _argnr;
};

void setvar(char *name, MATRIX *m);
void setfunction(FUNCTION *f);
FUNCTION **getfunction(char *name);
MATRIX **getvar(char *name);
std::ostream& operator <<(std::ostream &ostr, ENODE &v);
int level(char op);

#endif