#include "mainwindow.h"
#include <QApplication>

// Главная функция с которой начинается выполнение программы:
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
