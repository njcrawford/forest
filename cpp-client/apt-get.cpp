// exit()
#include <stdlib.h>

// cerr
#include <iostream>

#include "apt-get.h"
#include "forest-client.h"

int AptGet::getAvailableUpdates(vector<string> * outList)
{
	string command;
	int commandRetval = 0;

	command = "apt-get dist-upgrade -Vs 2>&1";

	mySystem(&command, outList, &commandRetval);

	if(commandRetval == 0)
	{
		for(int i = outList->size() - 1; i >= 0; i--)
		{
			// grep ^Inst | cut -d " " -f 2
			if(outList->at(i).substr(0, 4) == "Inst")
			{
				string::size_type pos = outList->at(i).find(' ', 0);
				string::size_type len = outList->at(i).find(' ', pos + 1) - pos - 1;
				outList->assign(i, outList->at(i).substr(pos, len));
			}
			else
			{
				outList->erase(outList->begin() + i);
			}
		}
	}
	else
	{
		cerr << "apt-get failed:" << endl << flattenStringList(outList, '\n') << endl;
		exit(10);
	}

}

int AptGet::applyUpdates(vector<string> * list)
{
	string command;
	int commandResponse;
	vector<string> commandOutput;	

	command = "apt-get -y -o DPkg::Options::\\=--force-confold install ";
	cerr << command << endl;
	command += flattenStringList(list, ' ');
	cerr << command << endl;
	command += " 2>&1";
	cerr << command << endl;

	mySystem(&command, &commandOutput, &commandResponse);
	
	if(commandResponse != 0)
	{
		cerr << "Error in applyUpdates: Package manager failed to apply updates:\n"; 
		cerr << flattenStringList(&commandOutput, ' ');
		exit(1);
	}
}