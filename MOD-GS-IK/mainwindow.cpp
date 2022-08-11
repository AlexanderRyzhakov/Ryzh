#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <optional>
#include <fstream>
#include <string>

#include <QFileDialog>
#include <QString>
#include <QStringList>

#include "msu_gs.h"
#include "global_consts.h"
#include "analisator.h"

void UpdateCache(const MsuGs& gs, const FocusManager& mngr) {
    std::ofstream file;
    file.open(kSerialFile.toLocal8Bit());
    if (file) {
        file << std::string(gs.GetSerialNumber().toLocal8Bit()) << '\n'
             << std::string(gs.GetRhNumber().toLocal8Bit()) << '\n'
             << mngr.GetGSFocus() << '\n'
             << mngr.GetCollimatorFocus() << '\n'
             << mngr.GetTargetCollimatorFocus();
    }
}

void MakeDummyGs(MsuGs& gs)
{
    for (uint8_t i = 0; i < kBandAmount; ++i) {
        Dots dots = kDummyPositions;
        gs.AddDots(i, std::move(dots));
    }
}

MainWindow::MainWindow(FocusManager& focus_mngr,MsuGs& gs, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , focus_mngr_(focus_mngr)
    , gs_(gs)
{
    ui->setupUi(this);
    ui->lineEdit_gs_sn->setText(gs_.GetSerialNumber());
    ui->lineEdit_rh_sn->setText(gs_.GetRhNumber());

    ui->lineEdit_target_col_pos->setText(QString::number(focus_mngr_.GetTargetCollimatorFocus()));
    ui->lineEdit_gs_focus->setText(QString::number(focus_mngr_.GetGSFocus()));
    ui->lineEdit_col_focus->setText(QString::number(focus_mngr_.GetCollimatorFocus()));

    file_manager_.SetOutput(ui->textBrowser);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_lineEdit_gs_focus_returnPressed()
{
    try {
        focus_mngr_.SetGSFocus(ui->lineEdit_gs_focus->text());
        ui->textBrowser->append("MSU-GS focus set to: " + QString::number(focus_mngr_.GetGSFocus()));
        UpdateCache(gs_, focus_mngr_);
    } catch (std::exception& err) {
        ui->textBrowser->append(QString::fromLocal8Bit(err.what()));
    }
}

void MainWindow::on_lineEdit_col_focus_returnPressed()
{
    try {
        focus_mngr_.SetCollimatorFocus(ui->lineEdit_col_focus->text());
        ui->textBrowser->append("Collimator focus set to: " + QString::number(focus_mngr_.GetCollimatorFocus()));
        UpdateCache(gs_, focus_mngr_);
    } catch (std::exception& err) {
        ui->textBrowser->append(QString::fromLocal8Bit(err.what()));
    }
}

void MainWindow::on_lineEdit_target_col_pos_returnPressed()
{
    try {
        focus_mngr_.SetTargetCollimatorFocus(ui->lineEdit_target_col_pos->text());
        ui->textBrowser->append("Target collimator focus set to: " + QString::number(focus_mngr_.GetTargetCollimatorFocus()));
        UpdateCache(gs_, focus_mngr_);
    } catch (std::exception& err) {
        ui->textBrowser->append(QString::fromLocal8Bit(err.what()));
    }
}

void MainWindow::on_lineEdit_gs_sn_returnPressed()
{
    gs_.SetSerialNumber(std::move(ui->lineEdit_gs_sn)->text());
    UpdateCache(gs_, focus_mngr_);
    ui->lineEdit_gs_sn->setText(gs_.GetSerialNumber());
    ui->textBrowser->append("MSU-GS S/N set to: " + gs_.GetSerialNumber());

}

void MainWindow::on_lineEdit_rh_sn_returnPressed()
{
    gs_.SetRhNumber(std::move(ui->lineEdit_rh_sn)->text());
    UpdateCache(gs_, focus_mngr_);
    ui->lineEdit_rh_sn->setText(gs_.GetRhNumber());
    ui->textBrowser->append("MSU-GS RH S/N set to: " + gs_.GetRhNumber());
}

void MainWindow::on_actionOpen_triggered()
{
    QFileDialog file_dialog;
    file_dialog.setAcceptMode(QFileDialog::AcceptOpen);
    file_dialog.setNameFilter("*.dat");
    file_dialog.setFileMode(QFileDialog::ExistingFiles);
    file_dialog.show();

    if (file_dialog.exec()) {
        QStringList file_names;
        file_names = file_dialog.selectedFiles();
        if (file_manager_.AddFiles(file_names)) {
            ui->pushButton_calculate->setEnabled(true);
        }
    }
}

auto MainWindow::Analise(QFile& file) {
    Analisator analisator(file);
    analisator.SetOutput(ui->textBrowser);
    analisator.SetSaveImg(ui->checkBox_save_img->isChecked());
    analisator.SetSaveFFT(ui->checkBox_save_fft->isChecked());
    return analisator.CalculateMod();
}

void MainWindow::on_pushButton_calculate_clicked()
{
    gs_.ClearBands();
    gMiraCache.Clear();
    ui->pushButton_show->setEnabled(false);

    std::map<double, QFile*> failed_files;

    int pos_count = 0;
    for (auto& [pos, file] : file_manager_) {
        auto modulation = Analise(file);
        if (modulation.has_value()) {
            ui->textBrowser->append(file.fileName() + " OK");
            for (int i= 0; i < static_cast<int>(modulation->size()); ++i) {
                gs_.AddDot(i, pos, modulation.value()[i]);
            }
            ++pos_count;
        } else {
            ui->textBrowser->append(file.fileName() + " FAIL");
            failed_files[pos] = &file;
        }
        QCoreApplication::processEvents();
    }

    if (!failed_files.empty()) {
        ui->textBrowser->append("Retry failed files:");
        for (auto& [pos, file_ptr] : failed_files) {
            auto modulation = Analise(*file_ptr);
            if (modulation.has_value()) {
                ui->textBrowser->append(file_ptr->fileName() + " OK");
                for (int i= 0; i < static_cast<int>(modulation->size()); ++i) {
                    gs_.AddDot(i, pos, modulation.value()[i]);
                }
                ++pos_count;
            } else {
                ui->textBrowser->append(file_ptr->fileName() + " FAIL AGAIN no more tries");
            }
            QCoreApplication::processEvents();
        }
    }

    if (pos_count > 2) {
        ui->pushButton_show->setEnabled(true);
        on_pushButton_show_clicked();
    }
}

void MainWindow::on_pushButton_show_clicked()
{
    win_graphic graphic(gs_, focus_mngr_, this);
    graphic.show();
    graphic.Draw();
    graphic.exec();
}

void MainWindow::on_actionClear_cache_triggered()
{
    gMiraCache.Clear();
    ui->textBrowser->append("Cached mira position cleared");
}

// *** TESTS
void MainWindow::on_actionShow_dummy_gs_triggered()
{
    MakeDummyGs(gs_);
    gs_.SetSerialNumber("TEST");
    gs_.SetRhNumber("test");

    ui->textBrowser->append("Creating dummy MSU-GS:");
    for (uint8_t i =  0; i < kBandAmount; ++i) {
        auto band = gs_.GetBand(i);
        QString str;
        for (const auto& [pos, mod]: band.pos_to_mod) {
            str.append(". P = " + QString::number(pos) + ". M = " + QString::number(mod));
        }
        ui->textBrowser->append("Channel #" + QString::number(i + 1) + str);
    }
    on_pushButton_show_clicked();
}

