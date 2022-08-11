#ifndef WIN_GRAPHIC_H
#define WIN_GRAPHIC_H

#include <QDialog>
#include <QColor>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTextItem>
#include <QPen>

#include "msu_gs.h"
#include "global_consts.h"

struct DrawZone {
    double top_left_x {};
    double top_left_y {};

    double bottom_right_x {};
    double bottom_right_y {};

    double single_grphic_y_zone {};
    double ordinata_height {};
};

namespace Ui {
class win_graphic;
}

class win_graphic : public QDialog
{
    Q_OBJECT

public:
    explicit win_graphic(MsuGs& gs, const FocusManager& f_mngr, QWidget *parent = nullptr);
    ~win_graphic();

    void Draw();

private slots:
    void on_pushButton_clicked();

private:
    void DrawHead();
    void DrawPlot();
    void DrawGraphic();
    void UpdateGraphicZone();
    void DrawCurve(std::vector<QPointF>&& dots, double zero_pos, double scale_y);

private:
    Ui::win_graphic *ui;
    QGraphicsScene* scene_;

    MsuGs& gs_;
    const FocusManager& focus_mngr_;

    DrawZone zone_;
};

#endif // WIN_GRAPHIC_H
