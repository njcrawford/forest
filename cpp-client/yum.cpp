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
		for(int lineNum = 0; lineNum < outList->size(); lineNum++)
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
