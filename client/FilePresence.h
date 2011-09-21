#ifndef _FILEPRESENCE_H
#define _FILEPRESENCE_H

#include "UnixShutdown.h"

class FilePresence : public UnixShutdown 
{
public:
    rebootState isRebootNeeded();
};

#endif
