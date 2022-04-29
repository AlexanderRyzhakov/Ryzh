#ifndef DIALOGADDBOOK_H
#define DIALOGADDBOOK_H

#include <QDialog>

#include "image_drawer.h"

namespace Ui {
class DialogAddBook;
}

class DialogAddBook : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAddBook(QWidget *parent = nullptr, size_t index = 0, ImageDrawer *drawer = nullptr);
    ~DialogAddBook();

private slots:
    void on_buttonBox_accepted();

//                 void on_radioButton_rating_1_toggled(bool checked);

     void on_title_edit_textChanged(const QString& arg1);

 private:
     void RenameCoverFile(const std::string& new_title) const;

     void SetFieldsWithCurrentBook();

private:
    Ui::DialogAddBook *ui;
    ImageDrawer* image_dawer_ = nullptr;
    size_t index_ = 0; // need for editing book
    std::string old_title_; // needed for changing cover file name
};

#endif // DIALOGADDBOOK_H
