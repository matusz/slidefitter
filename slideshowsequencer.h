#ifndef SLIDESHOWSEQUENCER_H
#define SLIDESHOWSEQUENCER_H

#include <QObject>

class QTimerEvent;

enum SequencerSate
{
    IDLE,
    RUNNING,
    PAUSED
};

enum SequencerOperation
{
    LOAD,
    SHOW
};

class SlideShowSequencer : public QObject
{
    Q_OBJECT
public:
    explicit SlideShowSequencer(QObject *parent = 0);
    ~SlideShowSequencer();

    void start();
    void stop();

    void resume();
    void pause();

    bool isIdle() const;
    bool isRunning() const;
    bool isPaused() const;

    qreal sequenceTimeout() const {return sequenceTimeout_;}
    void setSequenceTimeout(qreal sequenceTimeout);

    bool isManualStepping() const {return manualStepping_;}
    void setManualStepping(bool manualStepping) {manualStepping_ = manualStepping;}

signals:
    void loadNextSlide();
    void showNextSlide();

public slots:
    void nextOperation();

protected:
    SequencerSate state_;
    SequencerOperation operation_;

    qreal sequenceTimeout_;
    bool manualStepping_;
};

#endif // SLIDESHOWSEQUENCER_H
