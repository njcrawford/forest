#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lineList.h"

int getAvailableUpdatesAptGet(lineList* outList)
{
	char * command;
	int commandRetval = 0;

	command = "apt-get dist-upgrade -Vs 2>&1";

	mySystem(command, outList, &commandRetval);

}

int applyUpdatesAptGet(lineList* list)
{
	char * command = NULL;
	int i;
	int remainingBuf;
	int commandResponse;
	lineList commandOutput;	
	char * flattenedOutput = NULL;
	char * temp = NULL;

	memset(&commandOutput, 0, sizeof(lineList));

	command = "apt-get -y -o DPkg::Options::\\=--force-confold install ";

	flattenedOutput = flattenStringList(list, ' ');
	temp = malloc(sizeof(char) * (strlen(command) + strlen(flattenedOutput) + 5));
	sprintf(temp, "%s%s 2>&1", command, flattenedOutput);
	command = temp;

	free(flattenedOutput);
	flattenedOutput = NULL;

	mySystem(command, &commandOutput, &commandResponse);

	free(command);
	command = NULL;
	
	if(commandResponse != 0)
	{
		flattenedOutput = flattenStringList(&commandOutput, ' ');
		fprintf(stderr, "Error in applyUpdates: Package manager failed to apply updates:\n%s", flattenedOutput);
		free(flattenedOutput);
		exit(1);
	}

	clearStringList(&commandOutput);
}
