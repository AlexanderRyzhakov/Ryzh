#ifndef WIN_DIALOG_SAVE_IF_EDITED_H
#define WIN_DIALOG_SAVE_IF_EDITED_H

#include <QDialog>

namespace Ui {
class win_Dialog_save_if_edited;
}

class win_Dialog_save_if_edited : public QDialog
{
    Q_OBJECT

        public:
                 explicit win_Dialog_save_if_edited(QWidget *parent = nullptr, bool* execute_action = nullptr);
    ~win_Dialog_save_if_edited();

             private slots:
                 void on_buttonBox_accepted();

                 void on_buttonBox_rejected();

             private:
    Ui::win_Dialog_save_if_edited *ui;
    bool* accepted_;
};

#endif // WIN_DIALOG_SAVE_IF_EDITED_H
