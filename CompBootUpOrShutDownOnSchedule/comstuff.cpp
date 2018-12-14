#include "comstuff.h"
#include <cstdio>
#include <QDebug>

// Конструктор:
ComStuff::ComStuff()
{
//    qDebug() << "Com::Com()";

    init();
    setGeneralSecurityLevels();
}

// Деструктор:
ComStuff::~ComStuff()
{
    CoUninitialize();
}

// Реализация шаблона проектирования Синглтон (одиночка) для гарантии, что
// в приложении будет единственный экземпляр этого класса:
ComStuff& ComStuff::instance()
{
    static ComStuff comStuff;
    return comStuff;
}

// Инициализация COM-библиотеки для использования в вызывающем потоке:
int ComStuff::init()
{
    hr = CoInitializeEx(
        nullptr,
        COINIT_APARTMENTTHREADED  // модель многопоточности.
    );

    if (FAILED(hr))
    {
        qDebug() << QString("CoInitializeEx failed: %1").arg(hr, 0, 16);
        return 1;
    }

    // Если нужно использовать Windows Runtime API и COM,
    // то нужно вызывать Windows::Foundation::Initialize.

    return 0;
}

// Установка для процесса значений касающихся безопасности:
int ComStuff::setGeneralSecurityLevels()
{
    hr = CoInitializeSecurity(
        nullptr,
        -1,
        nullptr,
        nullptr,
        RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        nullptr,
        0,
        nullptr
    );

    if (FAILED(hr))
    {
        qDebug() << QString("CoInitializeSecurity failed: %1").arg(hr, 0, 16);
        return 1;
    }

    return 0;
}
