#ifndef DISTANCE_POINT_H
#define DISTANCE_POINT_H

#include <QPainter>
#include <QString>
#include <QDebug>

struct DistancePoint{
    static const int rectSize = 10;
    static const int fontSize = 10;

    int x;
    int y;

    QPoint widgetPosition;
    QSize widgetSize;
    QRect rectangle;
    float distance;

    DistancePoint(QPoint _widgetPosition, QSize _widgetSize, int x, int y): distance(0){
        this->x = x;
        this->y = y;
        this->widgetSize = _widgetSize;
        this->widgetPosition = _widgetPosition;
        rectangle = QRect();
        updateRectPosition();
    }

    void updateRectPosition(){
        rectangle.setLeft(widgetPosition.x() - rectSize / 2);
        rectangle.setTop(widgetPosition.y() - rectSize / 2);
        rectangle.setWidth(rectSize);
        rectangle.setHeight(rectSize);
    }

    void updateWidgetPos(int canvasWidth, int canvasHeight){
        int newX = canvasWidth * widgetPosition.x() / widgetSize.width();
        int newY = canvasHeight * widgetPosition.y() / widgetSize.height();
        this->widgetPosition = QPoint(newX, newY);
        this->widgetSize = QSize(canvasWidth, canvasHeight);
        updateRectPosition();
    }

    void render(QPainter *painter){
        QPen pen = painter->pen();
        painter->setPen(Qt::red);
        painter->drawRect(rectangle);
        painter->drawText(widgetPosition.x() - rectangle.width() - fontSize, widgetPosition.y() + rectangle.height() + fontSize, QString::number(distance, 'f', 2));
        painter->setPen(pen);
    }
};

#endif // DISTANCE_POINT_H
