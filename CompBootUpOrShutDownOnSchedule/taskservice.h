#ifndef TASKSERVICE_H
#define TASKSERVICE_H


#include <taskschd.h>

// Класс предоставляющий доступ к сервису планировщика заданий:
class TaskService
{

public:

    // Реализация шаблона проектирования Синглтон (одиночка) для гарантии, что
    // в приложении будет единственный экземпляр этого класса:
    static TaskService& instance();

    // Создать задание:
    ITaskDefinition* createTaskDefinitionObject();

    // Сохранить задание в корневой папке предназначенной для хранения заданий
    // (как правило это C:\Windows\System32\Tasks\):
    IRegisteredTask* saveTaskInRootFolder(LPCWSTR wszTaskName, ITaskDefinition *pTask);

    // Удалить задание:
    void delTask(LPCWSTR wszTaskName);

    // Создать экземпляр сервиса для доступа к планировщику заданий:
    static ITaskService* createInstanceOfTaskService();

    // Подключиться к сервису планировщика заданий:
    static int connectToTaskService(ITaskService* pService);

    // Получить указатель на папку для хранения заданий:
    static ITaskFolder* getPtrToRootTaskFolder(ITaskService* pService);

protected:

    // Конструктор:
    TaskService();

    // Деструктор:
    ~TaskService();

    // Переменная для хранения результатов вызова функций:
    static HRESULT hr;

    // Сервис планировщика заданий:
    ITaskService* pService = nullptr;

    // Папка для хранения заданий:
    ITaskFolder* pRootFolder = nullptr;

};

#endif // TASKSERVICE_H
