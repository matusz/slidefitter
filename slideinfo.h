#ifndef SLIDEINFO_H
#define SLIDEINFO_H

#include <QFileInfo>
#include <QDateTime>
#include <QImage>

class SlideInfo
{
public:
    enum SortCriteria {SortByFileName = 0, SortByCreationTime = 1, SortByFileExtension = 2};

    enum Orientation {OrientationNormal = 0,
                      OrientationHorizontalFlip = 1,
                      OrientationRotate180 = 2,
                      OrientationVerticalFlip = 3,
                      OrientationTranspose = 4,
                      OrientationRotate90 = 5,
                      OrientationTransverse = 6,
                      OrientationRotate270 = 7};

    SlideInfo();
    SlideInfo(const QString& file);
    SlideInfo(const QFile& file);
    SlideInfo(const QDir& dir, const QString& file);
    SlideInfo(const QFileInfo& fileinfo);

    virtual ~SlideInfo();

    virtual Orientation orientation() const;
    virtual QString formattedCreationTime() const;
    virtual QImage thumbnailImage() const;

    QString fileName() const;
    QString completeSuffix() const;

    QString canonicalFilePath() const;

    void setFlag(bool flag) {flag_ = flag;}
    bool flag() const {return flag_;}

    static QImage imageWithOrientation(const QImage& originalImage, Orientation orientation);

protected:
    QString creationTimeFormat_;
    QFileInfo fileInfo_;
    bool flag_;
};

#endif // SLIDEINFO_H
