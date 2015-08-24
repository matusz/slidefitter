#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include "constants.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    setWindowTitle(QString("Settings"));

    // set min and max timeout values
    ui->spinBox->setMinimum(cMinSequenceTimeout);
    ui->spinBox->setMaximum(cMaxSequenceTimeout);

    // necessary to avoid resizable dialog frame
    setFixedSize(size());
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

qreal SettingsDialog::sequenceTimeout() const
{
    return static_cast<qreal>(ui->spinBox->value());
}

void SettingsDialog::setSequenceTimeout(qreal sequenceTimeout)
{
    ui->spinBox->setValue(static_cast<int>(sequenceTimeout));
}
