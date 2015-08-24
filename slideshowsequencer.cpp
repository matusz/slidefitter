#include "slideshowsequencer.h"

#include "constants.h"

#include <QTimer>
#include <QtCore/qmath.h>

const double cMaxSequenceTimeout = 999;
const double cMinSequenceTimeout = 1;

// this is the interval for loading a slide before
// it will be shown (loadNextSlide signal will be
// emited before showNextSlide signal with cSlideLoadSeconds
// seconds (if possible) in order to permit the slide
// loader to load the next slide)
const double cSlideLoadTimeoutSecs = 1;

SlideShowSequencer::SlideShowSequencer(QObject *parent) :
    QObject(parent),
    state_(IDLE),
    operation_(LOAD),
    sequenceTimeout_(cMinSequenceTimeout),
    manualStepping_(false)
{
}

SlideShowSequencer::~SlideShowSequencer()
{
    stop();
}

void SlideShowSequencer::start()
{
    // if sequencer was paused it can be resumed but
    // not started (at least logically should be so
    // otherwise the value of isPaused_ flag could
    // fall out of sync by having true value with timer
    // still running)
    if (state_ != IDLE)
        return;

    // emits a load signal and then calls the timer
    // straight away (with 0 timeout) in order to
    // avoid the empty slide at the beginning (this is
    // a bit hackish but no there is no better way)
    emit loadNextSlide();

    // first slide shown straight away
    QTimer::singleShot(0, this, SLOT(nextOperation()));

    state_ = RUNNING;
    // first operation should be show in order to
    // avoid the first empty slide
    operation_ = SHOW;
}

void SlideShowSequencer::stop()
{
    if (state_ == IDLE)
        return;

    state_ = IDLE;
}

void SlideShowSequencer::resume()
{
    // sequencer cane be resumed only from paused state
    if (state_ != PAUSED)
        return;

    QTimer::singleShot(0, this, SLOT(nextOperation()));

    state_ = RUNNING;
}

void SlideShowSequencer::pause()
{
    if (state_ != RUNNING)
        return;

    state_ = PAUSED;
}

bool SlideShowSequencer::isIdle() const
{
    if (state_ == IDLE)
        return true;

    return false;
}

bool SlideShowSequencer::isRunning() const
{
    if (state_ == RUNNING)
        return true;

    return false;
}

bool SlideShowSequencer::isPaused() const
{
    if (state_ == PAUSED)
        return true;

    return false;
}

void SlideShowSequencer::setSequenceTimeout(qreal sequenceTimeout)
{
    if ((sequenceTimeout < cMinSequenceTimeout) ||
        (sequenceTimeout > cMaxSequenceTimeout))
        return;

    sequenceTimeout_ = sequenceTimeout;
}

void SlideShowSequencer::nextOperation()
{
    if (state_ != RUNNING)
        return;

    switch (operation_) {
    case LOAD:
    {
        emit loadNextSlide();

        double showTimerInterval = cSlideLoadTimeoutSecs;
        if (showTimerInterval > sequenceTimeout_)
            showTimerInterval = sequenceTimeout_;

        int showTimeout = qFloor(static_cast<qreal>(1000.0) * showTimerInterval);

        QTimer::singleShot(showTimeout, this, SLOT(nextOperation()));

        operation_ = SHOW;

        break;
    }
    case SHOW:
    {
        emit showNextSlide();

        // in case of manual stepping there will not be automatic
        // reload after showing current slide (reload will come only
        // after user wants to show the next slide)
        if (!manualStepping_) {
            double loadTimerInterval = sequenceTimeout_ - cSlideLoadTimeoutSecs;
            int loadTimeout = qFloor(static_cast<qreal>(1000.0) * loadTimerInterval);

            // in order to avoid all kind of timeout complications
            // the minimum load timeout will be set to 1 msec
            if (loadTimeout < 1)
                loadTimeout = 1;

            QTimer::singleShot(loadTimeout, this, SLOT(nextOperation()));
        }

        operation_ = LOAD;

        break;
    }
    }
}
