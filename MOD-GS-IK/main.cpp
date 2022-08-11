#include "mainwindow.h"

#include <fstream>
#include <string>
#include <stdexcept>
#include <utility>
#include <cstdio>

#include <QApplication>
#include <QStyleFactory>
#include <QString>
#include <QMessageBox>

#include "msu_gs.h"
#include "global_consts.h"

void CheckSerialCache(MsuGs& gs, FocusManager& mngr) {
    std::ifstream file;
    file.open(kSerialFile.toLocal8Bit());
    try {
        if (file) {
            std::string line;
            if (std::getline(file, line)) {
                QString q_str =  QString::fromLocal8Bit(line.data(), line.size());
                gs.SetSerialNumber(std::move(q_str));
            }
            if (std::getline(file, line)) {
                QString q_str =  QString::fromLocal8Bit(line.data(), line.size());
                gs.SetRhNumber(std::move(q_str));
            }

            if (std::getline(file, line)) {
                QString q_str =  QString::fromLocal8Bit(line.data(), line.size());
                mngr.SetGSFocus(q_str);
            }
            if (std::getline(file, line)) {
                QString q_str =  QString::fromLocal8Bit(line.data(), line.size());
                mngr.SetCollimatorFocus(q_str);
            }
            if (std::getline(file, line)) {
                QString q_str =  QString::fromLocal8Bit(line.data(), line.size());
                mngr.SetTargetCollimatorFocus(q_str);
            }
        }
    } catch (std::exception& err) {
        file.close();
        std::remove(kSerialFile.toLocal8Bit());
        QMessageBox msg;
        msg.setText("Cache file not valid and was deleted.");
        msg.exec();
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // set style
    qApp->setStyle(QStyleFactory::create("Fusion"));
    // increase font size for better reading
    QFont defaultFont = QApplication::font();
    defaultFont.setPointSize(defaultFont.pointSize()+2);
    qApp->setFont(defaultFont);
    // modify palette to dark
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window,QColor(53,53,53));
    darkPalette.setColor(QPalette::WindowText,Qt::white);
    darkPalette.setColor(QPalette::Disabled,QPalette::WindowText,QColor(127,127,127));
    darkPalette.setColor(QPalette::Base,QColor(42,42,42));
    darkPalette.setColor(QPalette::AlternateBase,QColor(66,66,66));
    darkPalette.setColor(QPalette::ToolTipBase,Qt::white);
    darkPalette.setColor(QPalette::ToolTipText,Qt::white);
    darkPalette.setColor(QPalette::Text,Qt::white);
    darkPalette.setColor(QPalette::Disabled,QPalette::Text,QColor(127,127,127));
    darkPalette.setColor(QPalette::Dark,QColor(35,35,35));
    darkPalette.setColor(QPalette::Shadow,QColor(20,20,20));
    darkPalette.setColor(QPalette::Button,QColor(53,53,53));
    darkPalette.setColor(QPalette::ButtonText,Qt::white);
    darkPalette.setColor(QPalette::Disabled,QPalette::ButtonText,QColor(127,127,127));
    darkPalette.setColor(QPalette::BrightText,Qt::red);
    darkPalette.setColor(QPalette::Link,QColor(42,130,218));
    darkPalette.setColor(QPalette::Highlight,QColor(42,130,218));
    darkPalette.setColor(QPalette::Disabled,QPalette::Highlight,QColor(80,80,80));
    darkPalette.setColor(QPalette::HighlightedText,Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(127, 127, 127));
    qApp->setPalette(darkPalette);

    FocusManager focus_manger;
    MsuGs gs;
    CheckSerialCache(gs, focus_manger);

    MainWindow w(focus_manger, gs);
    w.show();
    return a.exec();
}
