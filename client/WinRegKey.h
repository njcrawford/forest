#ifndef _WINREGKEY_H
#define _WINREGKEY_H

#include "RebootManager.h"

class WinRegKey : public RebootManager
{
public:
    RebootState isRebootNeeded() override;
};

#endif
