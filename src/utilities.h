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

//utilities.h

#ifndef UTILITIES_H
#define UTILITIES_H 

#define PI			3.1415926535897932384626433832795
#define EULER		2.7182818284590452353602874713527
#define MAXLENGTH	32768 // 2^15
#define MAXINT		2147483648 // 2^31

//#define abs(x)		((x < 0) ? -(x) : (x))
//#define sign(x)		((x < 0) ? -1. : 1.)
//#define min(x,y)	((x < y) ? (x) : (y))
//#define max(x,y)	((x < y) ? (y) : (x))

template<class DATA>
class LIST
{
private:
	struct ITEM
	{
		DATA *_data;
		ITEM *_next;
	};

	int _count;
	ITEM *_item;
public:
	LIST() : _item(0), _count(0) {}

	~LIST()
	{
		ITEM *tmp;
		while(_item) {
			tmp = _item;
			delete _item->_data;
			_item = _item->_next;
			delete tmp;
		}
	}

	void add(DATA *data)
	{
		ITEM *tmp = new ITEM;
		tmp->_data = data;
		tmp->_next = _item;
		_item = tmp;
		_count++;
	}

	inline int count()
	{
		return _count;
	}

	DATA **makearray()
	{
		DATA **arr = new DATA*[_count];
		ITEM *tmp;
		for (_count--; _item; _count--) {
			tmp = _item;
			arr[_count] = _item->_data;
			_item = _item->_next;
			delete tmp;
		}
		return arr;
	}

	DATA *pop()
	{
		if (!_item) return 0;
		DATA *data = _item->_data;
		ITEM *tmp = _item;
		_item = _item->_next;
		delete tmp;
		return data;
	}
};

class ENODE;

char *getname(char* &text);
bool writewav(char *fname, ENODE *iterator, ENODE *term, unsigned long rate, unsigned short depth, unsigned short length, unsigned short channels);

#endif