/* COMP.CS.110 - Project IV: Path GUI
 *
 * Program author: Andrea Caruso
 * Student number: 151211751
 * UserID: ccanca
 * Email: andrea.caruso@tuni.fi
 *
 * Description:
 * This is the main.cpp file for the Path GUI application. This file was not
 * modified by the student.
 *
 * */

#include "mainwindow.hh"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
