#ifndef SLIDE_H
#define SLIDE_H

#include <QImage>

// in fact in this version it should be enough
// to use QImage class instead of Slide (only
// the next version will require slide class)
class Slide : public QImage
{
public:
    Slide();
    Slide(const QSize& size, Format format);
    Slide(int width, int height, Format format);
    Slide(uchar* data, int width, int height, Format format);
    Slide(const uchar* data, int width, int height, Format format);
    Slide(uchar* data, int width, int height, int bytesPerLine, Format format);
    Slide(const uchar* data, int width, int height, int bytesPerLine, Format format);
    Slide(const char* const xpm[]);
    Slide(const QString& fileName, const char* format = 0);
    Slide(const char* fileName, const char* format = 0);
    Slide(const Slide& slide);
    Slide(const QImage& image);
};

#endif // SLIDE_H
