//#############################################################################
// Forest - a web-based multi-system update manager
//
// Copyright (C) 2013 Nathan Crawford
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
#include "helpers.h"

// package managers
#include "aptcli.h"
#include "yum.h"
#include "wuaapi.h"
#include "MacSU.h"

//reboot managers
#include "RebootStub.h"
#include "FilePresence.h"
#include "KernelDifference.h"
#include "WinRegKey.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#include "exitcodes.h"
#include "verboselevels.h"
#include "defines.h"

// callback function for curl
size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp);

ForestClient::ForestClient()
{
	// If compilation breaks here, make sure PACKAGE_MANAGER and REBOOT_MANAGER
	// are defined in config.h.
	// ./configure should set a reasonable value for you.
	packageManager = new PACKAGE_MANAGER();
	rebootManager = new REBOOT_MANAGER();
	verboseLevel = VERBOSE_NORMAL;
	waitMode = false;
	// set a default for server url
	serverUrl = DEFAULT_SERVER_URL;
	myHostname = "NoHostName";
}

int ForestClient::run()
{
	vector<updateInfo> availableUpdates;
	vector<string> acceptedUpdates;
	bool acceptedReboot = false;
	bool rebootAttempted = false;

	if(verboseLevel >= VERBOSE_NORMAL)
	{
		cout << "Starting..." << endl;
	}

	getHostname();

	// read config file
	readConfigFile();

	if(verboseLevel >= VERBOSE_EXTRA)
	{
		cout << "Hostname: " << myHostname << endl;
		cout << "Server URL: " << serverUrl << endl;
		cout << "Using package manager " << TOSTRING(PACKAGE_MANAGER);
		cout << " and reboot manager " << TOSTRING(REBOOT_MANAGER) << "." << endl;
	}

	// get list of packages that have been accepted for update
	// also check to see if a reboot has been accepted
	// this should really be split into two rpc calls, but keeping backwards compatible for now
	if(packageManager->canApplyUpdates() || rebootManager->canApplyReboot())
	{
		if(verboseLevel >= VERBOSE_NORMAL)
		{
			cout << "Checking for accepted updates from server..." << endl;
		}
		getAcceptedUpdates(acceptedUpdates, &acceptedReboot);
		if(verboseLevel >= VERBOSE_EXTRA)
		{
			cout << acceptedUpdates.size() << " updates accepted by server:" << endl;
			for(auto itr = acceptedUpdates.begin(); itr != acceptedUpdates.end(); itr++)
			{
				cout << *itr;
			}
		}
	}

	// apply accepted updates (and only available updates) if backend is able
	if(acceptedUpdates.size() > 0 && packageManager->canApplyUpdates())
	{
		if(verboseLevel >= VERBOSE_NORMAL)
		{
			cout << "Applying accepted updates..." << endl;
		}
		packageManager->applyUpdates(acceptedUpdates);
	}


	if(verboseLevel >= VERBOSE_NORMAL)
	{
		cout << "Checking for available updates..." << endl;
	}
	// determine what packages are available to update
	// note that this is done AFTER applying accepted updates
	packageManager->getAvailableUpdates(availableUpdates);
	if(verboseLevel >= VERBOSE_EXTRA)
	{
		cout << "Package manager found " << availableUpdates.size() << " available updates." << endl;
	}

	// apply reboot
	// this is done before reporting updates so that reboot attempted can be reported
	if(acceptedReboot && rebootManager->canApplyReboot())
	{
		if(verboseLevel >= VERBOSE_NORMAL)
		{
			cout << "Scheduling reboot..." << endl;
		}
		rebootManager->applyReboot();
		rebootAttempted = true;
	}

	if(verboseLevel >= VERBOSE_EXTRA)
	{
		cout << "HTML encoding available updates to report to server..." << endl;
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

	if(verboseLevel >= VERBOSE_NORMAL)
	{
		cout << "Reporting available updates to server..." << endl;
	}

	// report packages that are available to update
	// this should also be split into two rpc calls, but keeping backward compatible for now
	reportAvailableUpdates(availableUpdates, rebootAttempted);

	if(verboseLevel >= VERBOSE_NORMAL)
	{
		cout << "Success." << endl;
	}
	if(verboseLevel >= VERBOSE_EXTRA)
	{
		cout << acceptedUpdates.size() << " accepted updates." << endl;
		cout << availableUpdates.size() << " available updates." << endl;
		if(rebootManager->isRebootNeeded())
		{
			cout << "Reboot is needed." << endl;
		}
		if(rebootAttempted)
		{
			cout << "Reboot was scheduled." << endl;
		}
	}

	if(waitMode)
	{
		// for debugging output
		cout << "Press enter to exit" << endl;
		cin.get();
	}

	return EXIT_CODE_OK;
}

// fills outList with names of accepted packages
void ForestClient::getAcceptedUpdates(vector<string> & outList, bool * rebootAccepted)
{
	string acceptedUrl;
	string command;

	// build accepted URL
	acceptedUrl = serverUrl;
	if(acceptedUrl[acceptedUrl.size() - 1] != '/')
	{
		acceptedUrl += "/";
	}
	acceptedUrl += "getaccepted.php?rpc_version=";
	acceptedUrl += to_string(RPC_VERSION);
	acceptedUrl += "&system=";
	acceptedUrl += myHostname;

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
		cerr << "cURL failed: " << curl_easy_strerror(res) << endl << curlOutput << endl;
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

void ForestClient::reportAvailableUpdates(vector<updateInfo> & list, bool rebootAttempted)
{
	string command;

	command = "rpc_version=";
	command += to_string(RPC_VERSION);
	command += "&system_name=";
	command += myHostname;

	command += "&client_can_apply_updates=";
	if(packageManager->canApplyUpdates())
	{
		command += "true";
	}
	else
	{
		command += "false";
	}

	command += "&client_can_apply_reboot=";
	if(rebootManager->canApplyReboot())
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
	int rebootNeeded = rebootManager->isRebootNeeded();
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

	string collectUrl = serverUrl;
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
		cerr << "cURL failed: " << curl_easy_strerror(res) << endl << curlOutput << endl;
		exit(EXIT_CODE_CURL);
	}

	if( (verboseLevel >= VERBOSE_EXTRA) || ((verboseLevel == VERBOSE_QUIET) && curlOutput.substr(0, 8) != "data_ok:"))
	{
		cout << curlOutput << endl;
	}
}

void ForestClient::readConfigFile()
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
					serverUrl = confValue;
				}
				else
				{
					cerr << "Unrecognized config item name: " << confName << endl;
				}
			}
		}
	}
}

void ForestClient::setVerboseLevel(unsigned int level)
{
	verboseLevel = level;
}

void ForestClient::setWaitMode(bool enabled)
{
	waitMode = enabled;
}

void ForestClient::getHostname()
{
	int response = 0;

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

	myHostname = temp;
}

// CURL callback
size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
	char * charBuf = (char *)buffer;
	string * data = (string *)userp;
	data->append(charBuf, nmemb);
	return nmemb;
}

