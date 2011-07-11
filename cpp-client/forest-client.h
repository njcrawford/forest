#ifndef _FOREST_CLIENT_H
#define _FOREST_CLIENT_H

#include <vector>
#include <string>
// to_string and trim_string
#include <sstream>

// Works like system(), but returns lines of stdout output in outList and the 
// command's return value in returnVal.
void mySystem(string * command, vector<string> & outList, int * returnVal);

string flattenStringList(vector<string> & list, char delimiter);

template <class T>
inline std::string to_string (const T& t)
{
	std::stringstream ss;
	ss << t;
	return ss.str();
}

inline string trim_string(const string& s)
{
	std::stringstream trimmer;
	trimmer << s;
	string retval;
	trimmer >> retval;
	return retval;
}

typedef struct updateInfoStruct
{
	string name;
	string version;
} updateInfo;

#endif
