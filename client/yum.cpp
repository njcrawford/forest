#include <stdlib.h>
#include <string>
#include <iostream>

#include "yum.h"
#include "forest-client.h"

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
		for(int line = commandOutput.size() - 1; line >= 0; line--)
		{
			//filter out empty lines
			//updates=`echo "${updates}" | grep -v " \* \|^$"`
			//filter out wait messages when other instances of yum are running
			//updates=`echo "${updates}" | grep -v "^Another app\|^Existing lock"`

			//filter out HTTP errors when a mirror is down
			//updates=`echo "${updates}" | grep -v "HTTP Error\|^Trying other mirror.$"`
			if(trim_string(commandOutput[line]).size() == 0 || // empty string, of no use to us
				commandOutput[line].substr(0, 11) == "Another app" || // warning that another app already has the yum lock
				commandOutput[line].substr(0, 13) == "Existing lock" || // usually follows previous line
				commandOutput[line].find("HTTP Error", 0) != string::npos || // ignore any kind of http error
				commandOutput[line] == "Trying other mirror." // usually follows previous line
			)
			{
				// remove this line and don't process it any further
				commandOutput.erase(commandOutput.begin() + line);
				continue;
			}

			if(commandOutput[line].substr(0, 19) == "Obsoleting Packages")
			{
				// We have to use for the obsolete package list right now
				break;
			}
			
			updateInfo temp;
			
			string::size_type pos = commandOutput[line].find(' ', 0);
			if(pos != string::npos)
			{
				//keep everything up to the first space (package name and arch)
				//updates=`echo "${updates}" | cut -d " " -f 1`
				temp.name = commandOutput[line].substr(0, pos);
			}

			pos = temp.name.rfind('.', string::npos);
			if(pos != string::npos)
			{
				//remove the architecture (.i386, .x86_64, etc)
				//updates=`echo "${updates}" | sed 's/\(.*\)\..*/\1/'`
				temp.name = temp.name.substr(0, pos);
			}

			// get the version number
			// there is a variable amount of space between the name and version
			pos = commandOutput[line].find(' ', 0);
			if(pos != string::npos)
			{
				// remove the package name, and trim the extra leading spaces
				temp.version = trim_string(commandOutput[line].substr(pos));
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
		for(int line = commandOutput.size() - 1; line >= 0; line--)
		{
			//remove duplicates from the list (produced by removing architecture)
			//the list may need to be sorted before running uniq
			//updates=`echo "${updates}" | uniq`
			for(int dupIndex = line - 1; dupIndex >= 0; dupIndex--)
			{
				if(commandOutput[line] == commandOutput[dupIndex])
				{
					commandOutput.erase(commandOutput.begin() + line);
				}
			}
		}
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
