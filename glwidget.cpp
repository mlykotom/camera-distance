#include "glwidget.h"
#include <QDebug>

GLWidget::GLWidget(QList<QSharedPointer<DistancePoint>> *_distancePointList, QQueue<QImage> *q_, QWidget *parent):
    QGLWidget(QGLFormat(QGL::DoubleBuffer),parent),
    distancePointList(_distancePointList),
    imageDistanceQueue(q_),
    texture(NULL)
{
    _image = QImage(QSize(320,240), QImage::Format_RGB888);
    _image.fill(Qt::white);

    setAutoFillBackground(false);
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(320, 240);
}

QSize GLWidget::sizeHint() const
{
    return QSize(640, 480);
}

void GLWidget::mousePressEvent(QMouseEvent *e)
{
    emit measuringPointCoordsChanged(e->pos(), this->size());
}

/**
  When new frame is sent to the widget
 * @brief GLWidget::onNewFrame
 */
void GLWidget::onNewFrame()
{
    // skipping empty queue..that should never happen
    if(imageDistanceQueue->empty()) return;

    QImage cameraFrame = imageDistanceQueue->dequeue();

    if(texture == NULL){
        texture = new QOpenGLTexture(cameraFrame);
        texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
        texture->setMagnificationFilter(QOpenGLTexture::Linear);
    }
    else{
        texture->destroy();
        texture->create();
        texture->setMipLevels(0);
        texture->setData(cameraFrame);
    }

    texture->bind();
    update();
}

/**
 * Initializing openGL options
 * @brief GLWidget::initializeGL
 */
void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    qglClearColor(Qt::black);
    setupViewPort(width(), height());
}

/**
 * HW accelerated rendering (GL + QT drawing)
 * @brief GLWidget::paintEvent
 * @param event
 */
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
    // distance list (not when depth map showing)
    if(distancePointList != NULL){
        // render all checked points
        for(int i = 0; i < distancePointList->length(); i++){
            QSharedPointer<DistancePoint> point = distancePointList->at(i);
            point->render(&painter);
        }
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

/**
 * When GL recognizes resize
 * @brief GLWidget::resizeGL
 * @param width
 * @param height
 */
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


