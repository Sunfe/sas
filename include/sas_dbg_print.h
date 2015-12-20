/******************************************************************************
 ******************************************************************************/
#ifndef SAS_ERROR_HANDLER_H
# define SAS_ERROR_HANDLER_H

# include <stdio.h>

# define SAS_MAX_ERROR_LEN 200

# define SAS_LOG_NOTHING       0
# define SAS_LOG_FAILURE_LEVEL 1
# define SAS_LOG_ERROR_LEVEL   2
# define SAS_LOG_INFO_LEVEL    3
# define SAS_LOG_DETAIL_LEVEL  4
# define SAS_LOG_DEBUG_LEVEL   5
# define SAS_LOG_JUNK_LEVEL    6

# ifndef SAS_LOG_DEFAULT
#  define SAS_LOG_DEFAULT SAS_LOG_INFO_LEVEL
# endif

#define SAS_LOG_VIEW_DEFAULT 0
#define SAS_LOG_VIEW_HMIQA   1
#define SAS_LOG_VIEW_PROXY   2
#define SAS_LOG_VIEW_COUNT   3
#define SAS_LOG_VIEW_OVH     4
#define SAS_LOG_VIEW_VECT    5
#define SAS_LOG_VIEW_CALLB   6
#define SAS_LOG_NB_VIEWS     7

#define SAS_LOG_VIEW SAS_LOG_VIEW_DEFAULT


int errorHandler(const char *fileName,   /* file name where error was seen */
                 const int   line,       /* line in file */
                 const int   errorLevel, /* error level */
                 const int   errorView,  /* error view */
                 const char *fmt,        /* format string (like printf) */
                 ...);

#  define SAS_LOG errorHandler
#  define SAS_PRINT_FAILURE(format, args...) \
  SAS_LOG(__FILE__, __LINE__, SAS_LOG_FAILURE_LEVEL, SAS_LOG_VIEW, format , ## args)

#  define SAS_PRINT_ERROR(format, args...) \
  SAS_LOG(__FILE__, __LINE__, SAS_LOG_ERROR_LEVEL, SAS_LOG_VIEW, format , ## args)

#  define SAS_PRINT_INFO(format, args...) \
  SAS_LOG(__FILE__, __LINE__, SAS_LOG_INFO_LEVEL, SAS_LOG_VIEW, format , ## args)

#  define SAS_PRINT_DETAIL(format, args...) \
  SAS_LOG(__FILE__, __LINE__, SAS_LOG_DETAIL_LEVEL, SAS_LOG_VIEW, format , ## args)

#  define SAS_PRINT_DEBUG(format, args...) \
  SAS_LOG(__FILE__, __LINE__, SAS_LOG_DEBUG_LEVEL, SAS_LOG_VIEW, format , ## args)

#  define SAS_PRINT_JUNK(format, args...) \
  SAS_LOG(__FILE__, __LINE__, SAS_LOG_JUNK_LEVEL, SAS_LOG_VIEW, format , ## args)


#define SAS_NO_TIME_IN_LOG

#endif
