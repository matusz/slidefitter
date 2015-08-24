#ifndef SLIDEINFOMODEL_H
#define SLIDEINFOMODEL_H

//#include <QAbstractTableModel>
#include <QStandardItemModel>

#include "slideinfo.h"
#include "slideinfolist.h"

class SlideInfoModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit SlideInfoModel(QObject* parent = 0);

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;

    bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);

    void swapRows(int row1, int row2);

    QStringList canonicalFilePaths(const QModelIndexList& indexes) const;
    QModelIndexList modelIndexes(const QStringList& canonicalFilePaths) const;

    bool appendSlide(SlideInfo* pSlideInfo);

    bool removeRows(const QList<int>& rows);
    bool filterSlides(const QStringList& filters);

    bool containsSlideWithCanonicalFilePath(const QString& canonicalFilePath) const {return slideInfoList_.containsSlideWithCanonicalFilePath(canonicalFilePath);}
    int indexOf(const QString& canonicalFilePath) const {return slideInfoList_.indexOf(canonicalFilePath);}

    void reset() {slideInfoList_.reset();}
    void setFirstSlideAt(int index) {slideInfoList_.setFirstSlideAt(index);}
    SlideInfo* currentSlideInfo() {return slideInfoList_.currentSlideInfo();}
    SlideInfo* currentSlideInfo(int fromCursor) {return slideInfoList_.currentSlideInfo(fromCursor);}

    void setNextSlide() {slideInfoList_.setNextSlide();}

    bool isSlideListFinished() const {return slideInfoList_.isSlideListFinished();}

    bool isDescreasingSlideIndex() const {return slideInfoList_.isDescreasingSlideIndex();}
    void setDescreasingSlideIndex(bool descreasingSlideIndex) {slideInfoList_.setDescreasingSlideIndex(descreasingSlideIndex);}

signals:

public slots:

protected:
    bool removeFlaggedSlides();
    void sortSlides(SlideInfo::SortCriteria sortCriteria, Qt::SortOrder sortOrder);

protected:
    SlideInfoList slideInfoList_;
};

#endif // SLIDEINFOMODEL_H
