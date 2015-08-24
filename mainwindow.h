#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QList>

class QSettings;

class QDir;
class QFileSystemWatcher;
class QDirModel;

class SlideInfoModel;

class Slide;
class SlideLoader;
class SlideShowSequencer;
class SlideDisplayWidget;
class SlideDisplayWidgetList;

class FirstColumnStretchTreeView;
class QCheckBox;
class QPushButton;

class FirstColumnStretchTableView;

class QSplitter;

class QHBoxLayout;

class QKeyEvent;
class QMouseEvent;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void directoryChanged(const QString& path);

    void dirSortIndicatorChanged(int logicalIndex, Qt::SortOrder order);
    void refreshDirModel();

    void showSelectedSlide(int state);
    void contentSortIndicatorChanged(int logicalIndex, Qt::SortOrder order);

    void addFilesAction();
    void addFilesRecursivelyAction();

    void addFiles();
    void removeFiles();
    void clearContent();

    void onlyImages();

    void moveUp();
    void moveDown();

    void moveFirst();
    void moveLast();

    void resized(int screen);
    void screenCountChanged(int newCount);
    void workAreaResized(int screen);

    void closeMainWindow();

    void slideShowSettings();

    void showSlideAt(int index);
    void startSlideShow();
    void startSlideShowAt(int index);
    void stopSlideShow();

    void startSlideStepping();
    void startSlideSteppingAt(int index);

    void loadNextSlide();
    void showNextSlide();

    void slideEscapeKeyPressed();
    void slideSpaceKeyPressed();
    void slideLeftKeyPressed();
    void slideRightKeyPressed();

    void slideLeftButtonPressed();
    void slideRightButtonPressed();

protected:
    void beginSlideShowWithSlide(int index);

    void createDirControlWidgets(QWidget* pParentDirControlWidget);
    void createContentControlWidgets(QWidget* pParentContentControlWidget);

    QStringList allSubDirs(QStringList& entries, const QDir& rootDir) const;

    void appendSlideFromCanonicalFilePath(const QString& canonicalFilePath);
    void appendAllFilesFromDir(const QString& canonicalDirPath, bool recurseToSubDirs);

protected:
    Ui::MainWindow *ui;

    QSettings* pSettings_;

    QFileSystemWatcher* pFileSystemWatcher_;
    QDirModel* pDirModel_;

    SlideInfoModel* pSlideInfoModel_;

    bool finishSlideShow_;
    int currentSlideIndex_;

    SlideLoader* pSlideLoader_;
    SlideShowSequencer* pSlideShowSequencer_;

    SlideDisplayWidget* pSlideDisplayWidget_;

    FirstColumnStretchTreeView* pDirTreeView_;
    QCheckBox* pRecursiveCheckBox_;
    QPushButton* pRefreshButton_;

    QCheckBox* pShowSelectedSlideCheckBox_;
    FirstColumnStretchTableView* pContentTableView_;
    QPushButton* pAddFilesButton_;
    QPushButton* pRemoveFilesButton_;
    QPushButton* pClearContentButton_;
    QPushButton* pOnlyImagesButton_;
    QPushButton* pMoveUpButton_;
    QPushButton* pMoveDownButton_;
    QPushButton* pMoveFirstButton_;
    QPushButton* pMoveLastButton_;

    QSplitter* pMainSplitter_;

    SlideDisplayWidgetList* pSlideDisplayWidgetListFullScreen_;
};

#endif // MAINWINDOW_H
