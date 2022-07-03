#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QColor>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMainWindow>
#include <QPainter>
#include <QTimer>

#include <memory>
#include <sstream>

#include "life.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

struct RenderPars {
    double start_x {};
    double start_y {};
    double step_x { 5 };
    double step_y { 5 };
    double diameter { 5 };
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(Life& life, QWidget* parent = nullptr);
    ~MainWindow();

    void resizeEvent(QResizeEvent* event);

private slots:
    void slotTimerAlarm();

private:
    QGraphicsEllipseItem* AddToScene(int col, int row);

    void Draw();

    void UpdateRenderPars();

private:
    Ui::MainWindow* ui;
    QTimer* timer_;

    QGraphicsScene* scene_;
    QBrush brush_;
    QPen pen_;
    QGraphicsEllipseItem* ellipse_;

    RenderPars render_pars_;

    bool initialized_ = false;
    std::stringstream txt_stream_;

    std::shared_ptr<Life> life_;

    std::vector<QGraphicsEllipseItem*> dots_;
};
#endif // MAINWINDOW_H
