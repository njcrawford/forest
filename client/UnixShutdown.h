#ifndef _UNIXSHUTDOWN_H
#define _UNIXSHUTDOWN_H

#include "RebootManager.h"

class UnixShutdown : public RebootManager
{
public:
    virtual rebootState isRebootNeeded() = 0;
    void applyReboot();
    bool canApplyReboot();
};

#endif
