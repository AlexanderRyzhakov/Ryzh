#include "win_delete_book.h"
#include "ui_win_delete_book.h"

#include "library_main_class.h"
#include "global_library.h"

win_delete_book::win_delete_book(QWidget* parent, size_t id)
    : QDialog(parent)
    , ui(new Ui::win_delete_book)
    , index_(id)
{
    ui->setupUi(this);


    QString text = "Delete \"";
    std::string title = library.At(index_).GetTitle();
    text += QString::fromLocal8Bit(title) + "\"?";

    ui->label->setText(text);
}

win_delete_book::~win_delete_book()
{
    delete ui;
}

void win_delete_book::on_buttonBox_accepted()
{
    library.DeleteBook(index_);
}

