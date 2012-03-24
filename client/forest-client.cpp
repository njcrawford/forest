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
#if defined _WIN32
// windows
#include <winsock2.h>
#include <WS2tcpip.h>
#else
// unix-like systems 
#include <unistd.h>
#endif

/* Get HOST_NAME_MAX definition.  */
#include <limits.h>

// string functions
#include <string>
#include <string.h>

// cerr, cout
#include <iostream>

// curl
#include <curl/curl.h>

using namespace std;

#include "config.h"

#include "forest-client.h"

#include "version.h"

// package managers
#include "apt-get.h"
#include "yum.h"
#include "wuaapi.h"
#include "MacSU.h"

//reboot managers
#include "RebootStub.h"
#include "FilePresence.h"
#include "KernelDifference.h"
#include "WinRegKey.h"

#define RPC_VERSION 2
#define BUFFER_SIZE 1024

#define EXIT_CODE_OK             0
#define EXIT_CODE_CURL           1
#define EXIT_CODE_WSASTARTUP     2
#define EXIT_CODE_SOCKETERROR    3
#define EXIT_CODE_RESPONSEDATA   4
#define EXIT_CODE_CONFIGFILE     5
#define EXIT_CODE_HOSTNAME       6
#define EXIT_CODE_INVALIDSWITCH  7

typedef struct forestConfigStruct
{
	string serverUrl;
} forestConfig;

void getAcceptedUpdates(vector<string> & outList, string * serverUrl, string * myHostname, bool * rebootAccepted);
void reportAvailableUpdates(vector<updateInfo> & list, string * serverUrl, string * myHostname, rebootState rebootNeeded, bool canApplyUpdates, bool canApplyReboot, bool rebootAttempted);
void readConfigFile(forestConfig * config);
// callback function for curl
size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp);

// icky global goes here because I'm lazy
bool cronMode = false;

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

	if(argc >= 2)
	{
		if(strcmp(args[1], "--cron") == 0)
		{
			cronMode = true;
		}
		else if(strcmp(args[1], "--version") == 0)
		{
			cout << "Forest client version " << getForestVersion() << endl;
			exit(EXIT_CODE_OK);
		}
		else
		{
			cout << "Unrecognized switch: " << args[1] << endl;
			exit(EXIT_CODE_INVALIDSWITCH);
		}
	}

	// If compilation breaks here, make sure PACKAGE_MANAGER and REBOOT_MANAGER
	// are defined in config.h.
	// ./configure should set a reasonable value for you.
	packageManager = new PACKAGE_MANAGER();
	rebootManager = new REBOOT_MANAGER();

// some OSes have different names for HOST_NAME_MAX
#if !defined HOST_NAME_MAX

	// osx has a different name for HOST_NAME_MAX
	#if defined _POSIX_HOST_NAME_MAX
		#define HOST_NAME_MAX _POSIX_HOST_NAME_MAX
	#endif

	// windows has a different name for HOST_NAME_MAX
	#if defined NI_MAXHOST
		#define HOST_NAME_MAX NI_MAXHOST
	#endif

#endif // !defined HOST_NAME_MAX

	char temp[HOST_NAME_MAX + 1];
	temp[HOST_NAME_MAX] = '\0';
#ifdef _WIN32
	WSADATA WSAData;
	if(WSAStartup(MAKEWORD(1,0), &WSAData) != 0)
	{
		response = WSAGetLastError();
		//FormatMessage() to get the error text
		cerr << "Could not get hostname: WSAStartup error " << response << endl;
		cerr << "Exiting..." << endl;
		exit(EXIT_CODE_WSASTARTUP);
	}
#endif
	// get the current host name
	response = gethostname(temp, HOST_NAME_MAX);
#ifdef _WIN32
	if(response == SOCKET_ERROR)
	{
		response = WSAGetLastError();
		//FormatMessage() to get the error text
		cerr << "Could not get hostname: Socket error " << response << endl;
		cerr << "Exiting..." << endl;
		exit(EXIT_CODE_SOCKETERROR);
	}
	WSACleanup();
