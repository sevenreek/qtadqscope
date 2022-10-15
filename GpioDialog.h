#ifndef GPIODIALOG_H
#define GPIODIALOG_H

#include <QDialog>
#include <QLabel>
#include <QCheckBox>
#include "DigitizerGUIComponent.h"

namespace Ui {
class GPIODialog;
}

class GPIODialog : public QDialog, public DigitizerGUIComponent
{
    Q_OBJECT

public:
    static const int GPIO_COUNT = 16;
    explicit GPIODialog(QWidget *parent = nullptr);
    ~GPIODialog();
    void reloadUI() override;
    void initialize(ApplicationContext *context) override;
    void enableVolatileSettings(bool enabled) override;
public slots:
    void onStateChanged(Digitizer::DIGITIZER_STATE newState);

private:
    QLabel * portLabels[GPIO_COUNT];
    QLabel * portInputValues[GPIO_COUNT];
    QCheckBox * portOutputEnables[GPIO_COUNT];
    QCheckBox * portOutputValues[GPIO_COUNT];
    QLabel * headerLeftOutput;
    QLabel * headerLeftNames;
    QLabel * headerInput;
    QLabel * headerRightNames;
    QLabel * headerRightOutput;
    void setOutputEnable(int port, bool enable);
    void setValue(int port, bool val);

    Ui::GPIODialog *ui;
    void createGPIOGrid();

private slots:
    void readGPIO();
    void enableSupplyOutput(bool en);
};

#endif // GPIODIALOG_H
