#ifndef DIALOGADDBOOK_H
#define DIALOGADDBOOK_H

#include <QDialog>

namespace Ui {
class DialogAddBook;
}

class DialogAddBook : public QDialog
{
    Q_OBJECT

        public:
                 explicit DialogAddBook(QWidget *parent = nullptr, size_t index = 0);
    ~DialogAddBook();

             private slots:
                 void on_buttonBox_accepted();

//                 void on_radioButton_rating_1_toggled(bool checked);

                 void on_title_edit_textChanged(const QString& arg1);

             private:
                 void SetFieldsWithCurrentBook();

             private:
    Ui::DialogAddBook *ui;
    size_t index_ = 0; // need for editing book
};

#endif // DIALOGADDBOOK_H
