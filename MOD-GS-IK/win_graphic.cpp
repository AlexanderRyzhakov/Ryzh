#include "win_graphic.h"
#include "ui_win_graphic.h"

#include <QString>
#include <QDateTime>
#include <QPainterPath>
#include <QFileDialog>

#include <vector>

#include "spline.h"

struct MaxModPos {
    double mod {};
    double pos {};
};

MaxModPos MaxMod(const Band& band)
{
    MaxModPos result;
    for (const auto& [pos, mod] : band.pos_to_mod) {
        if (mod > result.mod) {
            result.mod = mod;
            result.pos = pos;
        }
    }
    return result;
}

win_graphic::win_graphic(MsuGs& gs, const FocusManager& focus_mngr, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::win_graphic),
    gs_(gs),
    focus_mngr_(focus_mngr)
{
    ui->setupUi(this);

    scene_ = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene_);
    ui->graphicsView->setAlignment(Qt::AlignTop | Qt::AlignLeft);
}

win_graphic::~win_graphic()
{
    delete ui;
}


void win_graphic::Draw()
{
    DrawHead();
    UpdateGraphicZone();
    DrawPlot();
    DrawGraphic();
}

void win_graphic::UpdateGraphicZone()
{
    auto height = ui->graphicsView->height();
    auto width = ui->graphicsView->width();

    zone_.top_left_x = width * 0.05;
    zone_.top_left_y = height * 0.05;
    zone_.bottom_right_x = width * 0.95;
    zone_.bottom_right_y = height * 0.95;
    zone_.single_grphic_y_zone = (zone_.bottom_right_y - zone_.top_left_y) / kBandAmount;
    zone_.ordinata_height = zone_.single_grphic_y_zone * 0.75;
}

void win_graphic::DrawGraphic()
{
    QPen linear_pen;
    linear_pen.setColor(Qt::blue);
    linear_pen.setStyle(Qt::DotLine);
    linear_pen.setWidth(2);

    auto range = focus_mngr_.GetFocusRange(gs_);

    for (int channel = 1; channel <= kBandAmount; ++channel) {
        auto& band = gs_.GetBand(channel - 1);
        auto max_mod_pos = MaxMod(band);

        std::vector<QPointF> dots;
        double scale_y = 0.8 * zone_.ordinata_height / max_mod_pos.mod;
        int pos_number = 0;
        for (const auto& [pos, mod] : band.pos_to_mod) {
            QPointF point;
            double scale_x = (pos - range.min_pos) / (range.max_pos - range.min_pos);
            double x  = zone_.top_left_x + (zone_.bottom_right_x - zone_.top_left_x) * scale_x;
            point.setX(x);
            point.setY(zone_.top_left_y + channel * zone_.single_grphic_y_zone - mod * scale_y);
            dots.push_back(point);
            ++pos_number;
        }

        QPainterPath linear_path(dots[0]);

        for (uint16_t i = 1; i < dots.size(); ++i) {
            linear_path.lineTo(dots[i]);
        }
        scene_->addPath(linear_path, linear_pen);
        DrawCurve(std::move(dots), zone_.top_left_y + channel * zone_.single_grphic_y_zone, scale_y);
    }
}

 void win_graphic::DrawCurve(std::vector<QPointF>&& dots, double zero_pos, double scale_y)
 {
         QPen pen;
         pen.setColor(Qt::red);
         pen.setStyle(Qt::DotLine);
         pen.setWidth(2);
         std::vector<double> x_dots(dots.size());
         std::vector<double> y_dots(dots.size());

         for (size_t i = 0; i < dots.size(); ++i) {
             x_dots[i] = dots[i].rx();
             y_dots[i] = dots[i].ry();
         }

         tk::spline spl(x_dots, y_dots);
         QPainterPath path(dots[0]);
         QPointF max_mod_coord{0, zero_pos};

         for (double x = zone_.top_left_x; x < zone_.bottom_right_x; x+=5) {
             double y = spl(x);
             QPointF point(x, y);
             if (y < max_mod_coord.ry()) {
                 max_mod_coord = point;
             }
             path.lineTo(point);
         }
         scene_->addPath(path, pen);
         scene_->addLine(max_mod_coord.rx(), zero_pos, max_mod_coord.rx(), zero_pos - zone_.ordinata_height, pen);

         auto range = focus_mngr_.GetFocusRange(gs_);
         double max_pos = range.min_pos
                          + (range.max_pos - range.min_pos)
                          * (( max_mod_coord.rx()- zone_.top_left_x) / (zone_.bottom_right_x - zone_.top_left_x));
         double max_mod = (zero_pos - max_mod_coord.ry()) / scale_y;
         double delta = std::abs(max_pos - focus_mngr_.GetTargetCollimatorFocus())
                        * (focus_mngr_.GetGSFocus() / focus_mngr_.GetCollimatorFocus())
                        * (focus_mngr_.GetGSFocus() / focus_mngr_.GetCollimatorFocus());

         QString text("Max m = " + QString::number(max_mod) + "\nPos = " + QString::number(max_pos) + "\n|delta| = " + QString::number(delta));
         QGraphicsTextItem* text_itm = scene_->addText(text);
         text_itm->setPos(zone_.top_left_x, zero_pos - zone_.ordinata_height);
         text_itm->setDefaultTextColor(Qt::black);
 }

