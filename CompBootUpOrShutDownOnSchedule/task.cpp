#include "task.h"
#include <comdef.h>
#include <QDebug>
#include <QtGlobal>

// Переменная для хранения результатов вызова функций:
HRESULT Task::hr = 0;

GUID Task::IID_IShowMessageAction = {0};

// Конструктор:
Task::Task(
        TASK_ACTION_TYPE type,  // тип задания.
        LPCWSTR wszTaskName,  // наименование задания.
        wstring wstrExecutablePath,  // путь к запускаемой программе.
        wstring args,  // аргументы запускаемой программы.
        LPCWSTR wszAuthorName,  // имя автора.
        LPCWSTR startDateTime,  // дата начала.
//        LPCWSTR endDateTime,  // дата конца.
        bool isWake  // пробуждать компьютер для выполнения задания?
)
{
    // Сервис планировщика заданий:
    taskService = &TaskService::instance();

    // Удалить задание, если такое уже существует:
    taskService->delTask(wszTaskName);

    // Сохранить задание:
    pTask = taskService->createTaskDefinitionObject();
    Q_ASSERT(pTask != nullptr);


    // Получить регистрационную информацию о задании:
    pRegInfo = Task::getRegistrationInfo(pTask);
    Q_ASSERT(pRegInfo != nullptr);

    // Установить имя автора задания:
    Task::setAuthorName(pRegInfo, wszAuthorName);


    // Получить параметры безопасности для задания:
    pPrincipal = Task::getPrincipalForTask(pTask);
    Q_ASSERT(pPrincipal != nullptr);

    // Установить тип входа:
    Task::setUpPrincipalLogonType(pPrincipal, TASK_LOGON_INTERACTIVE_TOKEN);


    // Получить настройки для задания:
    pSettings = Task::getSettingsForTask(pTask);
    Q_ASSERT(pSettings != nullptr);

    // Пробуждать компьютер для выполнения задания?
    pSettings->put_WakeToRun(isWake);

    // Установить настройки для задания:
    Task::setSettingValuesForTask(pSettings);


    // Получить настройки простоя для задания:
//    pIdleSettings = Task::getIdleSettingsForTask(pSettings);
//    Q_ASSERT(pIdleSettings != nullptr);

    // Установить настройки простоя для задания:
//    Task::setIdleSettingsForTask(pIdleSettings);


    // Получить коллекцию триггеров:
    pTriggerCollection = Task::getTriggerCollection(pTask);
    Q_ASSERT(pTriggerCollection != nullptr);

    // Создать триггер:
    pTrigger = createTrigger(pTriggerCollection);
    Q_ASSERT(pTrigger != nullptr);

    // Получить интерфейс временного триггера:
    pTimeTrigger = Task::qiTimeTrigger(pTrigger);
    Q_ASSERT(pTimeTrigger != nullptr);

    // Установить идентификатор триггера:
    Task::putTriggerId(pTimeTrigger, L"Trigger1");

    // Установить время начала задания:
    Task::setTaskStartAtCertainTime(pTimeTrigger, startDateTime);

    // Установить время конца задания:
//    Task::setTaskEndAtCertainTime(pTimeTrigger, endDateTime);


    // Получить коллекцию действий:
    pActionCollection = Task::getTheTaskActionCollectionPointer(pTask);
    Q_ASSERT(pActionCollection != nullptr);

    // Если тип задания равен заданию на запуск программы:
    if (type == TASK_ACTION_EXEC)
    {
        // Создать действие:
        pAction = Task::createAction(pActionCollection, TASK_ACTION_EXEC);
        Q_ASSERT(pAction != nullptr);

        // Получить интерфейс действия на запуск программы:
        pExecAction = Task::qiForExecTaskPtr(pAction);
        Q_ASSERT(pExecAction != nullptr);

        // Установить путь к запускаемой программе:
        Task::setPathOfExecutable(pExecAction, wstrExecutablePath);

        if (args.length() > 0)
        {
            // Установить аргументы для запускаемой программы:
            Task::setArgsOfExec(pExecAction, args);
        }
    }

    // В другом случае считаем, что тип задания равен заданию на отображение окна сообщения:
    else
    {
        // Создать действие:
        pAction = Task::createAction(pActionCollection, TASK_ACTION_SHOW_MESSAGE);
        Q_ASSERT(pAction != nullptr);

        // Получить интерфейс действия на отображение окна сообщения:
        CLSIDFromString(L"{505E9E68-AF89-46B8-A30F-56162A83D537}", &IID_IShowMessageAction);
        pShowMsgAction = Task::qiForShowMsgPtr(pAction);
        Q_ASSERT(pShowMsgAction != nullptr);

        // Установить заголовок сообщения:
        Task::setMsgTitle(pShowMsgAction, wstrExecutablePath);

        // Установить текст сообщения:
        Task::setMsgBody(pShowMsgAction, args);
    }

    // Зарегистрированное задание:
    pRegisteredTask = taskService->saveTaskInRootFolder(wszTaskName, pTask);
    Q_ASSERT(pRegisteredTask != nullptr);
}

