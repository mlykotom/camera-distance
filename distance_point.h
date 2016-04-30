#ifndef DISTANCE_POINT_H
#define DISTANCE_POINT_H

#include <QPainter>
#include <QString>

struct DistancePoint{
    static const int rectSize = 10;
    static const int fontSize = 15;

    QRect rectangle;
    float distance;

    DistancePoint(int x, int y, float _distance): distance(_distance){
        rectangle = QRect(x - rectSize / 2, y - rectSize / 2, rectSize, rectSize);
    }

    void render(QPainter *painter){
        painter->drawRect(rectangle);
        painter->drawText(rectangle.x(), rectangle.y() + rectangle.height() + fontSize, QString::number(distance, 'f', 2));
    }
};

#endif // DISTANCE_POINT_H
