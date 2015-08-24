#ifndef FIRSTCOLUMNSTRETCHTABLEVIEW_H
#define FIRSTCOLUMNSTRETCHTABLEVIEW_H

#include <QHeaderView>
#include <QTableView>

class QResizeEvent;
class QMouseEvent;

class FirstColumnStretchTableView : public QTableView
{
    Q_OBJECT
public:
    explicit FirstColumnStretchTableView(QWidget *parent = 0);

    void setShowSelectedSlide(bool showSelectedSlide) {showSelectedSlide_ = showSelectedSlide;}
    bool showSelectedSlide() const {return showSelectedSlide_;}

    void setModel(QAbstractItemModel* model);

    void resizeNonStretchableColumnsToContents();

signals:
    void showSlideAt(int index);
    void startSlideShowAt(int index);
    void startSlideSteppingAt(int index);

public slots:

protected slots:
    void columnResized(int logicalIndex, int oldSize, int newSize);
    void showSlide();
    void startSlideShow();
    void startSlideStepping();


protected:
    void resizeEvent(QResizeEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void selectionChanged(const QItemSelection & selected, const QItemSelection & deselected);

protected:
    bool showSelectedSlide_;
    int firstColumnMinWidth_;
};

#endif // FIRSTCOLUMNSTRETCHTABLEVIEW_H
