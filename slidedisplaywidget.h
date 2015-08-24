#ifndef SLIDEDISPLAYWIDGET_H
#define SLIDEDISPLAYWIDGET_H

#include <QWidget>
#include <QColor>

#include "slide.h"

class QKeyEvent;
class QMouseEvent;
class QPaintEvent;

class SlideDisplayWidget : public QWidget
{
    Q_OBJECT
public:
    // new values have to contain only one bit of 1
    // this basically in order to be able to combine
    // values by | logical operator
    enum Orientation {Landscape = 0x01, Portrait = 0x02};

    explicit SlideDisplayWidget(int screen, QWidget *parent = 0);

    int screen() const {return screen_;}

    void setOrientation(int orientation);
    int orientation() const {return orientation_;}

    void setBackgroundColor(const QColor& color);
    QColor backgroundColor() const {return backgroundColor_;}

    void setFinished(bool finished);
    bool isFinished() const {return isFinished_;}

    void play(const Slide& slide);

signals:
    void escapeKeyPressed();
    void spaceKeyPressed();
    void leftKeyPressed();
    void rightKeyPressed();

    void leftButtonPressed();
    void rightButtonPressed();

protected:
    void keyPressEvent(QKeyEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void paintEvent(QPaintEvent* paintEvent);

protected:
    int screen_;

    int orientation_;

    QColor backgroundColor_;
    Slide slide_;

    bool isFinished_;
};

#endif // SLIDEDISPLAYWIDGET_H
