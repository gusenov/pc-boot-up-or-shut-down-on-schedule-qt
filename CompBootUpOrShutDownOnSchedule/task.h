#ifndef TIMETRIGGER_H
#define TIMETRIGGER_H


#include "taskservice.h"
#include <string>
#include "ishowmessageaction.h"

using namespace std;

// Класс представляющий собой задание для планировщика:
class Task
{

public:

    // Конструктор:
    Task(
            TASK_ACTION_TYPE type,  // тип задания.
            LPCWSTR wszTaskName,  // наименование задания.
            wstring wstrExecutablePath,  // путь к запускаемой программе.
            wstring args,  // аргументы запускаемой программы.
            LPCWSTR wszAuthorName,  // имя автора.
            LPCWSTR startDateTime,  // дата начала.
//            LPCWSTR endDateTime,  // дата конца.
            bool isWake  // пробуждать компьютер для выполнения задания?
    );

    // Деструктор:
    ~Task();


    // Получить регистрационную информацию о задании:
    static IRegistrationInfo* getRegistrationInfo(ITaskDefinition* pTask);

    // Установить имя автора задания:
    static int setAuthorName(IRegistrationInfo* pRegInfo, LPCWSTR authorName);


    // Получить параметры безопасности для задания:
    static IPrincipal* getPrincipalForTask(ITaskDefinition* pTask);

    // Установить тип входа:
    static int setUpPrincipalLogonType(IPrincipal* pPrincipal, TASK_LOGON_TYPE logonType);


    // Получить настройки для задания:
    static ITaskSettings* getSettingsForTask(ITaskDefinition *pTask);

    // Установить настройки для задания:
    static int setSettingValuesForTask(ITaskSettings* pSettings);


    // Получить настройки простоя для задания:
    static IIdleSettings* getIdleSettingsForTask(ITaskSettings *pSettings);

    // Установить настройки простоя для задания:
    static int setIdleSettingsForTask(IIdleSettings* pIdleSettings);


    // Получить коллекцию триггеров:
    static ITriggerCollection* getTriggerCollection(ITaskDefinition* pTask);

    // Создать триггер:
    static ITrigger* createTrigger(ITriggerCollection* pTriggerCollection);

    // Установить идентификатор триггера:
    static int putTriggerId(ITimeTrigger* pTimeTrigger, LPCWSTR id);


    // Получить интерфейс временного триггера:
    static ITimeTrigger* qiTimeTrigger(ITrigger* pTrigger);

    // Установить время начала задания:
    static int setTaskStartAtCertainTime(ITimeTrigger* pTimeTrigger, LPCWSTR dateTime);

    // Установить время конца задания:
    static int setTaskEndAtCertainTime(ITimeTrigger* pTimeTrigger, LPCWSTR dateTime);


    // Получить коллекцию действий:
    static IActionCollection* getTheTaskActionCollectionPointer(ITaskDefinition* pTask);

    // Создать действие:
    static IAction* createAction(IActionCollection* pActionCollection, TASK_ACTION_TYPE type);


    // Получить интерфейс действия на запуск программы:
    static IExecAction* qiForExecTaskPtr(IAction* pAction);

    // Установить путь к запускаемой программе:
    static int setPathOfExecutable(IExecAction* pExecAction, wstring wstrExecutablePath);    

    // Установить аргументы для запускаемой программы:
    static int setArgsOfExec(IExecAction* pExecAction, wstring args);


    // Получить интерфейс действия на отображение окна сообщения:
    static IShowMessageAction* qiForShowMsgPtr(IAction* pAction);

    // Установить заголовок сообщения:
    static int setMsgTitle(IShowMessageAction* pShowMsgAction, wstring title);

    // Установить текст сообщения:
    static int setMsgBody(IShowMessageAction* pShowMsgAction, wstring msg);

protected:

    // Переменная для хранения результатов вызова функций:
    static HRESULT hr;

    static GUID IID_IShowMessageAction;


    // Сервис планировщика заданий:
    TaskService* taskService;


    // Задание:
    ITaskDefinition* pTask = nullptr;

    // Регистрационная информация о задании (имя автора и пр.):
    IRegistrationInfo* pRegInfo = nullptr;

    // Параметры безопасности:
    IPrincipal* pPrincipal = nullptr;

    // Настройки задания:
    ITaskSettings* pSettings = nullptr;

    // Настройки простоя:
    IIdleSettings* pIdleSettings = nullptr;


    // Коллекция триггеров:
    ITriggerCollection* pTriggerCollection = nullptr;

    // Триггер:
    ITrigger* pTrigger = nullptr;

    // Триггер по времени:
    ITimeTrigger* pTimeTrigger = nullptr;


    // Коллекция действий:
    IActionCollection* pActionCollection = nullptr;

    // Действие:
    IAction* pAction = nullptr;

    // Действие на запуск программы:
    IExecAction* pExecAction = nullptr;

    // Действие на отображение окна сообщения:
    IShowMessageAction* pShowMsgAction = nullptr;


    // Зарегистрированное задание:
    IRegisteredTask* pRegisteredTask;

};

#endif // TIMETRIGGER_H
