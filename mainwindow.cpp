#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "settingsdialog.h"

#include "imageslideinfo.h"

#include "slideinfomodel.h"

#include "slideloader.h"

#include "slideshowsequencer.h"
#include "slidedisplaywidget.h"
#include "slidedisplaywidgetlist.h"

#include "firstcolumnstretchtableview.h"
#include "firstcolumnstretchtreeview.h"

#include <QSettings>

#include <QFileSystemWatcher>
#include <QDirModel>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QSplitter>

#include <QCheckBox>
#include <QPushButton>

#include <QHeaderView>

#include <QDesktopWidget>

#include <QKeyEvent>
#include <QMouseEvent>

#include <QImageReader>

const QString cMediaDir = "/media/";
const QString cMountDir = "/mnt/";

const QString cSequenceTimeoutKey = "sequenceTimeout";
const QString cShowSelectedSlide = "showSelectedSlide";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    pSettings_(NULL),
    pFileSystemWatcher_(NULL),
    pDirModel_(NULL),
    pSlideInfoModel_(NULL),
    finishSlideShow_(false),
    currentSlideIndex_(-1),
    pSlideLoader_(NULL),
    pSlideShowSequencer_(NULL),
    pSlideDisplayWidget_(NULL),
    pDirTreeView_(NULL),
    pRecursiveCheckBox_(NULL),
    pRefreshButton_(NULL),
    pShowSelectedSlideCheckBox_(NULL),
    pContentTableView_(NULL),
    pMainSplitter_(NULL),
    pSlideDisplayWidgetListFullScreen_(NULL)
{
    ui->setupUi(this);

    QString settingsFilePath = QDir::homePath() + "/.slidefitterrc";
    pSettings_ = new QSettings(settingsFilePath, QSettings::NativeFormat, this);

    setWindowIcon(QIcon(":/root/mainwindowicon.png"));
    setWindowTitle(QString("Slide fitter"));

    if (!centralWidget()) {
        QWidget* pCentralWidget= new QWidget(this);
        setCentralWidget(pCentralWidget);
    }

    QHBoxLayout* pCentralWidgetLayout = new QHBoxLayout(centralWidget());
    pCentralWidgetLayout->setMargin(0);
    pCentralWidgetLayout->setSpacing(0);

    connect(QApplication::desktop(), SIGNAL(resized(int)), this, SLOT(resized(int)));
    connect(QApplication::desktop(), SIGNAL(screenCountChanged(int)), this, SLOT(screenCountChanged(int)));
    connect(QApplication::desktop(), SIGNAL(workAreaResized(int)), this, SLOT(workAreaResized(int)));

    connect(ui->action_Quit, SIGNAL(triggered()), this, SLOT(closeMainWindow()));

    connect(ui->action_Settings, SIGNAL(triggered()), this, SLOT(slideShowSettings()));
    connect(ui->action_Start, SIGNAL(triggered()), this, SLOT(startSlideShow()));
    connect(ui->action_Stepping, SIGNAL(triggered()), this, SLOT(startSlideStepping()));

    pSlideLoader_ = new SlideLoader(this);

    pSlideShowSequencer_ = new SlideShowSequencer(this);
    connect(pSlideShowSequencer_, SIGNAL(loadNextSlide()), this, SLOT(loadNextSlide()));
    connect(pSlideShowSequencer_, SIGNAL(showNextSlide()), this, SLOT(showNextSlide()));

    qreal sequenceTimeout = pSettings_->value(cSequenceTimeoutKey).toReal();
    pSlideShowSequencer_->setSequenceTimeout(sequenceTimeout);

    pFileSystemWatcher_ = new QFileSystemWatcher(this);

    QStringList watchedSubDirs;

    allSubDirs(watchedSubDirs, cMediaDir);
    allSubDirs(watchedSubDirs, cMountDir);

    pFileSystemWatcher_->addPaths(watchedSubDirs);

    connect(pFileSystemWatcher_, SIGNAL(directoryChanged(QString)), this, SLOT(directoryChanged(QString)));

    // filesystem directory tree model, tree view
    // and afferent controls
    QWidget* pParentDirControlWidget = new QWidget(this);
    createDirControlWidgets(pParentDirControlWidget);

    //slide show content related controls
    QWidget* pParentContentControlWidget = new QWidget(this);
    createContentControlWidgets(pParentContentControlWidget);

    pSlideDisplayWidget_ = new SlideDisplayWidget(-1, this);
    // 200 x 200 seems to be enough good looking
    pSlideDisplayWidget_->setMinimumSize(200, 200);
    // in non full screen mode there will only one slide
    // displaying widget displaying both landscape and
    // portrait oriented slides
    pSlideDisplayWidget_->setOrientation(SlideDisplayWidget::Landscape | SlideDisplayWidget::Portrait);

    // central widget ie main splitter
    pMainSplitter_ = new QSplitter(Qt::Horizontal, this);
    pMainSplitter_->setChildrenCollapsible(true);

    QSplitter* menuSplitter = new QSplitter(Qt::Vertical, this);

    menuSplitter->addWidget(pParentDirControlWidget);
    menuSplitter->addWidget(pParentContentControlWidget);

    pMainSplitter_->addWidget(menuSplitter);
    pMainSplitter_->addWidget(pSlideDisplayWidget_);

    pMainSplitter_->setStretchFactor(0, 0);
    pMainSplitter_->setStretchFactor(1, 1);

    pCentralWidgetLayout->addWidget(pMainSplitter_);

    pSlideDisplayWidgetListFullScreen_ = new SlideDisplayWidgetList(this);

    connect(pSlideDisplayWidgetListFullScreen_, SIGNAL(escapeKeyPressed()), this, SLOT(slideEscapeKeyPressed()));
    connect(pSlideDisplayWidgetListFullScreen_, SIGNAL(spaceKeyPressed()), this, SLOT(slideSpaceKeyPressed()));
    connect(pSlideDisplayWidgetListFullScreen_, SIGNAL(leftKeyPressed()), this, SLOT(slideLeftKeyPressed()));
    connect(pSlideDisplayWidgetListFullScreen_, SIGNAL(rightKeyPressed()), this, SLOT(slideRightKeyPressed()));

    connect(pSlideDisplayWidgetListFullScreen_, SIGNAL(leftButtonPressed()), this, SLOT(slideLeftButtonPressed()));
    connect(pSlideDisplayWidgetListFullScreen_, SIGNAL(rightButtonPressed()), this, SLOT(slideRightButtonPressed()));

    pSlideDisplayWidgetListFullScreen_->create();
    pSlideDisplayWidgetListFullScreen_->setVisibile(false);

    // here everything should be inititalized (signals could
    // be emited) so every settings could be performed here
    pShowSelectedSlideCheckBox_->setChecked(pSettings_->value(cShowSelectedSlide).toBool());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::directoryChanged(const QString& path)
{
    QModelIndex modifiedIndex = pDirModel_->index(path);
    pDirModel_->refresh(modifiedIndex);

    refreshDirModel();
}

void MainWindow::dirSortIndicatorChanged(int logicalIndex, Qt::SortOrder order)
{
    QModelIndex currentIndex = pDirTreeView_->currentIndex();
    QString indexPath;

    if (currentIndex.isValid())
        indexPath = pDirModel_->fileInfo(currentIndex).canonicalFilePath();

    pDirModel_->sort(logicalIndex, order);

    if (!indexPath.isEmpty())
        pDirTreeView_->scrollTo(pDirModel_->index(indexPath));
}

void MainWindow::refreshDirModel()
{
    // if the directory / file corresponding to currently
    // selected tree view index does exist anymore
    // it moves up in directory structure until it finds
    // an existing directory or it reaches the root directory
    // (it has to exist)
    QModelIndex currentIndex = pDirTreeView_->currentIndex();

    // currentIndex can be invalid when externalUSB storage
    // device is unmounted (pulled out) from USB
    if (!currentIndex.isValid()) {
        QModelIndex index = pDirModel_->index(QDir::rootPath());
        pDirTreeView_->expand(index);
        pDirTreeView_->scrollTo(index);
        pDirTreeView_->setCurrentIndex(index);

        return;
    }

    QFileInfo fileInfo = pDirModel_->fileInfo(currentIndex);

    pDirModel_->refresh();

    bool directoryChanged = false;
    QDir dir(fileInfo.absolutePath());
    while (!dir.exists()) {
        directoryChanged = true;

        dir.cdUp();

        if (dir.isRoot())
            break;
    }

    if (directoryChanged) {
        QModelIndex index = pDirModel_->index(dir.absolutePath());
        pDirTreeView_->expand(index);
        pDirTreeView_->scrollTo(index);
        pDirTreeView_->setCurrentIndex(index);
    }
}

void MainWindow::showSelectedSlide(int state)
{
    Q_UNUSED(state);

    bool showSelectedSlide = pShowSelectedSlideCheckBox_->isChecked();
    pContentTableView_->setShowSelectedSlide(showSelectedSlide);

    pSettings_->setValue(cShowSelectedSlide, showSelectedSlide);
    pSettings_->sync();

    // just to cause calling FirstColumnStretchTableView::selectionChanged
    // method in order to show the selected slide if it is necessary
    QItemSelection selection = pContentTableView_->selectionModel()->selection();

    pContentTableView_->selectionModel()->select(selection, QItemSelectionModel::Deselect);
    pContentTableView_->selectionModel()->select(selection, QItemSelectionModel::Select);
}

void MainWindow::contentSortIndicatorChanged(int logicalIndex, Qt::SortOrder order)
{
    QModelIndexList selectedIndexes = pContentTableView_->selectionModel()->selectedRows();
    QStringList selectedSlides = pSlideInfoModel_->canonicalFilePaths(selectedIndexes);

    pSlideInfoModel_->sort(logicalIndex, order);

    pContentTableView_->selectionModel()->clearSelection();

    QModelIndexList indexesToSelect = pSlideInfoModel_->modelIndexes(selectedSlides);

    foreach (QModelIndex index, indexesToSelect)
        pContentTableView_->selectionModel()->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
}

void MainWindow::addFilesAction()
{
    pRecursiveCheckBox_->setChecked(false);
    addFiles();
}

void MainWindow::addFilesRecursivelyAction()
{
    pRecursiveCheckBox_->setChecked(true);
    addFiles();
}

void MainWindow::addFiles()
{
    QModelIndexList indexList = pDirTreeView_->selectionModel()->selectedIndexes();

    foreach(QModelIndex index, indexList) {
        // takes into account only valid indexes
        // from the first column
        if (!index.isValid())
            continue;

        if (index.column() != 0)
            continue;

        QFileInfo fileInfo = pDirModel_->fileInfo(index);
        if (fileInfo.isDir())
            appendAllFilesFromDir(fileInfo.canonicalFilePath(), pRecursiveCheckBox_->isChecked());
        else
            appendSlideFromCanonicalFilePath(fileInfo.canonicalFilePath());
    }

    pContentTableView_->resizeRowsToContents();
    pContentTableView_->resizeNonStretchableColumnsToContents();
}

void MainWindow::removeFiles()
{
    // does not need all the indexes since the whole
    // row is selected at once without having the
    // possibility to select separate columns inside
    // rows
    QModelIndexList selectedIndexes = pContentTableView_->selectionModel()->selectedRows();

    QList<int> rows;
    foreach (QModelIndex index, selectedIndexes)
        rows.append(index.row());

    pSlideInfoModel_->removeRows(rows);

    pContentTableView_->resizeNonStretchableColumnsToContents();
}

void MainWindow::clearContent()
{
    pSlideInfoModel_->removeRows(0, pSlideInfoModel_->rowCount());

    pContentTableView_->resizeNonStretchableColumnsToContents();
}

void MainWindow::onlyImages()
{
    // filters for supported image files
    QStringList filters;
    foreach (QByteArray format, QImageReader::supportedImageFormats())
        filters.append("*." + format);

    pSlideInfoModel_->filterSlides(filters);

    pContentTableView_->resizeNonStretchableColumnsToContents();
}

void MainWindow::moveUp()
{
    QModelIndexList selectedIndexes = pContentTableView_->selectionModel()->selectedRows();
    QStringList selectedSlides = pSlideInfoModel_->canonicalFilePaths(selectedIndexes);

    // selectedIndexes has to be sorted first by
    // rows of contained QModelIndex instances
    // (QModelIndex::operator< compares rows first)
    qSort(selectedIndexes);

    // newPosition is necessary to avoid item swap
    // when selected range is already on top of the
    // table and selected items would be swapped
    // among themselves
    int newPosition = 0;

    foreach (QModelIndex index, selectedIndexes) {
        int row = index.row();

        if (row > 0) {
            if (newPosition > row - 1)
                pSlideInfoModel_->swapRows(newPosition, row);
            else
                pSlideInfoModel_->swapRows(row - 1, row);
        }

        newPosition++;
    }

    pContentTableView_->selectionModel()->clearSelection();

    QModelIndexList indexesToSelect = pSlideInfoModel_->modelIndexes(selectedSlides);

    foreach (QModelIndex index, indexesToSelect)
        pContentTableView_->selectionModel()->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
}

void MainWindow::moveDown()
{
    QModelIndexList selectedIndexes = pContentTableView_->selectionModel()->selectedRows();
    QStringList selectedSlides = pSlideInfoModel_->canonicalFilePaths(selectedIndexes);

    // selectedIndexes has to be sorted first by
    // rows of contained QModelIndex instances
    // (QModelIndex::operator< compares rows first)
    qSort(selectedIndexes);

    // newPosition is necessary to avoid item swap
    // when selected range is already on bottom of
    // the table and selected items would be swapped
    // among themselves
    int newPosition = pSlideInfoModel_->rowCount() - 1;
    int count = pSlideInfoModel_->rowCount();

    QListIterator<QModelIndex> iter(selectedIndexes);
    iter.toBack();
    while (iter.hasPrevious()) {
        int row = iter.previous().row();

        if (row < count - 1) {
            if (newPosition < row + 1)
                pSlideInfoModel_->swapRows(newPosition, row);
            else
                pSlideInfoModel_->swapRows(row + 1, row);
        }

        newPosition--;
    }

    pContentTableView_->selectionModel()->clearSelection();

    QModelIndexList indexesToSelect = pSlideInfoModel_->modelIndexes(selectedSlides);

    foreach (QModelIndex index, indexesToSelect)
        pContentTableView_->selectionModel()->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
}

void MainWindow::moveFirst()
{
    QModelIndexList selectedIndexes = pContentTableView_->selectionModel()->selectedRows();
    QStringList selectedSlides = pSlideInfoModel_->canonicalFilePaths(selectedIndexes);

    // selectedIndexes has to be sorted first by
    // rows of contained QModelIndex instances
    // (QModelIndex::operator< compares rows first)
    qSort(selectedIndexes);

    int newPosition = 0;

    foreach (QModelIndex index, selectedIndexes) {
        pSlideInfoModel_->swapRows(newPosition, index.row());
        newPosition++;
    }

    pContentTableView_->selectionModel()->clearSelection();

    QModelIndexList indexesToSelect = pSlideInfoModel_->modelIndexes(selectedSlides);

    foreach (QModelIndex index, indexesToSelect)
        pContentTableView_->selectionModel()->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
}

void MainWindow::moveLast()
{
    QModelIndexList selectedIndexes = pContentTableView_->selectionModel()->selectedRows();
    QStringList selectedSlides = pSlideInfoModel_->canonicalFilePaths(selectedIndexes);

    // selectedIndexes has to be sorted first by
    // rows of contained QModelIndex instances
    // (QModelIndex::operator< compares rows first)
    qSort(selectedIndexes);

    int newPosition = pSlideInfoModel_->rowCount() - 1;

    QListIterator<QModelIndex> iter(selectedIndexes);
    iter.toBack();
    while (iter.hasPrevious()) {
        pSlideInfoModel_->swapRows(newPosition, iter.previous().row());
        newPosition--;
    }

    pContentTableView_->selectionModel()->clearSelection();

    QModelIndexList indexesToSelect = pSlideInfoModel_->modelIndexes(selectedSlides);

    foreach (QModelIndex index, indexesToSelect)
        pContentTableView_->selectionModel()->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
}

void MainWindow::resized(int screen)
{
    pSlideDisplayWidgetListFullScreen_->resize(screen);
}

void MainWindow::screenCountChanged(int newCount)
{
    pSlideDisplayWidgetListFullScreen_->adjustNumber(newCount);
}

void MainWindow::workAreaResized(int screen)
{
    pSlideDisplayWidgetListFullScreen_->resize(screen);
}

void MainWindow::closeMainWindow()
{
    if (isFullScreen())
        return;

    if (!pSlideShowSequencer_->isIdle())
        pSlideShowSequencer_->stop();

    close();
}

void MainWindow::slideShowSettings()
{
    if (isFullScreen())
        return;

    if (pSlideShowSequencer_->isRunning())
        return;

    // setting has to be allowed only if there
    // is no ongoing slideshow
    SettingsDialog settingsDialog(this);

    settingsDialog.setSequenceTimeout(pSlideShowSequencer_->sequenceTimeout());

    if (settingsDialog.exec() == QDialog::Accepted) {
        pSlideShowSequencer_->setSequenceTimeout(settingsDialog.sequenceTimeout());

        pSettings_->setValue(cSequenceTimeoutKey, settingsDialog.sequenceTimeout());
        pSettings_->sync();
    }
}

void MainWindow::showSlideAt(int index)
{
    if (pSlideShowSequencer_->isRunning())
        return;

    pSlideInfoModel_->setFirstSlideAt(index);
    pSlideLoader_->loadSlide(pSlideInfoModel_->currentSlideInfo());
    pSlideDisplayWidget_->play(pSlideLoader_->slide());
}

void MainWindow::startSlideShow()
{
    pSlideShowSequencer_->setManualStepping(false);

    beginSlideShowWithSlide(0);
}

void MainWindow::startSlideShowAt(int index)
{
    pSlideShowSequencer_->setManualStepping(false);

    beginSlideShowWithSlide(index);
}

void MainWindow::stopSlideShow()
{
    pSlideShowSequencer_->stop();

    pSlideDisplayWidgetListFullScreen_->setVisibile(false);

    setVisible(true);
}

void MainWindow::startSlideStepping()
{
    pSlideShowSequencer_->setManualStepping(true);

    beginSlideShowWithSlide(0);
}

void MainWindow::startSlideSteppingAt(int index)
{
    pSlideShowSequencer_->setManualStepping(true);

    beginSlideShowWithSlide(index);
}

void MainWindow::loadNextSlide()
{
    // loads no new slide after slideshow
    // was already finished
    if (pSlideInfoModel_->isSlideListFinished())
        // instead of stopping the slideshow here by displaying
        // the finish flag it will let one more iteration in
        // order to have the same timeout as a new slide would
        // be displayed (it is a kind of hack)
        //
        // also there is a hack for advancing the selection in
        // pContentTableView_ as slide show advances, but there
        // is no info anymore about canonical path of current
        // slide since slides are loaded in advance before they
        // are displayed
        finishSlideShow_ = true;
    else {
        // it would be much shorter and much faster just directly
        // to ask the index of currently loaded slide, but it would
        // brake the object models used to model slide show
        SlideInfo* pSlideInfo = pSlideInfoModel_->currentSlideInfo();
        if (pSlideInfo)
            currentSlideIndex_ = pSlideInfoModel_->indexOf(pSlideInfo->canonicalFilePath());

        pSlideLoader_->loadSlide(pSlideInfo);

        // in case of manual stepping index increasing / descresing
        // order will be decided by key or mouse button press event
        // (left / right arrow or left / right mouse button)
        if (!pSlideShowSequencer_->isManualStepping())
            pSlideInfoModel_->setNextSlide();
    }
}

void MainWindow::showNextSlide()
{
    if (currentSlideIndex_ >= 0) {
        pContentTableView_->selectionModel()->clearSelection();

        QModelIndex slideInfoModelIndex = pSlideInfoModel_->index(currentSlideIndex_, 0);
        pContentTableView_->selectionModel()->select(slideInfoModelIndex, QItemSelectionModel::Select | QItemSelectionModel::Rows);
    }

    if (finishSlideShow_) {
        pSlideDisplayWidgetListFullScreen_->setFinished(true);

        // slide show will be automatically stopped at the end
        // of slides sequence only when no manual stepping is used
        // (manual stepping is in fact no slide show only loading
        // the next or the previous slide)
        if (!pSlideShowSequencer_->isManualStepping())
            pSlideShowSequencer_->stop();

        finishSlideShow_ = false;
    } else {
        // in case of manual stepping finishSlideShow_ flag
        // is used only to indicate that the end of slides list
        // was reached but if the user reverses stepping order
        // then it can be reset
        if (pSlideShowSequencer_->isManualStepping())
            pSlideDisplayWidgetListFullScreen_->setFinished(false);

        pSlideDisplayWidgetListFullScreen_->play(pSlideLoader_->slide());
    }
}

void MainWindow::slideEscapeKeyPressed()
{
    stopSlideShow();
}

void MainWindow::slideSpaceKeyPressed()
{
    // space will be used to show the slides in normal
    // order in case of manual stepping and to start / stop
    // the slide show when slides are automatically refreshed
    if (pSlideShowSequencer_->isManualStepping()) {
        slideRightKeyPressed();
    } else {
        if (pSlideShowSequencer_->isPaused())
            pSlideShowSequencer_->resume();
        else if (pSlideShowSequencer_->isRunning())
            pSlideShowSequencer_->pause();
    }
}

void MainWindow::slideLeftKeyPressed()
{
    // left arrow is used only in case of manual stepping
    // when the user wants to show the slide show in reverse
    // (decreasing slide index) order
    if (pSlideShowSequencer_->isManualStepping()) {
        pSlideInfoModel_->setDescreasingSlideIndex(true);
        pSlideInfoModel_->setNextSlide();

        pSlideShowSequencer_->nextOperation();
    }
}

void MainWindow::slideRightKeyPressed()
{
    // right arrow is used only in case of manual stepping
    // when the user wants to show the slide show in normal
    // (increasing slide index) order
    if (pSlideShowSequencer_->isManualStepping()) {
        pSlideInfoModel_->setDescreasingSlideIndex(false);
        pSlideInfoModel_->setNextSlide();

        pSlideShowSequencer_->nextOperation();
    }
}

void MainWindow::slideLeftButtonPressed()
{
    if (pSlideShowSequencer_->isManualStepping()) {
        // left button is used in case of manual stepping
        // when the user wants to show the slide show in
        // reverse (decreasing slide index) order
        pSlideInfoModel_->setDescreasingSlideIndex(true);
        pSlideInfoModel_->setNextSlide();

        pSlideShowSequencer_->nextOperation();
    } else {
        // left button is used to start / stop the slide show
        // when slides are automatically refreshed
        if (pSlideShowSequencer_->isPaused())
            pSlideShowSequencer_->resume();
        else if (pSlideShowSequencer_->isRunning())
            pSlideShowSequencer_->pause();
    }
}

void MainWindow::slideRightButtonPressed()
{
    // right button is used only in case of manual stepping
    // when the user wants to show the slide show in normal
    // (increasing slide index) order
    if (pSlideShowSequencer_->isManualStepping()) {
        pSlideInfoModel_->setDescreasingSlideIndex(false);
        pSlideInfoModel_->setNextSlide();

        pSlideShowSequencer_->nextOperation();
    }
}

void MainWindow::beginSlideShowWithSlide(int index)
{
    // slide display widgets will be displayed in fullscreen
    // mode in order to have the same display mode for all
    // newly added widgets (when there new screens are added
    // new slide display widgets will be created to each of
    // those new widgets and there setting will be copied from
    // existing ones)
    pSlideDisplayWidgetListFullScreen_->showFullScreen();
    pSlideDisplayWidgetListFullScreen_->setVisibile(true);

    setVisible(false);

    finishSlideShow_ = false;
    currentSlideIndex_ = -1;

    // by default slide shows are always in normal order
    // (slide index is increasing)
    // in case of manual stepping slide show will be "stopped"
    // after the firts shown slide and slide show direction
    // will be decided after the next command
    pSlideInfoModel_->setDescreasingSlideIndex(false);

    pSlideInfoModel_->setFirstSlideAt(index);

    pSlideShowSequencer_->start();

    pSlideDisplayWidgetListFullScreen_->setFinished(false);
    pSlideDisplayWidgetListFullScreen_->play(Slide());
}

void MainWindow::createDirControlWidgets(QWidget* pParentDirControlWidget)
{
    if (!pParentDirControlWidget)
        return;

    pDirModel_ = new QDirModel;

    pDirModel_->setReadOnly(true);
    pDirModel_->setResolveSymlinks(true);
    pDirModel_->setSorting(QDir::DirsFirst | QDir::IgnoreCase | QDir::Name);
    pDirModel_->setFilter(QDir::AllEntries | QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::Readable);

    // filesystem treeview settings
    pDirTreeView_ = new FirstColumnStretchTreeView(this);

    // filesystem treeview header settings
    pDirTreeView_->header()->setStretchLastSection(true);
    pDirTreeView_->header()->setSortIndicator(0, Qt::AscendingOrder);
    pDirTreeView_->header()->setSortIndicatorShown(true);
    pDirTreeView_->header()->setClickable(true);

    // filesystem treeview content (model) setting
    pDirTreeView_->setModel(pDirModel_);

    QModelIndex index = pDirModel_->index(QDir::rootPath());
    pDirTreeView_->expand(index);
    pDirTreeView_->scrollTo(index);
    pDirTreeView_->setCurrentIndex(index);

    pDirTreeView_->setSelectionMode(QAbstractItemView::ExtendedSelection);

    connect(pDirTreeView_->header(), SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)), this, SLOT(dirSortIndicatorChanged(int, Qt::SortOrder)));
    connect(pDirTreeView_, SIGNAL(addFiles()), this, SLOT(addFilesAction()));
    connect(pDirTreeView_, SIGNAL(addFilesRecursively()), this, SLOT(addFilesRecursivelyAction()));

    pRecursiveCheckBox_ = new QCheckBox(this);
    pRecursiveCheckBox_->setText("Recurse to subdirectories");

    pRefreshButton_ = new QPushButton(this);
    pRefreshButton_->setText("Refresh");

    connect(pRefreshButton_, SIGNAL(clicked()), this, SLOT(refreshDirModel()));

    // creates a vertical layout in order to have directory
    // tree view and the afferent buttons groupped together
    QVBoxLayout* pDirLayout = new QVBoxLayout(pParentDirControlWidget);

    // the other choice for parent widget would be
    // pDirWidget but when using it there will be
    // a runtime warning message since it already
    // has a layout (pDirLayout)
    QHBoxLayout* pDirControlLayout = new QHBoxLayout(NULL);

    pDirControlLayout->addWidget(pRecursiveCheckBox_);
    pDirControlLayout->addStretch();
    pDirControlLayout->addWidget(pRefreshButton_);

    pDirLayout->addWidget(pDirTreeView_);
    pDirLayout->addLayout(pDirControlLayout);
}

