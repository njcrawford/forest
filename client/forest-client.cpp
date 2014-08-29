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
#include <fstream>

// curl
#include <curl/curl.h>

using namespace std;

#include "config.h"
#include "version.h"
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

// RPC managers
#include "Rpcv2.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#include "exitcodes.h"
#include "verboselevels.h"

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
	ForestClientCapabilities clientCaps;
	ForestRpc * rpcv2 = nullptr;

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

	rpcv2 = new Rpcv2(serverUrl, myHostname);

	// Get list of packages that have been accepted for update
	// Also check to see if a reboot has been accepted
	// This should really be split into two rpc calls, but keeping
	// backwards compatible for now.
	if(packageManager->canApplyUpdates() || rebootManager->canApplyReboot())
	{
		if(verboseLevel >= VERBOSE_NORMAL)
		{
			cout << "Checking for accepted updates from server..." << endl;
		}
		acceptedUpdates = rpcv2->getAcceptedUpdates();
		acceptedReboot = rpcv2->getRebootAccepted();
		if(verboseLevel >= VERBOSE_EXTRA)
		{
			cout << acceptedUpdates.size() << " updates accepted by server:" << endl;
			for(auto itr = acceptedUpdates.begin(); itr != acceptedUpdates.end(); itr++)
			{
				cout << *itr << endl;
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
	availableUpdates.push_back({"test-name", "test-version"});
	availableUpdates.push_back({"test-name2", "test-version2"});
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
	clientCaps.canApplyUpdates = packageManager->canApplyUpdates();
	clientCaps.canApplyReboot = rebootManager->canApplyReboot();
	clientCaps.clientVersion = FOREST_VERSION;
	rpcv2->checkInWithServer(clientCaps, availableUpdates, rebootManager->isRebootNeeded(), rebootAttempted, verboseLevel);
	//reportAvailableUpdates(availableUpdates, rebootAttempted);

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

void ForestClient::readConfigFile()
{
	string line;
	string confName;
	string confValue;

	ifstream configFile(CONFIG_FILE_PATH);

	if(!configFile.is_open())
	{
		cerr << "Failed to open config file " << CONFIG_FILE_PATH << endl;
		cerr << "Exiting..." << endl;
		exit(EXIT_CODE_CONFIGFILE);
	}
	while(getline(configFile, line))
	{
		if(line[0] != '#')
		{
            size_t splitIndex = line.find('=', 0);

			if(splitIndex != string::npos)
			{
                confName = line.substr(0, splitIndex);
                confName = trim_string(confName);
                confValue = line.substr(splitIndex + 1, line.size() - splitIndex - 1);
                confValue = trim_string(confValue);

				if(confName.size() >= 10 && confName.substr(0, 10) == "server_url")
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

	configFile.close();
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

