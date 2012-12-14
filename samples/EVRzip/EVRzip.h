/* definition of constants for the EVRzip example   ------------------------- */
/* Copyright (c) 1994-1995 bye Bernhard Bablok                                */

/* dialog IDs:   ------------------------------------------------------------ */

#define ZIPINFO_DIALOG   100
#define UNZIP_DIALOG     200

/* zipinfo options:   ------------------------------------------------------- */

#define  OPTION_1        1001        /* option -1: filenames only             */
#define  OPTION_SHORT    1003        /* option -s: short                      */
#define  OPTION_MEDIUM   1004        /* option -m: medium                     */
#define  OPTION_LONG     1005        /* option -l: long                       */

/* textfields for header and totals:   -------------------------------------- */

#define  ARCH_HEADER     2001
#define  ARCH_TOTALS     2002

/* listbox for file infos:   ------------------------------------------------ */

#define  ARCH_INFO       3001

/* pushbuttons:   ----------------------------------------------------------- */

#define  UNZIP_SELECTED  4001
#define  CHANGE_OPTIONS  4002

/* entryfield for directory:   ---------------------------------------------- */

#define UNZIP_DIR        5001

/* unzip options:   --------------------------------------------------------- */

#define OPTION_JUNK      1007
#define OPTION_OVERWRITE 1008
#define OPTION_NEVEROVWR 1009
#define OPTION_FRESHEN   1010
#define OPTION_UPDATE    1011
#define OPTION_PROMPT    1012
