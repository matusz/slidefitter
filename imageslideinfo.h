#ifndef IMAGESLIDEINFO_H
#define IMAGESLIDEINFO_H

#include "slideinfo.h"

#include <libexif/exif-data.h>

class ImageSlideInfo : public SlideInfo
{
public:
    ImageSlideInfo();
    ImageSlideInfo(const QString & file);
    ImageSlideInfo(const QFile & file);
    ImageSlideInfo(const QDir & dir, const QString & file);
    ImageSlideInfo(const QFileInfo & fileinfo);

    virtual Orientation orientation() const;
    virtual QString formattedCreationTime() const;
    virtual QImage thumbnailImage() const;

protected:
    void readExifData();

    template <typename ExifValueType>
    bool isExifEntryTypeAcceptable(ExifEntry* pExifEntry);

    template <typename ExifValueType>
    ExifValueType getExifValue(ExifEntry* pExifEntry, const ExifValueType& defaultValue);

    template <typename ExifValueType>
    ExifValueType readExifValue(ExifData* pExifData, ExifTag exifTag, const ExifValueType& defaultValue);

    QString readExifStringValue(ExifData* pExifData, ExifTag exifTag);
    // return value big enough to hold all
    // kinds of (signed and unsigned) integers
    quint64 readExifUIntValue(ExifData* pExifData, ExifTag exifTag);
    qint64 readExifIntValue(ExifData* pExifData, ExifTag exifTag);

protected:
    QImage substituteSlideImage_;

    Orientation exifOrientation_;
    QString exifCreationTimeFormat_;
    QString formattedExifCreationTime_;

    QImage thumbnailImage_;
};

#endif // IMAGESLIDEINFO_H
