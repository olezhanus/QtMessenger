// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include <QApplication>
#include <QTranslator>
#include "mainwindow.h"
#include <iostream>
#include <vector>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    QTranslator myappTranslator;
    myappTranslator.load("translations/my_ru.qm");
    a.installTranslator(&myappTranslator);

    QTranslator qtTranslator;
    qtTranslator.load("translations/qt_ru.qm");
    a.installTranslator(&qtTranslator);

    auto w = std::make_unique<MainWindow>();

    w->show();

    return a.exec();
}
