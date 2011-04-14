#include <stdlib.h>
#include <string.h>

#include "lineList.h"

// adds newItem to the end of list
void pushToStringList(lineList* list, char * newItem)
{
	char** tempList = NULL;
	int i;

	// malloc new list with enough space for old list plus newItem
	tempList = malloc(sizeof(char*) * (list->lineCount + 1));

	// copy pointers from old list to new list
	for(i = 0; i < list->lineCount; i++)
	{
		tempList[i] = list->lines[i];
	}

	// malloc new item
	tempList[list->lineCount] = malloc(sizeof(char) * (strlen(newItem) + 1));

	// copy new item into list
	strcpy(tempList[list->lineCount], newItem);

	// free the old list
	free(list->lines);

	// assign the new list in the old one's place
	list->lines = tempList;

	// increment the line count
	list->lineCount++;
}

// empties list, freeing memory used by lines and reseting lineCount to 0
void clearStringList(lineList * list)
{
	int i;

	for(i = 0; i < list->lineCount; i++)
	{
		free(list->lines[i]);
	}

	list->lineCount = 0;
}

// Returns the strings in list as a single string seperated by seperator.
// Every line in the list will be followed by a seperator, including the last line.
// The list is malloc'd and so must be free'd when finished.
char * flattenStringList(lineList * list, char seperator)
{
	char * retval = NULL;
	int i;
	int sizeCounter = 0;

	for(i = 0; i < list->lineCount; i++)
	{
		sizeCounter += strlen(list->lines[i]) + 1;
	}

	retval = malloc(sizeof(char) * sizeCounter);
	retval[0] = '\0';

	for(i = 0; i < list->lineCount; i++)
	{
		strcat(retval, list->lines[i]);
		strncat(retval, &seperator, 1);
	}

	return retval;
}
