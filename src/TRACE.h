/* Definition of debug-macro TRACE                                            */
/* Copyright (c) 1994-1995 by Bernhard Bablok                                 */

/* Depending on the constant _DEBUG_LEVEL, TRACE output is ignored (in case   */
/* the first argument is greater than _DEBUG_LEVEL) or printed to stderr.     */
/* In case _WANT_PMPRINTF is defined, TRACE output goes to stdout, so that it */
/* can be catched by PMPRINTF (useful for PM programs).                       */

#if !defined(TRACE_H)
  #define TRACE_H
  #if defined(_DEBUG) && defined(__GNUC__)
     #include <stdio.h>
     int pmprintf(__const__ char *, ...);
     #if defined(_DEBUG_LEVEL)

        #if defined(_WANT_PMPRINTF)
           #define TRACE(level,format,args...)      \
              if ((level) <= _DEBUG_LEVEL)          \
                 pmprintf(format,##args);           \
              else                                  \
                 ((void)0);
        #else
           #define TRACE(level,format,args...)      \
              if ((level) <= _DEBUG_LEVEL)          \
                 fprintf(stderr,format,##args);     \
              else                                  \
                 ((void)0);
        #endif

     #else    /* if defined (_DEBUG_LEVEL) */

        #if defined(_WANT_PMPRINTF)
           #define TRACE(level,format,args...)      \
                 pmprintf(format,##args);
        #else
           #define TRACE(level,format,args...)      \
                 fprintf(stderr,format,##args);
        #endif

     #endif   /* if defined (_DEBUG_LEVEL) */
  #else
     #define TRACE(format,args...) ((void)0)
  #endif
#endif
