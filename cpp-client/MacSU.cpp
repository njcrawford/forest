// exit()
#include <stdlib.h>

// cerr
#include <iostream>

#include "MacSU.h"
#include "forest-client.h"

void MacSU::getAvailableUpdates(vector<updateInfo> & outList)
{
	string command;
	int commandRetval = 0;
	vector<string> commandOutput;

	command = "/usr/sbin/softwareupdate --list 2>&1";

	mySystem(&command, commandOutput, &commandRetval);

	if(commandRetval == 0)
	{
		for(size_t i = 0; i < commandOutput.size(); i++)
		{
			// example of one update output
			//    * iTunesX-10.4.1
			//	iTunes (10.4.1), 90665K [recommended]

			// use lines with update name and version mixed together
			if(commandOutput[i].find('*') != string::npos)
			{
				string::size_type pos = commandOutput[i].find('*') + 2;
				//string::size_type len = commandOutput[i].find('-', pos + 1) - pos;
				updateInfo temp;
				temp.name = commandOutput[i].substr(pos);
				pos = commandOutput[i].find('-');
				temp.version = commandOutput[i].substr(pos + 1);
				outList.push_back(temp);
			}
		}
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
