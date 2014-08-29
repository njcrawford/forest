#ifndef _FORESTTYPES_H
#define _FORESTTYPES_H

#include <string>
using namespace std;

struct updateInfo
{
	string name;
	string version;
};

enum rebootState
{
	unknown = -1,
	yes = 1,
	no = 0
};

#endif
