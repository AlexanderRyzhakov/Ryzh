#ifndef LIB_H
#define LIB_H

#include <QMainWindow>
#include <QApplication>
#include <QTableWidget>

QT_BEGIN_NAMESPACE
    namespace Ui { class Lib; }
QT_END_NAMESPACE

class Lib : public QMainWindow
{
Q_OBJECT

public:
    Lib(QWidget *parent = nullptr);
    ~Lib();

private:
    void CallDeleteDialogue(const QString& id);

    void closeEvent(QCloseEvent* event);

    void ShowInTable();

    void ShowSelectedInfo(QTableWidgetItem *item);

    void SetWinTitle();

    bool SaveOnEditDialog();

private slots:
    void on_actionExit_triggered();

    void on_pushButton_change_lib_name_clicked();

    void on_actionNew_triggered();

    void on_pushButton_add_book_clicked(size_t index = 0);

    void on_actionSave_triggered();

    void on_actionOpen_triggered();

    void on_tableWidget_books_owned_itemClicked(QTableWidgetItem *item);

    void on_tableWidget_books_wished_itemClicked(QTableWidgetItem *item);

    void on_pushButton_delete_book_clicked();

    void on_tableWidget_books_owned_itemDoubleClicked(QTableWidgetItem *item);

    void on_tableWidget_books_wished_itemDoubleClicked(QTableWidgetItem *item);

    void on_pushButton_search_clicked();

    void on_tableWidget_search_result_itemClicked(QTableWidgetItem *item);

    void on_tableWidget_search_result_itemDoubleClicked(QTableWidgetItem *item);

    void on_lineEdit_search_returnPressed();

    void on_tableWidget_authors_itemClicked(QTableWidgetItem *item);

    void on_actionTest_add_random_books_triggered();

private:
    Ui::Lib *ui;
    bool test_activated_ = false;

    QBrush uneven_row_color;
    QBrush fit_color;
};
#endif // LIB_H