Task::~Task()
{
    // Освобождение памяти:


    // Зарегистрированное задание:
    if (pRegisteredTask != nullptr) pRegisteredTask->Release();


    // Действие на отображение окна сообщения:
    if (pShowMsgAction != nullptr) pShowMsgAction->Release();

    // Действие на запуск программы:
    if (pExecAction != nullptr) pExecAction->Release();

    // Действие:
    if (pAction != nullptr) pAction->Release();

    // Коллекция действий:
    if (pActionCollection != nullptr) pActionCollection->Release();


    // Триггер по времени:
    if (pTimeTrigger != nullptr) pTimeTrigger->Release();

    // Триггер:
    if (pTrigger != nullptr) pTrigger->Release();

    // Коллекция триггеров:
    if (pTriggerCollection != nullptr) pTriggerCollection->Release();


    // Настройки простоя:
    if (pIdleSettings != nullptr) pIdleSettings->Release();

    // Настройки задания:
    if (pSettings != nullptr) pSettings->Release();


    // Параметры безопасности:
    if (pPrincipal != nullptr) pPrincipal->Release();


    // Регистрационная информация о задании (имя автора и пр.):
    if (pRegInfo != nullptr) pRegInfo->Release();


    // Задание:
    if (pTask != nullptr) pTask->Release();
}


// Получить регистрационную информацию о задании:
IRegistrationInfo* Task::getRegistrationInfo(ITaskDefinition* pTask)
{
    IRegistrationInfo* pRegInfo = nullptr;

    hr = pTask->get_RegistrationInfo(&pRegInfo);
    if (FAILED(hr))
    {
        qDebug() << QString("\nCannot get identification pointer: %1").arg(hr, 0, 16);
        return nullptr;
    }

    return pRegInfo;
}

// Установить имя автора задания:
int Task::setAuthorName(IRegistrationInfo* pRegInfo, LPCWSTR authorName)
{
    hr = pRegInfo->put_Author(_bstr_t(authorName));

    if (FAILED(hr))
    {
        qDebug() << QString("\nCannot put identification info: %1").arg(hr, 0, 16);
        return 1;
    }

    return 0;
}


// Получить параметры безопасности для задания:
IPrincipal* Task::getPrincipalForTask(ITaskDefinition* pTask)
{
    IPrincipal* pPrincipal = nullptr;

    hr = pTask->get_Principal(&pPrincipal);

    if (FAILED(hr))
    {
        qDebug() << QString("\nCannot get principal pointer: %1").arg(hr, 0, 16);
        return nullptr;
    }

    return pPrincipal;
}

