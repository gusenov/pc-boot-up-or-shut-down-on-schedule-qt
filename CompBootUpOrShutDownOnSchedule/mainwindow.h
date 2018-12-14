#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "task.h"
#include <QTimer>

// Имя автора задания для планировщика заданий:
#define DEFAULT_AUTHOR_NAME L"user"

// Наименование задания на выключение компьютера:
#define SHUT_DOWN_TASK_NAME L"Time Trigger Computer Shut Down Task"

// Наименование задания на включение компьютера:
#define BOOT_UP_TASK_NAME L"Time Trigger Computer Boot Up Task"

// Длина буфера по умолчанию:
#define BUFF_LEN 20

namespace Ui {
class MainWindow;
}

// Класс представляющий собой главное окно приложения:
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    // Конструктор:
    explicit MainWindow(QWidget *parent = nullptr);

    // Деструктор:
    ~MainWindow();

private slots:

   // Обработчик события нажатия на кнопку "Применить":
   void handlePushButtonApply();

   // Обработчик события нажатия на кнопку "Отменить всё":
   void handlePushButtonCancelAll();

   // Обработчик события нажатия на флажок "Выключить компьютер":
   void handleCheckBoxShutDown(bool checked);

   // Обработчик события нажатия на флажок "Включить компьютер":
   void handleCheckBoxBootUp(bool checked);

   void doStuff();

private:

    void startOrResetTimer();

    Ui::MainWindow *ui;

    // Сервис для получения доступа к планировщику заданий:
    TaskService* taskService;

    // Строковое представление даты и времени выключения компьютера:
    QString dateTimeShutDown;

    // Строковое представление даты и времени включения компьютера:
    QString dateTimeBootUp;

    // Буфер для конвертирования даты и времени из QString типа в wchar_t* тип:
    wchar_t dateTime[BUFF_LEN];

    // Длина конвертируемой строки:
    int dateTimeLen;

    // Имя автора задания для планировщика:
    wchar_t* authorName = nullptr;

    // Длина имени автора:
    DWORD authorNameLen = BUFF_LEN;

    QTimer* mTimer;
};

#endif // MAINWINDOW_H
