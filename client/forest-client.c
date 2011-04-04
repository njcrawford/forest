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
/* Get gethostname() declaration.  */
#include <unistd.h>
/* Get HOST_NAME_MAX definition.  */
#include <limits.h>
// string functions
#include <string.h>

#define RPC_VERSION 2

#define MAX_INPUT_LINE_SIZE 200

#define BUFFER_SIZE 1024

// for now, define what PM to use at compile time (from list above)
// valid options are _APT, _YUM and _WINDOWS
#define PACKAGE_MANAGER_APT
//#define PACKAGE_MANAGER_YUM
//#define PACKAGE_MANAGER_WINDOWS

typedef struct lineListStruct
{
	char** lines;
	int lineCount;
} lineList;

typedef struct forestConfigStruct
{
	char * serverUrl;
} forestConfig;

int getAvailableUpdates(lineList* outList);
int getAcceptedUpdates(lineList* outList, const char * serverUrl, const char * myHostname);
int applyUpdates(lineList* list);
int reportAvailableUpdates(lineList* list, const char * serverUrl, const char * myHostname);
void readConfigFile(forestConfig * config);
void mySystem(const char* command, lineList* outList, int * returnVal);
void clearStringList(lineList* list);
void pushToStringList(lineList* list, char * newItem);
char * flattenStringList(lineList * list, char seperator);

int main(char** args, int argc)
{
	lineList availableUpdates;
	lineList acceptedUpdates;
	forestConfig config;
	char hostname[HOST_NAME_MAX + 1];
	int response = 0;

	memset(&availableUpdates, 0, sizeof(lineList));
	memset(&acceptedUpdates, 0, sizeof(lineList));
	memset(&config, 0, sizeof(forestConfig));

	hostname[HOST_NAME_MAX] = '\0';
	// get the current host name
	response = gethostname(hostname, HOST_NAME_MAX);
	if(response == -1)
	{
		//fprintf(stderr, "Could not get hostname, exiting.");
		perror("Error in main(): ");
		exit(1);
	}

	// set a default for server url
	config.serverUrl = "http://forest/forest";

	// read config file
	readConfigFile(&config);

	// determine what packages are available to update
	getAvailableUpdates(&availableUpdates);

	// get list of packages that have been accepted for update
	getAcceptedUpdates(&acceptedUpdates, config.serverUrl, hostname);

	// apply accepted updates (and only available updates)
	if(acceptedUpdates.lineCount > 0)
	{
		applyUpdates(&acceptedUpdates);
	}

	// report packages that are available to update
	reportAvailableUpdates(&availableUpdates, config.serverUrl, hostname);

	return 0;
}

int getAvailableUpdates(lineList* outList)
{
	char * command;
	int commandRetval = 0;

#if defined PACKAGE_MANAGER_APT
	command = "apt-get dist-upgrade -Vs 2>&1";
#elif defined PACKAGE_MANAGER_YUM
	//TODO: yum output will need further cleanup
	command = "yum check-update -q -C 2>&1";
#elif defined PACKAGE_MANAGER_WINDOWS
	// I don't think there is a command that will do this under windows.
	// This will probably need to be broken out into a seperate function.
#else
	//something to break compilation if no package manager is defined
	);
#endif

	mySystem(command, outList, &commandRetval);

#ifdef PACKAGE_MANAGER_YUM
	// yum check-update returns 0 if no updates are available or 100 if 
	// there is at least one update available
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
					tempLine = malloc(sizeof(char) * letterNum);

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

// fills outList with names of accepted packages
int getAcceptedUpdates(lineList* outList, const char * serverUrl, const char * myHostname)
{
	char acceptedUrl[BUFFER_SIZE];
	char command[BUFFER_SIZE];
	int response = 0;

	// build accepted URL
	snprintf(acceptedUrl, BUFFER_SIZE, "%s/getaccepted.php?rpc_version=%d&system=%s", serverUrl, RPC_VERSION, myHostname);

	// build command to run
	snprintf(command, BUFFER_SIZE, "curl --silent --show-error %s", acceptedUrl);
	mySystem(command, outList, &response);
	
	// exit silently if curl fails
	if(response != 0)
	{
		exit(0);
	}
}

int applyUpdates(lineList* list)
{
	char * command;
	int i;
	int remainingBuf;
	int commandResponse;
	lineList commandOutput;	
	char * flattenedOutput = NULL;
	char * temp = NULL;

	memset(&commandOutput, 0, sizeof(lineList));

#if defined PACKAGE_MANAGER_APT
	command = "apt-get -y -o DPkg::Options::\\=--force-confold install ";
#elif defined PACKAGE_MANAGER_YUM
	//TODO: yum output will need further cleanup
	command = "yum -y update ";
#elif defined PACKAGE_MANAGER_WINDOWS
	// I don't think there is a command that will do this under windows.
	// This will probably need to be broken out into a seperate function.
#else
	//something to break compilation if no package manager is defined
	);