#else
	if(response == -1)
	{
		//fprintf(stderr, "Could not get hostname, exiting.");
		perror("Error in main(): ");
		exit(EXIT_CODE_HOSTNAME);
	}
#endif
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

	CURL *handle = curl_easy_init();
	// URL encode package names and versions
	for(size_t i = 0; i < availableUpdates.size(); i++)
	{
		char *encodedURL = curl_easy_escape(handle, availableUpdates[i].name.c_str(), 0);
		availableUpdates[i].name = encodedURL;
		curl_free(encodedURL);
		encodedURL = curl_easy_escape(handle, availableUpdates[i].version.c_str(), 0);
		availableUpdates[i].version = encodedURL;
		curl_free(encodedURL);
	}
	curl_easy_cleanup(handle);

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

	// for debugging output
	//cin.get();

	return EXIT_CODE_OK;
}

// fills outList with names of accepted packages
void getAcceptedUpdates(vector<string> & outList, string * serverUrl, string * myHostname, bool * rebootAccepted)
{
	string acceptedUrl;
	string command;

	// build accepted URL
	acceptedUrl = *serverUrl;
	if(acceptedUrl[acceptedUrl.size() - 1] != '/')
	{
		acceptedUrl += "/";
	}
	acceptedUrl += "getaccepted.php?rpc_version=";
	acceptedUrl += to_string(RPC_VERSION);
	acceptedUrl += "&system=";
	acceptedUrl += *myHostname;

	// run curl command
	CURL * curlHandle;
	CURLcode res;
	string curlOutput;
	curlHandle = curl_easy_init();
	if(curlHandle)
	{
		curl_easy_setopt(curlHandle, CURLOPT_URL, acceptedUrl.c_str());
		curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, write_data); 
		curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &curlOutput); 
		res = curl_easy_perform(curlHandle);

		//cleanup
		curl_easy_cleanup(curlHandle);
	}
	
	// exit if curl fails
	if(res != CURLE_OK)
	{
		cerr << "cURL failed: " << curlOutput << endl;
		exit(EXIT_CODE_CURL);
	}

	// make sure the response was something we expected
	if(curlOutput.substr(0, 8) != "data_ok:")
	{
		// lack of data_ok: is ok if this system is new to the server
		if(curlOutput.find("System not found in database") != string::npos)
		{
			outList.clear();
		}
		else
		{
			cerr << "Error getting accepted updates: " << curlOutput << endl;
			exit(EXIT_CODE_RESPONSEDATA);
		}
	}
	else
	{
		string::size_type position;
		string::size_type position2;

		// check for data_ok:
		if(curlOutput.substr(0,8) != "data_ok:")
		{
			cerr << "Error getting accepted updates: " << curlOutput << endl;
			exit(EXIT_CODE_RESPONSEDATA);
		}

		// check for reboot-(true|false)
		position = curlOutput.find(':', 0);
		position2 = curlOutput.find(':', position + 1);
		if(position == string::npos)
		{
			cerr << "Error getting accepted updates: " << curlOutput << endl;
			exit(EXIT_CODE_RESPONSEDATA);
		}
		// don't die if reboot state isn't present
		*rebootAccepted = false;
		if(position2 != string::npos)
		{
			string rebootState = trim_string(curlOutput.substr(position + 1, position2 - position - 1));
			if(rebootState == "reboot-true")
			{
				*rebootAccepted = true;
			}
		}
		else
		{
			// for trimming output, below
			position2 = position;
		}
		
		// trim data_ok and reboot state from output
		curlOutput = curlOutput.substr(position2 + 1);

		outList.clear();

		// if there are no updates, the string will be empty (because of trim_string)
		if(trim_string(curlOutput).size() == 0)
		{
			return;
		}

		string::size_type startPosition = 0;
		string acceptedUpdate;
		for(position = curlOutput.find(' ', 0); position != string::npos; position = curlOutput.find(' ', position + 1))
		{
			acceptedUpdate = curlOutput.substr(startPosition, position - startPosition);
			if(acceptedUpdate.size() > 0)
			{
				//cerr << "DEBUG: accepted update " << acceptedUpdate << endl;
				outList.push_back(acceptedUpdate);
			}
			startPosition = position + 1;
			//curlOutput[0] = curlOutput[0].substr(position + 1);
		}
		//add the last item (whatever is after the last space) if it's not empty
		acceptedUpdate = curlOutput.substr(startPosition);
		if(trim_string(acceptedUpdate).size() > 0)
		{
			//cerr << "DEBUG: accepted update " << acceptedUpdate << endl;
			outList.push_back(acceptedUpdate);
		}
	}
}

