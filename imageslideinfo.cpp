#include "imageslideinfo.h"

ImageSlideInfo::ImageSlideInfo() :
    substituteSlideImage_(":/root/slideicon.png"),
    exifOrientation_(SlideInfo::OrientationNormal),
    exifCreationTimeFormat_("yyyy:MM:dd hh:mm:ss")
{
    readExifData();
}

ImageSlideInfo::ImageSlideInfo(const QString & file) :
    SlideInfo(file),
    substituteSlideImage_(":/root/slideicon.png"),
    exifOrientation_(SlideInfo::OrientationNormal),
    exifCreationTimeFormat_("yyyy:MM:dd hh:mm:ss")
{
    readExifData();
}

ImageSlideInfo::ImageSlideInfo(const QFile & file) :
    SlideInfo(file),
    substituteSlideImage_(":/root/slideicon.png"),
    exifOrientation_(SlideInfo::OrientationNormal),
    exifCreationTimeFormat_("yyyy:MM:dd hh:mm:ss")
{
    readExifData();
}

ImageSlideInfo::ImageSlideInfo(const QDir & dir, const QString & file) :
    SlideInfo(dir, file),
    substituteSlideImage_(":/root/slideicon.png"),
    exifOrientation_(SlideInfo::OrientationNormal),
    exifCreationTimeFormat_("yyyy:MM:dd hh:mm:ss")
{
    readExifData();
}

ImageSlideInfo::ImageSlideInfo(const QFileInfo & fileinfo) :
    SlideInfo(fileinfo),
    substituteSlideImage_(":/root/slideicon.png"),
    exifOrientation_(SlideInfo::OrientationNormal),
    exifCreationTimeFormat_("yyyy:MM:dd hh:mm:ss")
{
    readExifData();
}

SlideInfo::Orientation ImageSlideInfo::orientation() const
{
    return exifOrientation_;
}

QString ImageSlideInfo::formattedCreationTime() const
{
    if (formattedExifCreationTime_.isEmpty())
        return SlideInfo::formattedCreationTime();

    return formattedExifCreationTime_;
}

QImage ImageSlideInfo::thumbnailImage() const
{
    if (thumbnailImage_.isNull())
        return substituteSlideImage_;

    return thumbnailImage_;
}

void ImageSlideInfo::readExifData()
{
    // all EXIF data will be read here during slide
    // construction since EXIF data will not be modified
    // is much faster to access already read EXIF data
    // whenever is necessary instead of parsing it each
    // time it would be accessed
    ExifData* pExifData = exif_data_new_from_file(canonicalFilePath().toUtf8().constData());

    if (pExifData) {
        quint64 exifOrientation = readExifUIntValue(pExifData, EXIF_TAG_ORIENTATION);
        switch (exifOrientation) {
        case 1:
            exifOrientation_ = SlideInfo::OrientationNormal;
            break;
        case 2:
            exifOrientation_ = SlideInfo::OrientationHorizontalFlip;
            break;
        case 3:
            exifOrientation_ = SlideInfo::OrientationRotate180;
            break;
        case 4:
            exifOrientation_ = SlideInfo::OrientationVerticalFlip;
            break;
        case 5:
            exifOrientation_ = SlideInfo::OrientationTranspose;
            break;
        case 6:
            exifOrientation_ = SlideInfo::OrientationRotate90;
            break;
        case 7:
            exifOrientation_ = SlideInfo::OrientationTransverse;
            break;
        case 8:
            exifOrientation_ = SlideInfo::OrientationRotate270;
            break;
        }

        QString dateTimeStr = readExifStringValue(pExifData, EXIF_TAG_DATE_TIME);

        if (dateTimeStr.isEmpty()) {
            dateTimeStr = readExifStringValue(pExifData, EXIF_TAG_DATE_TIME_ORIGINAL);

            if (dateTimeStr.isEmpty())
                dateTimeStr = readExifStringValue(pExifData, EXIF_TAG_DATE_TIME_DIGITIZED);
        }

        if (!dateTimeStr.isEmpty()) {
            QDateTime dateTime = QDateTime::fromString(dateTimeStr, exifCreationTimeFormat_);
            if (dateTime.isValid())
                // here it will use the same date time format as
                // in case of other files without EXIF info in
                // order to be able to sort them by creation time
                formattedExifCreationTime_ = dateTime.toString(creationTimeFormat_);
        }

        // thumbnail
        if (pExifData->data) {
            QImage tmpThumbnailImage = QImage::fromData(pExifData->data, pExifData->size);
            if (!tmpThumbnailImage.isNull() &&
                 (tmpThumbnailImage.width() > 0) &&
                 (tmpThumbnailImage.height() > 0)) {
                QImage nonScaledThumbnailImage = SlideInfo::imageWithOrientation(tmpThumbnailImage, exifOrientation_);

                int thumbnailSize = substituteSlideImage_.width();
                if (thumbnailSize < substituteSlideImage_.height())
                    thumbnailSize = substituteSlideImage_.height();

                int width = nonScaledThumbnailImage.width();
                int height = nonScaledThumbnailImage.height();

                qreal scaleX = qreal(thumbnailSize) / qreal(width);
                qreal scaleY = qreal(thumbnailSize) / qreal(height);

                qreal scaleFactor = (width > height) ?  scaleX : scaleY;

                int thumbnailWidth = (int)(qreal(width) * scaleFactor);
                int thumbnailHeight = (int)(qreal(height) * scaleFactor);

                thumbnailImage_ = nonScaledThumbnailImage.scaled(QSize(thumbnailWidth, thumbnailHeight), Qt::KeepAspectRatio);
            }
        }

        exif_data_free(pExifData);
    }
}

