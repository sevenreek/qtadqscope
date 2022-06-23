#include "ApplicationContext.h"
#include <iostream>
#include <fstream>

ApplicationContext::ApplicationContext(ApplicationConfiguration *config, Digitizer *digitizer, ScopeUpdater *scope, spdlog::logger *logger) :
    appConfiguration(config), digitizer(digitizer), scopeUpdater(scope), primaryLogger(logger)
{

}