void win_graphic::DrawPlot()
{
    auto range = focus_mngr_.GetFocusRange(gs_);
    for (int band_num = 1; band_num <= kBandAmount; ++ band_num) {
        scene_->addLine(zone_.top_left_x,
                        zone_.top_left_y + band_num * zone_.single_grphic_y_zone,
                        zone_.bottom_right_x,
                        zone_.top_left_y + band_num * zone_.single_grphic_y_zone);
        scene_->addLine(zone_.top_left_x,
                        zone_.top_left_y + band_num * zone_.single_grphic_y_zone,
                        zone_.top_left_x,
                        zone_.top_left_y + band_num * zone_.single_grphic_y_zone - zone_.ordinata_height);

        double scale = (focus_mngr_.GetTargetCollimatorFocus() - range.min_pos) / (range.max_pos - range.min_pos);
        double x  = zone_.top_left_x + (zone_.bottom_right_x - zone_.top_left_x) * scale;
        scene_->addLine(x,
                        zone_.top_left_y + band_num * zone_.single_grphic_y_zone,
                        x,
                        zone_.top_left_y + band_num * zone_.single_grphic_y_zone - zone_.ordinata_height);

        for (int pos = 0; pos < static_cast<int>(range.positions.size()); ++pos) {
            auto* text = scene_->addText(QString::number(range.positions[pos]));
            text->setDefaultTextColor(Qt::black);

            double scale = (range.positions[pos] - range.min_pos) / (range.max_pos - range.min_pos);
            double x  = zone_.top_left_x + (zone_.bottom_right_x - zone_.top_left_x) * scale;
            text->setPos(x + text->textWidth()*10,
                         zone_.top_left_y + band_num * zone_.single_grphic_y_zone + zone_.ordinata_height * 0.01);
        }
    }
}

void win_graphic::DrawHead()
{
    QString str;

    QDateTime date = QDateTime::currentDateTime();
    QString format {"dd.MM.yyyy"};
    date.toString(format);
    str += date.toString(format);
    str += '\n';
    str += "MSU-GS s/n: " + gs_.GetSerialNumber() + ". RH s/n: " + gs_.GetRhNumber();
    QGraphicsTextItem *text = scene_->addText(str);
    text->setDefaultTextColor(Qt::black);

}

void win_graphic::on_pushButton_clicked()
{
    QString fileName= QFileDialog::getSaveFileName(this, "Save image", QCoreApplication::applicationDirPath(), "JPEG (*.JPEG)" );
        if (!fileName.isNull())
        {
            QPixmap pixMap = this->ui->graphicsView->grab();
            pixMap.save(fileName);
        }
}
