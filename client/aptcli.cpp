// exit()
#include <stdlib.h>

// cerr
#include <iostream>

#include "aptcli.h"
#include "forest-client.h"

void AptCli::getAvailableUpdates(vector<updateInfo> & outList)
{
	string command;
	int commandRetval = 0;
	vector<string> commandOutput;

	command = "/usr/bin/apt-get dist-upgrade -Vs 2>&1";

	mySystem(&command, commandOutput, &commandRetval);

	if(commandRetval == 0)
	{
		for(size_t i = 0; i < commandOutput.size(); i++)
		{
			// example of one output line
			// Inst libpam-modules [1.1.1-2ubuntu5] (1.1.1-2ubuntu5.3 Ubuntu:10.04/lucid-updates)

			// grep ^Inst | cut -d " " -f 2
			if(commandOutput[i].substr(0, 4) == "Inst")
			{
				string::size_type pos = commandOutput[i].find(' ', 0);
				string::size_type len = commandOutput[i].find(' ', pos + 1) - pos;
				updateInfo temp;
				temp.name = commandOutput[i].substr(pos, len);
				pos = commandOutput[i].find('[', 0);
				pos = commandOutput[i].find(']', pos + 1);
				pos = commandOutput[i].find('(', pos + 1);
				len = commandOutput[i].find(' ', pos + 1) - pos;
				temp.version = commandOutput[i].substr(pos + 1, len - 1);
				outList.push_back(temp);
			}
		}
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

	command = "aptitude -y -o DPkg::Options::\\=--force-confold safe-upgrade ";
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

bool AptGet::canApplyUpdates()
{
	return true;
}