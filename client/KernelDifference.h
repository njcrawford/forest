#ifndef _KERNELDIFFERENCE_H
#define _KERNELDIFFERENCE_H

#include "UnixShutdown.h"

class KernelDifference : public UnixShutdown 
{
public:
    RebootState isRebootNeeded() override;
};

#endif
