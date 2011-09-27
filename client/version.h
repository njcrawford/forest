#ifndef _FOREST_VERSION_H
#define _FOREST_VERSION_H

string getForestVersion()
{
	string svnUrl = "$URL$";
	size_t endPos;
	size_t startPos;
	endPos = svnUrl.rfind("/");
	endPos = svnUrl.rfind("/", endPos - 1);
	startPos = svnUrl.rfind("/", endPos - 1);
	return svnUrl.substr(startPos + 1, endPos - startPos - 1);
}

#endif
