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

// calculator.cpp

#include "calculator.h"

using namespace std;

bool CALCULATOR::expect(char* &term, char expected)
{
	while (*term == ' ') term++;
	if (*term++ == expected) return true;
	_log << "Error: \'" << expected << "\' expected!" << endl;
	return false;
}

ENODE *CALCULATOR::colon(char* &term)
{
	ELIST lst;
	lst.add(sum(term));

	while(*term == ' ') term++;
	while (*term == ':') {
		term++;
		lst.add(sum(term));
		while(*term == ' ') term++;
	}

	if (lst.count() == 1) return lst.pop();
	if (lst.count() < 4) return new TNODE(':', lst);
	_log << "Error: \':\'!" << endl;
	return new TNODE('?');
}

ELIST *CALCULATOR::arguments(char*& term)
{
	ELIST *lst = new ELIST();
	while(*term != 0) {
		lst->add(colon(term));
		while (*term == ' ') term++;
		if (*term == ',') 
			term++;
		else
			break;
	}
	return lst;
}

EELIST *CALCULATOR::sarguments(char*& term)
{
	EELIST *lst = new EELIST();
	while(*term != 0) {
		lst->add(arguments(term));
		while (*term == ' ') term++;
		if (*term == ';') 
			term++;
		else
			break;
	}
	return lst;
}

ENODE *CALCULATOR::function(char *name, char*& term)
{
	term++;
	FUNCTION** f = getfunction(name);
	if (!f) {
		_log << "Error: Function \"" << name << "\" not defined!" << endl;
		return new TNODE('?');
	}
	ELIST *lst = 0;
	if ((*f)->_argnr > 0) lst = arguments(term);

	while(*term == ' ') term++;
	if (*term != ')') {
		_log << "Error: \')\' expected!" << endl;
		delete lst;
		return new TNODE('?');
	}
	term++;

	if (lst) {
		if (lst->count() != (*f)->_argnr) {
			if ((*f)->_argnr == 1) {
				EELIST *elst = new EELIST();
				elst->add(lst);
				ENODE **t = new ENODE*;
				*t = new TNODE(elst);
				return new TNODE(f, t);
			}
			_log << "Error: Function \"" << name << "\" does not accept " << lst->count() << " arguments!" << endl;
			delete lst;
			return new TNODE('?');
		}
		TNODE *t = new TNODE(f, lst->makearray());
		delete lst;
		return t;
	}
	return new TNODE(f, 0);
}

ENODE *CALCULATOR::variable(char *name, char*& term, bool define = false)
{
	if (_argnr > 0) {
		for (int j = 0; j < _argnr; j++) {
      int i;
			for (i = 0; name[i] != 0 && _argname[j][i] == name[i]; i++);
			if (_argname[j][i] == name[i]) return new TNODE('#', j);
		}
	}
	MATRIX **m = getvar(name);
	if (m) return new VNODE(m, name);

	if (define) {
		m = new MATRIX*;
		*m = new MATRIX(0.);
		setvar(name, *m);
		return new VNODE(m, name);
	}
	_log << "Error: Variable \'" << name << "\' not defined!" << endl;
	return new TNODE('?');
}

ENODE *CALCULATOR::value(char* &term)
{
	while(*term == ' ') term++;
	if ((term[0] >= '0' && term[0] <= '9') || term[0] == '.') {
		stringstream ss;
		NRTYPE nr;
		ss << term << endl;
		ss >> nr;
		while ((term[0] >= '0' && term[0] <= '9') || term[0] == '.' || term[0] == 'e' || term[0] == 'E') term++;
		if (*term == 'i') {
			term++;
			return new CNODE(new MATRIX(CRTYPE(0.,nr)));
		} else
			return new CNODE(new MATRIX(nr));
	}
	if (*term == '(') {
		term++;
		ENODE *t = sum(term);
		while (*term == ' ') term++;
		if (*term == ')')
			term++;
		else {
			_log << "Error: \')\' expected!" << endl;
			return new TNODE('?');
		}
		return t;
	}
	if ((*term >= 'a' && *term <= 'z') || (*term >= 'A' && *term <= 'Z')) {
		char *name = getname(term);
		while (*term == ' ') term++;
		if (*term == '(')
			return function(name, term);
		else
			return variable(name, term);
	}
	if (*term == '[') {
		term++;
		ENODE *t = new TNODE(sarguments(term));
		while (*term == ' ') term++;
		if (*term == ']')
			term++;
		else {
			_log << "Error: \']\' expected!" << endl;
			return new TNODE('?');
		}
		return t;
	}
	if (*term == '\"') {
		term++;
		ENODE *t = new SNODE(term);
		if (*term == '\"')
			term++;
		else {
			_log << "Error: \'\"\' expected!" << endl;
			return new TNODE('?');
		}
		return t;
	}
	_log << "Error: Value expected!" << endl;
	return new TNODE('?');
}

