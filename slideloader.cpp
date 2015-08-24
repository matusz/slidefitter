#include "slideloader.h"

#include "slideinfo.h"
#include "slide.h"

SlideLoader::SlideLoader(QObject *parent) :
    QThread(parent),
    pSlideInfo_(NULL)
{
}

SlideLoader::~SlideLoader()
{
    wait();
}

void SlideLoader::loadSlide(SlideInfo* pSlideInfo)
{
    // it will wait until worker thread is finished, before
    // launching it again what means that there is no need
    // for mutexes or similar stuff
    wait();

    pSlideInfo_ = pSlideInfo;

    start();
}

Slide SlideLoader::slide()
{
    wait();

    return slide_;
}

void SlideLoader::run()
{
    if (!pSlideInfo_) {
        slide_ = Slide();
        return;
    }

    QImage tmpImage;
    if (!tmpImage.load(pSlideInfo_->canonicalFilePath())) {
        slide_ = Slide();
        return;
    }

    slide_ = SlideInfo::imageWithOrientation(tmpImage, pSlideInfo_->orientation());
}
