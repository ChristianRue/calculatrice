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

// expression.cpp

#include <iostream>
#include "calculator.h"

using namespace std;

void EXPSETVAR::execute()
{
	*_data = new MATRIX(_term->value(0).clone());
}

void EXPSETFUNCTION::execute()
{
	_function->_term = _term;
	return;
}

void EXPGETFUNCTION::execute()
{
	return;
}


void EXPSETVAR::print(CALCULATOR *calc)
{
	calc->_log << **_data << endl;
}

void EXPSETFUNCTION::print(CALCULATOR *calc)
{
	_function->printhead(calc->_log);
	calc->_log << " = ";
	_function->print(calc->_log);
	calc->_log << endl;
}

void EXPGETFUNCTION::print(CALCULATOR *calc)
{
	if (_function) {
		(*_function)->describe(calc->_log);
	} else {
		calc->_log << "Function not defined!" << endl;
	}
}