ENODE *CALCULATOR::submatrix(char* &term)
{
	ENODE *t = value(term);
	while(*term == ' ') term++;
	while (*term == '[') {
		ENODE **args;
		int i;
		args = new ENODE*[5];
		args[0] = t;
		for (i = 1; i < 5; i++) args[i] = 0;

		term++;
		for (i = 0; i < 2; i++) {
			while(*term == ' ') term++;
			if (*term == ':')		// "[:,:]"
				term++;
			else {
				args[i*2+1] = value(term);

				while(*term == ' ') term++;
				if (*term == ':') {
					term++;
					args[i*2+2] = value(term);
				}
			}
			while(*term == ' ') term++;
			if (*term == ',') {
				term++;
				if (i > 0) {
					_log << "Error: Too many arguments (submatrix)!" << endl;
					return new TNODE('?');
				} else
					continue;
			} else if (*term == ']') {
				term++;
				break;
			} else {
				_log << "Error: \"" << *term << "\" unexpected (submatrix)!" << endl;
				return new TNODE('?');
			}
		}
		t = new TNODE(']', 5, args);
	}
	return t;
}

ENODE *CALCULATOR::imagine(char* &term)
{
	ENODE *t = submatrix(term);

	while(*term == ' ') term++;
	while(*term == 'i') {
		term++;
		t = new TNODE('i', t);
		while(*term == ' ') term++;
	}
	return t;
}

ENODE *CALCULATOR::power(char* &term)
{
	ELIST lst;
	lst.add(imagine(term));

	while(*term == ' ') term++;
	while (*term == '^') {
		term++;
		lst.add(imagine(term));
		while(*term == ' ') term++;
	}

	if (lst.count() < 2) return lst.pop();
	return new TNODE('^', lst);
}

ENODE *CALCULATOR::transpose(char* &term)
{
	ENODE *t = power(term);

	while(*term == ' ') term++;
	while(*term == '\'') {
		term++;
		t = new TNODE('\'', t);
		while(*term == ' ') term++;
	}
	return t;
}

ENODE *CALCULATOR::factorial(char* &term)
{
	ENODE *t = transpose(term);

	while(*term == ' ') term++;
	while(*term == '!') {
		term++;
		t = new TNODE('!', t);
		while(*term == ' ') term++;
	}
	return t;
}

ENODE *CALCULATOR::inverse(char* &term)
{
	while(*term == ' ') term++;
	if (*term == '/') {
		term++;
		return new TNODE('/', inverse(term));
	}
	return factorial(term);
}

ENODE *CALCULATOR::product(char* &term)
{
	ELIST lst;
	lst.add(inverse(term));

	while(*term == ' ') term++;
	while (*term == '*' || *term == '/') {
		if (*term == '*') term++;
		lst.add(inverse(term));
		while(*term == ' ') term++;
	}

	if (lst.count() < 2) return lst.pop();
	return new TNODE('*', lst);
}

ENODE *CALCULATOR::modulo(char* &term)
{
	ELIST lst;
	lst.add(product(term));

	while(*term == ' ') term++;
	while (*term == '%') {
		if (*term == '%') term++;
		lst.add(inverse(term));
		while(*term == ' ') term++;
	}

	if (lst.count() < 2) return lst.pop();
	return new TNODE('%', lst);
}

ENODE *CALCULATOR::negative(char* &term)
{
	while(*term == ' ') term++;
	if (*term == '-') {
		term++;
		return new TNODE('-', negative(term));
	}
	return modulo(term);
}
	
ENODE *CALCULATOR::sum(char* &term)
{
	ELIST lst;
	lst.add(negative(term));

	while(*term == ' ') term++;
	while (*term == '+' || *term == '-') {
		if (*term == '+') term++;
		lst.add(negative(term));
		while(*term == ' ') term++;
	}

	if (lst.count() < 2) return lst.pop();
	return new TNODE('+', lst);
}

