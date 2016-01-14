#ifndef _UNIXSHUTDOWN_H
#define _UNIXSHUTDOWN_H

#include "RebootManager.h"

class UnixShutdown : public RebootManager
{
public:
    virtual RebootState isRebootNeeded() override = 0;
    void applyReboot() override;
    bool canApplyReboot() override;
};

#endif
