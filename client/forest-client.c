//#############################################################################
// Forest - a web-based multi-system update manager
//
// Copyright (C) 2011 Nathan Crawford
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

#include "apt-get.h"
#include "yum.h"
#include "wuaapi.h"
#include "lineList.h"

#define RPC_VERSION 2

#define BUFFER_SIZE 1024

#define CONFIG_FILE_PATH "/etc/forest-client.conf"

// for now, define what PM to use at compile time (from list above)
// valid options are _APTGET (apt-get), _YUM (yum) and _WUAAPI (Windows 
// update agent API)
#define PACKAGE_MANAGER_APTGET
//#define PACKAGE_MANAGER_YUM
//#define PACKAGE_MANAGER_WUAAPI

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
#if defined PACKAGE_MANAGER_APTGET
	return getAvailableUpdatesAptGet(outList);
#elif defined PACKAGE_MANAGER_YUM
	return getAvailableUpdatesYum(outList);
#elif defined PACKAGE_MANAGER_WUAAPI
	return getAvailableUpdatesWuaApi(outList);
#endif
}

// fills outList with names of accepted packages
int getAcceptedUpdates(lineList* outList, const char * serverUrl, const char * myHostname)
{
	char acceptedUrl[BUFFER_SIZE];
	char command[BUFFER_SIZE];
	int response = 0;
	char wordBuffer[BUFFER_SIZE];

	// build accepted URL
	snprintf(acceptedUrl, BUFFER_SIZE, "%s/getaccepted.php?rpc_version=%d&system=%s", serverUrl, RPC_VERSION, myHostname);

	// build command to run
	snprintf(command, BUFFER_SIZE, "curl --silent --show-error \"%s\"", acceptedUrl);
	mySystem(command, outList, &response);
	
	// exit silently if curl fails
	if(response != 0)
	{
		exit(0);
	}

	// make sure the response was something we expected
	sscanf(outList->lines[0], "%s", wordBuffer);
	if(strcmp(wordBuffer, "data_ok:") != 0)
	{
		fprintf(stderr, "Error getting accepted updates: %s\n", outList->lines[0]);
		exit(1);
	}
	else
	{
		// remove data_ok and reboot lines
	}
}

int applyUpdates(lineList* list)
{
#if defined PACKAGE_MANAGER_APTGET
	return applyUpdatesAptGet(list);
#elif defined PACKAGE_MANAGER_YUM
	return applyUpdatesYum(list);
#elif defined PACKAGE_MANAGER_WUAAPI
	return applyUpdatesWuaApi(list);
#endif
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
	char * cleanupPtr = NULL;

	configFile = fopen(CONFIG_FILE_PATH, "r");
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
				// remove the equals sign
				*splitPtr = '\0';

				// remove a newline, if it exists
				cleanupPtr = strchr(line, '\n');
				if(cleanupPtr != NULL)
				{
					*cleanupPtr = '\0';
				}

				// remove two double quotes, if they exist
				cleanupPtr = strchr(splitPtr + 1, '"');
				if(cleanupPtr != NULL)
				{
					*cleanupPtr = '\0';
					// advance the split pointer to where the first double quote was
					splitPtr = cleanupPtr;
				}
				
				cleanupPtr = strchr(splitPtr + 1, '"');
				if(cleanupPtr != NULL)
				{
					*cleanupPtr = '\0';
				}
				
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



// Works like system(), but returns lines of stdout output in outList and the 
// command's return value in returnVal.
void mySystem(const char* command, lineList* outList, int * returnVal)
{
	FILE * pipe;
	char line[BUFFER_SIZE];
	char * response = NULL;
	//int lineLen = 0;
	char * newlinePtr = NULL;

	line[0] = '\0';

	pipe = popen(command, "r");

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
		pushToStringList(outList, line);
		
	} while (response != NULL);

	*returnVal = pclose(pipe);
}


