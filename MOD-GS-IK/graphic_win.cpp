#include "graphic_win.h"
#include "ui_graphic_win.h"

graphic_win::graphic_win(MsuGs& gs, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::graphic_win),
    gs_(gs)
{
    ui->setupUi(this);
}

graphic_win::~graphic_win()
{
    delete ui;
}
