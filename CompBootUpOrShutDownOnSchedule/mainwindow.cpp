#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "waitabletimer.h"
#include <QMessageBox>

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

    // По умолчанию выключить кнопку "Отменить всё":
    ui->pushButtonCancelAll->setEnabled(false);

    // Связываем элементы интерфейса с обработчиками их событий:

    // Нажатие на кнопку "Применить":
    QObject::connect(ui->pushButtonApply, SIGNAL(released()), this, SLOT(handlePushButtonApply()));

    // Нажатие на кнопку "Отменить всё":
    QObject::connect(ui->pushButtonCancelAll, SIGNAL(released()), this, SLOT(handlePushButtonCancelAll()));

    // Установка флажка выключения компьютера:
    QObject::connect(ui->checkBoxShutDown, SIGNAL(clicked(bool)), this, SLOT(handleCheckBoxShutDown(bool)));

    // Установка флажка включения компьютера:
    QObject::connect(ui->checkBoxBootUp, SIGNAL(clicked(bool)), this, SLOT(handleCheckBoxBootUp(bool)));

    // Таймер для отсчета времени до выключения:
    timerShutDown = new QTimer(this);
    timerShutDown->setSingleShot(true);
    connect(timerShutDown, SIGNAL(timeout()), SLOT(doShutDown()));

    // Таймер для обратного отсчета секунд:
    timerCountDown = new QTimer(this);
    connect(timerCountDown, SIGNAL(timeout()), SLOT(doCountDown()));
}

// Деструктор:
MainWindow::~MainWindow()
{
    // Таймер для отсчета времени до выключения:

    // Остановка таймера:
    if (timerShutDown->isActive())
        timerShutDown->stop();

    // Освобождеие памяти:
    delete timerShutDown;


    // Таймер для обратного отсчета секунд:

    // Остановка таймера:
    if (timerCountDown->isActive())
        timerCountDown->stop();

    // Освобождеие памяти:
    delete timerCountDown;


    delete ui;
}

// Обработчик события нажатия на кнопку "Применить":
void MainWindow::handlePushButtonApply()
{
    // Получаем дату/время выключения компьютера в строковом виде:
    strDateTimeShutDown = QString("%1T%2").arg(
        ui->dateEditShutDown->date().toString("yyyy-MM-dd"),
        ui->timeEditShutDown->time().toString("HH:mm:ss")
    );

    // Получаем дату/время включения компьютера в строковом виде:
    strDateTimeBootUp = QString("%1T%2").arg(
        ui->dateEditBootUp->date().toString("yyyy-MM-dd"),
        ui->timeEditBootUp->time().toString("HH:mm:ss")
    );

    if (startOrResetShutDownTimer())
    {
        ui->checkBoxShutDown->setEnabled(false);
        ui->dateEditShutDown->setEnabled(false);
        ui->timeEditShutDown->setEnabled(false);

        ui->checkBoxBootUp->setEnabled(false);
        ui->dateEditBootUp->setEnabled(false);
        ui->timeEditBootUp->setEnabled(false);

        // Выключаем кнопку "Применить":
        ui->pushButtonApply->setEnabled(false);

        // Включаем кнопку "Отменить всё":
        ui->pushButtonCancelAll->setEnabled(true);
    }
}

// Обработчик события нажатия на кнопку "Отменить всё":
void MainWindow::handlePushButtonCancelAll()
{
    ui->checkBoxShutDown->setEnabled(true);
    if (ui->checkBoxShutDown->isChecked())
    {
        ui->dateEditShutDown->setEnabled(true);
        ui->timeEditShutDown->setEnabled(true);
    }

    ui->checkBoxBootUp->setEnabled(true);
    if (ui->checkBoxBootUp->isChecked())
    {
        ui->dateEditBootUp->setEnabled(true);
        ui->timeEditBootUp->setEnabled(true);
    }

    // Включаем кнопку "Применить":
    ui->pushButtonApply->setEnabled(true);

    // Выключаем кнопку "Отменить всё" после нажатия:
    ui->pushButtonCancelAll->setEnabled(false);

    // Останавливаем таймер отсчитывающий время до выключения:
    if (timerShutDown->isActive())
        timerShutDown->stop();

    // Останавливаем таймер обратного отсчета по секундам:
    if (timerCountDown->isActive())
        timerCountDown->stop();

    // Очистить метку обратного отсчета:
    ui->labelCountdown->setText("");
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

        ui->checkBoxBootUp->setEnabled(true);

        ui->pushButtonApply->setEnabled(true);
    }

    // В другом случае:
    else
    {
        // Выключаем соответствующие элементы интерфейса:

        ui->dateEditShutDown->setEnabled(false);
        ui->timeEditShutDown->setEnabled(false);

        ui->checkBoxBootUp->setChecked(false);
        ui->checkBoxBootUp->setEnabled(false);
        ui->dateEditBootUp->setEnabled(false);
        ui->timeEditBootUp->setEnabled(false);

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
    }

    // В другом случае:
    else
    {
        // Выключаем соответствующие элементы интерфейса:
        ui->dateEditBootUp->setEnabled(false);
        ui->timeEditBootUp->setEnabled(false);
    }
}

// Запустить таймер отсчитывающий время до выключения:
bool MainWindow::startOrResetShutDownTimer()
{
    QDateTime now = QDateTime::currentDateTime();

    QDateTime dateTimeShutDown = QDateTime::fromString(strDateTimeShutDown, "yyyy-MM-ddTHH:mm:ss");
    millisecondsDiff = now.msecsTo(dateTimeShutDown);

    QDateTime dateTimeBootUp = QDateTime::fromString(strDateTimeBootUp, "yyyy-MM-ddTHH:mm:ss");
    secondsDiff = dateTimeShutDown.msecsTo(dateTimeBootUp) / 1000;

    if (millisecondsDiff < 0 ||
            (ui->checkBoxBootUp->isChecked() && secondsDiff < 0))
    {
        QMessageBox::warning(this, "Ошибка!", "Неправильная дата и время!", QMessageBox::Ok);
        return false;
    }
    else
    {
        timerShutDown->start(millisecondsDiff);

        // Начать обратный отсчёт:
        timerCountDown->start(1000);

        return true;
    }
}

// Метод, который выполняется, когда наступает время выключения:
void MainWindow::doShutDown()
{
    timerCountDown->stop();

    // Если флажок включения компьютера установлен:
    if (ui->checkBoxBootUp->isChecked())
    {
        ui->labelCountdown->setText("Shut Down & Boot Up");
        qDebug() << "secondsDiff =" << secondsDiff;
        WaitableTimer::HibernateAndReboot(secondsDiff);
    }

    // Иначе, если удалось получить права на выключение компьютера:
    else if (WaitableTimer::_EnableShutDownPriv())
    {
        ui->labelCountdown->setText("Shut Down");

        // Выключить компьютер:
        SetSystemPowerState(FALSE, FALSE);
    }
}

// Метод, который выполняется при обратном отсчете времени:
void MainWindow::doCountDown()
{
    ui->labelCountdown->setText(QString("%1").arg(millisecondsDiff / 1000));
    millisecondsDiff -= 1000;
}
