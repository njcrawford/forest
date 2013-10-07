#include <string>
#include <vector>
#include <iostream>
using namespace std;
#include <string.h>

#include "../aptcli.h"
#include "../yum.h"
#include "../MacSU.h"

// Takes stdin and converts it to the format expected by package managers
vector<string> readInput()
{
	vector<string> retval;
	string line;
	while (getline(cin, line))
	{
		retval.push_back(line);
	}

	return retval;
}

// Takes list of updates and prints it to stdout
void printResults(vector<updateInfo> & results)
{
	for(size_t i = 0; i < results.size(); i++)
	{
		cout << "Package: " << results[i].name << ", version: " << results[i].version << endl;
	}
}

void testApt()
{
	AptCli * testObj = new AptCli();

	vector<updateInfo> updates;
	vector<string> testInput = readInput();

	testObj->parseUpdates(updates, testInput);

	printResults(updates);

	delete testObj;
}

void testYum()
{
	Yum * testObj = new Yum();

	vector<updateInfo> updates;
	vector<string> testInput = readInput();

	testObj->parseUpdates(updates, testInput);

	printResults(updates);

	delete testObj;
}

void testMac()
{

}

int main(int argc, char** args)
{
	if(argc != 2)
	{
		cout << "Must have one argument to specify class under test" << endl;
		return 1;
	}

	if(strcmp(args[1], "--yum") == 0)
	{
		testYum();
	}
	else if(strcmp(args[1], "--apt") == 0)
	{
		testApt();
	}
	else if(strcmp(args[1], "--mac") == 0)
	{
		testMac();
	}
	else
	{
		cout << "Unrecognized argument '" << args[1] << "'" << endl;
		return 1;
	}

	return 0;
}
