#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include "DigitizerGUIComponent.h"

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog, public DigitizerGUIComponent
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();
    Ui::RegisterDialog *ui;
private:
    Digitizer *digitizer;
public:
    void initialize(ApplicationContext *context) override;
    void reloadUI() override;
    void apply();

    // DigitizerGUIComponent interface
public:
    void enableVolatileSettings(bool enabled) override;
};

#endif // REGISTERDIALOG_H
