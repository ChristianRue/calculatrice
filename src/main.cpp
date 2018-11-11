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

// main.cpp

#include <iostream>

#include "calculator.h"

using namespace std;

int main(int argc, char **argv)
{
	const int MAXINPUT = 32768;
	char input[MAXINPUT];

	CALCULATOR calc(cout, "vmem.mem", "fmem.mem");

	cout.precision(16);
	while (true) {
		cin.getline(input, MAXINPUT);
		if (input[0] == '#') break;
		calc << input;
	}
	return 0;
}
