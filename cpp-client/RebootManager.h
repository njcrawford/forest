#ifndef _REBOOTMANAGER_H
#define _REBOOTMANAGER_H

enum rebootState { unknown = -1, yes = 1, no = 0 };

class RebootManager
{
public:
    virtual rebootState isRebootNeeded() = 0;
    virtual void applyReboot() = 0;
    bool canApplyReboot();
};

#endif
