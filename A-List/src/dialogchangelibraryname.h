#ifndef DIALOGCHANGELIBRARYNAME_H
#define DIALOGCHANGELIBRARYNAME_H

#include <QDialog>

namespace Ui {
class DialogChangeLibraryName;
}

class DialogChangeLibraryName : public QDialog
{
    Q_OBJECT

        public:
                 explicit DialogChangeLibraryName(QWidget *parent = nullptr);
    ~DialogChangeLibraryName();

             private slots:
                 void on_buttonBox_accepted();

//                 void on_textEdit_textChanged();

             private:
    Ui::DialogChangeLibraryName *ui;
};

#endif // DIALOGCHANGELIBRARYNAME_H
