#include "firstcolumnstretchtableview.h"

#include <QHeaderView>

#include <QMenu>

#include <QResizeEvent>
#include <QMouseEvent>

FirstColumnStretchTableView::FirstColumnStretchTableView(QWidget *parent) :
    QTableView(parent),
    showSelectedSlide_(false),
    firstColumnMinWidth_(0)
{
}

void FirstColumnStretchTableView::setModel(QAbstractItemModel* model)
{
    QTableView::setModel(model);

    if (horizontalHeader()) {
        firstColumnMinWidth_ = horizontalHeader()->sectionSize(0);
    }
}

void FirstColumnStretchTableView::resizeNonStretchableColumnsToContents()
{
    QAbstractItemModel* pModel = model();
    if (!pModel)
        return;

    int columnCount = pModel->columnCount();

    for (int i = 0; i < columnCount; i++) {
        if (i == 0)
            continue;

        resizeColumnToContents(i);
    }
}

// this slot is already connected in
// set header method of QTableView class
void FirstColumnStretchTableView::columnResized(int logicalIndex, int oldSize, int newSize)
{
    QTableView::columnResized(logicalIndex, oldSize, newSize);

    if (logicalIndex == 0)
        firstColumnMinWidth_ = newSize;
}

void FirstColumnStretchTableView::showSlide()
{
    QObject* pSender = sender();

    if (pSender) {
        QAction* pAction = qobject_cast<QAction*>(pSender);
        QVariant userData = pAction->data();

        if (userData.isValid()) {
            if (userData.canConvert<int>())
                emit showSlideAt(userData.toInt());
        }
    }
}

void FirstColumnStretchTableView::startSlideShow()
{
    QObject* pSender = sender();

    if (pSender) {
        QAction* pAction = qobject_cast<QAction*>(pSender);
        QVariant userData = pAction->data();

        if (userData.isValid()) {
            if (userData.canConvert<int>())
                emit startSlideShowAt(userData.toInt());
        }
    }
}

void FirstColumnStretchTableView::startSlideStepping()
{
    QObject* pSender = sender();

    if (pSender) {
        QAction* pAction = qobject_cast<QAction*>(pSender);
        QVariant userData = pAction->data();

        if (userData.isValid()) {
            if (userData.canConvert<int>())
                emit startSlideSteppingAt(userData.toInt());
        }
    }
}

void FirstColumnStretchTableView::resizeEvent(QResizeEvent* event)
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
    if (horizontalHeader())
        disconnect(horizontalHeader(), SIGNAL(sectionResized(int,int,int)), this, SLOT(columnResized(int,int,int)));

    setColumnWidth(0, firstColumnWidth);

    if (horizontalHeader())
        connect(horizontalHeader(), SIGNAL(sectionResized(int,int,int)), this, SLOT(columnResized(int,int,int)));

    QTableView::resizeEvent(event);
}

void FirstColumnStretchTableView::mousePressEvent(QMouseEvent* event)
{
    QTableView::mousePressEvent(event);

    if (event->button() == Qt::RightButton) {
        QPoint pos = event->pos();
        QModelIndex index = indexAt(pos);
        if (index.isValid()) {
            selectionModel()->clearSelection();
            selectionModel()->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);

            QMenu *menu=new QMenu(this);

            if (!showSelectedSlide_) {
                // selected slide will be shown when selection changes
                QAction* pShowSlideAtAction = new QAction("S&how slide", this);
                pShowSlideAtAction->setData(QVariant(index.row()));

                connect(pShowSlideAtAction, SIGNAL(triggered()), this, SLOT(showSlide()));

                menu->addAction(pShowSlideAtAction);

                QAction* pSeparatorAction = new QAction(this);
                pSeparatorAction->setSeparator(true);

                menu->addAction(pSeparatorAction);
            }

            // start slide show action
            QAction* pSlideShowStartAtAction = new QAction("&Start slide show here", this);
            pSlideShowStartAtAction->setData(QVariant(index.row()));

            connect(pSlideShowStartAtAction, SIGNAL(triggered()), this, SLOT(startSlideShow()));

            menu->addAction(pSlideShowStartAtAction);

            // start slide stepping action
            QAction* pSlideSteppingStartAtAction = new QAction("&Start slide stepping here", this);
            pSlideSteppingStartAtAction->setData(QVariant(index.row()));

            connect(pSlideSteppingStartAtAction, SIGNAL(triggered()), this, SLOT(startSlideStepping()));

            menu->addAction(pSlideSteppingStartAtAction);

            menu->popup(viewport()->mapToGlobal(pos));
        }
    }
}

void FirstColumnStretchTableView::selectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
    QTableView::selectionChanged(selected, deselected);

    if (showSelectedSlide_) {
        if (selectionModel()->selectedRows().count() == 1) {
            QModelIndex selectedRowIndex = selectionModel()->selectedRows().at(0);
            emit showSlideAt(selectedRowIndex.row());
        }
        else
            emit showSlideAt(-1);
    }
}
