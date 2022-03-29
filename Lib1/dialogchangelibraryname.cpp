#include "dialogchangelibraryname.h"
#include "ui_dialogchangelibraryname.h"

#include <QString>

#include "library_main_class.h"
#include "global_library.h"

DialogChangeLibraryName::DialogChangeLibraryName(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogChangeLibraryName)
{
    ui->setupUi(this);
    setWindowTitle("New library name");
}

DialogChangeLibraryName::~DialogChangeLibraryName()
{
    delete ui;
}

void DialogChangeLibraryName::on_buttonBox_accepted()
{
    QString new_name = ui->lineEdit->text();
    library.ChangeLibName(new_name.toLocal8Bit().data());
}


