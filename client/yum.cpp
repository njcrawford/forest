#include <stdlib.h>
#include <string>
#include <iostream>

#include "yum.h"
#include "helpers.h"

void Yum::parseUpdates(vector<updateInfo> & outList, vector<string> & input)
{
	for(size_t line = 0; line < input.size(); line++)
	{
		// Trim all lines
		input[line] = trim_string(input[line]);

		if(input[line].size() == 0 || // empty string, of no use to us
			input[line].substr(0, 11) == "Another app" || // warning that another app already has the yum lock
			input[line].substr(0, 13) == "Existing lock" || // usually follows Another app line
			input[line].find("HTTP Error", 0) != string::npos || // ignore any kind of http error
			input[line] == "Trying other mirror." // usually follows HTTP error line
		)
		{
			// Don't process this line any further
			continue;
		}

		if(input[line].substr(0, 19) == "Obsoleting Packages")
		{
			// We have no use for the obsolete package list right now
			break;
		}
		
		updateInfo temp;
		
		string::size_type pos = input[line].find(' ', 0);
		if(pos != string::npos)
		{
			//keep everything up to the first space (package name and arch)
			temp.name = input[line].substr(0, pos);
		}

		pos = temp.name.rfind('.', string::npos);
		if(pos != string::npos)
		{
			//remove the architecture (.i386, .x86_64, etc)
			temp.name = temp.name.substr(0, pos);
		}

		// get the version number
		// there is a variable amount of space between the name and version
		pos = input[line].find(' ', 0);
		if(pos != string::npos)
		{
			// remove the package name, and trim the extra leading spaces
			temp.version = trim_string(input[line].substr(pos));
		}
		// there is more info after the version, so the rest will need to be cut off
		pos = temp.version.find(' ', 0);
		if(pos != string::npos)
		{
			temp.version = temp.version.substr(0, pos);
		}

		// Don't duplicate items in the list.
		// On rpm based systems, multiple architectures of the same package may be installed
		// removing the architecture will create duplicates, this will filter them out.
		bool alreadyInList = false;
		for(size_t x = 0; x < outList.size(); x++)
		{
			if(temp.name == outList[x].name && temp.version == outList[x].version)
			{
				alreadyInList = true;
				break;
			}
		}
		if(alreadyInList)
		{
			continue;
		}

		outList.push_back(temp);
	}
}

void Yum::getAvailableUpdates(vector<updateInfo> & outList)
{
	string command;
	int commandRetval = 0;
	vector<string> commandOutput;

	// run yum with quiet (-q) and only critical error reporting (-e0)
	// add -C to run from cache
	command = "/usr/bin/yum check-update -q -e0 2>&1";

	mySystem(&command, commandOutput, &commandRetval);

	// yum check-update returns 0 if no updates are available or 100 if 
	// there is at least one update available
	if(commandRetval == 100)
	{
		parseUpdates(outList, commandOutput);
	}
}

void Yum::applyUpdates(vector<string> & list)
{
	string command;
	int commandResponse;
	vector<string> commandOutput;	

	command = "yum -y update ";
	command += flattenStringList(list, ' ');
	command += " 2>&1";

	mySystem(&command, commandOutput, &commandResponse);

	
	if(commandResponse != 0)
	{
		cerr << "Error in applyUpdates: Package manager failed to apply updates:\n";
		cerr << flattenStringList(commandOutput, ' ');
		exit(1);
	}
}

bool Yum::canApplyUpdates()
{
	return true;
}
