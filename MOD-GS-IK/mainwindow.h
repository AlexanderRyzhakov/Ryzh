#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "win_graphic.h"
#include "msu_gs.h"
#include "file_manager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(FocusManager& focus_mngr,MsuGs& gs, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionExit_triggered();

    void on_lineEdit_gs_focus_returnPressed();

    void on_lineEdit_col_focus_returnPressed();

    void on_lineEdit_target_col_pos_returnPressed();

    void on_lineEdit_gs_sn_returnPressed();

    void on_lineEdit_rh_sn_returnPressed();

    void on_actionShow_dummy_gs_triggered();

    void on_actionOpen_triggered();

    void on_pushButton_calculate_clicked();

    void on_pushButton_show_clicked();

    void on_actionClear_cache_triggered();

private:
    auto Analise(QFile& file);

private:
    Ui::MainWindow *ui;
    FocusManager& focus_mngr_;
    MsuGs& gs_;
    FileManager file_manager_;
};
#endif // MAINWINDOW_H
