#ifndef SLIDELOADER_H
#define SLIDELOADER_H

#include <QThread>

#include "slide.h"

class SlideInfo;

class SlideLoader : public QThread
{
    Q_OBJECT
public:
    explicit SlideLoader(QObject *parent = 0);
    ~SlideLoader();

    void loadSlide(SlideInfo* pSlideInfo);
    Slide slide();

signals:

public slots:

protected:
    void run();

protected:
    SlideInfo* pSlideInfo_;
    Slide slide_;
};

#endif // SLIDELOADER_H
