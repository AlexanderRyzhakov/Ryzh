#include "win_dialog_save_if_edited.h"
#include "ui_win_dialog_save_if_edited.h"
#include "file_read_write.h"

win_Dialog_save_if_edited::win_Dialog_save_if_edited(QWidget *parent, bool *execute_action) :
    QDialog(parent),
    ui(new Ui::win_Dialog_save_if_edited),
    accepted_(execute_action)
{
    ui->setupUi(this);
}

win_Dialog_save_if_edited::~win_Dialog_save_if_edited()
{
    delete ui;
}

void win_Dialog_save_if_edited::on_buttonBox_accepted()
{
    SaveLib();
}


void win_Dialog_save_if_edited::on_buttonBox_rejected()
{

}