// Установить тип входа:
int Task::setUpPrincipalLogonType(IPrincipal* pPrincipal, TASK_LOGON_TYPE logonType)
{
    hr = pPrincipal->put_LogonType(logonType);

    if (FAILED(hr))
    {
        qDebug() << QString("\nCannot put principal info: %1").arg(hr, 0, 16);
        return 1;
    }

    return 0;
}


 // Получить настройки для задания:
ITaskSettings* Task::getSettingsForTask(ITaskDefinition *pTask)
{
    ITaskSettings* pSettings = nullptr;

    hr = pTask->get_Settings(&pSettings);

    if (FAILED(hr))
    {
        qDebug() << QString("Cannot get settings pointer: %1").arg(hr, 0, 16);
        return nullptr;
    }

    return pSettings;
}

// Установить настройки для задания:
int Task::setSettingValuesForTask(ITaskSettings* pSettings)
{
    hr = pSettings->put_StartWhenAvailable(VARIANT_TRUE);

    if (FAILED(hr))
    {
        qDebug() << QString("Cannot put setting information: %1").arg(hr, 0, 16);
        return 1;
    }

    return 0;
}


// Получить настройки простоя для задания:
IIdleSettings* Task::getIdleSettingsForTask(ITaskSettings *pSettings)
{
    IIdleSettings* pIdleSettings = nullptr;

    hr = pSettings->get_IdleSettings(&pIdleSettings);

    if (FAILED(hr))
    {
        qDebug() << QString("Cannot get idle setting information: %1").arg(hr, 0, 16);
        return nullptr;
    }

    return pIdleSettings;
}

// Установить настройки простоя для задания:
int Task::setIdleSettingsForTask(IIdleSettings* pIdleSettings)
{
    hr = pIdleSettings->put_WaitTimeout(_bstr_t(L"PT5M"));

    if (FAILED(hr))
    {
        qDebug() << QString("Cannot put idle setting information: %1").arg(hr, 0, 16);
        return 1;
    }

    return 0;
}


// Получить коллекцию триггеров:
ITriggerCollection* Task::getTriggerCollection(ITaskDefinition* pTask)
{
    ITriggerCollection* pTriggerCollection = nullptr;

    hr = pTask->get_Triggers(&pTriggerCollection);

    if( FAILED(hr) )
    {
        qDebug() << QString("Cannot get trigger collection: %1").arg(hr, 0, 16);
        return nullptr;
    }

    return pTriggerCollection;
}

// Создать триггер:
ITrigger* Task::createTrigger(ITriggerCollection* pTriggerCollection)
{
    ITrigger* pTrigger = nullptr;

    hr = pTriggerCollection->Create(TASK_TRIGGER_TIME, &pTrigger);

    if (FAILED(hr))
    {
        qDebug() << QString("Cannot create trigger: %1").arg(hr, 0, 16);
        return nullptr;
    }

    return pTrigger;
}

// Установить идентификатор триггера:
int Task::putTriggerId(ITimeTrigger* pTimeTrigger, LPCWSTR id)
{
    hr = pTimeTrigger->put_Id(_bstr_t(id));

    if (FAILED(hr))
    {
        qDebug() << QString("Cannot put trigger ID: %1").arg(hr, 0, 16);
    }

    return 0;

}


// Получить интерфейс временного триггера:
ITimeTrigger* Task::qiTimeTrigger(ITrigger* pTrigger)
{
    ITimeTrigger* pTimeTrigger = nullptr;

    hr = pTrigger->QueryInterface(IID_ITimeTrigger, (void**)&pTimeTrigger);

    if (FAILED(hr))
    {
        qDebug() << QString("QueryInterface call failed for ITimeTrigger: %1").arg(hr, 0, 16);
        return nullptr;
    }

    return pTimeTrigger;

}

// Установить время начала задания:
int Task::setTaskStartAtCertainTime(ITimeTrigger* pTimeTrigger, LPCWSTR dateTime)
{
    hr = pTimeTrigger->put_StartBoundary(_bstr_t(dateTime));

    if (FAILED(hr))
    {
        qDebug() << QString("Cannot add start boundary to trigger: %1").arg(hr, 0, 16);
        return 1;
    }

    return 0;
}