#endif

	flattenedOutput = flattenStringList(list, ' ');
	temp = malloc(sizeof(char) * (strlen(command) + strlen(flattenedOutput)));
	sprintf(temp, "%s%s", command, flattenedOutput);
	command = temp;

	free(flattenedOutput);
	flattenedOutput = NULL;

	mySystem(command, &commandOutput, &commandResponse);

	free(command);
	command = NULL;
	
	if(commandResponse != 0)
	{
		flattenedOutput = flattenStringList(&commandOutput, ' ');
		fprintf(stderr, "Error in applyUpdates: Package manager failed to apply updates:\n%s", flattenedOutput);
		free(flattenedOutput);
		exit(1);
	}

	clearStringList(&commandOutput);
}

int reportAvailableUpdates(lineList* list, const char * serverUrl, const char * myHostname)
{
	char * updateStr = NULL;
	char command[BUFFER_SIZE];
	lineList commandResponse;
	int commandRetval;
	int rebootNeeded = isRebootNeeded();

	memset(&commandResponse, 0, sizeof(lineList));

	sprintf(command, "curl --silent --show-error --data \"rpc_version=%d\" --data \"system_name=%s\"", RPC_VERSION, myHostname);

	if(list->lineCount == 0)
	{
		strcat(command, " --data no_updates_available=true");
	}
	else
	{
		// this output also needs to be escaped for HTML
		updateStr = flattenStringList(list, ',');
		strcat(command, " --data available_updates=");
		strcat(command, updateStr);
		free(updateStr);
	}

	strcat(command, " --data reboot_required=");
	if(rebootNeeded == 1)
	{
		strcat(command, "true");
	}
	else if(rebootNeeded == 0)
	{
		strcat(command, "false");
	}
	else
	{
		strcat(command, "unknown");
	}

	//TODO: add reboot attempted

	strcat(command, " ");
	strcat(command, serverUrl);

	mySystem(command, &commandResponse, &commandRetval);

	// do something to check return value
}

// returns 1 if reboot is needed, 0 if not and -1 if unable to determine
int isRebootNeeded()
{
	
}

void readConfigFile(forestConfig * config)
{
	FILE * configFile;
	char line[BUFFER_SIZE];
	char * response = line;
	char * splitPtr;
	char confName[BUFFER_SIZE];
	char confValue[BUFFER_SIZE];

	configFile = fopen("/etc/forest-client.conf", "r");
	while(response)
	{
		response = fgets(line, BUFFER_SIZE, configFile);
		if(response == NULL)
		{
			break;
		}
		else if(line[0] != '#')
		{
			splitPtr = strchr(line, '=');
			if(splitPtr != NULL)
			{
				*splitPtr = '\0';
				strncpy(confName, line, BUFFER_SIZE);
				strncpy(confValue, splitPtr + 1, BUFFER_SIZE);
				if(strcmp(confName, "server_url") == 0)
				{
					config->serverUrl = malloc(sizeof(char) * strlen(confValue));
					strcpy(config->serverUrl, confValue);
				}
				else
				{
					fprintf(stderr, "Unrecognized config item name: %s\n", confName);
				}
			}
		}
	}
}

// adds newItem to the end of list
void pushToStringList(lineList* list, char * newItem)
{
	char** tempList = NULL;
	int i;

	// malloc new list with enough space for old list plus newItem
	tempList = malloc(sizeof(char*) * (list->lineCount + 1));

	// copy pointers from old list to new list
	for(i = 0; i < list->lineCount; i++)
	{
		tempList[i] = list->lines[i];
	}

	// malloc new item
	tempList[list->lineCount] = malloc(sizeof(char) * (strlen(newItem) + 1));

	// copy new item into list
	strcpy(tempList[list->lineCount], newItem);

	// free the old list
	free(list->lines);

	// assign the new list in the old one's place
	list->lines = tempList;

	// increment the line count
	list->lineCount++;
}

// empties list, freeing memory used by lines and reseting lineCount to 0
void clearStringList(lineList * list)
{
	int i;

	for(i = 0; i < list->lineCount; i++)
	{
		free(list->lines[i]);
	}

	list->lineCount = 0;
}

// Works like system(), but returns lines of stdout output in outList and the 
// command's return value in returnVal.
void mySystem(const char* command, lineList* outList, int * returnVal)
{
	FILE * pipe;
	char line[MAX_INPUT_LINE_SIZE];
	char * response = NULL;
	//int lineLen = 0;
	char * newlinePtr = NULL;

	line[0] = '\0';

	pipe = popen(command, "r");

	do
	{
		// read a line
		response = fgets(line, MAX_INPUT_LINE_SIZE, pipe);

		// The last character that fgets reads may be a newline, but we
		// don't want newlines in our list, so remove it.
		newlinePtr = strchr(line, '\n');
		if(newlinePtr != NULL)
		{
			*newlinePtr = '\0';
		}

		// add this line to outList
		pushToStringList(outList, line);
		
	} while (response != NULL);

	*returnVal = pclose(pipe);
}

// Returns the strings in list as a single string seperated by seperator.
// Every line in the list will be followed by a seperator, including the last line.
// The list is malloc'd and so must be free'd when finished.
char * flattenStringList(lineList * list, char seperator)
{
	char * retval = NULL;
	int i;
	int sizeCounter = 0;

	for(i = 0; i < list->lineCount; i++)
	{
		sizeCounter += strlen(list->lines[i]) + 1;
	}

	retval = malloc(sizeof(char) * sizeCounter);

	for(i = 0; i < list->lineCount; i++)
	{
		strcat(retval, list->lines[i]);
		strncat(retval, &seperator, 1);
	}
}
