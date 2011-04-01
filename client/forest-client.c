//#############################################################################
// Forest - a web-based multi-system update manager
//
// Copyright (C) 2010 Nathan Crawford
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
//
// A copy of the full GPL 2 license can be found in the docs directory.
// You can contact me at http://www.njcrawford.com/contact
//#############################################################################

#include <stdlib.h>
#include <stdio.h>

#define RPC_VERSION 2
#define MAX_INPUT_LINE_SIZE 200

// for now, define what PM to use at compile time (from list above)
// valid options are _APT, _RPM and _WINDOWS
#define PACKAGE_MANAGER_APT

typedef struct lineListStruct
{
	char** lines;
	int lineCount;
} lineList;

int getAvailableUpdates(lineList* outList);
int getAcceptedUpdates(lineList* outList);
int applyUpdates(lineList* list);
int reportAvailableUpdates(lineList* list);

int main(char** args, int argc)
{
	lineList availableUpdates;
	availableUpdates.lines = NULL;
	availableUpdates.lineCount = 0;

	lineList acceptedUpdates;
	acceptedUpdates.lines = NULL;
	acceptedUpdates.lineCount = 0;

	// set a default for server url
	char * serverUrl = "http://forest/forest";

	// read config file

	// determine what packages are available to update
	getAvailableUpdates(&availableUpdates);

	// get list of packages that have been accepted for update
	getAcceptedUpdates(&acceptedUpdates);

	// apply accepted updates (and only available updates)
	applyUpdates(&acceptedUpdates);

	// report packages that are available to update
	reportAvailableUpdates(&availableUpdates);

	return 0;
}

int getAvailableUpdates(lineList* outList)
{
	char * command;
	int commandRetval = 0;

#if defined PACKAGE_MANAGER_APT
	command = "apt-get dist-upgrade -Vs 2>&1";
#elif defined PACKAGE_MANAGER_RPM
	//TODO: yum output will need further cleanup
	command = "yum check-update -q -C 2>&1";
#else
	//something to break compilation if no package manager is defined
	()
#endif

	mySystem(command, outList, &commandRetval);

#ifdef PACKAGE_MANAGER_RPM
	if(commandRetval == 0)
	{
		clearList(outList);
	}
	else if(commandRetval == 100)
	{
		int lineNum;
		int lineLen;
		int letterNum;
		char * tempLine;
		for(lineNum = 0; lineNum < outList->lineCount; lineNum++)
		{
			lineLen = strlen(outList->lines[lineNum]);
			// only keep everything up to the first '.'
			for(letterNum = 0; letterNum < lineLen; letterNum++)
			{
				if(outList->lines[lineNum][letterNum] == '.')
				{
					// null terminate so we can use strcpy
					outList->lines[lineNum][letterNum] = '\0';

					// malloc space for shortened line
					tempLine = malloc(sizeof(char) * letterNum));

					// copy the shortened line into the new space
					strcpy(tempLine, outList->lines[lineNum]);

					// free the old line
					free(outList->lines[lineNum]);

					// put the shortened line into the list where the original was
					outList->lines[lineNum] = tempLine;

					// don't process this line any further
					break;
				}
			}
		}
	}
#endif
}

int getAcceptedUpdates(lineList* outList)
{

}

int applyUpdates(lineList* list)
{

}

int reportAvailableUpdates(lineList* list)
{

}

// adds newItem to the end of list
void pushToStringList(lineList* list, char * newItem)
{
	char** tempList = NULL;
	int i;

	// malloc new list with enough space for old list plus newItem
	tempList = malloc(sizeof(char*) * (listCount + 1));

	// copy pointers from old list to new list
	for(i = 0; i < list.lineCount; i++)
	{
		tempList[i] = list->lines[i];
	}

	// malloc new item
	tempList[listCount] = malloc(sizeof(char) * (strlen(newItem) + 1));

	// copy new item into list
	strcpy(tempList[listCount], newItem);

	// free the old list
	free(list->lines);

	// assign the new list in the old one's place
	list->lines = templist;
}

// Works like system(), but returns lines of stdout in outList and the command's
// return value in returnVal.
void mySystem(const char* command, lineList* outList, int * returnVal)
{
	FILE * pipe;
	char line[MAX_INPUT_LINE_SIZE];
	char * response = NULL;
	int lineLen = 0;
	char * command;

	line[0] = '\0';

	pipe = popen(command, "r");

	do
	{
		response = fgets(line, MAX_INPUT_LINE_SIZE, pipe);
		lineLen = strlen(line);
		pushToStringList(outList, line);
		
	} while (response != NULL);

	*returnVal = pclose(pipe);
}
