#ifndef _FOREST_CLIENT_H
#define _FOREST_CLIENT_H

#include <vector>
#include <string>

// Works like system(), but returns lines of stdout output in outList and the 
// command's return value in returnVal.
void mySystem(string * command, vector<string> * outList, int * returnVal);

string flattenStringList(vector<string> * list, char delimiter);

template <class T>
inline std::string to_string (const T& t);

inline string trim_string(const string& s);

#endif
