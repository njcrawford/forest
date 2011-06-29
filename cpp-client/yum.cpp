#include <stdlib.h>
#include <string>
#include <iostream>

#include "yum.h"
#include "forest-client.h"

int Yum::getAvailableUpdates(vector<string> * outList)
{
	string command;
	int commandRetval = 0;

	//TODO: yum output will need further cleanup
	command = "yum check-update -q -C 2>&1";

	mySystem(&command, outList, &commandRetval);

	for(int i = outList->size(); i >= 0; i--)
	{
		//filter out empty lines
		//updates=`echo "${updates}" | grep -v " \* \|^$"`
		//filter out wait messages when other instances of yum are running
		//updates=`echo "${updates}" | grep -v "^Another app\|^Existing lock"`

		//filter out HTTP errors when a mirror is down
		//updates=`echo "${updates}" | grep -v "HTTP Error\|^Trying other mirror.$"`
		if(trim_string(outList->at(i)) == "" || 
			outList->at(i).substr(0, 11) == "Another app" ||
			outList->at(i).substr(0, 13) == "Existing lock" ||
			outList->at(i).find("HTTP Error", 0) != string::npos ||
			outList->at(i) == "Trying other mirror.")
		{
			// remove this line and don't process it any further
			outList->erase(outList->begin() + i);
			continue;
		}

		string::size_type pos = outList->at(i).find(' ', 0);
		//keep everything up to the first space (package name and arch)
		//updates=`echo "${updates}" | cut -d " " -f 1`
		outList->assign(i, outList->at(i).substr(0, pos - 1));

		pos = outList->at(i).rfind('.', string::npos);
		//remove the architecture (.i386, .x86_64, etc)
		//updates=`echo "${updates}" | sed 's/\(.*\)\..*/\1/'`
		outList->assign(i, outList->at(i).substr(0, pos - 1));
	}
	for(int i = outList->size(); i >= 0; i--)
	{
		//remove duplicates from the list (produced by removing architecture)
		//the list may need to be sorted before running uniq
		//updates=`echo "${updates}" | uniq`
		for(int j = i; i >= 0; j--)
		{
			if(outList->at(i) == outList->at(j))
			{
				outList->erase(outList->begin() + i);
			}
		}
	}

	// yum check-update returns 0 if no updates are available or 100 if 
	// there is at least one update available
	if(commandRetval == 0)
	{
		outList->clear();
	}
	else if(commandRetval == 100)
	{
		int lineLen;
		string tempLine;
		for(size_t lineNum = 0; lineNum < outList->size(); lineNum++)
		{
			string tempstring = outList->at(lineNum);
			lineLen = tempstring.length();
			// only keep everything up to the first '.'
			string::size_type position = tempstring.find(".", 0);
			if(position != string::npos)
			{
				outList->assign(lineNum, tempstring.substr(0, position));
			}
		}
	}
}

int Yum::applyUpdates(vector<string> * list)
{
	string command;
	int commandResponse;
	vector<string> commandOutput;	

	//TODO: yum output will need further cleanup
	command = "yum -y update ";
	command += flattenStringList(list, ' ');
	command += " 2>&1";

	mySystem(&command, &commandOutput, &commandResponse);

	
	if(commandResponse != 0)
	{
		cerr << "Error in applyUpdates: Package manager failed to apply updates:\n";
		cerr << flattenStringList(&commandOutput, ' ');
		exit(1);
	}
}
