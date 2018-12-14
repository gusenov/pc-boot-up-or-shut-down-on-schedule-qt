#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "waitabletimer.h"

// Конструктор:
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Установить фиксированный размер окна:
    this->setFixedSize(QSize(width(), height()));

    // Установить текущую дату/время в секции выключения компьютера:
    ui->dateEditShutDown->setDate(QDate::currentDate());
    ui->timeEditShutDown->setTime(QTime::currentTime());

    // По умолчанию выключить элементы интерфейса для выбора даты/времени выключения компьютера:
    ui->dateEditShutDown->setEnabled(false);
    ui->timeEditShutDown->setEnabled(false);

    // Установить текущую дату/время в секции включения компьютера:
    ui->dateEditBootUp->setDate(QDate::currentDate());
    ui->timeEditBootUp->setTime(QTime::currentTime());

    // По умолчанию выключить элементы интерфейса для выбора даты/времени включения компьютера:
    ui->checkBoxBootUp->setEnabled(false);
    ui->dateEditBootUp->setEnabled(false);
    ui->timeEditBootUp->setEnabled(false);

    // По умолчанию выключить кнопку "Применить":
    ui->pushButtonApply->setEnabled(false);

    // Получаем имя текущего пользователя системы:
    authorName = new wchar_t[authorNameLen];
    GetUserName(authorName, &authorNameLen);
//    qDebug() << "authorName =" << QString::fromWCharArray(authorName, (int)authorNameLen);

    // Получаем сервис для получения доступа к планировщику заданий:
    taskService = &TaskService::instance();

    // Связываем элементы интерфейса с обработчиками их событий:

    // Нажатие на кнопку "Применить":
    QObject::connect(ui->pushButtonApply, SIGNAL(released()), this, SLOT(handlePushButtonApply()));

    // Нажатие на кнопку "Отменить всё":
    QObject::connect(ui->pushButtonCancelAll, SIGNAL(released()), this, SLOT(handlePushButtonCancelAll()));

    // Установка флажка выключения компьютера:
    QObject::connect(ui->checkBoxShutDown, SIGNAL(clicked(bool)), this, SLOT(handleCheckBoxShutDown(bool)));

    // Установка флажка включения компьютера:
    QObject::connect(ui->checkBoxBootUp, SIGNAL(clicked(bool)), this, SLOT(handleCheckBoxBootUp(bool)));

    mTimer = new QTimer(this);
    mTimer->setSingleShot(true);
    connect(mTimer, SIGNAL(timeout()), SLOT(doStuff()));
}

// Деструктор:
MainWindow::~MainWindow()
{
    // Освобождение памяти занятой под имя пользователя:
    if (authorName != nullptr)
    {
        delete[] authorName;
    }

    delete ui;
}

// Обработчик события нажатия на кнопку "Применить":
void MainWindow::handlePushButtonApply()
{
    taskService->delTask(SHUT_DOWN_TASK_NAME);
    taskService->delTask(BOOT_UP_TASK_NAME);

    // Получаем дату/время выключения компьютера в строковом виде:
    dateTimeShutDown = QString("%1T%2").arg(
        ui->dateEditShutDown->date().toString("yyyy-MM-dd"),
        ui->timeEditShutDown->time().toString("HH:mm:ss")
    );

    // Получаем дату/время включения компьютера в строковом виде:
    dateTimeBootUp = QString("%1T%2").arg(
        ui->dateEditBootUp->date().toString("yyyy-MM-dd"),
        ui->timeEditBootUp->time().toString("HH:mm:ss")
    );

    // Если флажок выключения компьютера установлен:
    if (ui->checkBoxShutDown->isChecked())
    {
        // Путь к программе rundll32.exe:
        wstring wstrExecutablePath = _wgetenv(L"WINDIR");
        wstrExecutablePath += L"\\SYSTEM32\\rundll32.exe";

        // Аргументы для запуска программы rundll32.exe:

        // Блокировать:
//        wstring args = L"user32.dll,LockWorkStation";

        // Перейти в ждущий режим:
        wstring args = L"powrprof.dll,SetSuspendState 0,1,0";

        // Сигнатура функции SetSuspendState:
        // BOOLEAN SetSuspendState(BOOLEAN bHibernate, BOOLEAN bForce, BOOLEAN bWakeupEventsDisabled);

        // Время перехода в спящий режим:
        dateTimeLen = dateTimeShutDown.toWCharArray(dateTime);
//        qDebug() << "dateTime =" << QString::fromWCharArray(dateTime, dateTimeLen);
//        LPCWSTR dateTime = L"2018-12-11T00:50:00";

        if (!ui->checkBoxBootUp->isChecked())
        {
        // Создание задания на выключение компьютера:
            Task(
                        TASK_ACTION_EXEC,  // тип задания.
                        SHUT_DOWN_TASK_NAME,  // наименование задания.
                        wstrExecutablePath,  // путь к запускаемой программе.
                        args,  // аргументы запускаемой программы.
                        authorName,  // имя автора.
                        dateTime,  // дата начала.
//                        dateTime,  // дата конца.
                        false  // пробуждать компьютер для выполнения задания?
            );
        }
        else
        {
            startOrResetTimer();
        }
    }
    // Иначе, если флажок выключения компьютера снят:
    else
    {
        // Удалить задание на выключение компьютера:
        taskService->delTask(SHUT_DOWN_TASK_NAME);
    }

    // Если флажок включения компьютера установлен:
    if (ui->checkBoxBootUp->isChecked())
    {
        // Путь к программе Проводник:
        wstring wstrExecutablePath = _wgetenv(L"WINDIR");
        wstrExecutablePath += L"\\explorer.exe";

        // Аргументы для запуска:
        wstring args = L"";

        // Время пробуждения:
        dateTimeLen = dateTimeBootUp.toWCharArray(dateTime);
//        qDebug() << "dateTime =" << QString::fromWCharArray(dateTime, dateTimeLen);
//        LPCWSTR dateTime = L"2018-12-11T23:30:00";

        // Создание задания на включение компьютера:

        // Запустить Проводник при пробуждении:
        Task(
                    TASK_ACTION_EXEC,  // тип задания.
                    BOOT_UP_TASK_NAME,  // наименование задания.
                    wstrExecutablePath,  // путь к запускаемой программе.
                    args,  // аргументы запускаемой программы.
                    authorName,  // имя автора.
                    dateTime,  // дата начала.
//                    dateTime,  // дата конца.
                    true  // пробуждать компьютер для выполнения задания?
        );

        // Показать окно сообщения при пробуждении:
//        Task(TASK_ACTION_SHOW_MESSAGE, BOOT_UP_TASK_NAME, L"Приветствие", L"Добро пожаловать!", authorName, dateTime, dateTime, true);

    }
    // Иначе, если флажок включения компьютера снят:
    else
    {
        // Удалить задание на включение компьютера:
        taskService->delTask(BOOT_UP_TASK_NAME);
    }

    // Включаем кнопку "Отменить всё":
    ui->pushButtonCancelAll->setEnabled(true);

    ui->pushButtonApply->setEnabled(false);
}

