#ifndef WAITABLETIMER_H
#define WAITABLETIMER_H


#include <windows.h>

class WaitableTimer
{

public:

    // Получение разрешений на выключений компьютера:
    static BOOL WINAPI _EnableShutDownPriv();

    // Выключить компьютер и проснуться через secs секунд:
    static BOOL WINAPI HibernateAndReboot(int secs);

private:

    // Конструктор:
    WaitableTimer();

};

#endif // WAITABLETIMER_H
