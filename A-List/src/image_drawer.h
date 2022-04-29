#pragma once

#include <QGraphicsView>
#include <QString>
#include <QLabel>

#include <fstream>

class ImageDrawer {
public:
    void DrawFromStorage(const std::string& title) const;

    void DrawFromNet() const;

    void DrawTest();

    void SetTargetWidget(QLabel *target);

private:
    bool IsFile(const std::string& path) const;

    QPixmap ResizeImg(const QPixmap& source) const;

private:
    QLabel* canvas_ = nullptr;
};

//QGraphicsScene scene;
//QGraphicsView view(&scene);
//QGraphicsPixmapItem item(QPixmap("c:\\test.png"));
//scene.addItem(&item);
//view.show();
