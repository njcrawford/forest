// exit()
#include <stdlib.h>

// cerr
#include <iostream>

#include "aptcli.h"
#include "forest-client.h"
#include "helpers.h"

void AptCli::parseUpdates(vector<updateInfo> & outList, vector<string> & input)
{
	for(size_t i = 0; i < input.size(); i++)
	{
		// example of one output line
		// Inst libpam-modules [1.1.1-2ubuntu5] (1.1.1-2ubuntu5.3 Ubuntu:10.04/lucid-updates)

		// example of a line without an existing version number
		// It appears that we might be better off ignoring these lines, at least when it involves the kernel.
		// Directly installing these packages means they won't be marked for autoremove later.
		// Inst linux-image-3.2.0-54-virtual (3.2.0-54.82 Ubuntu:12.04/precise-updates [amd64])

		// grep ^Inst | cut -d " " -f 2
		if(input[i].substr(0, 4) == "Inst")
		{
			string::size_type pos = input[i].find(' ', 0);
			string::size_type len = input[i].find(' ', pos + 1) - pos;
			updateInfo temp;
			temp.name = input[i].substr(pos, len);
			// Ignore packages that don't have an existing version number
			if(input[i][pos + 1] == '[')
			{
				pos = input[i].find('(', pos + 1);
				len = input[i].find(' ', pos + 1) - pos;
				temp.version = input[i].substr(pos + 1, len - 1);
				outList.push_back(temp);
			}
		}
	}
}

void AptCli::getAvailableUpdates(vector<updateInfo> & outList)
{
	string command;
	int commandRetval = 0;
	vector<string> commandOutput;

	command = "/usr/bin/apt-get dist-upgrade -Vs 2>&1";

	mySystem(&command, commandOutput, &commandRetval);

	if(commandRetval == 0)
	{
		parseUpdates(outList, commandOutput);
	}
	else
	{
		cerr << "apt-get failed:" << endl << flattenStringList(commandOutput, '\n') << endl;
		exit(10);
	}

}

void AptCli::applyUpdates(vector<string> & list)
{
	string command;
	int commandResponse;
	vector<string> commandOutput;	

	// DEBIAN_FRONTEND=noninteractive tells ucf not to ask configuration questions.
	// --force-confdef tells dconf to make config file modifications when it knows what to do.
	// --force-confold tells dconf to leave the existing config files as-is when it doesn't know what to do.
	command = "DEBIAN_FRONTEND=noninteractive apt-get -y -o Dpkg::Options::\\=\"--force-confdef\" -o Dpkg::Options::\\=\"--force-confold\" --only-upgrade ";
	//cerr << command << endl;
	command += flattenStringList(list, ' ');
	//cerr << command << endl;
	command += " 2>&1";
	//cerr << command << endl;

	mySystem(&command, commandOutput, &commandResponse);
	
	if(commandResponse != 0)
	{
		cerr << "Error in applyUpdates: Package manager failed to apply updates:\n"; 
		cerr << flattenStringList(commandOutput, ' ');
		exit(1);
	}
}

bool AptCli::canApplyUpdates()
{
	return true;
}