void MainWindow::createContentControlWidgets(QWidget* pParentContentControlWidget)
{
    if (!pParentContentControlWidget)
        return;

    pShowSelectedSlideCheckBox_ = new QCheckBox(this);
    pShowSelectedSlideCheckBox_->setText("Show selected slide (slower selection)");

    connect(pShowSelectedSlideCheckBox_, SIGNAL(stateChanged(int)), this, SLOT(showSelectedSlide(int)));

    pSlideInfoModel_ = new SlideInfoModel(this);

    pContentTableView_ = new FirstColumnStretchTableView(this);

    pContentTableView_->setModel(pSlideInfoModel_);

    pContentTableView_->horizontalHeader()->setStretchLastSection(true);
    pContentTableView_->horizontalHeader()->setSortIndicator(1, Qt::AscendingOrder);
    pContentTableView_->horizontalHeader()->setSortIndicatorShown(true);
    pContentTableView_->horizontalHeader()->setClickable(true);
    pContentTableView_->horizontalHeader()->resizeMode(QHeaderView::ResizeToContents);

    pContentTableView_->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);

    pContentTableView_->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);

    pContentTableView_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    pContentTableView_->setShowGrid(false);

    //table cell selection mode
    pContentTableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
    pContentTableView_->setSelectionMode(QAbstractItemView::ExtendedSelection);

    connect(pContentTableView_->horizontalHeader(), SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)), this, SLOT(contentSortIndicatorChanged(int, Qt::SortOrder)));
    connect(pContentTableView_, SIGNAL(showSlideAt(int)), this, SLOT(showSlideAt(int)));
    connect(pContentTableView_, SIGNAL(startSlideShowAt(int)), this, SLOT(startSlideShowAt(int)));
    connect(pContentTableView_, SIGNAL(startSlideSteppingAt(int)), this, SLOT(startSlideSteppingAt(int)));

    pAddFilesButton_ = new QPushButton(this);
    pAddFilesButton_->setText("Add files");

    connect(pAddFilesButton_, SIGNAL(clicked()), this, SLOT(addFiles()));

    pRemoveFilesButton_ = new QPushButton(this);
    pRemoveFilesButton_->setText("Remove files");

    connect(pRemoveFilesButton_, SIGNAL(clicked()), this, SLOT(removeFiles()));

    pClearContentButton_ = new QPushButton(this);
    pClearContentButton_->setText("Clear list");

    connect(pClearContentButton_, SIGNAL(clicked()), this, SLOT(clearContent()));

    pOnlyImagesButton_ = new QPushButton(this);
    pOnlyImagesButton_->setText("Only images");

    connect(pOnlyImagesButton_, SIGNAL(clicked()), this, SLOT(onlyImages()));

    pMoveUpButton_ = new QPushButton(this);
    pMoveUpButton_->setText("Move up");

    connect(pMoveUpButton_, SIGNAL(clicked()), this, SLOT(moveUp()));

    pMoveDownButton_ = new QPushButton(this);
    pMoveDownButton_->setText("Move down");

    connect(pMoveDownButton_, SIGNAL(clicked()), this, SLOT(moveDown()));

    pMoveFirstButton_ = new QPushButton(this);
    pMoveFirstButton_->setText("Move first");

    connect(pMoveFirstButton_, SIGNAL(clicked()), this, SLOT(moveFirst()));

    pMoveLastButton_ = new QPushButton(this);
    pMoveLastButton_->setText("Move last");

    connect(pMoveLastButton_, SIGNAL(clicked()), this, SLOT(moveLast()));

    // creates a vertical layout in order to have directory
    // tree view and the afferent buttons groupped together
    QVBoxLayout* pContentLayout = new QVBoxLayout(pParentContentControlWidget);

    // the other choice for parent widget would be
    // pContentWidget but when using it there will
    // be a runtime warning message since it already
    // has a layout (pContentLayout)
    QHBoxLayout* pContentControlLayout = new QHBoxLayout(NULL);

    pContentControlLayout->addWidget(pAddFilesButton_);
    pContentControlLayout->addStretch();
    pContentControlLayout->addWidget(pRemoveFilesButton_);
    pContentControlLayout->addStretch();
    pContentControlLayout->addWidget(pClearContentButton_);
    pContentControlLayout->addStretch();
    pContentControlLayout->addWidget(pOnlyImagesButton_);
    pContentControlLayout->addStretch();
    pContentControlLayout->addWidget(pMoveUpButton_);
    pContentControlLayout->addWidget(pMoveDownButton_);
    pContentControlLayout->addStretch();
    pContentControlLayout->addWidget(pMoveFirstButton_);
    pContentControlLayout->addWidget(pMoveLastButton_);

    pContentLayout->addWidget(pShowSelectedSlideCheckBox_);
    pContentLayout->addWidget(pContentTableView_);
    pContentLayout->addLayout(pContentControlLayout);
}

