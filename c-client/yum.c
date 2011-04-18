#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lineList.h"

int getAvailableUpdatesYum(lineList* outList)
{
	char * command;
	int commandRetval = 0;

	//TODO: yum output will need further cleanup
	command = "yum check-update -q -C 2>&1";

	mySystem(command, outList, &commandRetval);

	// yum check-update returns 0 if no updates are available or 100 if 
	// there is at least one update available
	if(commandRetval == 0)
	{
		clearStringList(outList);
	}
	else if(commandRetval == 100)
	{
		int lineNum;
		int lineLen;
		int letterNum;
		char * tempLine;
		for(lineNum = 0; lineNum < outList->lineCount; lineNum++)
		{
			lineLen = strlen(outList->lines[lineNum]);
			// only keep everything up to the first '.'
			for(letterNum = 0; letterNum < lineLen; letterNum++)
			{
				if(outList->lines[lineNum][letterNum] == '.')
				{
					// null terminate so we can use strcpy
					outList->lines[lineNum][letterNum] = '\0';

					// malloc space for shortened line
					tempLine = malloc(sizeof(char) * letterNum);

					// copy the shortened line into the new space
					strcpy(tempLine, outList->lines[lineNum]);

					// free the old line
					free(outList->lines[lineNum]);

					// put the shortened line into the list where the original was
					outList->lines[lineNum] = tempLine;

					// don't process this line any further
					break;
				}
			}
		}
	}
}

int applyUpdatesYum(lineList* list)
{
	char * command = NULL;
	int i;
	int remainingBuf;
	int commandResponse;
	lineList commandOutput;	
	char * flattenedOutput = NULL;
	char * temp = NULL;

	memset(&commandOutput, 0, sizeof(lineList));

	//TODO: yum output will need further cleanup
	command = "yum -y update ";

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
