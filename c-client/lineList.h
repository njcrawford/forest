#ifndef _LINELIST_H
#define _LINELIST_H
typedef struct lineListStruct
{
	char** lines;
	int lineCount;
} lineList;

void pushToStringList(lineList* list, char * newItem);
void clearStringList(lineList * list);
char * flattenStringList(lineList * list, char seperator);
#endif
