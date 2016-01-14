#ifndef _REBOOTSTUB_H
#define _REBOOTSTUB_H

#include "RebootManager.h"

class RebootStub : public RebootManager
{
public:
    RebootState isRebootNeeded() override;
};

#endif
