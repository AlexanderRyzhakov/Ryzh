#include "mainwindow.h"

#include <future>
#include <stdexcept>

#include <QApplication>
#include <QMessageBox>

#include "life.h"
#include "timers.h"

static int console_cols = 100;
static int console_rows = 100;

int main(int argc, char* argv[])
{
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::Base, QColor(42, 42, 42));
    darkPalette.setColor(QPalette::Text, Qt::white);
    qApp->setPalette(darkPalette);

    Life life;
    if (argc == 1) {
        life.GenerateBoard(console_cols, console_rows);
    } else if (argc == 2) {
        // get board from file (txt, excel)
    }

    QApplication a(argc, argv);
    MainWindow w(life);
    w.show();
    a.exec();

    return 0;
}
