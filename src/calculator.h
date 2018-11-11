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

// calculator.h

#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <ostream>

#include "utilities.h"
#include "nrtype.h"
#include "matrix.h"

#include "term.h"
#include "function.h"
#include "resfun.h"

#include "memory.h"

#include "expression.h"

class CALCULATOR
{
public:
	CALCULATOR(std::ostream& log, const char* varmemfile, const char *funmemfile);
	~CALCULATOR();

	void operator <<(char *input);

private:
	bool expect(char* &term, char expected);

	MATRIX **arglist(char* &term);
	ELIST *arguments(char*& term);
	ENODE *colon(char* &term);
	EELIST *sarguments(char*& term);
	TNODE *reservedfunction(char *name, ELIST *lst);
	ENODE *function(char *name, char*& term);
	ENODE *variable(char *name, char*& term, bool define);
	ENODE *value(char* &term);
	ENODE *submatrix(char*& term);
	ENODE *imagine(char* &term);
	ENODE *power(char* &term);
	ENODE *transpose(char* &term);
	ENODE *factorial(char* &term);
	ENODE *inverse(char* &term);
	ENODE *product(char* &term);
	ENODE *modulo(char* &term);
	ENODE *negative(char* &term);
	ENODE *sum(char* &term);

	char expression(char* &term);

	//USERFUNCTION *create_userfunction(char *head, char *term);

	inline void setfunction(FUNCTION *f)		{	_fmem.setvar(f->_name, f);	_argname = f->_argname;	_argnr = f->_argnr;	}
	inline FUNCTION** getfunction(char *name)	{	return _fmem.getvar(name);	}
	inline void setvar(char *name, MATRIX *m)	{	_mem.setvar(name, m);		}
	inline MATRIX** getvar(char *name)			{	return _mem.getvar(name);	}

	char **_argname;
	int _argnr;

	VARMEMORY<MATRIX>	_mem;
	VARMEMORY<FUNCTION>	_fmem;
	EXPRESSION *_expression;

	std::ostream& _log;
	const char *_vmemfile;
	const char *_fmemfile;
	friend class EXPSETVAR;
	friend class EXPSETFUNCTION;
	friend class EXPGETFUNCTION;
};

/*
admissible syntax in Backus-Naur form:
======================================

<equation> ::= <variable> = <sum> | <function> = <sum>
<sum> ::= <negative> | <sum>+<sum> | <sum><negative>
<negative> ::= <modulo> | -<negative>
<modulo>::= <product> | <modulo> % <modulo> 
<product>	::= <inverse> | <product> * <product> | <product><inverse> 
<inverse>	::= <factorial> | /<inverse>
<factorial> ::= <transpose> | <factorial> !
<transpose>	::= <power> | <transpose> '
<power> ::= <imagine> | <power> ^ <power>
<imagine> ::= <submatrix> | <imagine> i
<submatrix> ::= <value> | <submatrix> [ <submatargs> ]
<submatargs> ::= [<NR>] [: [<NR>]] [, [<NR> [: [<NR>]]]
<value> ::= <NR> | <variable> | <function> | ( <sum> ) | <matrix> | <string>
<variable> ::= <NAME>
<function> ::= <NAME>( <aguments> )
<matrix> ::= [ <sarguments> ]
<sarguments> ::= <arguments> | <arguments> ; <sarguments>
<arguments> ::= <colon> | <colon> , <arguments>
<colon> ::= <sum> | <sum> : <sum>
<vector> ::= <nr> : <nr> | <nr> : <nr> : <nr>
<string> ::= " <CHARACTERS> "
*/

#endif