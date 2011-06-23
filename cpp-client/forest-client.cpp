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
#include <string>
#include <string.h>
// cerr, cout
#include <iostream>
// to_string
#include <sstream>

using namespace std;

#include "forest-client.h"

// package managers
#include "apt-get.h"
#include "yum.h"
#include "wuaapi.h"

//reboot managers
#include "RebootStub.h"
#include "FilePresence.h"
#include "KernelDifference.h"

#define RPC_VERSION 2

#define BUFFER_SIZE 1024

#define CONFIG_FILE_PATH "/etc/forest-client.conf"

// for now, define what PM to use at compile time (from list above)
// valid options are _APTGET (apt-get), _YUM (yum) and _WUAAPI (Windows 
// update agent API)
#define PACKAGE_MANAGER_APTGET
//#define PACKAGE_MANAGER_YUM
//#define PACKAGE_MANAGER_WUAAPI

// If no reboot manager is defined, the stub will be used
#define REBOOT_MANAGER_FILEPRESENCE
//#define REBOOT_MANAGER_KERNELDIFFERENCE
//#define REBOOT_MANAGER_REBOOTSTUB

typedef struct forestConfigStruct
{
	string serverUrl;
} forestConfig;

int getAvailableUpdates(vector<string> * outList);
int getAcceptedUpdates(vector<string> * outList, string * serverUrl, string * myHostname);
int applyUpdates(vector<string> * list);
int reportAvailableUpdates(vector<string> * list, string * serverUrl, string * myHostname, rebootState rebootNeeded);
void readConfigFile(forestConfig * config);
int isRebootNeeded();

int main(int argc, char** args)
{
	vector<string> availableUpdates;
	vector<string> acceptedUpdates;
	forestConfig config;
	string hostname;
	int response = 0;
	PackageManager * packageManager;
	RebootManager * rebootManager;

#if defined PACKAGE_MANAGER_APTGET
	packageManager = new AptGet();
#elif defined PACKAGE_MANAGER_YUM
	packageManager = new Yum();
#elif defined PACKAGE_MANAGER_WUAAPI
	packageManager = new WuaApi();
#else
	#error "No package manager defined!"
#endif

#if defined REBOOT_MANAGER_FILEPRESENCE
	rebootManager = new FilePresence();
#elif defined REBOOT_MANAGER_KERNELDIFFERENCE
	rebootManager = new KernelDifference();
#else
	#warning "No reboot manager defined, using stub"
	rebootManager = new RebootStub();
#endif

	char temp[HOST_NAME_MAX + 1];
	temp[HOST_NAME_MAX] = '\0';
	// get the current host name
	response = gethostname(temp, HOST_NAME_MAX);
	if(response == -1)
	{
		//fprintf(stderr, "Could not get hostname, exiting.");
		perror("Error in main(): ");
		exit(1);
	}
	hostname = temp;

	// set a default for server url
	config.serverUrl = "http://forest/forest";

	// read config file
	readConfigFile(&config);

	// determine what packages are available to update
	packageManager->getAvailableUpdates(&availableUpdates);

	// get list of packages that have been accepted for update
	getAcceptedUpdates(&acceptedUpdates, &config.serverUrl, &hostname);

	// apply accepted updates (and only available updates)
	if(acceptedUpdates.size() > 0)
	{
		packageManager->applyUpdates(&acceptedUpdates);
	}

	// report packages that are available to update
	reportAvailableUpdates(&availableUpdates, &config.serverUrl, &hostname, rebootManager->isRebootNeeded());

	return 0;
}

