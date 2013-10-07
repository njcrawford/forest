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
	char * response = NULL;
	//int lineLen = 0;
	char * newlinePtr = NULL;

	line[0] = '\0';

	pipe = popen(command->c_str(), "r");

	do
	{
		// read a line
		response = fgets(line, BUFFER_SIZE, pipe);

		// The last character that fgets reads may be a newline, but we
		// don't want newlines in our list, so remove it.
		newlinePtr = strchr(line, '\n');
		if(newlinePtr != NULL)
		{
			*newlinePtr = '\0';
		}

		// add this line to outList
		outList.push_back(line);
		
	} while (response != NULL);
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
