#include "glwidget.h"
#include "QDebug"
#include <QQueue>

#define FRAME_TIMEOUT_MS 10

GLWidget::GLWidget(bool showRect_, QList<QString> *distanceStringsList_, QQueue<QPair<QImage, float>> *q_, QWidget *parent):
    QGLWidget(QGLFormat(QGL::DoubleBuffer),parent),
    rectHeight(20),
    rectWidht(20),
    showRect(showRect_),
    multipleMeasuringPoints(false),
    distanceStringsList(distanceStringsList_),
    q(q_),
    texture(NULL)
{
    _image = QImage(QSize(200,90), QImage::Format_RGB888);
    _image.fill(Qt::white);

    // singleRect = QRectF(0,0,rectWidht,rectHeight);
    singleTextPoint = QPointF(0,0);
    singleDistanceString = "";
    setAutoFillBackground(false);
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(300, 200);
}

void GLWidget::mousePressEvent(QMouseEvent *e)
{
    if(!showRect) return;

    int x = e->pos().x()-10;
    int y = e->pos().y()-10;

    if(multipleMeasuringPoints){
        rectList.append(QRectF(x,y,rectWidht,rectHeight));
        textPointsList.append(QPointF(x,y+35));
        //distanceStringsList.append("");
    }
    else{
        singleRect = QRectF(x,y,rectWidht,rectHeight);
        singleTextPoint = QPointF(x,y+35);
    }

    // TODO
    int frameX = (x / (double)this->width()) * 320;
    int frameY = (y / (double)this->height()) * 240;
    emit measuringPointCoordsChanged(frameX, frameY);
}



void GLWidget::onNumberOfMeasuringPointsChanged(bool multipleMeasuringPoints_)
{
    multipleMeasuringPoints = multipleMeasuringPoints_;

    // multiple points turned on
    if(multipleMeasuringPoints_){

        //hide single rect
        singleRect = QRectF();
        singleTextPoint = QPointF();

    }
    else{
        //clear multiple points

        rectList.clear();
        textPointsList.clear();
        distanceStringsList->clear();
    }

    //update();
}

void GLWidget::onPointsClear()
{
    rectList.clear();
    textPointsList.clear();
    distanceStringsList->clear();

    singleRect = QRectF();
    singleTextPoint = QPointF();
}

void GLWidget::onNewFrame()
{
    if(q->empty()) {
        // skipping empty queue, that should not happen
        return;
    }

    QPair<QImage,float> cameraFrame = q->dequeue();

    singleDistanceString = QString::number(cameraFrame.second,'f',2);


    if(texture == NULL){
        texture = new QOpenGLTexture(cameraFrame.first);
        texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
        texture->setMagnificationFilter(QOpenGLTexture::Linear);
    }
    else{
        texture->destroy();
        texture->create();
        texture->setMipLevels(0);
        texture->setData(cameraFrame.first);
    }

    texture->bind();
    update();
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    qglClearColor(Qt::black);
    setupViewPort(width(), height());
}



//void GLWidget::paintGL()
void GLWidget::paintEvent(QPaintEvent *event)
{
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glClear(GL_COLOR_BUFFER_BIT);
    // draw a textured quad
    drawFramePicture();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    QPainter painter(this);
    if(showRect){
        painter.setPen(Qt::red);
        painter.drawRect(singleRect);
        painter.drawText(singleTextPoint,singleDistanceString);
    }
    painter.end();
}

/**
 * Draws rectangle with picture texture on it
 * @brief GLWidget::makeFrameObject
 */
void GLWidget::drawFramePicture(){
    glBegin(GL_QUADS);
    // -- start specifying vertices
    glTexCoord2d(0, 0);
    glVertex2i(-1, -1);
    glTexCoord2d(0, 1);
    glVertex2i(-1, 1);
    glTexCoord2d(1, 1);
    glVertex2i(1, 1);
    glTexCoord2d(1, 0);
    glVertex2i(1, -1);
    // -- end specifying vertices
    glEnd();
}

void GLWidget::resizeGL(int width, int height)
{
    setupViewPort(width, height);
}

/**
 * Setups openGL viewport
 * @brief GLWidget::setupViewPort
 * @param width
 * @param height
 */
void GLWidget::setupViewPort(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);
}


//void GLWidget::paintEvent(QPaintEvent *event)
//{
//    if(!showRect) return;
//    QPainter painter;
//    painter.begin(this);

//    painter.setPen(Qt::red);

//    if(multipleMeasuringPoints){
//        for(int i = 0; i < rectList.length() ; i++){
//            painter.drawRect(rectList.at(i));
//            painter.drawText(textPointsList.at(i),distanceStringsList->at(i));
//        }
//    }
//    else{
//        painter.drawRect(singleRect);
//        painter.drawText(singleTextPoint,singleDistanceString);
//    }

//    painter.end();
//}



