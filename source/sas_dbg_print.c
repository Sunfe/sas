/******************************************************************************
 ******************************************************************************/
#include <stdarg.h>
#include <string.h>
#include "sas_common.h"
#include "sas_dbg_print.h"
#include "sas_timer.h"

static uint8 logVerboseLevel[] = 
{
  SAS_LOG_DEFAULT, SAS_LOG_DEFAULT,
  SAS_LOG_DEFAULT, SAS_LOG_DEFAULT,
  SAS_LOG_DEFAULT, SAS_LOG_DEFAULT,
  SAS_LOG_DEFAULT
};

static char *errorName[] = {
  "DUMMY",
  "FAILURE",
  "ERROR",
  "INFO",
  "NOTICE",
  "DEBUG",
  "JUNK",
};

const char *errorViewName[] = {
  "DEFAULT",
  "FILE",
  "DATABASE",
  "APPLICATION",
};

int errorHandler(const char *fileName,	/* file name where error was seen */
	      const int line,	/* line in file */
	      const int errorLevel,	/* error level */
	      const int errorView,	/* error view */
	      const char *fmt,	/* format string (like printf) */
	      ...)
{
  char format[SAS_MAX_ERROR_LEN], timeStr[25], str[SAS_MAX_ERROR_LEN];
  int l = 0;
  va_list args;

  if (logVerboseLevel[errorView] >= errorLevel)
    {

# ifdef SAS_SHOW_TARGET_NAME_IN_LOG
      l = sprintf (format, "%s ", SAS_TARGET_NAME);
# endif

# ifndef SAS_NO_TIME_IN_LOG
      getCurrentTimeStr (timeStr);
# else
      timeStr[0] = '\0';
# endif

      if (fileName)
	{
	  const char *lastDirSep = strrchr (fileName, '/');
	  const char *baseName = lastDirSep ? lastDirSep + 1 : fileName;

	  sprintf (format + l, "%s%s at %s,%d: %s.\r\n",
		   timeStr, errorName[errorLevel], baseName, line, fmt);
	}
      else
	sprintf (format + l, "%s%s: %s.\r\n",
		 timeStr, errorName[errorLevel], fmt);

      va_start (args, fmt);
      vsprintf (str, format, args);
      va_end (args);

      fprintf(stderr,str);
    }
  return errorLevel;
}


int errorHandler_GetLogLevel (int view)
{
  if (view < SAS_LOG_NB_VIEWS)
    return logVerboseLevel[view];
  else
    return 0;
}


void errorHandler_SetLogLevel (int view, int level)
{
  if (view < SAS_LOG_NB_VIEWS)
    logVerboseLevel[view] = level;
}


const char * errorHandler_GetLogViewName (int view)
{
  if (view < SAS_LOG_NB_VIEWS)
    return errorViewName[view];
  else
    return 0;
}
