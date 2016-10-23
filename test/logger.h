/*
                     +-Subcategories (bits 9-13)
                     |   +-Subsystems (bits 5-9)
                     |   |   +-Priorities (bits 0-5)
                     |   |   |    
                     v   v   v    
  +--------------------------------+
  |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
  +--------------------------------+
   ^ bit 32                       ^ bit 0
*/

extern int GlobalVerbosity;

enum
{
  //NOTE: Priorities
  LOG_VERBOSE        = 1 <<  0,
  LOG_INFO           = 1 <<  1,
  LOG_WARN           = 1 <<  2,
  LOG_ERROR          = 1 <<  3,
  LOG_FATAL          = 1 <<  4,

  //NOTE: Subsystems
  LOG_PLATFORM       = 1 <<  5,
  LOG_AUDIO          = 1 <<  6,
  LOG_VIDEO          = 1 <<  7,
  LOG_NETWORK        = 1 <<  8,

  //NOTE: Subcategories
  LOG_INIT           = 1 <<  9,
  LOG_SHUTDOWN       = 1 << 10,
  LOG_ACTIVATE       = 1 << 11,
  LOG_DEACTIVATE     = 1 << 12,

  //NOTE: Masks
  LOG_PRIORITIES        = 0x0000001f,  /* (bits 00-04) */
  LOG_SUBSYSTEMS        = 0x000001e0,  /* (bits 05-08) */
  LOG_SUBCATEGORIES     = 0x00001e00,  /* (bits 09-12) */
  LOG_EVERYTHING        = 0x7fffffff
};

#define Log(Flags, Format, ...) \
{ \
  enum { \
  VERBOSE    = LOG_VERBOSE, \
  INFO       = LOG_INFO, \
  WARN       = LOG_WARN, \
  ERROR      = LOG_ERROR, \
  FATAL      = LOG_FATAL, \
  PLATFORM   = LOG_PLATFORM, \
  AUDIO      = LOG_AUDIO, \
  VIDEO      = LOG_VIDEO, \
  NETWORK    = LOG_NETWORK, \
  INIT       = LOG_INIT, \
  SHUTDOWN   = LOG_SHUTDOWN, \
  ACTIVATE   = LOG_ACTIVATE, \
  DEACTIVATE = LOG_DEACTIVATE, \
  }; \
  if ((Flags) & GlobalVerbosity) { \
    printf("%s%s%s %12.12s:%4i " Format "\n", \
           LOG_PrioritiesLabel(Flags), LOG_SubsystemsLabel(Flags), LOG_SubcategoriesLabel(Flags), \
           __FILE__, __LINE__, ##__VA_ARGS__); \
  } \
}
static inline const char* LOG_PrioritiesLabel(int Flags)
{
  switch (Flags & LOG_PRIORITIES)
  {
    case LOG_VERBOSE:     return "[VERBOSE]";
    case LOG_INFO:        return "[INFO   ]";
    case LOG_WARN:        return "[WARN   ]";
    case LOG_ERROR:       return "[ERROR  ]";
    case LOG_FATAL:       return "[FATAL  ]";
  }
  return "[       ]";
}

static inline const char* LOG_SubsystemsLabel(int Flags)
{
  switch (Flags & LOG_SUBSYSTEMS)
  {
    case LOG_PLATFORM:     return "[PLATFORM]";
    case LOG_AUDIO:        return "[AUDIO   ]";
    case LOG_VIDEO:        return "[VIDEO   ]";
    case LOG_NETWORK:      return "[NETWORK ]";
  }
  return "[        ]";
}

static inline const char* LOG_SubcategoriesLabel(int Flags)
{
  switch (Flags & LOG_SUBCATEGORIES)
  {
    case LOG_INIT:           return "[INIT      ]";
    case LOG_SHUTDOWN:       return "[SHUTDOWN  ]";
    case LOG_ACTIVATE:       return "[ACTIVATE  ]";
    case LOG_DEACTIVATE:     return "[DEACTIVATE]";
  }
  return "[          ]";
}

