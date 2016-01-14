#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "helpers.h"
#include "defines.h"


// Works like system(), but returns lines of stdout output in outList and the 
// command's return value in returnVal.
#ifndef _WIN32
void mySystem(string * command, vector<string> & outList, int * returnVal)
{
	FILE * pipe;
	char line[BUFFER_SIZE];
	//int lineLen = 0;
	char * newlinePtr = NULL;

	line[0] = '\0';

	pipe = popen(command->c_str(), "r");

	// Read lines until end of file or an error occurs
	for(char * response = fgets(line, BUFFER_SIZE, pipe);
		response != NULL;
		response = fgets(line, BUFFER_SIZE, pipe))
	{

		// The last character that fgets reads may be a newline, but we
		// don't want newlines in our list, so remove it.
		newlinePtr = strchr(line, '\n');
		if(newlinePtr != NULL)
		{
			*newlinePtr = '\0';
		}

		// add this line to outList
		outList.push_back(line);
		
	}
	int pcloseVal = pclose(pipe);
	*returnVal = WEXITSTATUS(pcloseVal);
}
#endif

string flattenStringList(vector<string> & list, char delimiter)
{
	string retval = "";
	bool first = true;
	for(size_t i = 0; i < list.size(); i++)
	{
		if(first)
		{
			first = false;
		}
		else
		{
			retval += delimiter;
		}
		retval += list[i];
	}
	return retval;
}

string trim_string(const string& s)
{
	int start = -1;
	int end = -1;
	// Find the laft whitespace
	for(size_t i = 0; i < s.size(); i++)
	{
		if(!isspace(s[i]) && start == -1)
		{
			start = i;
			break;
		}
	}
	// Find the right whitespace
	for(size_t i = s.size() - 1; i >= 0; i--)
	{
		if(!isspace(s[i]) && end == -1)
		{
			end = i;
			break;
		}
	}

	// No whitespace
	if(start == -1 && end == -1)
	{
		return s;
	}
	// Whitespace at end
	else if(start == -1 && end != -1)
	{
		return s.substr(0, (end + 1));
	}
	// Whitespace at beginning
	else if(start != -1 && end == -1)
	{
		return s.substr(start, string::npos);
	}
	// Whitespace at both beginning and end
	else
	{
		return s.substr(start, (end - start + 1));
	}
}
