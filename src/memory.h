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

// memory.h

#ifndef MEMORY_H
#define MEMORY_H 

#include <iostream>
#include <fstream>
#include <string.h>

template<class DATA>
class VARMEMORY
{
public:
	class VARIABLE
	{
	public:
		VARIABLE(char *name, DATA *data) : _data(data)
		{
			_name = new char[strlen(name)+1];
      int i;
			for (i = 0; name[i]; i++)
				_name[i] = name[i];
			_name[i] = 0;
		}
		char *_name;
		DATA *_data;
	};

private:
	struct VARLIST
	{
		VARIABLE *_data;
		VARLIST *_next;
	};

	VARLIST *list;

	VARLIST *getvarlist(char *name)
	{
		if (list == 0 || strlen(name) == 0) return 0;

		VARLIST *li = list;

		while(li != 0) {
      int i;
			for (i = 0; name[i] != 0 && li->_data->_name[i] == name[i]; i++);
			if (li->_data->_name[i] != name[i]) 
				li = li->_next;
			else
				return li;
		}
		return 0;
	}

public:
	VARMEMORY() : list(0) {}
	VARMEMORY(const char* memfile) : list(0)
	{
		std::ifstream file(memfile, std::ios::binary);
		if (file.is_open()) {
			char name[256];
			DATA data;

			while(true) {
				for (int i = 0; i < 256; i++) {
					if (file.eof()) return;
					file.read(&name[i], 1);
					if (name[i] == 0) break;
				}
				if (name[0] == 0) break;
				VARLIST *li = new VARLIST;

				li->_data = new VARIABLE(name, new DATA(file));
				li->_next = list;
				list = li;
			}

			file.close();
		}
	}

	DATA** getvar(char *name)
	{
		VARLIST *l = getvarlist(name);
		if (l == 0) return 0;
		return &getvarlist(name)->_data->_data;
	}

	int setvar(char *name, DATA *data)
	{
		VARLIST *l = getvarlist(name);
		if (l != 0) {	// MATRIX existiert bereits
			delete l->_data->_data;
			l->_data->_data = data;
			return 0;
		}

		l = new VARLIST;
		l->_data = new VARIABLE(name, data);
		l->_next = list;
		list = l;
		return 0;
	}

	bool write(const char *fname)
	{
		if (list == 0) return false;

		std::ofstream file(fname, std::ios::binary);
		if (file.is_open()) {
			VARLIST *li = list;

			while(li != 0) {
				file << li->_data->_name;
				file.put(0);
				file << *li->_data->_data;
				li = li->_next;
			}
			file.put(0);

			file.close();
			return true;
		}
		return false;
	}
};

#endif