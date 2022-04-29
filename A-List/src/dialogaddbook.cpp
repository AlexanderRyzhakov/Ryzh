#include "dialogaddbook.h"
#include "ui_dialogaddbook.h"

#include <cstdio>
#include <string>
#include <utility>

#include <QFile>
#include <QString>
#include <QPushButton>

#include "global_library.h"
#include "library_main_class.h"
#include "book.h"
#include "author.h"
#include "string_process.h"

// ----------------------------------------------------------------------------- constructors
DialogAddBook::DialogAddBook(QWidget *parent, size_t index, ImageDrawer *drawer) :
    QDialog(parent),
    ui(new Ui::DialogAddBook),
    image_dawer_(drawer),
    index_(index)
{
    ui->setupUi(this);
    if (index != 0) {
        setWindowTitle("Edit book");
        SetFieldsWithCurrentBook();
        old_title_ = library.At(index).GetTitle();
    } else {
        setWindowTitle("New book");
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
}

DialogAddBook::~DialogAddBook()
{
    delete ui;
}

// ------------------------------------------------------------------------------- slots
void DialogAddBook::on_buttonBox_accepted()
{
    std::string title = ui->title_edit->text().toLocal8Bit().data();
    std::string authors = ui->author_edit->text().toLocal8Bit().data();
    Book book(std::move(title), std::move(authors));

    uint8_t rating = 0;
    if (ui->radioButton_rating_1->isChecked()) {
        rating = 1;
    } else if (ui->radioButton_rating_2->isChecked()) {
        rating = 2;
    } else if (ui->radioButton_rating_3->isChecked()) {
        rating = 3;
    } else if (ui->radioButton_rating_4->isChecked()) {
        rating = 4;
    } else if (ui->radioButton_rating_5->isChecked()) {
        rating = 5;
    }
    book.SetRating(rating);

    book.SetComment(std::move(ui->comment_edit->toPlainText().toLocal8Bit().data()));

    if (ui->radioButton_owned->isChecked()) {
        book.SetOwned();
    } else if (ui->radioButton_wish_list->isChecked()) {
        book.SetWished();
    }

    if (index_ == 0) {
        library.AddBook(book);
    } else {
        library.DeleteBook(index_);
        library.AddBook(book, index_);
        RenameCoverFile(book.GetTitle());
    }
}

void DialogAddBook::on_title_edit_textChanged(const QString& arg1)
{
    if (arg1.isEmpty()) {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    } else {
        std::string input = arg1.toLocal8Bit().data();
        if (index_ == 0) {
            if (library.Contains(input)) {
                ui->warning_text->setText("book has been already added");
                ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
            } else {
                ui->warning_text->setText("");
                ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
            }
        } else {
            if (CleanString(input) != CleanString(library.At(index_).GetTitle())
                && library.Contains(input)) {
                ui->warning_text->setText("another book with this title exists");
                ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
            } else {
                ui->warning_text->setText("");
                ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
            }
        }
    }
}

// ---------------------------------------------------------------------------- private methods
void DialogAddBook::SetFieldsWithCurrentBook()
{
    const Book* const book = &library.At(index_);
    ui->title_edit->setText(QString::fromLocal8Bit(book->GetTitle()));

    std::string authors;
    bool first_word = true;
    for (const auto& author : book->GetAuthors()) {
        if (!first_word) {
            authors += ',';
            authors += ' ';
        }
        first_word = false;
        authors += author.GetName();
    }
    ui->author_edit->setText(QString::fromLocal8Bit(std::move(authors)));

    auto rating = book->GetRating();
    switch (rating) {
    case 1:
        ui->radioButton_rating_1->setChecked(true);
        break;
    case 2:
        ui->radioButton_rating_2->setChecked(true);
        break;
    case 3:
        ui->radioButton_rating_3->setChecked(true);
        break;
    case 4:
        ui->radioButton_rating_4->setChecked(true);
        break;
    case 5:
        ui->radioButton_rating_5->setChecked(true);
        break;
    }

    if (book->IsOwned()) {
        ui->radioButton_owned->setChecked(true);
    } else {
        ui->radioButton_wish_list->setChecked(true);
    }

    ui->comment_edit->setText(QString::fromLocal8Bit(book->GetComment()));
    image_dawer_->SetTargetWidget(ui->label_edit_book_cover);
    image_dawer_->DrawFromStorage(CleanString(book->GetTitle()));
}

void DialogAddBook::RenameCoverFile(const std::string& new_title) const
{
    std::string file_name_old = kCoversDir + CleanString(old_title_) + ".jpg";
    QString from = QString::fromLocal8Bit(file_name_old);
    QFile file(QString::fromLocal8Bit(file_name_old));
    if (file.exists()) {
        std::string file_name_new = kCoversDir + CleanString(new_title) + ".jpg";
        QString to = QString::fromLocal8Bit(file_name_new);
        file.rename(from, to);
    }
}


