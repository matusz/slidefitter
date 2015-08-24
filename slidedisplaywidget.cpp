#include "slidedisplaywidget.h"

#include <QtSvg>

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPaintEvent>

#include <QPainter>

SlideDisplayWidget::SlideDisplayWidget(int screen, QWidget *parent) :
    QWidget(parent),
    screen_(screen),
    orientation_(SlideDisplayWidget::Landscape),
    backgroundColor_(0, 0, 0),
    isFinished_(false)
{
}

void SlideDisplayWidget::setOrientation(int orientation)
{
    orientation_ = orientation;
}

void SlideDisplayWidget::setBackgroundColor(const QColor& color)
{
    backgroundColor_ = color;
    update();
}

void SlideDisplayWidget::setFinished(bool finished)
{
    isFinished_ = finished;
    update();
}

void SlideDisplayWidget::play(const Slide& slide)
{
    slide_ = slide;
    update();
}

void SlideDisplayWidget::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_Escape:
        emit escapeKeyPressed();
        break;

    case Qt::Key_Space:
        emit spaceKeyPressed();
        break;

    case Qt::Key_Left:
        emit leftKeyPressed();
        break;

    case Qt::Key_Right:
        emit rightKeyPressed();
        break;
    }

    QWidget::keyPressEvent(event);
}

void SlideDisplayWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        emit leftButtonPressed();
    }

    if (event->button() == Qt::RightButton) {
        emit rightButtonPressed();
    }

    QWidget::mousePressEvent(event);
}

void SlideDisplayWidget::paintEvent(QPaintEvent *paintEvent)
{
    Q_UNUSED(paintEvent);

    QPainter painter;
    painter.begin(this);

    int width = size().width();
    int height = size().height();
    QRect clientRect = QRect(0, 0, width, height);

    painter.fillRect(clientRect, backgroundColor_);

    int slideWidth = slide_.width();
    int slideHeight = slide_.height();

    if (slide_.isNull() || (slideWidth < 1) || (slideHeight < 1)) {
        // generic no picture icon
        QSvgRenderer renderer(QString(":/root/nopicture.svg"));

        int missingPictureImageSize = width;
        if (missingPictureImageSize > height)
            missingPictureImageSize = height;

        missingPictureImageSize = (int)((qreal)missingPictureImageSize * (qreal)0.85);

        QPixmap pixmap(missingPictureImageSize, missingPictureImageSize);
        pixmap.fill(backgroundColor_);

        QPainter pixmapPainter(&pixmap);
        renderer.render(&pixmapPainter, pixmap.rect());

        int pixmapX = (width - missingPictureImageSize)/ 2;
        int pixmapY = (height - missingPictureImageSize)/ 2;

        painter.drawPixmap(pixmapX, pixmapY, pixmap);
    } else {

        bool drawSlide = false;

        // slide will be drawn only if it is correctly oriented
        // (square shaped slides are always correctly oriented)
        if (orientation_ & SlideDisplayWidget::Landscape) {
            if (slide_.width() >= slide_.height())
                drawSlide = true;
        }
        if (orientation_ & SlideDisplayWidget::Portrait) {
            if (slide_.height() >= slide_.width())
                drawSlide = true;
        }

        if (drawSlide) {
            qreal scaleX = qreal(width) / qreal(slideWidth);
            qreal scaleY = qreal(height) / qreal(slideHeight);

            qreal scaleFactor = (scaleX < scaleY) ? scaleX: scaleY;

            slideWidth = (int)(qreal(slideWidth) * scaleFactor);
            slideHeight = (int)(qreal(slideHeight) * scaleFactor);

            QImage scaledImage = slide_.scaled(QSize(slideWidth, slideHeight), Qt::KeepAspectRatio);

            int slideX = (width - scaledImage.width())/ 2;
            int slideY = (height - scaledImage.height())/ 2;

            painter.drawImage(slideX, slideY, scaledImage);
        }
    }

    if (isFinished_) {
        // slide show finished icon
        QSvgRenderer renderer(QString(":/root/finishflag.svg"));

        int finishedpictureImageSize = width;
        if (finishedpictureImageSize > height)
            finishedpictureImageSize = height;

        finishedpictureImageSize = (int)((qreal)finishedpictureImageSize * (qreal)0.85);

        QPixmap pixmap(finishedpictureImageSize, finishedpictureImageSize);

        QPainter pixmapPainter(&pixmap);
        renderer.render(&pixmapPainter, pixmap.rect());

        int pixmapX = (width - finishedpictureImageSize)/ 2;
        int pixmapY = (height - finishedpictureImageSize)/ 2;

        painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
        painter.drawPixmap(pixmapX, pixmapY, pixmap);

        return;
    }
}
