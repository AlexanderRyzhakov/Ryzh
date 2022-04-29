#include "lib.h"
#include "ui_lib.h"

#include <string>
#include <set>
#include <iostream>
#include <fstream>
#include <vector>
#include <utility>

#include <QString>
#include <QCloseEvent>
#include <QColor>
#include <QFileDialog>
#include <QTableView>
#include <QMessageBox>

#include "author.h"
#include "book.h"
#include "file_read_write.h"
#include "global_library.h"
#include "library_main_class.h"
#include "string_process.h"
#include "tests.h"

#include "dialogaddbook.h"
#include "dialogchangelibraryname.h"
#include "win_dialog_save_if_edited.h"
#include "win_delete_book.h"

// ------------------------------------------------------------------------------- constructor
Lib::Lib(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Lib)
{
    ui->setupUi(this);

    uneven_row_color = QBrush(QColor(50,50,58));
    fit_color = QBrush(QColor(55, 70, 58));

    std::vector<QTableWidget*> tables {
        ui->tableWidget_books_owned,
        ui->tableWidget_books_wished
    };
    const QString style = "QTableWidget {gridline-color : #565656;}";

    for (auto ptr : tables) {
        ptr->setColumnWidth(0, 40);
        ptr->setColumnWidth(1, 300);
        ptr->setColumnWidth(2, 370);
        ptr->setColumnWidth(3, 40);
        ptr->setStyleSheet(style);
    }

    auto table_search = ui->tableWidget_search_result;
    table_search->setStyleSheet(style);
    table_search->setColumnWidth(0, 40);
    table_search->setColumnWidth(1, 130);
    table_search->setColumnWidth(2, 130);
    table_search->setColumnWidth(3, 210);
    table_search->setColumnWidth(4, 80);
    table_search->setColumnWidth(5, 80);
    table_search->setColumnWidth(6, 80);

    ui->tableWidget_authors->setColumnWidth(0, 630);
    ui->tableWidget_authors->setColumnWidth(1, 100);
    ui->tableWidget_authors->setStyleSheet(style);

    SetWinTitle();
}

Lib::~Lib()
{
    delete ui;
}

// ----------------------------------------------------------------------- private helper methods
void Lib::CallDeleteDialogue(const QString& id)
{
    win_delete_book window(this, id.toInt());
    window.setModal(true);
    window.exec();
    SetWinTitle();
}

bool Lib::SaveOnEditDialog()
{
    bool execute_action = true;
    if (library.Edited()) {
        win_Dialog_save_if_edited window(this, &execute_action);
        window.setModal(true);
        window.exec();
    }
    return execute_action;
}

void Lib::SetWinTitle()
{
    QString win_title = QString::fromLocal8Bit("ALIB " + library.GetLibName());
    setWindowTitle(win_title);
    ui->Lib_name->setText(QString::fromLocal8Bit(library.GetLibName()));
    ui->statusbar->showMessage("Books: " + QString::number(library.GetBooks().size())
        + ". Authors: " + QString::number(library.GetAuthors().size()));
    ui->textBrowser_selected_book->setText("");
    ShowInTable();
    ui->tableWidget_search_result->clearContents();
    ui->tableWidget_search_result->setRowCount(0);
    ui->tabWidget->setCurrentWidget(ui->tab);
}

