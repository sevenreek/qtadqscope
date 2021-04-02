#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum TRIGGER_MODE_SELECTOR {
    S_FREE_RUNNING = 0,
    S_SOFTWARE = 1,
    S_LEVEL = 2,
    S_EXTERNAL = 3
};
enum FILE_TYPE_SELECTOR {
    S_BINARY,
    S_BINARY_BUFFERED
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H
