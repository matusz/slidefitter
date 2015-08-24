#include "slideinfolist.h"
#include "slideinfo.h"

#include <QtAlgorithms>

SlideInfoList::SlideInfoList() :
    cursorIndex_(0),
    descreasingSlideIndex_(false)
{
}

SlideInfoList::SlideInfoList(const SlideInfoList& other) :
    QList<SlideInfo*>(other),
    cursorIndex_(0),
    descreasingSlideIndex_(false)
{
}

SlideInfoList::~SlideInfoList()
{
    qDeleteAll(*this);
}

bool SlideInfoList::containsSlideWithCanonicalFilePath(const QString& canonicalFilePath) const
{
    for (QList<SlideInfo*>::const_iterator iter = begin(); iter != end(); iter++) {
        if (*iter == NULL)
            continue;

        if ((*iter)->canonicalFilePath() == canonicalFilePath)
            return true;
    }

    return false;
}

int SlideInfoList::indexOf(const QString& canonicalFilePath) const
{
    int slides = count();
    for (int i = 0; i < slides; i++) {
        SlideInfo* pSlideInfo = at(i);

        if (!pSlideInfo)
            continue;

        if (pSlideInfo->canonicalFilePath() == canonicalFilePath)
            return i;
    }

    return -1;
}

void SlideInfoList::setFlag(bool flag)
{
    for (QList<SlideInfo*>::iterator iter = begin(); iter != end(); iter++) {
        if (*iter == NULL)
            continue;

        (*iter)->setFlag(flag);
    }
}

void SlideInfoList::reset()
{
    cursorIndex_ = 0;
}

void SlideInfoList::setFirstSlideAt(int index)
{
    cursorIndex_ = index;
}

SlideInfo* SlideInfoList::currentSlideInfo()
{
    if ((cursorIndex_ < 0) || cursorIndex_ > count() - 1)
        return NULL;

    return at(cursorIndex_);
}

void SlideInfoList::setNextSlide()
{
    if (descreasingSlideIndex_)
        cursorIndex_--;
    else
        cursorIndex_++;
}

SlideInfo* SlideInfoList::currentSlideInfo(int fromCursor)
{
    cursorIndex_ = fromCursor;
    return currentSlideInfo();
}

bool SlideInfoList::isSlideListFinished() const
{
    if (descreasingSlideIndex_) {
        if (cursorIndex_ < 0)
            return true;
    } else {
        if (cursorIndex_ > count() - 1)
            return true;
    }

    return false;
}