// Установить время конца задания:
int Task::setTaskEndAtCertainTime(ITimeTrigger* pTimeTrigger, LPCWSTR dateTime)
{
    hr = pTimeTrigger->put_EndBoundary(_bstr_t(dateTime));

    if (FAILED(hr))
    {
        qDebug() << QString("Cannot put end boundary on trigger: %1").arg(hr, 0, 16);
        return 1;
    }

    return 0;
}


// Получить коллекцию действий:
IActionCollection* Task::getTheTaskActionCollectionPointer(ITaskDefinition* pTask)
{
    IActionCollection *pActionCollection = nullptr;

    hr = pTask->get_Actions(&pActionCollection);

    if (FAILED(hr))
    {
        qDebug() << QString("Cannot get Task collection pointer: %1").arg(hr, 0, 16);
        return nullptr;
    }

    return pActionCollection;
}

// Создать действие:
IAction* Task::createAction(IActionCollection* pActionCollection, TASK_ACTION_TYPE type)
{
    IAction* pAction = nullptr;

    hr = pActionCollection->Create(type, &pAction);

    if (FAILED(hr))
    {
        qDebug() << QString("Cannot create the action: %1").arg(hr, 0, 16);
        return nullptr;
    }

    return pAction;
}


// Получить интерфейс действия на запуск программы:
IExecAction* Task::qiForExecTaskPtr(IAction* pAction)
{
    IExecAction* pExecAction = nullptr;

    hr = pAction->QueryInterface(IID_IExecAction, (void**)&pExecAction);

    if (FAILED(hr))
    {
        qDebug() << QString("QueryInterface call failed for IExecAction: %1").arg(hr, 0, 16);
        return nullptr;
    }

    return pExecAction;
}

// Установить путь к запускаемой программе:
int Task::setPathOfExecutable(IExecAction* pExecAction, wstring wstrExecutablePath)
{
    hr = pExecAction->put_Path(_bstr_t(wstrExecutablePath.c_str()));

    if (FAILED(hr))
    {
        qDebug() << QString("Cannot put action path: %1").arg(hr, 0, 16);
        return 1;
    }

    return 0;
}

// Установить аргументы для запускаемой программы:
int Task::setArgsOfExec(IExecAction* pExecAction, wstring args)
{
    hr = pExecAction->put_Arguments(_bstr_t(args.c_str()));

    if (FAILED(hr))
    {
        qDebug() << QString("Cannot put action arguments: %1").arg(hr, 0, 16);
        return 1;
    }

    return 0;
}


// Получить интерфейс действия на отображение окна сообщения:
IShowMessageAction* Task::qiForShowMsgPtr(IAction* pAction)
{
    IShowMessageAction* pShowMsgAction = nullptr;

    hr = pAction->QueryInterface(IID_IShowMessageAction, (void**)&pShowMsgAction);

    if (FAILED(hr))
    {
        qDebug() << QString("QueryInterface call failed for IExecAction: %1").arg(hr, 0, 16);
        return nullptr;
    }

    return pShowMsgAction;
}

// Установить заголовок сообщения:
int Task::setMsgTitle(IShowMessageAction* pShowMsgAction, wstring title)
{
    hr = pShowMsgAction->put_Title(_bstr_t(title.c_str()));

    if (FAILED(hr))
    {
        qDebug() << QString("Cannot put show message action title: %1").arg(hr, 0, 16);
        return 1;
    }

    return 0;
}

// Установить текст сообщения:
int Task::setMsgBody(IShowMessageAction* pShowMsgAction, wstring msg)
{
    hr = pShowMsgAction->put_MessageBody(_bstr_t(msg.c_str()));

    if (FAILED(hr))
    {
        qDebug() << QString("Cannot put show message action body: %1").arg(hr, 0, 16);
        return 1;
    }

    return 0;

}
