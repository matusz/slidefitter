#include "slide.h"

Slide::Slide()
{
}

Slide::Slide(const QSize& size, Format format) :
    QImage(size, format)
{
}

Slide::Slide(int width, int height, Format format) :
    QImage(width, height, format)
{
}

Slide::Slide(uchar* data, int width, int height, Format format) :
    QImage(data, width, height, format)
{
}
Slide::Slide(const uchar* data, int width, int height, Format format) :
    QImage(data, width, height, format)
{

}

Slide::Slide(uchar* data, int width, int height, int bytesPerLine, Format format) :
    QImage(data, width, height, bytesPerLine, format)
{
}

Slide::Slide(const uchar* data, int width, int height, int bytesPerLine, Format format) :
    QImage(data, width, height, bytesPerLine, format)
{
}

Slide::Slide(const char* const xpm[]) :
    QImage(xpm)
{
}

Slide::Slide(const QString& fileName, const char* format) :
    QImage(fileName, format)
{
}

Slide::Slide(const char* fileName, const char* format) :
    QImage(fileName, format)
{
}

Slide::Slide(const Slide& slide) :
    QImage(slide)
{
}

Slide::Slide(const QImage& image) :
    QImage(image)
{
}