QStringList MainWindow::allSubDirs(QStringList& entries, const QDir& rootDir) const
{
    QFileInfoList subDirEntries = rootDir.entryInfoList(QDir::Dirs | QDir::AllDirs | QDir::NoDot | QDir::NoDotAndDotDot);

    foreach (QFileInfo sudDirEntry, subDirEntries)
        entries.append(sudDirEntry.absoluteFilePath());

    return entries;
}

void MainWindow::appendSlideFromCanonicalFilePath(const QString& canonicalFilePath)
{
    if (!pSlideInfoModel_->containsSlideWithCanonicalFilePath(canonicalFilePath)) {

        pSlideInfoModel_->appendSlide(new ImageSlideInfo(canonicalFilePath));
    }
}

void MainWindow::appendAllFilesFromDir(const QString& canonicalDirPath, bool recurseToSubDirs)
{
    QDir dir(canonicalDirPath);

    foreach (QString file, dir.entryList(QDir::Files | QDir::NoSymLinks | QDir::Readable)) {
        QString canonicalFilePath = QDir::cleanPath(dir.canonicalPath() + QDir::separator() + file);

        appendSlideFromCanonicalFilePath(canonicalFilePath);
    }

    if (recurseToSubDirs) {
        foreach (QString subDir, dir.entryList(QDir::Dirs | QDir::NoSymLinks | QDir::NoDot | QDir::NoDotAndDotDot)) {
            QString canonicalDirPath = QDir::cleanPath(dir.canonicalPath() + QDir::separator() + subDir);

            appendAllFilesFromDir(canonicalDirPath, recurseToSubDirs);
        }
    }
}
