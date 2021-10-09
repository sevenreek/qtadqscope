#ifndef SCOPEPLOTPANEL_H
#define SCOPEPLOTPANEL_H

#include <QWidget>

namespace Ui {
class ScopePlotPanel;
}

class ScopePlotPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ScopePlotPanel(QWidget *parent = nullptr);
    ~ScopePlotPanel();

private:
    Ui::ScopePlotPanel *ui;
};

#endif // SCOPEPLOTPANEL_H
