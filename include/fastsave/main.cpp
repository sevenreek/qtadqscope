#include "application.hpp"

int main(int argc, char *argv[])
{
    spdlog::info("Program starting");
    Application* app = new Application(argc, argv, 1);
    
    app->start();
    cleanup:
    delete app;
    spdlog::info("Program done");
}