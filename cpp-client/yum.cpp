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

	command = "yum check-update -q -C 2>&1";

	mySystem(&command, &commandOutput, &commandRetval);

	// yum check-update returns 0 if no updates are available or 100 if 
	// there is at least one update available
	if(commandRetval == 100)
	{
		for(int i = commandOutput.size() - 1; i >= 0; i--)
		{
			//filter out empty lines
			//updates=`echo "${updates}" | grep -v " \* \|^$"`
			//filter out wait messages when other instances of yum are running
			//updates=`echo "${updates}" | grep -v "^Another app\|^Existing lock"`

			//filter out HTTP errors when a mirror is down
			//updates=`echo "${updates}" | grep -v "HTTP Error\|^Trying other mirror.$"`
			if(trim_string(commandOutput[i]).size() == 0 || 
				commandOutput[i].substr(0, 11) == "Another app" ||
				commandOutput[i].substr(0, 13) == "Existing lock" ||
				commandOutput[i].find("HTTP Error", 0) != string::npos ||
				commandOutput[i] == "Trying other mirror.")
			{
				// remove this line and don't process it any further
				commandOutput.erase(commandOutput.begin() + i);
				continue;
			}
			
			updateInfo temp;
			
			string::size_type pos = commandOutput[i].find(' ', 0);
			if(pos != string::npos)
			{
				//keep everything up to the first space (package name and arch)
				//updates=`echo "${updates}" | cut -d " " -f 1`
				temp.name = commandOutput[i].substr(0, pos);
			}

			pos = temp.name.rfind('.', string::npos);
			if(pos != string::npos)
			{
				//remove the architecture (.i386, .x86_64, etc)
				//updates=`echo "${updates}" | sed 's/\(.*\)\..*/\1/'`
				temp.name = temp.name.substr(0, pos);
			}

			// don't duplicate items in the list
			// on rpm based systems, multiple architectures of the same package may be installed
			// removing the architecture will create duplicates, this will filter them out
			bool alreadyInList = false;
			for(int i = 0; i < outList.size(); i++)
			{
				if(temp.name == outList[i].name)
				{
					alreadyInList = true;
					break;
				}
			}
			if(alreadyInList)
			{
				continue;
			}

			// get the version number
			// there is a variable amount of space between the name and version
			pos = commandOutput[i].find(' ', 0);
			if(pos != string::npos)
			{
				// remove the package name, and trim the extra leading spaces
				temp.version = trim_string(commandOutput[i].substr(pos));
			}
			// there is more info after the version, so the rest will need to be cut off
			pos = temp.version.find(' ', 0);
			if(pos != string::npos)
			{
				temp.version = temp.version.substr(0, pos);
			}
			
			outList.push_back(temp);
		}
		for(int i = commandOutput.size() - 1; i >= 0; i--)
		{
			//remove duplicates from the list (produced by removing architecture)
			//the list may need to be sorted before running uniq
			//updates=`echo "${updates}" | uniq`
			for(int dupIndex = i - 1; dupIndex >= 0; dupIndex--)
			{
				if(commandOutput[i] == commandOutput[dupIndex])
				{
					commandOutput.erase(commandOutput.begin() + i);
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

	//TODO: yum output will need further cleanup
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