QString ImageSlideInfo::readExifStringValue(ExifData* pExifData, ExifTag exifTag)
{
    QString exifValueStr;

    if (!pExifData)
        return exifValueStr;

    ExifEntry* pExifEntry = exif_data_get_entry(pExifData, exifTag);
    if (pExifEntry) {
        if (pExifEntry->format == EXIF_FORMAT_ASCII) {
            char* pBuffer = new char[pExifEntry->size];
            memset(pBuffer, '\0', pExifEntry->size);

            exif_entry_get_value(pExifEntry, pBuffer, pExifEntry->size);
            if (strlen(pBuffer) > 0)
                exifValueStr = pBuffer;

            delete[] pBuffer;
        }
    }

    return exifValueStr;
}

quint64 ImageSlideInfo::readExifUIntValue(ExifData* pExifData, ExifTag exifTag)
{
    quint64 exifValue = 0;

    if (!pExifData)
        return exifValue;

    ExifByteOrder exifByteOrder = exif_data_get_byte_order(pExifData);

    ExifEntry* pExifEntry = exif_data_get_entry(pExifData, exifTag);
    if (pExifEntry) {
        // paranoic a bit
        if (pExifEntry->size <= sizeof(quint64)) {
            if (pExifEntry->format == EXIF_FORMAT_BYTE)
                exifValue = static_cast<quint64>(*pExifEntry->data);

            if (pExifEntry->format == EXIF_FORMAT_SHORT)
                exifValue = static_cast<quint64>(exif_get_short(pExifEntry->data, exifByteOrder));

            if (pExifEntry->format == EXIF_FORMAT_LONG)
                exifValue = static_cast<quint64>(exif_get_long(pExifEntry->data, exifByteOrder));
        }
    }

    return exifValue;
}

qint64 ImageSlideInfo::readExifIntValue(ExifData* pExifData, ExifTag exifTag)
{
    qint64 exifValue = 0;

    if (!pExifData)
        return exifValue;

    ExifByteOrder exifByteOrder = exif_data_get_byte_order(pExifData);

    ExifEntry* pExifEntry = exif_data_get_entry(pExifData, exifTag);
    if (pExifEntry) {
        // paranoic a bit
        if (pExifEntry->size <= sizeof(qint64)) {
            if (pExifEntry->format == EXIF_FORMAT_SBYTE)
                exifValue = static_cast<qint64>(*pExifEntry->data);

            if (pExifEntry->format == EXIF_FORMAT_SSHORT)
                exifValue = static_cast<qint64>(exif_get_sshort(pExifEntry->data, exifByteOrder));

            if (pExifEntry->format == EXIF_FORMAT_SLONG)
                exifValue = static_cast<qint64>(exif_get_slong(pExifEntry->data, exifByteOrder));
        }
    }

    return exifValue;
}
