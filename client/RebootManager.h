#ifndef _REBOOTMANAGER_H
#define _REBOOTMANAGER_H

#include "foresttypes.h"

class RebootManager
{
public:
    virtual rebootState isRebootNeeded() = 0;
    virtual void applyReboot();
    virtual bool canApplyReboot();
};

#endif
