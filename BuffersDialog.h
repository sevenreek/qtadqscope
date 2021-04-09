#ifndef BUFFERSDIALOG_H
#define BUFFERSDIALOG_H

#include <QDialog>

namespace Ui {
class BuffersDialog;
}

class BuffersDialog : public QDialog
{
    Q_OBJECT
public:
    static double FILE_SIZE_LIMIT_SPINBOX_MULTIPLIER;
    explicit BuffersDialog(QWidget *parent = nullptr);
    ~BuffersDialog();
    Ui::BuffersDialog *ui;

private:
};

#endif // BUFFERSDIALOG_H
