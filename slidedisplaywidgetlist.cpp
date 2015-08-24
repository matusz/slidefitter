#include "slidedisplaywidgetlist.h"

#include "slide.h"

#include "slidedisplaywidget.h"

#include <QApplication>
#include <QDesktopWidget>

SlideDisplayWidgetList::SlideDisplayWidgetList(QObject* parent) :
    QObject(parent)
{
}

SlideDisplayWidgetList::~SlideDisplayWidgetList()
{
    qDeleteAll(slideDisplayWidgets_);
}


void SlideDisplayWidgetList::adjustNumber(int newCount)
{
    if (newCount < 0)
        return;

    while (slideDisplayWidgets_.count() > newCount) {
        SlideDisplayWidget* pLastSlideDisplayWidget = slideDisplayWidgets_.takeLast();
        delete pLastSlideDisplayWidget;
    }

    while (slideDisplayWidgets_.count() < newCount) {
        // these widgets have no parent and they will be
        // always replaced as the screen settings changes
        SlideDisplayWidget* pSlideDisplayWidget = new SlideDisplayWidget(slideDisplayWidgets_.count());

        // connects signals for newly created slide display
        // widgets
        connect(pSlideDisplayWidget, SIGNAL(escapeKeyPressed()), this, SIGNAL(escapeKeyPressed()));
        connect(pSlideDisplayWidget, SIGNAL(spaceKeyPressed()), this, SIGNAL(spaceKeyPressed()));
        connect(pSlideDisplayWidget, SIGNAL(leftKeyPressed()), this, SIGNAL(leftKeyPressed()));
        connect(pSlideDisplayWidget, SIGNAL(rightKeyPressed()), this, SIGNAL(rightKeyPressed()));

        connect(pSlideDisplayWidget, SIGNAL(leftButtonPressed()), this, SIGNAL(leftButtonPressed()));
        connect(pSlideDisplayWidget, SIGNAL(rightButtonPressed()), this, SIGNAL(rightButtonPressed()));

        // copies settings for newly created slide display
        // widgets from existing ones (there always should
        // be at least one such slide display widget)
        if (slideDisplayWidgets_.count() > 0) {
            pSlideDisplayWidget->setVisible(slideDisplayWidgets_.first()->isVisible());
            pSlideDisplayWidget->setFinished(slideDisplayWidgets_.first()->isFinished());
            if (slideDisplayWidgets_.first()->isFullScreen())
                pSlideDisplayWidget->showFullScreen();
            else if (slideDisplayWidgets_.first()->isMaximized())
                pSlideDisplayWidget->showMaximized();
            else
                pSlideDisplayWidget->showNormal();
        }

        slideDisplayWidgets_.append(pSlideDisplayWidget);
    }
}

void SlideDisplayWidgetList::create()
{
    qDeleteAll(slideDisplayWidgets_);

    // clear creates a brand new list by return QList<T>()
    // what can cause crashes
    slideDisplayWidgets_.erase(slideDisplayWidgets_.begin(), slideDisplayWidgets_.end());

    int numberOfScreens = QApplication::desktop()->screenCount();

    for (int i = 0; i < numberOfScreens; i++) {
        // these widgets have no parent and they will be
        // always replaced as the screen settings changes
        SlideDisplayWidget* pSlideDisplayWidget = new SlideDisplayWidget(i);

        connect(pSlideDisplayWidget, SIGNAL(escapeKeyPressed()), this, SIGNAL(escapeKeyPressed()));
        connect(pSlideDisplayWidget, SIGNAL(spaceKeyPressed()), this, SIGNAL(spaceKeyPressed()));
        connect(pSlideDisplayWidget, SIGNAL(leftKeyPressed()), this, SIGNAL(leftKeyPressed()));
        connect(pSlideDisplayWidget, SIGNAL(rightKeyPressed()), this, SIGNAL(rightKeyPressed()));

        connect(pSlideDisplayWidget, SIGNAL(leftButtonPressed()), this, SIGNAL(leftButtonPressed()));
        connect(pSlideDisplayWidget, SIGNAL(rightButtonPressed()), this, SIGNAL(rightButtonPressed()));

        pSlideDisplayWidget->setGeometry(QApplication::desktop()->screenGeometry(i));

        slideDisplayWidgets_.append(pSlideDisplayWidget);
    }

    setOrientation();
}

void SlideDisplayWidgetList::resize(int screen)
{
    // changes the geometry and orientation of the
    // corresponding slide display widget and at the
    // end checks whether the orientation of the last
    // slide display widget should be enforced to be
    // different
    for (QList<SlideDisplayWidget*>::iterator iter = slideDisplayWidgets_.begin(); iter != slideDisplayWidgets_.end(); iter++) {
        if (*iter == NULL)
            continue;

        if ((*iter)->screen() == screen)
            (*iter)->setGeometry(QApplication::desktop()->screenGeometry(screen));
    }

    setOrientation();
}

