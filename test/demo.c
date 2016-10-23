#include <stdio.h>
#include "logger.h"

int GlobalVerbosity;

int main()
{
  GlobalVerbosity = LOG_EVERYTHING;
  Log(VERBOSE|PLATFORM, "Startup");
  Log(INFO|VIDEO, "Video init");
  Log(AUDIO|ERROR|INIT, "Fatal error");
}

