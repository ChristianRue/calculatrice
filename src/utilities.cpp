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

//utilities.cpp

#include <iostream>
#include <fstream>

#include "utilities.h"
#include "nrtype.h"
#include "matrix.h"
#include "term.h"
#include "function.h"

using namespace std;

int strlen(char *string)
{
	if (string == NULL) return -1;
	for(int i = 0; i < MAXLENGTH; i++) if (string[i] == 0) return i;
	return 0;
}

bool compare(char *string1, char *string2)
{
  int i;
	for(i = 0; string1[i] != 0 && string1[i] == string2[i]; i++);
	if (string1[i] == string2[i]) return true;
	return false;
}

char *str(char *string)
{
	int l = strlen(string);
	char *s = new char[l+1];
	for(int i = 0; i < l; i++) s[i] = string[i];
	s[l] = 0;
	return s;
}

bool isint(double d)
{
	if ((double)(int)d == d) return true;
	return false;
}

double fac(int i)
{
	unsigned long ii = (unsigned long)i;
	unsigned long l = 1;
	for (; ii > 0; ii--) l *= ii;
	return (double)l;
}

char *getname(char* &text)
{
	int i;

	// get length:
	int len = 0;
	bool first = true;
	for (i = 0; ; i++) {
		if (	(text[i] >= 'a' && text[i] <= 'z') 
			||	(text[i] >= 'A' && text[i] <= 'Z') 
			||	((text[i] >= '0' && text[i] <= '9') || text[i] == '_') && !first) {
			len++;
			first = false;
		} else if (!(text[i] == ' ' && first)) {
			len++;
			break;
		}
	}
	if (len < 1) return 0;
	char *tmp = new char[len];
	len = 0;

	// copy string:
	first = true;
	for (i = 0; ; i++) {
		if (	(text[i] >= 'a' && text[i] <= 'z') 
			||	(text[i] >= 'A' && text[i] <= 'Z') 
			||	((text[i] >= '0' && text[i] <= '9') || text[i] == '_') && !first) {
			tmp[len] = text[i];
			len++;
			first = false;
		} else if (!(text[i] == ' ' && first)) {
			tmp[len] = 0;
			break;
		}
	}
	text = &text[i];
	return tmp;
}

/*
bool loadwav(char *fname, char **buffer, long& length)
{
	ifstream file(fname, ios::binary);
	char *tmp;

	if (file.is_open()) {
		file.seekg(0, ios::end);
		length = streamoff(file.tellg());
		file.seekg(0, ios::beg);

		*buffer = new char[44];
		file.read(*buffer, 44);

		if		(*(short *)&(*buffer)[20] != 1)				cout << "Error: Loading WAV-File " << fname << "! File is not PCM!" << endl;
		else if (*(short *)&(*buffer)[22] != 1) 			cout << "Error: Loading WAV-File " << fname << "! File is not mono!" << endl;
		else if (*(long  *)&(*buffer)[24] != rate) 			cout << "Error: Loading WAV-File " << fname << "! Wrong Sample-Rate!" << endl;
		else if (*(long  *)&(*buffer)[28] != rate*1*size) 	cout << "Error: Loading WAV-File " << fname << "! Wrong Sample-Size!" << endl;
		else if (*(short *)&(*buffer)[32] != 1*size)		cout << "Error: Loading WAV-File " << fname << "! Wrong Sample-Size!" << endl;
		else if (*(short *)&(*buffer)[34] != size*8) 		cout << "Error: Loading WAV-File " << fname << "! Wrong Sample-Size!" << endl;
		else {
			tmp = &(*buffer)[36];
			while (tmp[0] != 'd' || tmp[1] != 'a' || tmp[2] != 't' || tmp[3] != 'a') {
				length = *(long  *)&tmp[4];
				delete[] *buffer;
				*buffer = new char[length + 8];
				file.read(*buffer, length + 8);
				tmp = &(*buffer)[length];
			}
			length = *(long  *)&tmp[4];
			delete[] *buffer;
			*buffer = new char[length];
			file.read(*buffer, length);

			length /= (size * 1);

			file.close();
			return true;
		}
		file.close();
	}
	return false;
}
*/

bool writewav(char *fname, ENODE *it, ENODE *term, unsigned long rate, unsigned short depth, unsigned short length, unsigned short channels)
{
	//unsigned long		rate		= 44100;	// Samples/Second
	//unsigned short	size		= 2;		// Bytes/Sample (1 = 8Bit, 2 = 16Bit)
	//unsigned short	length		= 5;		// Length (in Seconds)
	//unsigned short	channels	= 1;		// Channels (1 = Mono, 2 = Stereo)
	unsigned long sl;
	unsigned short si;
	int s;

	//if (f._argnr != 1) return false;

	ofstream file(fname, ios::binary);

	if (file.is_open()) {
		//FORMAT-CHUNK
		file.write("RIFF", 4);										// 'RIFF'
		sl = rate*length*depth+44;	file.write((char *)&sl, 4);		// depth			- Filesize - 8
		file.write("WAVE", 4);										// 'WAVE'
		file.write("fmt ", 4);										// 'fmt '
		sl = 16;					file.write((char *)&sl, 4);		// fmt-size			- 16 Byte
		si = 1;						file.write((char *)&si, 2);		// Format-Tag		- 1 = PCM
		si = channels;				file.write((char *)&si, 2);		// Channels			- 1 = Mono
		sl = rate;					file.write((char *)&sl, 4);		// Rate (Samples/s)	- zB. 44100 Hz
		sl = rate*channels*depth;	file.write((char *)&sl, 4);		// Bytes/s			- Rate*BlockAlign
		si = channels*depth;		file.write((char *)&si, 2);		// BlockAlign		- Channels * Bytes/Sample
		si = depth * 8;				file.write((char *)&si, 2);		// Bits/Sample		- Bytes/Sample * 8

		//DATA CHUNK
		file.write("data", 4);													// 'data'
		sl = rate*length*channels*depth;		file.write((char *)&sl, 4);		// depth			

		sl = rate*length;
		CRTYPE d;
		NRTYPE drate = 1./(NRTYPE)rate; //(NRTYPE)rate;
		NRTYPE resize = .5*pow(2., 8.*(NRTYPE)depth);
		CRTYPE& cr = it->data().value(0,0);
		if (channels == 1) {
			for (unsigned long i = 0; i < sl; i++) {
				cr = drate*(NRTYPE)i;
				d = term->value(0).value(0,0);
				s = (int)(real(d)*resize);
				file.write((char *)&s, depth);
			}
		} else if (channels == 2) {
			for (unsigned long i = 0; i < sl; i++) {
				cr = drate*(NRTYPE)i;
				d = term->value(0).value(0,0);
				s = (int)(real(d)*resize);
				file.write((char *)&s, depth);
				s = (int)(imag(d)*resize);
				file.write((char *)&s, depth);
			}
		}

		file.close();
	}
	return true;
}