void SlideDisplayWidgetList::setVisibile(bool visible)
{
    for (QList<SlideDisplayWidget*>::iterator iter = slideDisplayWidgets_.begin(); iter != slideDisplayWidgets_.end(); iter++) {
        if (*iter == NULL)
            continue;

        (*iter)->setVisible(visible);
    }
}

void SlideDisplayWidgetList::play(const Slide& slide)
{
    for (QList<SlideDisplayWidget*>::iterator iter = slideDisplayWidgets_.begin(); iter != slideDisplayWidgets_.end(); iter++) {
        if (*iter == NULL)
            continue;

        // in order to have keyboard input ensured
        // (from the current slide display widget)
        (*iter)->activateWindow();

        (*iter)->play(slide);
    }
}

void SlideDisplayWidgetList::setFinished(bool finished)
{
    // foreach (SlideDisplayWidget* p, *this)
    // causes crashes for some strange reason
    for (QList<SlideDisplayWidget*>::iterator iter = slideDisplayWidgets_.begin(); iter != slideDisplayWidgets_.end(); iter++) {
        if (*iter == NULL)
            continue;

        (*iter)->setFinished(finished);
    }
}

void SlideDisplayWidgetList::showFullScreen()
{
    // foreach (SlideDisplayWidget* p, *this)
    // causes crashes for some strange reason
    for (QList<SlideDisplayWidget*>::iterator iter = slideDisplayWidgets_.begin(); iter != slideDisplayWidgets_.end(); iter++) {
        if (*iter == NULL)
            continue;

        (*iter)->showFullScreen();;
    }
}

void SlideDisplayWidgetList::setOrientation()
{
    int slides = slideDisplayWidgets_.count();

    int numberOfScreens = QApplication::desktop()->screenCount();

    bool foundLandscapeScreen = this->foundLandscapeScreen();
    bool foundPortraitScreen = this->foundPortraitScreen();

    for (int i = 0; i < slides; i++) {
        SlideDisplayWidget *pSlideDisplayWidget = slideDisplayWidgets_.at(i);
        QRect screenRect = QApplication::desktop()->screenGeometry(pSlideDisplayWidget->screen());

        if (numberOfScreens == 1)
            // if there is only one screen, it has to be
            // able to show all the slides
            pSlideDisplayWidget->setOrientation(SlideDisplayWidget::Landscape | SlideDisplayWidget::Portrait);
        else {
            // if there are many screens and they are oriented
            // in the same way the slide display widget placed
            // to the last screen will be able to show slides
            // in different orientations than the screens are
            // oriented
            if (foundLandscapeScreen && foundPortraitScreen) {
                // there are screens in both orientations
                if (screenRect.width() >= screenRect.height())
                    pSlideDisplayWidget->setOrientation(SlideDisplayWidget::Landscape);

                if (screenRect.height() >= screenRect.width())
                    pSlideDisplayWidget->setOrientation(SlideDisplayWidget::Portrait);
            } else {
                // all the screens are oriented in the same way
                // (slide display widget corresponding to the last
                // screen will be oriented in different way)
                if (i < numberOfScreens - 1) {
                    // in case of screens excepting the last one
                    // slide display widget orientation similar
                    // with screen orientation
                    if (screenRect.width() >= screenRect.height())
                        pSlideDisplayWidget->setOrientation(SlideDisplayWidget::Landscape);

                    if (screenRect.height() >= screenRect.width())
                        pSlideDisplayWidget->setOrientation(SlideDisplayWidget::Portrait);
                } else {
                    // for the last screen enforces slide display
                    // widget with different orientation
                    if (!foundLandscapeScreen)
                        pSlideDisplayWidget->setOrientation(SlideDisplayWidget::Landscape);

                    if (!foundPortraitScreen)
                        pSlideDisplayWidget->setOrientation(SlideDisplayWidget::Portrait);
                }
            }
        }
    }
}

bool SlideDisplayWidgetList::foundLandscapeScreen() const
{
    int numberOfScreens = QApplication::desktop()->screenCount();

    for (int i = 0; i < numberOfScreens; i++) {
        QRect screenRect = QApplication::desktop()->screenGeometry(i);

        // square shaped screens (never seen such)
        // are oriented in both ways
        if (screenRect.width() >= screenRect.height())
            return true;
    }

    return false;
}

bool SlideDisplayWidgetList::foundPortraitScreen() const
{
    int numberOfScreens = QApplication::desktop()->screenCount();

    for (int i = 0; i < numberOfScreens; i++) {
        QRect screenRect = QApplication::desktop()->screenGeometry(i);

        // square shaped screens (never seen such)
        // are oriented in both ways
        if (screenRect.height() >= screenRect.width())
            return true;
    }

    return false;
}
