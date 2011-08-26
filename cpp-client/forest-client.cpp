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

// exit()
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

using namespace std;

#include "config.h"

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

typedef struct forestConfigStruct
{
	string serverUrl;
} forestConfig;

//int getAvailableUpdates(vector<string> & outList);
void getAcceptedUpdates(vector<string> & outList, string * serverUrl, string * myHostname, bool * rebootAccepted);
//int applyUpdates(vector<string> & list);
void reportAvailableUpdates(vector<updateInfo> & list, string * serverUrl, string * myHostname, rebootState rebootNeeded, bool canApplyUpdates, bool canApplyReboot, bool rebootAttempted);
void readConfigFile(forestConfig * config);
int isRebootNeeded();

int main(int argc, char** args)
{
	vector<updateInfo> availableUpdates;
	vector<string> acceptedUpdates;
	forestConfig config;
	string hostname;
	int response = 0;
	PackageManager * packageManager;
	RebootManager * rebootManager;
	bool acceptedReboot = false;
	bool rebootAttempted = false;

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
	config.serverUrl = DEFAULT_SERVER_URL;

	// read config file
	readConfigFile(&config);

	// get list of packages that have been accepted for update
	// also check to see if a reboot has been accepted
	// this should really be split into two rpc calls, but keeping backwards compatible for now
	if(packageManager->canApplyUpdates() || rebootManager->canApplyReboot())
	{
		getAcceptedUpdates(acceptedUpdates, &config.serverUrl, &hostname, &acceptedReboot);
	}

	// apply accepted updates (and only available updates) if backend is able
	if(acceptedUpdates.size() > 0 && packageManager->canApplyUpdates())
	{
		packageManager->applyUpdates(acceptedUpdates);
	}

	// determine what packages are available to update
	// note that this is done AFTER applying accepted updates
	packageManager->getAvailableUpdates(availableUpdates);

	// apply reboot
	// this is done before reporting updates so that reboot attempted can be reported
	if(acceptedReboot && rebootManager->canApplyReboot())
	{
		rebootManager->applyReboot();
		rebootAttempted = true;
	}

	// report packages that are available to update
	// this should also be split into two rpc calls, but keeping backward compatible for now
	reportAvailableUpdates(availableUpdates, 
		&config.serverUrl, 
		&hostname, 
		rebootManager->isRebootNeeded(), 
		packageManager->canApplyUpdates(), 
		rebootManager->canApplyReboot(),
		rebootAttempted
	);

/*	// if backend can apply updates or reboot
	if(packageManager->canApplyUpdates() || rebootManager->canApplyReboot())
	{
		// get list of accepted updates (and check for accepted reboot)
		vector<string> acceptedUpdates;
		bool acceptedReboot = false;

		// apply accepted updates
		if(packageManager->canApplyUpdates() && acceptedUpdates.size() > 0)
		{
			packageManager->applyUpdates(acceptedUpdates);
		}

		// apply reboot
		if(rebootManager->canApplyReboot() && acceptedReboot)
		{
			rebootManager->applyReboot();
		}
	}*/

	return 0;
}

// fills outList with names of accepted packages
void getAcceptedUpdates(vector<string> & outList, string * serverUrl, string * myHostname, bool * rebootAccepted)
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
	mySystem(&command, curlOutput, &response);
	
	// exit silently if curl fails
	if(response != 0)
	{
		exit(0);
	}

	// make sure the response was something we expected
	if(curlOutput[0].substr(0, 8) != "data_ok:")
	{
		// lack of data_ok: is ok if this system is new to the server
		if(curlOutput[0].find("System not found in database") != string::npos)
		{
			outList.clear();
		}
		else
		{
			cerr << "Error getting accepted updates: " << flattenStringList(curlOutput, '\n') << endl;
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
			cerr << "Error getting accepted updates: " << flattenStringList(curlOutput, '\n') << endl;
			exit(1);
		}
		else
		{
			curlOutput[0] = trim_string(curlOutput[0].substr(position + 1));
		}

		outList.clear();
		string::size_type startPosition = 0;
		for(position = curlOutput[0].find(',', 0); position != string::npos; position = curlOutput[0].find(',', position + 1))
		{
			string acceptedUpdate = curlOutput[0].substr(startPosition, position - startPosition);
			cerr << "DEBUG: accepted update " << acceptedUpdate;
			outList.push_back(acceptedUpdate);
			startPosition = position + 1;
			//curlOutput[0] = curlOutput[0].substr(position + 1);
		}
	}

	//TODO: fill in reboot accepted code
	*rebootAccepted = false;
}

void reportAvailableUpdates(vector<updateInfo> & list, string * serverUrl, string * myHostname, rebootState rebootNeeded, bool canApplyUpdates, bool canApplyReboot, bool rebootAttempted)
{
	string command;
	vector<string> commandResponse;
	int commandRetval;

	command = "curl --silent --show-error --data \"rpc_version=";
	command += to_string(RPC_VERSION);
	command += "\" --data \"system_name=";
	command += *myHostname;
	command += "\"";

	command += " --data \"client_can_apply_updates=";
	if(canApplyUpdates)
	{
		command += "true";
	}
	else
	{
		command += "false";
	}
	command += "\"";

	command += " --data \"client_can_apply_reboot=";
	if(canApplyReboot)
	{
		command += "true";
	}
	else
	{
		command += "false";
	}
	command += "\"";

	if(list.size() == 0)
	{
		command += " --data \"no_updates_available=true\"";
	}
	else
	{
		// this output also needs to be escaped for HTML
		command += " --data \"available_updates=";
		for(int i = 0; i < list.size(); i++)
		{
			if(i != 0)
			{
				command += ",";
			}
			command += list[i].name;
		}
		command += "\"";
		// also add version info
		command += " --data \"versions=";
		for(int i = 0; i < list.size(); i++)
		{
			if(i != 0)
			{
				command += "|";
			}
			command += list[i].version;
		}
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

	command += " --data \"reboot_attempted=";
	if(rebootAttempted)
	{
		command += "true";
	}
	else
	{
		command += "false";
	}
	command += "\"";

	command += " ";
	command += *serverUrl;
	command += "collect.php";

	mySystem(&command, commandResponse, &commandRetval);
	cout << flattenStringList(commandResponse, '\n') << endl;

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

