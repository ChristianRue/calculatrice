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

// function.cpp

#include <iostream>

#include "utilities.h"
#include "matrix.h"
#include "term.h"
#include "function.h"

using namespace std;

FUNCTION::FUNCTION(char *name, char*& term) : _name(name)
{
	LIST<char> lst;

	while (*term == ' ') term++;
	lst.add(getname(term));
	while (*term == ' ') term++;

	while (*term == ',') {
		term++;
		while (*term == ' ') term++;
		lst.add(getname(term));
		while (*term == ' ') term++;
	}

	_argnr = lst.count();
	_argname = lst.makearray();
}

FUNCTION::~FUNCTION()
{
	delete[] _name;
	for (int i = 0; i < _argnr; i++)
		delete[] _argname[i];
	delete[] _argname;
}

void FUNCTION::printhead (std::ostream& ostr)
{
	ostr << _name << "(";
	if (_argnr > 0) {
		ostr << _argname[0];
		for (int i = 1; i < _argnr; i++)
			ostr << ", " << _argname[i];
	}
	ostr << ")";
}

void FUNCTION::describe (std::ostream& ostr)
{
	ostr << "description:" << endl << "   ";
	print(ostr);
	ostr << endl << "arguments:" << endl;
	if (_argnr > 0) {
		ostr << " - " << _argname[0];
		for (int i = 1; i < _argnr; i++) ostr << ", " << endl << " - " << _argname[i];
	}
	ostr << endl;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MATRIX USERFUNCTION::value(ENODE **eargs, MATRIX **args)
{
	if (_term) {
		MATRIX **margs = new MATRIX*[_argnr];
		for (int i = 0; i < _argnr; i++) {
			MATRIX m = eargs[i]->value(args);
			margs[i] = new MATRIX(m);
		}

		MATRIX m(_term->value(margs));

		for (int i = 0; i < _argnr; i++) 
			delete margs[i];
		delete[] margs;
		return m;
	}
	return MATRIX(0.);
}

MATRIX USERFUNCTION::value(MATRIX **args)
{
	if (_term)
		return _term->value(args);
	return MATRIX(0.);
}

void USERFUNCTION::print (std::ostream& ostr)
{
	if (_term) _term->print(ostr, _argname, 0);
}

ostream& operator <<(ostream &ostr, FUNCTION &f)
{
	f.print(ostr);
	return ostr;
}

