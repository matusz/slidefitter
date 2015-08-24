#include "slideinfocomparator.h"

#include <QtAlgorithms>

SlideInfoComparator::SlideInfoComparator(SlideInfo::SortCriteria sortCriteria, Qt::SortOrder sortOrder) :
    sortCriteria_(sortCriteria),
    sortOrder_(sortOrder)
{
}

int SlideInfoComparator::operator()(SlideInfo* pSlideInfo1, SlideInfo* pSlideInfo2)
{
    if (!pSlideInfo1 || !pSlideInfo2)
        return 0;

    if (sortCriteria_ == SlideInfo::SortByFileName) {
        if (pSlideInfo1->fileName() == pSlideInfo2->fileName())
            return 0;

        if (sortOrder_ == Qt::AscendingOrder)
            return pSlideInfo1->fileName() < pSlideInfo2->fileName();

        if (sortOrder_ == Qt::DescendingOrder)
            return pSlideInfo1->fileName() > pSlideInfo2->fileName();
    }

    if (sortCriteria_ == SlideInfo::SortByCreationTime) {
        if (pSlideInfo1->formattedCreationTime() == pSlideInfo2->formattedCreationTime())
            return 0;

        if (sortOrder_ == Qt::AscendingOrder)
            return pSlideInfo1->formattedCreationTime() < pSlideInfo2->formattedCreationTime();

        if (sortOrder_ == Qt::DescendingOrder)
            return pSlideInfo1->formattedCreationTime() > pSlideInfo2->formattedCreationTime();
    }

    if (sortCriteria_ == SlideInfo::SortByFileExtension) {
        if (pSlideInfo1->completeSuffix() == pSlideInfo2->completeSuffix())
            return 0;

        if (sortOrder_ == Qt::AscendingOrder)
            return pSlideInfo1->completeSuffix() < pSlideInfo2->completeSuffix();

        if (sortOrder_ == Qt::DescendingOrder)
            return pSlideInfo1->completeSuffix() > pSlideInfo2->completeSuffix();
    }

    return 0;
}
