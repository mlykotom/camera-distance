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
    float distance;

    QPoint widgetPosition;
    QRect rectangle;

    DistancePoint(QPoint _widgetPosition, int x, int y): distance(0){
        this->x = x;
        this->y = y;
        this->widgetPosition = _widgetPosition;
        rectangle = QRect(_widgetPosition.x() - rectSize / 2, _widgetPosition.y() - rectSize / 2, rectSize, rectSize);

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