void Lib::ShowInTable()
{
    ui->tableWidget_books_owned->setRowCount(library.GetOwnedAmount());
    ui->tableWidget_books_wished->setRowCount(library.GetWishedAmount());

    size_t row_owned_id = 0;
    size_t row_wished_id = 0;

    for (const auto& [id, book] : library) {
        QTableWidgetItem* itm_id = new QTableWidgetItem(QString::number(id));
        QTableWidgetItem* itm_title = new QTableWidgetItem(QString::fromLocal8Bit(book.GetTitle()));
        QTableWidgetItem* itm_author = new QTableWidgetItem(QString::fromLocal8Bit(book.GetAuthorsString()));
        QTableWidgetItem* itm_rating = book.GetRating() == 0 ? new QTableWidgetItem("-") : new QTableWidgetItem(QString::number(book.GetRating()));

        QTableWidget* destination_table = (book.IsOwned()) ? ui->tableWidget_books_owned : ui->tableWidget_books_wished;
        size_t* row = (book.IsOwned()) ? &row_owned_id : &row_wished_id;

        if (*row % 2 == 0) {
            itm_id->setBackground(uneven_row_color);
            itm_title->setBackground(uneven_row_color);
            itm_author->setBackground(uneven_row_color);
            itm_rating->setBackground(uneven_row_color);
        }
        destination_table->setItem(*row, 0, itm_id);
        destination_table->setItem(*row, 1, itm_title);
        destination_table->setItem(*row, 2, itm_author);
        destination_table->setItem(*row, 3, itm_rating);
        ++(*row);
    }

    size_t row_author = 0u;
    ui->tableWidget_authors->setRowCount(library.GetAuthors().size());
    for (const auto& [author, books] : library.GetAuthors()) {
        QTableWidgetItem* itm_author = new QTableWidgetItem(QString::fromLocal8Bit(author.GetName()));
        QTableWidgetItem* itm_book_amount = new QTableWidgetItem(QString::number(books.size()));

        if (row_author % 2 == 0) {
            itm_author->setBackground(uneven_row_color);
            itm_book_amount->setBackground(uneven_row_color);
        }
        ui->tableWidget_authors->setItem(row_author, 0, itm_author);
        ui->tableWidget_authors->setItem(row_author, 1, itm_book_amount);
        ++row_author;
    }
}

void Lib::ShowSelectedInfo(QTableWidgetItem *book) {
    size_t index = book->text().toUInt();

    QString out = QString::fromLocal8Bit("\"" + library.At(index).GetTitle() + "\"\n");

    out += QString::fromLocal8Bit(library.At(index).GetAuthorsString());
    out += '\n';

    if (!library.At(index).GetComment().empty()) {
        out += "______\n";
        out += QString::fromLocal8Bit(library.At(index).GetComment());
        out += "\n______\n";
    }

    ui->textBrowser_selected_book->setText(out);

    image_drawer_.SetTargetWidget(ui->label_cover_zone);
    image_drawer_.DrawFromStorage(CleanString(library.At(index).GetTitle()));
}

// ----------------------------------------------------------------------------- slots

// ------------------------------------------------------------------- actions
void Lib::on_actionExit_triggered()
{
    QApplication::quit();
}

void Lib::on_actionNew_triggered()
{
    SaveOnEditDialog();
    library.Clear();
    DialogChangeLibraryName window;
    window.setModal(true);
    window.exec();
    SetWinTitle();
}

void Lib::on_actionOpen_triggered()
{
    SaveOnEditDialog();
    std::string file_name = QFileDialog::getOpenFileName(this, "Open the file").toStdString();
    if (!file_name.empty()) {
        OpenLib(file_name);
        SetWinTitle();
    }
}

void Lib::on_actionSave_triggered()
{
    if (library.Edited()) {
        SaveLib();
    }
}

