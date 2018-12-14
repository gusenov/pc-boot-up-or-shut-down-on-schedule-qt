#ifndef WAITABLETIMER_H
#define WAITABLETIMER_H


#include <windows.h>

class WaitableTimer
{
public:
    WaitableTimer();
    static BOOL WINAPI HibernateAndReboot(int secs);
};

#endif // WAITABLETIMER_H
