#include "image_drawer.h"

#include <QPixmap>

#include "global_library.h"

void ImageDrawer::DrawFromStorage(const std::string& title) const
{
    canvas_->setText("IMG");

    if (!canvas_) {
        return;
    }

    std::string file_name = kCoversDir + title + ".jpg";

//    canvas_->setText(QString::fromLocal8Bit(file_name));

    if (title.empty() || !IsFile(file_name)) {
        canvas_->clear();
        return;
    }

    QPixmap img(QString::fromStdString(file_name));
    canvas_->setPixmap(ResizeImg(img));
}

//    void DrawFromNet() const;

void ImageDrawer::DrawTest()
{
    if (!canvas_) {
        return;
    }
    std::string file_name = kCoversDir + "BookCover2.jpg";
    if (!IsFile(file_name)) {
        canvas_->clear();
        return;
    }

    QPixmap img(QString::fromStdString(file_name));
    canvas_->setPixmap(ResizeImg(img));
}

void ImageDrawer::SetTargetWidget(QLabel* target)
{
    canvas_ = target;
}

QPixmap ImageDrawer::ResizeImg(const QPixmap& source) const
{
    int target_height = canvas_->height();
    return source.scaledToHeight(target_height);
}

bool ImageDrawer::IsFile(const std::string& path) const
{
    std::ifstream src(path);
    return src.good();
}


