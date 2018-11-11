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

// expression.h

#ifndef EXPRESSION_H
#define EXPRESSION_H

class ENODE;
class CALCULATOR;

class EXPRESSION
{
public:
	virtual void execute() = 0;
	virtual void print(CALCULATOR *calc) = 0;
};

class EXPSETVAR : public EXPRESSION
{
public:
	EXPSETVAR(MATRIX **m, ENODE *term) : _data(m), _term(term) {}

	virtual void execute();
	virtual void print(CALCULATOR *calc);

private:
	MATRIX **_data;
	ENODE *_term;
};

class EXPSETFUNCTION : public EXPRESSION
{
public:
	EXPSETFUNCTION(USERFUNCTION *f, ENODE *term) : _function(f), _term(term) {}

	virtual void execute();
	virtual void print(CALCULATOR *calc);

private:
	USERFUNCTION *_function;
	ENODE *_term;
};

class EXPGETFUNCTION : public EXPRESSION
{
public:
	EXPGETFUNCTION(FUNCTION **f) : _function(f) {}

	virtual void execute();
	virtual void print(CALCULATOR *calc);

private:
	FUNCTION **_function;
};

#endif