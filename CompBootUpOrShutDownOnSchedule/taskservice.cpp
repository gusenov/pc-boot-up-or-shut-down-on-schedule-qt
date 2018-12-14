#include "taskservice.h"
#include "comstuff.h"
#include <cstdio>
#include <comdef.h>
#include <QDebug>
#include <QtGlobal>

// Переменная для хранения результатов вызова функций:
HRESULT TaskService::hr;

// Класс предоставляющий доступ к сервису планировщика заданий:
TaskService::TaskService()
{
//    qDebug() << "TaskService::TaskService()";

//    ComStuff::instance();

    // Создать экземпляр сервиса для доступа к планировщику заданий:
    pService = TaskService::createInstanceOfTaskService();
    Q_ASSERT(pService != nullptr);

    // Подключиться к сервису планировщика заданий:
    TaskService::connectToTaskService(pService);

    // Получить указатель на папку для хранения заданий:
    pRootFolder = TaskService::getPtrToRootTaskFolder(pService);
    Q_ASSERT(pRootFolder != nullptr);
}

// Деструктор:
TaskService::~TaskService()
{
//    if (pRootFolder != nullptr)
//    {
//        pRootFolder->Release();
//    }

//    if (pService != nullptr)
//    {
//        pService->Release();
//    }
}

// Реализация шаблона проектирования Синглтон (одиночка) для гарантии, что
// в приложении будет единственный экземпляр этого класса:
TaskService& TaskService::instance()
{
    static TaskService taskService;
    return taskService;
}

// Создать экземпляр сервиса для доступа к планировщику заданий:
ITaskService* TaskService::createInstanceOfTaskService()
{
    ITaskService* pService = nullptr;

    hr = CoCreateInstance(
        CLSID_TaskScheduler,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_ITaskService,
        (void**)&pService
    );

    if (FAILED(hr))
    {
        qDebug() << QString("Failed to create an instance of ITaskService: %1").arg(hr, 0, 16);
        return nullptr;
    }

    return pService;
}

// Подключиться к сервису планировщика заданий:
int TaskService::connectToTaskService(ITaskService* pService)
{
    hr = pService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());

    if( FAILED(hr) )
    {
        qDebug() << QString("ITaskService::Connect failed: %1").arg(hr, 0, 16);
        return 1;
    }

    return 0;
}

// Получить указатель на папку для хранения заданий:
ITaskFolder* TaskService::getPtrToRootTaskFolder(ITaskService* pService)
{
    ITaskFolder* pRootFolder = nullptr;

    hr = pService->GetFolder(_bstr_t( L"\\"), &pRootFolder);

    if (FAILED(hr))
    {
        qDebug() << QString("Cannot get Root folder pointer: %1").arg(hr, 0, 16);
        return nullptr;
    }

    return pRootFolder;
}

// Удалить задание:
void TaskService::delTask(LPCWSTR wszTaskName)
{
    pRootFolder->DeleteTask(_bstr_t(wszTaskName), 0);
}

// Создать задание:
ITaskDefinition* TaskService::createTaskDefinitionObject()
{
    ITaskDefinition* pTask = nullptr;

    hr = pService->NewTask(0, &pTask);

    if (FAILED(hr))
    {
        qDebug() << QString("Failed to CoCreate an instance of the TaskService class: %1").arg(hr, 0, 16);
        return nullptr;
    }

    return pTask;
}

// Сохранить задание в корневой папке предназначенной для хранения заданий
// (как правило это C:\Windows\System32\Tasks\):
IRegisteredTask* TaskService::saveTaskInRootFolder(LPCWSTR wszTaskName, ITaskDefinition *pTask)
{
    IRegisteredTask *pRegisteredTask = nullptr;

    hr = pRootFolder->RegisterTaskDefinition(
        _bstr_t(wszTaskName),
        pTask,
        TASK_CREATE_OR_UPDATE,
        _variant_t(),
        _variant_t(),
        TASK_LOGON_INTERACTIVE_TOKEN,
        _variant_t(L""),
        &pRegisteredTask
    );

    if (FAILED(hr))
    {
        qDebug() << QString("Error saving the Task: %1").arg(hr, 0, 16);
        return nullptr;
    }

    qDebug() << "Success! Task successfully registered.";

    return pRegisteredTask;
}
