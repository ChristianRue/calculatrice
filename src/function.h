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

// function.h

#ifndef FUNCTION_H
#define FUNCTION_H

class FUNCTION {
public:
	char *_name;
	char **_argname;
	int _argnr;

	FUNCTION() {}
	FUNCTION(char *name, char*& term);
	FUNCTION(char *name, int argnr, char **argname) : _name(name), _argnr(argnr), _argname(argname) {}
	FUNCTION(std::ifstream &ifstr) {}
	~FUNCTION();

	virtual MATRIX value(ENODE **eargs, MATRIX **args) = 0;
	virtual void print (std::ostream& ostr) = 0;
	virtual void printhead (std::ostream& ostr);
	virtual void describe (std::ostream& ostr);
};

class USERFUNCTION : public FUNCTION {
public:
	USERFUNCTION(char *name, int argnr, char **argname, ENODE *term) : FUNCTION(name, argnr, argname) , _term(term) {}
	USERFUNCTION(char *name, char*& term) : FUNCTION(name, term), _term(0) {}
	~USERFUNCTION() { delete _term; }

	virtual MATRIX value(ENODE **eargs, MATRIX **args);
	MATRIX value(MATRIX **args);
	virtual void print (std::ostream& ostr);

	ENODE *_term;
};

std::ostream& operator <<(std::ostream &ostr, FUNCTION &f);

#endif