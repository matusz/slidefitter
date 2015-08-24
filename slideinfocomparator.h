#ifndef SLIDEINFOCOMPARATOR_H
#define SLIDEINFOCOMPARATOR_H

#include "slideinfo.h"

class SlideInfoComparator
{
public:
    explicit SlideInfoComparator(SlideInfo::SortCriteria sortCriteria, Qt::SortOrder sortOrder);

    int operator()(SlideInfo* pSlideInfo1, SlideInfo* pSlideInfo2);

protected:
    SlideInfo::SortCriteria sortCriteria_;
    Qt::SortOrder sortOrder_;
};

#endif // SLIDEINFOCOMPARATOR_H
