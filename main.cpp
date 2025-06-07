#include "mainwindow.h" // salarymanager.h에서 mainwindow.h로 변경
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;       // SalaryManager에서 MainWindow로 변경
    w.show();
    return a.exec();
}
