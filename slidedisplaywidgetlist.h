#ifndef SLIDEDISPLAYWIDGETLIST_H
#define SLIDEDISPLAYWIDGETLIST_H

#include <QObject>
#include <QList>

class Slide;
class SlideDisplayWidget;

// it must inherit publicly QObject in order
// to be able to emit signals
class SlideDisplayWidgetList : public QObject
{
    Q_OBJECT
public:
    SlideDisplayWidgetList(QObject* parent = 0);

    ~SlideDisplayWidgetList();

    void adjustNumber(int newCount);
    void create();
    void resize(int screen);

    void setVisibile(bool visible);
    void play(const Slide& slide);
    void setFinished(bool finished);

    void showFullScreen();

signals:
    void escapeKeyPressed();
    void spaceKeyPressed();
    void leftKeyPressed();
    void rightKeyPressed();

    void leftButtonPressed();
    void rightButtonPressed();

protected:
    void setOrientation();

    bool foundLandscapeScreen() const;
    bool foundPortraitScreen() const;

protected:
    QList<SlideDisplayWidget*> slideDisplayWidgets_;
};

#endif // SLIDEDISPLAYWIDGETLIST_H
