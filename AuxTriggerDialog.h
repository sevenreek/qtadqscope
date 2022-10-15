#ifndef AUXTRIGGERDIALOG_H
#define AUXTRIGGERDIALOG_H

#include <QDialog>

namespace Ui {
class AuxTriggerDialog;
}

class AuxTriggerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AuxTriggerDialog(QWidget *parent = nullptr);
    ~AuxTriggerDialog();

private:
    Ui::AuxTriggerDialog *ui;
};

#endif // AUXTRIGGERDIALOG_H