// fills outList with names of accepted packages
int getAcceptedUpdates(vector<string> * outList, string * serverUrl, string * myHostname)
{
	string acceptedUrl;
	string command;
	int response = 0;
	vector<string> curlOutput;

	// build accepted URL
	acceptedUrl = *serverUrl;
	acceptedUrl += "getaccepted.php?rpc_version=";
	acceptedUrl += to_string(RPC_VERSION);
	acceptedUrl += "&system=";
	acceptedUrl += *myHostname;

	// build command to run
	//snprintf(command, BUFFER_SIZE, "curl --silent --show-error \"%s\"", acceptedUrl);
	command = "curl --silent --show-error \"" + acceptedUrl + "\"";
	mySystem(&command, &curlOutput, &response);
	
	// exit silently if curl fails
	if(response != 0)
	{
		exit(0);
	}

	// make sure the response was something we expected
	if(curlOutput[0].substr(0, 8) != "data_ok:")
	{
		// lack of data_ok: is ok if this system is new to the server
		if(curlOutput[0].substr(0, 28) == "System not found in database")
		{
			outList->clear();
		}
		else
		{
			cerr << "Error getting accepted updates: " << flattenStringList(&curlOutput, '\n') << endl;
			exit(1);
		}
	}
	else
	{
		// remove data_ok:
		// TODO: this is pretty bad coding practice but I want to get going quickly
		curlOutput[0] = trim_string(curlOutput[0].substr(9));
		
		string::size_type position = curlOutput[0].find(':', 0);
		if(position == string::npos)
		{
			cerr << "Error getting accepted updates: " << flattenStringList(&curlOutput, '\n') << endl;
			exit(1);
		}
		else
		{
			curlOutput[0] = trim_string(curlOutput[0].substr(position + 1));
		}

		outList->clear();
		for(position = curlOutput[0].find(',', 0); position != string::npos; position = curlOutput[0].find(',', position + 1))
		{
			outList->push_back(curlOutput[0].substr(0, position - 1));
			curlOutput[0] = curlOutput[0].substr(position + 1);
		}
	}
}

int reportAvailableUpdates(vector<string> * list, string * serverUrl, string * myHostname, rebootState rebootNeeded)
{
	string command;
	vector<string> commandResponse;
	int commandRetval;

	command = "curl --silent --show-error --data \"rpc_version=";
	command += to_string(RPC_VERSION);
	command += "\" --data \"system_name=";
	command += *myHostname;
	command += "\"";

	if(list->size() == 0)
	{
		command += " --data \"no_updates_available=true\"";
	}
	else
	{
		// this output also needs to be escaped for HTML
		command += " --data \"available_updates=";
		command += flattenStringList(list, ',');
		command += "\"";
	}

	command += " --data \"reboot_required=";
	if(rebootNeeded == 1)
	{
		command += "true";
	}
	else if(rebootNeeded == 0)
	{
		command += "false";
	}
	else
	{
		command += "unknown";
	}
	command += "\"";

	//TODO: add reboot attempted

	command += " ";
	command += *serverUrl;
	command += "collect.php";

	mySystem(&command, &commandResponse, &commandRetval);
	cout << flattenStringList(&commandResponse, '\n') << endl;

	// do something to check return value
}

void readConfigFile(forestConfig * config)
{
	FILE * configFile;
	char line[BUFFER_SIZE];
	char * response = line;
	char * splitPtr;
	string confName;
	string confValue;
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
				
				confName = line;
				confValue = (splitPtr + 1);
				if(confName.substr(0, 10) == "server_url")
				{
					config->serverUrl = confValue;
				}
				else
				{
					cerr << "Unrecognized config item name: " << confName << "\n";
				}
			}
		}
	}
}



// Works like system(), but returns lines of stdout output in outList and the 
// command's return value in returnVal.
void mySystem(string * command, vector<string> * outList, int * returnVal)
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
		outList->push_back(line);
		
	} while (response != NULL);

	*returnVal = pclose(pipe);
}

string flattenStringList(vector<string> * list, char delimiter)
{
	string retval = "";
	bool first = true;
	for(int i = 0; i < list->size(); i++)
	{
		if(first)
		{
			first = false;
		}
		else
		{
			retval += delimiter;
		}
		retval += list->at(i);
	}
	return retval;
}

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
