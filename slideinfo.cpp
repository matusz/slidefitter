#include "slideinfo.h"

SlideInfo::SlideInfo() :
    creationTimeFormat_("yyyy.MM.dd hh:mm:ss"),
    flag_(false)
{
}

SlideInfo::SlideInfo(const QString& file) :
    creationTimeFormat_("yyyy.MM.dd hh:mm:ss"),
    fileInfo_(file),
    flag_(false)
{
}

SlideInfo::SlideInfo(const QFile& file) :
    creationTimeFormat_("yyyy.MM.dd hh:mm:ss"),
    fileInfo_(file),
    flag_(false)
{
}

SlideInfo::SlideInfo(const QDir& dir, const QString& file) :
    creationTimeFormat_("yyyy.MM.dd hh:mm:ss"),
    fileInfo_(dir, file),
    flag_(false)
{
}

SlideInfo::SlideInfo(const QFileInfo& fileinfo) :
    creationTimeFormat_("yyyy.MM.dd hh:mm:ss"),
    fileInfo_(fileinfo),
    flag_(false)
{
}

SlideInfo::~SlideInfo()
{
}

SlideInfo::Orientation SlideInfo::orientation() const
{
    return SlideInfo::OrientationNormal;
}

QString SlideInfo::formattedCreationTime() const
{
    QDateTime creationTime = fileInfo_.created();

    return creationTime.toString(creationTimeFormat_);
}

QImage SlideInfo::thumbnailImage() const
{
    return QImage(":/root/slideicon.png");
}

QString SlideInfo::fileName() const
{
    return fileInfo_.fileName();
}

QString SlideInfo::completeSuffix() const
{
    return fileInfo_.completeSuffix();
}

QString SlideInfo::canonicalFilePath() const
{
    return fileInfo_.canonicalFilePath();
}

QImage SlideInfo::imageWithOrientation(const QImage& originalImage, SlideInfo::Orientation orientation)
{
    if (originalImage.isNull())
        return originalImage;

    switch (orientation) {
    case SlideInfo::OrientationNormal:
        return originalImage;
    case SlideInfo::OrientationHorizontalFlip:
        return originalImage.mirrored(true, false);
    case SlideInfo::OrientationRotate180: {
        QTransform transform;
        transform.rotate(180);
        return originalImage.transformed(transform);
    }
    case SlideInfo::OrientationVerticalFlip:
        return originalImage.mirrored(false, true);
    case SlideInfo::OrientationTranspose: {
        QTransform transform;
        transform.rotate(90);
        QImage intermediateImage = originalImage.transformed(transform);
        return intermediateImage.mirrored(true, false);
    }
    case SlideInfo::OrientationRotate90: {
        QTransform transform;
        transform.rotate(90);
        return originalImage.transformed(transform);
    }
    case SlideInfo::OrientationTransverse: {
        QTransform transform;
        transform.rotate(270);
        QImage intermediateImage = originalImage.transformed(transform);
        return intermediateImage.mirrored(true, false);
    }
    case SlideInfo::OrientationRotate270: {
        QTransform transform;
        transform.rotate(270);
        return originalImage.transformed(transform);
    }
    }

    return originalImage;
}
