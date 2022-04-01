#ifndef WIN_DELETE_BOOK_H
#define WIN_DELETE_BOOK_H

#include <QDialog>

namespace Ui {
class win_delete_book;
}

class win_delete_book : public QDialog
{
    Q_OBJECT

        public:
                 explicit win_delete_book(QWidget *parent = nullptr, size_t id = 0);
    ~win_delete_book();

             private slots:
                 void on_buttonBox_accepted();

             private:
    Ui::win_delete_book *ui;
    size_t index_;
};

#endif // WIN_DELETE_BOOK_H
