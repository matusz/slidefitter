#include "slideinfomodel.h"

#include "slideinfolist.h"
#include "slideinfo.h"
#include "slideinfocomparator.h"

#include <QBrush>

#include <QDir>
#include <QtAlgorithms>

SlideInfoModel::SlideInfoModel(QObject* parent) :
    QAbstractTableModel(parent)
{
}

QVariant SlideInfoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Vertical) {
        return QString("%1").arg(section + 1);
    }

    if (orientation == Qt::Horizontal) {
        if (section == 0) {
            return tr("Slide (File name)");
        }

        if (section == 1) {
            return tr("Creation time");
        }

        // no header text for thumbnail since it cannot
        // align decoration to center and the text "thumbnail"
        // is much wider then the thumbnail image would be
        if (section == 2) {
            return tr("Thumbnail");
        }
    }

    return QVariant();
}

QVariant SlideInfoModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if ((index.row() < 0) || (index.row() > slideInfoList_.count() - 1))
        return QVariant();

    if (role == Qt::DisplayRole) {
        if (index.column() == 0)
            return slideInfoList_.at(index.row())->fileName();
        if (index.column() == 1)
            return slideInfoList_.at(index.row())->formattedCreationTime();
    }

    if (role == Qt::DecorationRole) {
        if (index.column() == 2)
            return slideInfoList_.at(index.row())->thumbnailImage();
    }

    if (role == Qt::SizeHintRole) {
        if (index.column() == 2)
            return slideInfoList_.at(index.row())->thumbnailImage().size();
    }

    return QVariant();
}

int SlideInfoModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)

    return slideInfoList_.count();
}

int SlideInfoModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)

    // in present implementation there are 3 columns
    // thumbnail, filename and creation time
    return 3;
}

bool SlideInfoModel::removeRows(int row, int count, const QModelIndex & parent)
{
    if (count <= 0)
        return false;

    if ((row < 0) || (row + count) > slideInfoList_.count())
        return false;

    beginRemoveRows(parent, row, row + count - 1);

    for (int i = 0; i < count; i++) {
        // the list will get shorter as the elements are removed
        SlideInfo* pSlideInfo = slideInfoList_.at(row);

        if (pSlideInfo)
            delete pSlideInfo;

        slideInfoList_.removeAt(row);
    }

    endRemoveRows();

    return true;
}

void SlideInfoModel::sort(int column, Qt::SortOrder order)
{
    if (column == 0)
        sortSlides(SlideInfo::SortByFileName, order);

    if (column == 1)
        sortSlides(SlideInfo::SortByCreationTime, order);

    if (column == 2)
        sortSlides(SlideInfo::SortByFileExtension, order);

    emit dataChanged(createIndex(0, 0), createIndex(rowCount() - 1, columnCount() - 1));
}

void SlideInfoModel::swapRows(int row1, int row2)
{
    if ((row1 < 0) || (row1 > slideInfoList_.count() - 1))
        return;

    if ((row2 < 0) || (row2 > slideInfoList_.count() - 1))
        return;

    if (row1 == row2)
        return;

    slideInfoList_.swap(row1, row2);

    emit dataChanged(createIndex(row1, 0), createIndex(row1, columnCount() - 1));
    emit dataChanged(createIndex(row2, 0), createIndex(row2, columnCount() - 1));
}

QStringList SlideInfoModel::canonicalFilePaths(const QModelIndexList& indexes) const
{
    QStringList canonicalFilePaths;

    foreach (QModelIndex index, indexes) {
        SlideInfo* pSlideInfo = slideInfoList_.at(index.row());

        if (!pSlideInfo)
            continue;

        canonicalFilePaths.append(pSlideInfo->canonicalFilePath());
    }

    return canonicalFilePaths;
}

QModelIndexList SlideInfoModel::modelIndexes(const QStringList& canonicalFilePaths) const
{
    QModelIndexList indexes;

    foreach (QString canonicalFilePath, canonicalFilePaths) {
        int row = slideInfoList_.indexOf(canonicalFilePath);

        if (row < 0)
            continue;

        QModelIndex index = createIndex(row, 0);

        indexes.append(index);
    }

    return indexes;
}

bool SlideInfoModel::appendSlide(SlideInfo* pSlideInfo)
{
    if (!pSlideInfo)
        return false;

    beginInsertRows(QModelIndex(), rowCount(), rowCount());

    slideInfoList_.append(pSlideInfo);

    endInsertRows();

    return true;
}

bool SlideInfoModel::removeRows(const QList<int>& rows)
{
    slideInfoList_.setFlag(false);

    foreach (int row, rows) {
        SlideInfo* pSlideInfo = slideInfoList_.at(row);

        if (!pSlideInfo)
            continue;

        pSlideInfo->setFlag(true);
    }

    return removeFlaggedSlides();
}

bool SlideInfoModel::filterSlides(const QStringList& filters)
{
    slideInfoList_.setFlag(false);

    int slides = slideInfoList_.count();

    for (int i = 0; i < slides; i++) {
        SlideInfo* pSlideInfo = slideInfoList_.at(i);

        if (!pSlideInfo)
            continue;

        bool match = false;
        foreach (QString filter, filters) {
            if (QDir::match(filter, pSlideInfo->canonicalFilePath())) {
                match = true;
                break;
            }
        }

        if (!match)
            pSlideInfo->setFlag(true);
    }

    return removeFlaggedSlides();
}

bool SlideInfoModel::removeFlaggedSlides()
{
    bool foundFlagged = false;
    do {
        foundFlagged = false;

        int slides = slideInfoList_.count();

        for (int i = 0; i < slides; i++) {
            SlideInfo* pSlideInfo = slideInfoList_.at(i);

            if (!pSlideInfo)
                continue;

            if (pSlideInfo->flag()) {
                if (!removeRow(i, QModelIndex()))
                    return false;

                foundFlagged = true;
                break;
            }
        }
    } while (foundFlagged);

    return true;
}

void SlideInfoModel::sortSlides(SlideInfo::SortCriteria sortCriteria, Qt::SortOrder sortOrder)
{
    SlideInfoComparator slideInfoComparator(sortCriteria, sortOrder);

    qSort(slideInfoList_.begin(), slideInfoList_.end(), slideInfoComparator);
}
