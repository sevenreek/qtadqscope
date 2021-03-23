#include "MainWindow.h"
#include "Application.h"
#include <QApplication>
#include "spdlog/spdlog.h"
#include <thread>
#include <iostream>

int main(int argc, char *argv[])
{
    spdlog::set_level(spdlog::level::debug); // Set global log level to debug
    spdlog::flush_on(spdlog::level::debug);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    Application app(w);
    app.start(argc, argv);
    spdlog::info("Starting");
    return a.exec();
}