void reportAvailableUpdates(vector<updateInfo> & list, string * serverUrl, string * myHostname, rebootState rebootNeeded, bool canApplyUpdates, bool canApplyReboot, bool rebootAttempted)
{
	string command;

	command = "rpc_version=";
	command += to_string(RPC_VERSION);
	command += "&system_name=";
	command += *myHostname;

	command += "&client_can_apply_updates=";
	if(canApplyUpdates)
	{
		command += "true";
	}
	else
	{
		command += "false";
	}

	command += "&client_can_apply_reboot=";
	if(canApplyReboot)
	{
		command += "true";
	}
	else
	{
		command += "false";
	}

	if(list.size() == 0)
	{
		command += "&no_updates_available=true";
	}
	else
	{
		// this output also needs to be escaped for HTML
		command += "&available_updates=";
		for(size_t i = 0; i < list.size(); i++)
		{
			if(i != 0)
			{
				command += ",";
			}
			command += list[i].name;
		}
		// also add version info
		command += "&versions=";
		for(size_t i = 0; i < list.size(); i++)
		{
			if(i != 0)
			{
				command += "|";
			}
			command += list[i].version;
		}
	}

	command += "&reboot_required=";
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

	command += "&reboot_attempted=";
	if(rebootAttempted)
	{
		command += "true";
	}
	else
	{
		command += "false";
	}

	string collectUrl = *serverUrl;
	if(collectUrl[collectUrl.size() - 1] != '/')
	{
		collectUrl += "/";
	}
	collectUrl += "collect.php";

	CURL * curlHandle;
	CURLcode res = CURLE_OK;
	string curlOutput;
	curlHandle = curl_easy_init();
	if(curlHandle)
	{
		curl_easy_setopt(curlHandle, CURLOPT_POSTFIELDS, command.c_str());
		curl_easy_setopt(curlHandle, CURLOPT_URL, collectUrl.c_str());
		curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, write_data); 
		curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &curlOutput); 
		res = curl_easy_perform(curlHandle);

		//cleanup
		curl_easy_cleanup(curlHandle);
	}
	
	// exit if curl fails
	if(res != CURLE_OK)
	{
		cerr << "cURL failed: " << curlOutput << endl;
		exit(EXIT_CODE_CURL);
	}

	if(!cronMode || (cronMode && curlOutput.substr(0, 8) != "data_ok:"))
	{
		cout << curlOutput << endl;
	}
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

	line[0] = '\0';

	configFile = fopen(CONFIG_FILE_PATH, "r");
	if(configFile == NULL)
	{
		cerr << "Failed to open config file " << CONFIG_FILE_PATH << endl;
		cerr << "Exiting..." << endl;
		exit(EXIT_CODE_CONFIGFILE);
	}
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
					// advance the split pointer to just past where the first double quote was
					splitPtr = cleanupPtr + 1;
				}
				
				cleanupPtr = strchr(splitPtr, '"');
				if(cleanupPtr != NULL)
				{
					*cleanupPtr = '\0';
				}
				
				confName = line;
				confValue = (splitPtr);
				if(confName.substr(0, 10) == "server_url")
				{
					config->serverUrl = confValue;
				}
				else
				{
					cerr << "Unrecognized config item name: " << confName << endl;
				}
			}
		}
	}
}



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

// CURL callback
size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
	char * charBuf = (char *)buffer;
	string * data = (string *)userp;
	data->append(charBuf, nmemb);
	return nmemb;
}

