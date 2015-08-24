#ifndef SLIDEINFOLIST_H
#define SLIDEINFOLIST_H

#include <QList>

class SlideInfo;

class SlideInfoList : public QList<SlideInfo*>
{
public:
    SlideInfoList();
    SlideInfoList(const SlideInfoList& other);

    ~SlideInfoList();

    bool containsSlideWithCanonicalFilePath(const QString& canonicalFilePath) const;
    int indexOf(const QString& canonicalFilePath) const;

    void setFlag(bool flag);

    void reset();
    void setFirstSlideAt(int index);
    SlideInfo* currentSlideInfo();
    SlideInfo* currentSlideInfo(int fromCursor);

    void setNextSlide();

    bool isSlideListFinished() const;

    bool isDescreasingSlideIndex() const {return descreasingSlideIndex_;}
    void setDescreasingSlideIndex(bool descreasingSlideIndex) {descreasingSlideIndex_ = descreasingSlideIndex;}

protected:
    int cursorIndex_;

    bool descreasingSlideIndex_;
};

#endif // SLIDEINFOLIST_H
