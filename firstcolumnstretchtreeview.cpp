#include "firstcolumnstretchtreeview.h"

#include <QHeaderView>

#include <QMenu>

#include <QResizeEvent>
#include <QMouseEvent>

FirstColumnStretchTreeView::FirstColumnStretchTreeView(QWidget *parent) :
    QTreeView(parent),
    firstColumnMinWidth_(0)
{
}

// this siot is already connected in
// set header method of QTreeView class
void FirstColumnStretchTreeView::columnResized(int logicalIndex, int oldSize, int newSize)
{
    QTreeView::columnResized(logicalIndex, oldSize, newSize);

    if (logicalIndex == 0)
        firstColumnMinWidth_ = newSize;
}

void FirstColumnStretchTreeView::setModel(QAbstractItemModel* model)
{
    QTreeView::setModel(model);

    if (header()) {
        firstColumnMinWidth_ = header()->sectionSize(0);
    }
}

void FirstColumnStretchTreeView::resizeEvent(QResizeEvent* event)
{
    QAbstractItemModel* pModel = model();
    if (!pModel)
        return;

    int columnCount = pModel->columnCount();
    int accumulatedWidth = 0;

    for (int i = 1; i < columnCount; i++) {
        accumulatedWidth += columnWidth(i);
    }

    int firstColumnWidth = event->size().width() - accumulatedWidth;

    if (firstColumnWidth < firstColumnMinWidth_)
        firstColumnWidth = firstColumnMinWidth_;

    // avoid calling resizeColumn slot during
    // this header section resize
    if (header())
        disconnect(header(), SIGNAL(sectionResized(int,int,int)), this, SLOT(columnResized(int,int,int)));

    setColumnWidth(0, firstColumnWidth);

    if (header())
        connect(header(), SIGNAL(sectionResized(int,int,int)), this, SLOT(columnResized(int,int,int)));

    QTreeView::resizeEvent(event);
}

void FirstColumnStretchTreeView::mousePressEvent(QMouseEvent* event)
{
    QTreeView::mousePressEvent(event);

    if (event->button() == Qt::RightButton) {
        QPoint pos = event->pos();
        QModelIndex index = indexAt(pos);
        if (index.isValid()) {
            if (!selectionModel()->selectedIndexes().isEmpty()) {
                QMenu *menu=new QMenu(this);

                QAction* pAddFilesAction = new QAction("&Add files", this);

                connect(pAddFilesAction, SIGNAL(triggered()), this, SIGNAL(addFiles()));

                menu->addAction(pAddFilesAction);

                QAction* pAddFilesActionRecursively = new QAction("Add files &recursivelly", this);

                connect(pAddFilesActionRecursively, SIGNAL(triggered()), this, SIGNAL(addFilesRecursively()));

                menu->addAction(pAddFilesActionRecursively);

                menu->popup(viewport()->mapToGlobal(pos));
            }
        }
    }
}