char CALCULATOR::expression(char* &term)
{
	for (int i = 0; term[i] != 0; i++) {
		if (term[i] == '=' || term[i] == '?') {
			if ((*term >= 'a' && *term <= 'z') || (*term >= 'A' && *term <= 'Z')) {
				char *name = getname(term);

				while(*term == ' ') term++;
				if (*term == '(') {
					term++;
					USERFUNCTION *f = new USERFUNCTION(name, term);
					setfunction(f);
					if (!expect(term, ')')) return '?';
					if (!expect(term, '=')) return '?';

					_expression = new EXPSETFUNCTION(f, sum(term));
					return '~';
				} else if (*term == '=') {
					term++;
					setvar(name, 0);
					_expression = new EXPSETVAR(getvar(name), sum(term));
					return '=';
				} else if (*term == '?') {
					_expression = new EXPGETFUNCTION(getfunction(name));
					return 'i';
				} else {
					_log << "Error: \'(\' or \'=\' expected!" << endl;
					return '?';
				}
			} else {
				_log << "Error: Variable or function name expected!" << endl;
				return '?';
			}
		}
	}
	MATRIX **ans = getvar("ans");
	if (ans == 0) {
		setvar("ans", new MATRIX(0.));
		ans = getvar("ans");
	}
	_expression = new EXPSETVAR(ans, sum(term));
	return '=';
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CALCULATOR::CALCULATOR(ostream& log, const char* varmemfile, const char *funmemfile) : _log(log), _vmemfile(varmemfile), _fmemfile(funmemfile) , _mem(varmemfile) //, _fmem(funmemfile)
{
	setfunction(new RFNR1("exp", &std::exp<NRTYPE>, "exponential function", "x (in IR)"));
	setfunction(new RFNR1("log", &std::log<NRTYPE>, "natural logarithm", "x (in IR)"));
	setfunction(new RFNR1("sinh", &std::sinh<NRTYPE>, "hyperbolic sine", "x (in IR)"));
	setfunction(new RFNR1("cosh", &std::cosh<NRTYPE>, "hyperbolic cosine", "x (in IR)"));

	setfunction(new RFNR1("sin", &crsin, "sine function", "x (in IR)"));
	setfunction(new RFNR1("cos", &crcos, "cosine function", "x (in IR)"));
	setfunction(new RFNR1("tan", &crtan, "tangent function", "x (in IR)"));
	setfunction(new RFNR1("asin", &crasin, "arcsine function", "x (in [-1,1])"));
	setfunction(new RFNR1("acos", &cracos, "arccosine function", "x (in [-1,1])"));
	setfunction(new RFNR1("atan", &cratan, "arctangent function", "x (in [-1,1])"));

	setfunction(new RFNR1("ceil", &crceil, "the smallest integer bigger than x", "x (in IR)"));
	setfunction(new RFNR1("floor", &crfloor, "the biggest integer smaller than x", "x (in IR)"));
	setfunction(new RFNR1("abs", &crabs, "absolute value of x", "x (in IR)"));
	setfunction(new RFNR1("sign", &signum, "sign of x", "x (in IR)"));

	setfunction(new RFMAT1("min", &minimum, "minimum of all vector entries", "v (in IR^n)"));
	setfunction(new RFMAT1("max", &maximum, "maximum of all vector entries", "v (in IR^n)"));

	setfunction(new RFRND());

	setfunction(new RFMAT1("eye", &eye, "identity matrix in IR^(n x n)", "n (in IN)"));
	setfunction(new RFMAT2("zero", &zero, "zero matrix in IR^(m x n)", "m (in IN)", "n (in IN)"));
	setfunction(new RFMAT1("diag", &diag, "the diagonal matrix with the vector's entries as diagonal entries", "v (in IR^n)"));
	setfunction(new RFMAT1("dim", &dim, "dimension n of the vector", "v (in IR^n)"));
	setfunction(new RFMAT1("size", &size, "size of the matrix M", "M (in IR^(m x n))"));
	setfunction(new RFMAT2("norm", &norm, "p-norm n of the vector/matrix M", "M (in IR^(m x n))", "p (in IR+ if M is vector or in {1, 2, -1} (-1 for infinity) if M is matrix"));
	setfunction(new RFMAT1("eig", &eig, "eigenvalues of a quadratic matrix M", "M (in IR^(n x n))"));

	setfunction(new RFMAT1("lrl", &lrl, "L of LR-decomposition M", "M (in IR^(n x n))"));
	setfunction(new RFMAT1("lrr", &lrr, "R of LR-decomposition M", "M (in IR^(n x n))"));
	setfunction(new RFMAT1("qrq", &qrq, "Q of QR-decomposition M", "M (in IR^(n x n))"));
	setfunction(new RFMAT1("qrr", &qrr, "R of QR-decomposition M", "M (in IR^(n x n))"));

	setfunction(new RFMAT1("mrm", &mrm, "M of MR-decomposition M", "M (in IR^(m x n), 1 < n)"));
	setfunction(new RFMAT1("mrr", &mrr, "R of MR-decomposition M", "M (in IR^(m x n), 1 < n)"));
	setfunction(new RFMAT1("lgs", &lgs, "solves LGS with Gaussian Algorithm (with pivoting)", "M (in IR^(m x n), 1 < n)"));
	setfunction(new RFMAT1("ker", &ker, "calculates kernel of a matrix with Gaussian Algorithm (with pivoting)", "M (in IR^(m x n))"));
	setfunction(new RFMAT1("det", &det, "calculates determinant", "M (in IR^(n x n))"));
	setfunction(new RFMAT1("trace", &trc, "calculates trace", "M (in IR^(m x n))"));
	setfunction(new RFMAT3("cofactor", &cofactor, "calculates cofactor i,j of M", "M (in IR^(m x n))", "i in IN", "j in IN"));

	setfunction(new RFSUM());
	setfunction(new RFPROD());
	setfunction(new RFWAV());

	setvar("pi", new MATRIX(PI));
}

CALCULATOR::~CALCULATOR()
{
	_mem.write(_vmemfile);
	_fmem.write(_fmemfile);
}

void CALCULATOR::operator <<(char *input)
{
	if (*input != 0) {
		char result = expression(input);
		if (result == '?') return;
		_expression->execute();
		_expression->print(this);
	}
	return;
}