// Обработчик события нажатия на кнопку "Отменить всё":
void MainWindow::handlePushButtonCancelAll()
{
    // Удаляем задание на выключение компьютера:
    taskService->delTask(SHUT_DOWN_TASK_NAME);

    // Удаляем задание на включение компьютера:
    taskService->delTask(BOOT_UP_TASK_NAME);

    // Выключаем кнопку "Отменить всё" после нажатия:
    ui->pushButtonCancelAll->setEnabled(false);

    ui->pushButtonApply->setEnabled(true);

    mTimer->stop();
}

// Обработчик события нажатия на флажок "Выключить компьютер":
void MainWindow::handleCheckBoxShutDown(bool checked)
{
    // Если флажок "Выключить компьютер" поставлен:
    if (checked)
    {
        // Включаем соответствующие элементы интерфейса:
        ui->dateEditShutDown->setEnabled(true);
        ui->timeEditShutDown->setEnabled(true);
        ui->pushButtonApply->setEnabled(true);

        ui->checkBoxBootUp->setEnabled(true);
    }
    // В другом случае:
    else
    {
        // Выключаем соответствующие элементы интерфейса:
        ui->dateEditShutDown->setEnabled(false);
        ui->timeEditShutDown->setEnabled(false);

        ui->checkBoxBootUp->setEnabled(false);
        ui->checkBoxBootUp->setChecked(false);

        ui->pushButtonApply->setEnabled(false);
    }
}

// Обработчик события нажатия на флажок "Включить компьютер":
void MainWindow::handleCheckBoxBootUp(bool checked)
{
    // Если флажок "Включить компьютер" поставлен:
    if (checked)
    {
        // Включаем соответствующие элементы интерфейса:
        ui->dateEditBootUp->setEnabled(true);
        ui->timeEditBootUp->setEnabled(true);
        ui->pushButtonApply->setEnabled(true);
    }
    // В другом случае:
    else
    {
        // Выключаем соответствующие элементы интерфейса:
        ui->dateEditBootUp->setEnabled(false);
        ui->timeEditBootUp->setEnabled(false);
    }
}

void MainWindow::startOrResetTimer()
{
    QDateTime now = QDateTime::currentDateTime();
    QDateTime shutdown = QDateTime::fromString(dateTimeShutDown, "yyyy-MM-ddTHH:mm:ss");
    qint64 millisecondsDiff = now.msecsTo(shutdown);
    qint64 secondsDiff = millisecondsDiff / 1000;
    qDebug() << "secondsDiff = " << secondsDiff;

    mTimer->start(millisecondsDiff);
}

void MainWindow::doStuff()
{
    qDebug() << "Shut Down...";

    QDateTime now = QDateTime::currentDateTime();
    QDateTime boot = QDateTime::fromString(dateTimeBootUp, "yyyy-MM-ddTHH:mm:ss");
    qint64 millisecondsDiff = now.msecsTo(boot);
    qint64 secondsDiff = millisecondsDiff / 1000;
    qDebug() << "secondsDiff = " << secondsDiff;

//    WaitableTimer::HibernateAndReboot(secondsDiff);
}
