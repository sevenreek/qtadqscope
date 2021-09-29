#ifndef PRIMARYCONTROLS_H
#define PRIMARYCONTROLS_H

#include <QWidget>
#include "DigitizerGUIComponent.h"
#include "GUILogger.h"
namespace Ui {
class PrimaryControls;
}

class PrimaryControls : public QWidget, public DigitizerGUIComponent
{
    Q_OBJECT

public:
    explicit PrimaryControls(QWidget *parent = nullptr);
    ~PrimaryControls();
    void reloadUI();
    void initialize(ApplicationContext * context);
public slots:
    void resetFillIndicators();
private:
    std::shared_ptr<QGUILogSink_mt> logSink;
    Ui::PrimaryControls *ui;
    QTimer periodicUpdateTimer;
    ApplicationContext *context;
private slots:
    void primaryButtonClicked();
    void digitizerStateChanged(Digitizer::DIGITIZER_STATE state);
    void periodicUIUpdate();

signals:
    void resetPlot();

    // DigitizerGUIComponent interface
public:
    void enableVolatileSettings(bool enabled) override;
};

#endif // PRIMARYCONTROLS_H
