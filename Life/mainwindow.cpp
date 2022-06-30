#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QString>

#include <future>
#include <functional>
#include <vector>
#include <sstream>
#include <locale>
#include "timers.h"

template <class Char>
class MyFacet : public std::numpunct<Char> {
public:
  std::string do_grouping() const { return "\3"; }
  Char do_thousands_sep() const { return ' '; }
};

MainWindow::MainWindow(Life& life, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      life_(&life)
{
    ui->setupUi(this);

    scene_ = new QGraphicsScene(this);
    ui->graphicsView_board->setScene(scene_);
    ui->graphicsView_board->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    QColor color(Qt::green);
    brush_.setColor(color);
    brush_.setStyle(Qt::BrushStyle::SolidPattern);
    pen_.setColor(color);
    pen_.setWidth(0);

    ui->graphicsView_board->scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
    ui->graphicsView_board->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    dots_.resize(life_->GetWidth() * life_->GetHeight());
    std::locale oldLoc = txt_stream_.imbue(std::locale(txt_stream_.getloc(), new MyFacet<char>));

    timer_ = new QTimer();
    connect(timer_, SIGNAL(timeout()), this, SLOT(slotTimerAlarm()));
    timer_->start(100);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::slotTimerAlarm() {
    if (!initialized_) {
        UpdateRenderPars();
        initialized_ = true;
    }
    Draw();
}

void MainWindow::resizeEvent(QResizeEvent* event) {
   QMainWindow::resizeEvent(event);
   UpdateRenderPars();
}

void  MainWindow::Draw() {
    txt_stream_.seekp(0);

    const Board* board_ptr;
    {
        LOG_DURATION_STREAM(" Calculation time", txt_stream_);
        board_ptr = &life_->NextStep();
    }
    {
        LOG_DURATION_STREAM(" Render time", txt_stream_);
        auto &board { *board_ptr };
        for (int row = 0; row < static_cast<int>(board.size()); ++ row) {
            for (int col = 0; col < static_cast<int>(board[row].size());  ++col) {
                if (board[row][col] != 0) {
                    if (!dots_[row * life_->GetWidth() + col]->isVisible()) {
                        dots_[row * life_->GetWidth() + col]->setVisible(true);
                    }
                 } else {
                    if ( dots_[row * life_->GetWidth() + col]->isVisible()) {
                        dots_[row * life_->GetWidth() + col]->setVisible(false);
                    }
                }
            }
        }
    }
    ui->textBrowser->setText(QString::fromStdString(txt_stream_.str()));
}

QGraphicsEllipseItem* MainWindow::AddToScene(int col, int row) {
    return scene_->addEllipse(
                render_pars_.start_x + col *render_pars_.step_x,
                render_pars_.start_y + row * render_pars_.step_y,
                render_pars_.diameter,
                render_pars_.diameter,
                pen_,
                brush_);
}

void MainWindow::UpdateRenderPars() {
    render_pars_.step_x = 1. * ui->graphicsView_board->width() / (life_->GetWidth() + 1);
    render_pars_.step_y = render_pars_.step_x;
    render_pars_.diameter = 0.8 * render_pars_.step_x;
    render_pars_.start_x = 0.5 * render_pars_.diameter;
    render_pars_.start_y = 0.5 * render_pars_.diameter;

    scene_->clear();
    for (int row = 0; row < life_->GetHeight(); ++row) {
        for (int col = 0; col < life_->GetWidth(); ++col) {
            dots_[row * life_->GetWidth() + col] = AddToScene(col, row);
        }
    }
}

