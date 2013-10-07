// exit()
#include <stdlib.h>

// cerr
#include <iostream>

#include "MacSU.h"
#include "helpers.h"

void MacSU::parseUpdates(vector<updateInfo> & outList, vector<string> & input)
{
	for(size_t i = 0; i < input.size(); i++)
	{
		// example of one update output
		//    * iTunesX-10.4.1
		//	iTunes (10.4.1), 90665K [recommended]

		// use lines with update name and version mixed together
		if(input[i].find('*') != string::npos)
		{
			string::size_type pos = input[i].find('*') + 2;
			//string::size_type len = input[i].find('-', pos + 1) - pos;
			updateInfo temp;
			temp.name = input[i].substr(pos);
			pos = input[i].find('-');
			temp.version = input[i].substr(pos + 1);
			outList.push_back(temp);
		}
	}
}

void MacSU::getAvailableUpdates(vector<updateInfo> & outList)
{
	string command;
	int commandRetval = 0;
	vector<string> commandOutput;

	command = "/usr/sbin/softwareupdate --list 2>&1";

	mySystem(&command, commandOutput, &commandRetval);

	if(commandRetval == 0)
	{
		parseUpdates(outList, commandOutput);
	}
	else
	{
		cerr << "softwareupdate failed:" << endl << flattenStringList(commandOutput, '\n') << endl;
		exit(10);
	}

}

void MacSU::applyUpdates(vector<string> & list)
{
	string command;
	int commandResponse;
	vector<string> commandOutput;	

	command = "/usr/sbin/softwareupdate --install ";
	cerr << command << endl;
	command += flattenStringList(list, ' ');
	cerr << command << endl;
	command += " 2>&1";
	cerr << command << endl;

	mySystem(&command, commandOutput, &commandResponse);
	
	if(commandResponse != 0)
	{
		cerr << "Error in applyUpdates: Package manager failed to apply updates:\n"; 
		cerr << flattenStringList(commandOutput, ' ');
		exit(1);
	}
}

bool MacSU::canApplyUpdates()
{
	return true;
}
