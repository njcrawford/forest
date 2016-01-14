#ifndef _REBOOTMANAGER_H
#define _REBOOTMANAGER_H

enum class RebootState { Unknown = 0, Yes = 1, No = 2 };

class RebootManager
{
public:
    virtual RebootState isRebootNeeded() = 0;
    virtual void applyReboot();
    virtual bool canApplyReboot();
};

#endif
