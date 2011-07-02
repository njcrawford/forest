#include <stdlib.h>
#include <string>
#include <iostream>

#include "yum.h"
#include "forest-client.h"

void Yum::getAvailableUpdates(vector<string> & outList)
{
	string command;
	int commandRetval = 0;

	command = "yum check-update -q -C 2>&1";

	mySystem(&command, outList, &commandRetval);

	for(int i = outList.size() - 1; i >= 0; i--)
	{
		//filter out empty lines
		//updates=`echo "${updates}" | grep -v " \* \|^$"`
		//filter out wait messages when other instances of yum are running
		//updates=`echo "${updates}" | grep -v "^Another app\|^Existing lock"`

		//filter out HTTP errors when a mirror is down
		//updates=`echo "${updates}" | grep -v "HTTP Error\|^Trying other mirror.$"`
		if(trim_string(outList[i]).size() == 0 || 
			outList[i].substr(0, 11) == "Another app" ||
			outList[i].substr(0, 13) == "Existing lock" ||
			outList[i].find("HTTP Error", 0) != string::npos ||
			outList[i] == "Trying other mirror.")
		{
			// remove this line and don't process it any further
			outList.erase(outList.begin() + i);
			continue;
		}

		string::size_type pos = outList[i].find(' ', 0);
		if(pos != string::npos)
		{
			//keep everything up to the first space (package name and arch)
			//updates=`echo "${updates}" | cut -d " " -f 1`
			outList[i] = outList[i].substr(0, pos - 1);
		}

		pos = outList[i].rfind('.', string::npos);
		if(pos != string::npos)
		{
			//remove the architecture (.i386, .x86_64, etc)
			//updates=`echo "${updates}" | sed 's/\(.*\)\..*/\1/'`
			outList[i] = outList[i].substr(0, pos - 1);
		}
	}
	for(int i = outList.size() - 1; i >= 0; i--)
	{
		//remove duplicates from the list (produced by removing architecture)
		//the list may need to be sorted before running uniq
		//updates=`echo "${updates}" | uniq`
		for(int dupIndex = i - 1; dupIndex >= 0; dupIndex--)
		{
			if(outList[i] == outList[dupIndex])
			{
				outList.erase(outList.begin() + i);
			}
		}
	}

	// yum check-update returns 0 if no updates are available or 100 if 
	// there is at least one update available
	if(commandRetval == 0)
	{
		outList.clear();
	}
	else if(commandRetval == 100)
	{
		int lineLen;
		string tempLine;
		for(size_t lineNum = 0; lineNum < outList.size(); lineNum++)
		{
			string tempstring = outList[lineNum];
			lineLen = tempstring.length();
			// only keep everything up to the first '.'
			string::size_type position = tempstring.find(".", 0);
			if(position != string::npos)
			{
				outList[lineNum] = tempstring.substr(0, position);
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
