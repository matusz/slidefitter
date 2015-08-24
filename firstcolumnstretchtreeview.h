#ifndef FIRSTCOLUMNSTRETCHTREEVIEW_H
#define FIRSTCOLUMNSTRETCHTREEVIEW_H

#include <QHeaderView>
#include <QTreeView>

class QResizeEvent;
class QMouseEvent;

class FirstColumnStretchTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit FirstColumnStretchTreeView(QWidget *parent = 0);

    void setModel(QAbstractItemModel* model);

signals:
    void addFiles();
    void addFilesRecursively();

public slots:

protected slots:
    void columnResized(int logicalIndex, int oldSize, int newSize);

protected:
    void resizeEvent(QResizeEvent* event);
    void mousePressEvent(QMouseEvent* event);

protected:
    int firstColumnMinWidth_;
};

#endif // FIRSTCOLUMNSTRETCHTREEVIEW_H
