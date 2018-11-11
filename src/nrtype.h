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

// nrtype.h

#ifndef NRTYPE_H
#define NRTYPE_H 

#include <iostream>
#include <complex>
#define NRTYPE double

typedef std::complex<NRTYPE> CRTYPE;

inline
std::ostream& operator <<(std::ostream &ostr, const CRTYPE &c)
{
	if (std::real(c) != 0.) {
		ostr << std::real(c);
		if (std::imag(c) != 0.) ostr << " + " << std::imag(c) << "i";
	} else if (std::imag(c) != 0.)
		ostr << std::imag(c) << "i";
	else
		ostr << 0.;
	return ostr;
}

inline
NRTYPE sign(NRTYPE x)
{
  if (x < 0.)
    return -x;
  else
    return x;
}

#endif