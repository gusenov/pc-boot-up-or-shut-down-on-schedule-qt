#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

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

   // Метод, который выполняется, когда наступает время выключения:
   void doShutDown();

   // Метод, который выполняется при обратном отсчете времени:
   void doCountDown();

private:

    // Запустить таймер отсчитывающий время до выключения:
    bool startOrResetShutDownTimer();

    // Графический интерфейс приложения:
    Ui::MainWindow *ui;

    // Строковое представление даты и времени выключения компьютера:
    QString strDateTimeShutDown;

    // Строковое представление даты и времени включения компьютера:
    QString strDateTimeBootUp;

    // Таймер для отсчета времени до выключения:
    QTimer* timerShutDown;

    // Таймер для обратного отсчета секунд:
    QTimer* timerCountDown;

    // Разница в миллисекундах:
    qint64 millisecondsDiff;

    // Разница в секундах:
    qint64 secondsDiff;
};

#endif // MAINWINDOW_H
