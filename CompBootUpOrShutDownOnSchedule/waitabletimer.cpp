#include "waitabletimer.h"

// Получение разрешений на выключений компьютера:
BOOL WINAPI WaitableTimer::_EnableShutDownPriv()
{
    // Результирующее значение:
    BOOL ret = FALSE;

    // Текущий процесс:
    HANDLE process = GetCurrentProcess();

    // Маркер доступа:
    HANDLE token;

    // Открыть маркер доступа ассоциированный с процессом:
    if (!OpenProcessToken(process, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token))
        return FALSE;  // возвращаем ЛОЖЬ, если не удалось получить маркер доступа.

    // 64-битное значение, которое гарантированно уникально только на системе,
    // где оно сгенерировано:
    LUID luid;

    // Получить LUID для привелегии выключать компьютер:
    LookupPrivilegeValue(nullptr, L"SeShutdownPrivilege", &luid);

    // Настройка привелегий:
    TOKEN_PRIVILEGES priv;
    priv.PrivilegeCount = 1;  // количество привелегий.
    priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;  // включаем привелегию.
    priv.Privileges[0].Luid = luid;  // указываем LUID привелегии.

    // Включаем привелегию для маркера доступа:
    ret = AdjustTokenPrivileges(token, FALSE, &priv, 0, nullptr, nullptr);

    // Закрываем маркер доступа:
    CloseHandle(token);

    // Возвращаем ИСТИНА, если всё хорошо:
    return TRUE;
}

// Функция, которая выполнится, когда сработает таймер:
DWORD CALLBACK ShutDownProc(LPVOID p)
{
    // Указатель на дескриптор:
    PHANDLE timer = (PHANDLE)p;

    // Ждём доступа к таймеру:
    WaitForSingleObject(*timer, INFINITE);

    // Закрываем таймер:
    CloseHandle(*timer);

    // Включить монитор:
    SetThreadExecutionState(ES_DISPLAY_REQUIRED);

    return 0;
}

// Выключить компьютер и проснуться через secs секунд:
BOOL WINAPI WaitableTimer::HibernateAndReboot(int secs)
{
    // Получение прав на выключение компьютера:
    if (!_EnableShutDownPriv())
        return FALSE;

    // Создание таймера пробуждения:
    HANDLE timer = CreateWaitableTimer(nullptr, TRUE, L"MyWaitableTimer");

    // Если получить таймер не удалось, то возвращаем ЛОЖЬ:
    if (timer == nullptr)
        return FALSE;

    // Таймер срабатывает каждые 100 наносекунд.

    __int64 nanoSecs;
    LARGE_INTEGER due;

    // Указываем секунды в отрицательном виде, чтобы задать время относительно
    // момента старта таймера:
    nanoSecs = -secs * 1000 * 1000 * 10;

    // Указываем время в специальной структуре типа FILETIME:
    due.LowPart = (DWORD) (nanoSecs & 0xFFFFFFFF);  // нижняя часть файлового времени.
    due.HighPart = (LONG) (nanoSecs >> 32);  // старшая часть файлового времени.

    // Структура FILETIME - 64-битовое значение,
    // представляющее число интервалов по 100 наносекунд с 1 января 1601
    // (универсальное глобальное время (UTC)).

    // Установка таймера пробуждения:
    if (!SetWaitableTimer(timer, &due, 0, 0, 0, TRUE))
        return FALSE;

    // Если произошла ошибка:
    if (GetLastError() == ERROR_NOT_SUPPORTED)
        return FALSE;

    // Создаем поток:
    HANDLE thread = CreateThread(nullptr, 0, ShutDownProc, &timer, 0, nullptr);

    // Если поток создать не удалось:
    if (!thread)
    {
        CloseHandle(timer);  // закрываем таймер:
        return FALSE;  // возвращаем ЛОЖЬ.
    }

    // Закрываем поток:
    CloseHandle(thread);

    // Выключение компьютера:
    SetSystemPowerState(FALSE, FALSE);

    return TRUE;
}

// Конструктор:
WaitableTimer::WaitableTimer()
{
}