void Lib::on_actionTest_add_random_books_triggered()
{
    if (!test_activated_) {
        SaveOnEditDialog();
        ui->lineEdit_search->clear();
        ui->textBrowser_selected_book->setText("Input book amount (1 to 50'000) to add in search field and press test again");
        test_activated_ = true;
    } else {
        library.Clear();
        library.ChangeLibName("Speed test");
        size_t amount_to_add = ui->lineEdit_search->text().toULongLong();
        assert(amount_to_add >= 1 && amount_to_add <= 50'000);
        Tests add_test = {};

        QString line = "Adding ";
        line += QString::number(amount_to_add);
        line += " books.\n";
        ui->textBrowser_selected_book->setText(line);

        std::string generate_time = add_test.GenerateBooks(amount_to_add);
        ui->textBrowser_selected_book->append(QString::fromLocal8Bit(generate_time));

        std::string adding_time = add_test.AddToLib();
        ui->textBrowser_selected_book->append(QString::fromLocal8Bit(adding_time));

        std::string show_time;
        {
            OPERATION_DURATION("show time", show_time);
            SetWinTitle();
        }
        std::string result;
        result += generate_time;
        result += adding_time;
        result += show_time;

        ui->textBrowser_selected_book->append(QString::fromLocal8Bit(result));

        ui->lineEdit_search->clear();
        test_activated_ = false;
    }
}

void Lib::closeEvent(QCloseEvent* event)
{
        SaveOnEditDialog();
        event->accept();
}

void Lib::on_actionTest_show_test_img_triggered()
{
    ui->textBrowser_selected_book->setText("Image requested");
    image_drawer_.SetTargetWidget(ui->label_cover_zone);
    image_drawer_.DrawTest();
}


void Lib::on_actionTest_write_clean_titles_to_file_triggered()
{
    std::ofstream file;
    file.open("clean_titles.txt");
    for (const auto& [_, book] : library) {
        file << CleanString(book.GetTitle()) << '\n';
    }
}

// ------------------------------------------------------------------- buttons

void Lib::on_pushButton_change_lib_name_clicked()
{
    DialogChangeLibraryName window;
    window.setModal(true);
    window.exec();
    ui->Lib_name->setText(QString::fromLocal8Bit(library.GetLibName()));
    SetWinTitle();
}

void Lib::on_pushButton_add_book_clicked(size_t index)
{
//    ImageDrawer* image_drawer_ptr = &image_drawer_;
    DialogAddBook window(this, index, &image_drawer_);
    window.setModal(true);
    window.exec();
    SetWinTitle();
}

void Lib::on_pushButton_delete_book_clicked()
{
    QString id = "";
    if (ui->tableWidget_books_owned->isVisible() && ui->tableWidget_books_owned->currentItem()) {
        id = ui->tableWidget_books_owned->item(ui->tableWidget_books_owned->currentRow(), 0)->text();
        CallDeleteDialogue(id);
    } else if (ui->tableWidget_books_wished->isVisible() && ui->tableWidget_books_wished->currentItem()) {
        id = ui->tableWidget_books_wished->item(ui->tableWidget_books_wished->currentRow(), 0)->text();
        CallDeleteDialogue(id);
    } else if (ui->tableWidget_search_result->isVisible() && ui->tableWidget_search_result->currentItem()) {
        id = ui->tableWidget_search_result->item(ui->tableWidget_search_result->currentRow(), 0)->text();
        CallDeleteDialogue(id);
    }
    ui->textBrowser_selected_book->setText("");
}

void Lib::on_pushButton_search_clicked()
{
    auto output = ui->tableWidget_search_result;
    std::vector<std::pair<size_t, BookAttribute>> results = library.Search(ui->lineEdit_search->text().toLocal8Bit().data());

    if (results.empty()) {
        ui->textBrowser_selected_book->setText("Nothing's found");
        ui->tabWidget->setCurrentWidget(ui->tab);
        output->clearContents();
        output->setRowCount(0);
    } else {
        ui->textBrowser_selected_book->setText("");
        size_t row = 0u;
        ui->tabWidget->setCurrentWidget(ui->tab_3);
        output->setRowCount(results.size());

        for (const auto& [id, attr] : results) {
            const Book* book = &library.At(id);

            QTableWidgetItem* itm_id = new QTableWidgetItem(QString::number(id));
            QTableWidgetItem* itm_title = new QTableWidgetItem(QString::fromLocal8Bit(book->GetTitle()));
            QTableWidgetItem* itm_author = new QTableWidgetItem(QString::fromLocal8Bit(book->GetAuthorsString()));
            QTableWidgetItem* itm_comment = new QTableWidgetItem(QString::fromLocal8Bit(book->GetComment()));
            QTableWidgetItem* itm_own_or_wish = new QTableWidgetItem("*");
            QTableWidgetItem* itm_blank = new QTableWidgetItem(" ");
            QTableWidgetItem* itm_rating = new QTableWidgetItem(QString::number(book->GetRating()));

            if (row % 2 == 0) {
                itm_id->setBackground(uneven_row_color);
                itm_title->setBackground(uneven_row_color);
                itm_author->setBackground(uneven_row_color);
                itm_comment->setBackground(uneven_row_color);
                itm_own_or_wish->setBackground(uneven_row_color);
                itm_blank->setBackground(uneven_row_color);
                itm_rating->setBackground(uneven_row_color);
            }
            if (attr == BookAttribute::TITLE) {
                itm_title->setBackground(fit_color);
            } else if (attr == BookAttribute::AUTHORS) {
                itm_author->setBackground(fit_color);
            } else if (attr == BookAttribute::COMMENT) {
                itm_comment->setBackground(fit_color);
            }

            output->setItem(row, 0, itm_id);
            output->setItem(row, 1, itm_title);
            output->setItem(row, 2, itm_author);
            output->setItem(row, 3, itm_comment);
            if (book->IsOwned()) {
                output->setItem(row, 4, itm_own_or_wish);
                output->setItem(row, 5, itm_blank);
            } else {
                output->setItem(row, 4, itm_blank);
                output->setItem(row, 5, itm_own_or_wish);
            }
            output->setItem(row, 6, itm_rating);
            ++row;
        }
    }
}

void Lib::on_lineEdit_search_returnPressed()
{
    if (!test_activated_) {
        on_pushButton_search_clicked();
    } else {
        on_actionTest_add_random_books_triggered();
    }
}

// ---------------------------------------------------------------------- table widget clicks

void Lib::on_tableWidget_books_owned_itemClicked(QTableWidgetItem* item)
{
    size_t row_number = item->row();
    size_t column_number = 0; //Index column
    QTableWidgetItem* book = ui->tableWidget_books_owned->item(row_number, column_number);
    ShowSelectedInfo(book);
}


void Lib::on_tableWidget_books_wished_itemClicked(QTableWidgetItem* item)
{
    size_t row_number = item->row();
    size_t column_number = 0; //Index column
    QTableWidgetItem* book = ui->tableWidget_books_wished->item(row_number, column_number);
    ShowSelectedInfo(book);
}

void Lib::on_tableWidget_search_result_itemClicked(QTableWidgetItem *item)
{
    size_t row_number = item->row();
    size_t column_number = 0; //Index column
    QTableWidgetItem* book = ui->tableWidget_search_result->item(row_number, column_number);
    ShowSelectedInfo(book);
}

void Lib::on_tableWidget_books_owned_itemDoubleClicked(QTableWidgetItem* item)
{
    size_t index = ui->tableWidget_books_owned->item(item->row(), 0)->text().toULongLong();
    on_pushButton_add_book_clicked(index);
}

void Lib::on_tableWidget_books_wished_itemDoubleClicked(QTableWidgetItem *item)
{
    size_t index = ui->tableWidget_books_wished->item(item->row(), 0)->text().toULongLong();
    on_pushButton_add_book_clicked(index);
}

void Lib::on_tableWidget_search_result_itemDoubleClicked(QTableWidgetItem *item)
{
    size_t index = ui->tableWidget_search_result->item(item->row(), 0)->text().toULongLong();
    on_pushButton_add_book_clicked(index);
}

void Lib::on_tableWidget_authors_itemClicked(QTableWidgetItem* item)
{
    QString name = ui->tableWidget_authors->item(item->row(), 0)->text();
    Author author(std::move(name.toLocal8Bit().data()));
    QString out;
    QString owned;
    QString wished;

    for (const auto& id : library.GetAuthors().at(author)) {
        if (library.At(id).IsOwned()) {
            owned += "\"";
            owned += QString::fromLocal8Bit(library.At(id).GetTitle());
            owned += "\"\n";
        } else {
            wished += "\"";
            wished += QString::fromLocal8Bit(library.At(id).GetTitle());
            wished += "\"\n";
        }
    }
    if (!owned.isEmpty()) {
        out += "*** OWNED ***\n";
        out += std::move(owned);
        out += '\n';
    }
    if (!wished.isEmpty()) {
        out += "*** WISHED ***\n";
        out += std::move(wished);
    }

    ui->textBrowser_selected_book->setText(out);
    image_drawer_.SetTargetWidget(ui->label_cover_zone);
    image_drawer_.DrawFromStorage("");
}
