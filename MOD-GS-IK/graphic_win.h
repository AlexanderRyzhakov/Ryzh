#ifndef GRAPHIC_WIN_H
#define GRAPHIC_WIN_H

#include <QMainWindow>

#include "msu_gs.h"

namespace Ui {
class graphic_win;
}

class graphic_win : public QMainWindow
{
    Q_OBJECT

public:
    explicit graphic_win(MsuGs& gs, QWidget *parent = nullptr);
    ~graphic_win();

private:
    Ui::graphic_win *ui;
    MsuGs& gs_;
};

#endif // GRAPHIC_WIN_H
