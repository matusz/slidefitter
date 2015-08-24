#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

    qreal sequenceTimeout() const;
    void setSequenceTimeout(qreal sequenceTimeout);

protected:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
