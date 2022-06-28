#include "ApplicationContext.h"
#include "ConfigurationController.h"
#include "Digitizer.h"
#include <iostream>
#include <fstream>

ApplicationContext ApplicationContext::instance;
ApplicationContext &ApplicationContext::get()
{
    return ApplicationContext::instance;
}
QConfigurationController *ApplicationContext::config() {return this->config_};
void ApplicationContext::setConfig(QConfigurationController *config)
{
    this->config_ = config;
}
void ApplicationContext::setLogger(spdlog::logger *logger)
{
    this->logger_ = logger;
}
void ApplicationContext::setDigitizer(Digitizer *digitizer)
{
    this->digitizer_ = digitizer;;
}