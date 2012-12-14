/*
    EVRexx.h: global constants, macros etc.
    Copyright (C) 1994 - 1995 by Bernhard Bablok
    This file is part of the EVRexx package.

    EVRexx is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    EVRexx is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    email: ua302cb@sunmail.lrz-muenchen.de
    smail: B. Bablok
           K.-Schumacher-Str. 70
           D-82256 Fuerstenfeldbruck
*/
#if !defined (EVRexx_H)
  #define EVRexx_H

  #define  INCL_REXXSAA
  #define  INCL_WIN
  #define  INCL_DOS
  #define  INCL_DOSERRORS
  #include <os2.h>
  #include <stdlib.h>
  #include <string.h>

  #define EVR_VERSION         1.00                       /* current version   */
  #define EVR_ACTIVATE_SHADOW "EVRactivateShadow"        /* name of atom      */
  #define EVR_SEMAPHORE_NAME  "\\sem32\\EVR"             /* name of named sem */
  #define EVR_SEM_NAME_LEN    16
  #define EVR_MEMORY_NAME     "\\sharemem\\EVR"          /* base of shared mem*/
  #define EVR_MEM_NAME_LEN    19
  #define EVR_PAGE_SIZE       4096UL                     /* memory page size  */
  #define EVR_SHADOW_CLASS    "EVRshadowClass"           /* name of window cl.*/

  #define EVR_SHADOW_ID       1000                       /* id of object wind.*/

  #define _WANT_PMPRINTF                                 /* enable PMPRINTF   */

  #if defined __EMX__
    #define _System
  #else
    #define P_PM  _P_NOWAIT
  #endif

  /* defines for external functions   --------------------------------------- */

  #define  INVALID_ROUTINE 40                   /* Raise Rexx error           */
  #define  VALID_ROUTINE    0                   /* Successful completion      */

  #define return_rc(rc)                           \
       _ultoa(rc,retstr->strptr,10);              \
       retstr->strlength = strlen(retstr->strptr); \
       return VALID_ROUTINE;

  /* macros   --------------------------------------------------------------- */

  #define max(a,b) ((a) > (b) ? (a) : (b))

  #define SetBroadcastWait(message,text)                                  \
     DosResetEventSem(EVRsemaphore,&pcount);                              \
     TRACE(4,"%x: Broadcasting "text" message\n",EVRpid);                 \
     rc = WinBroadcastMsg(HWND_OBJECT,EVRactivateShadow,(MPARAM) EVRpid,  \
                         (MPARAM) message,BMSG_POST);                     \
     TRACE(4,"%x: RC of WinBroadcastMsg: %lu\n",EVRpid,rc);               \
     TRACE(3,"%x: Waiting on event semaphore ...\n",EVRpid);              \
     DosWaitEventSem(EVRsemaphore,SEM_INDEFINITE_WAIT);                   \
     TRACE(3,"%x: ... and continuing again\n",EVRpid);

  /* prototypes   ----------------------------------------------------------- */

  #include "rexxUtils.h"

  /* internal message ids   ------------------------------------------------- */

  #define EVR_CLEANUP          0
  #define EVR_MESSAGE_BOX      (EVR_CLEANUP +  1)
  #define EVR_FILE_DIALOG      (EVR_CLEANUP +  2)
  #define EVR_FONT_DIALOG      (EVR_CLEANUP +  3)
  #define EVR_LOAD_DIALOG      (EVR_CLEANUP +  4)
  #define EVR_PROC_DIALOG      (EVR_CLEANUP +  5)
  #define EVR_DEST_DIALOG      (EVR_CLEANUP +  6)
  #define EVR_MODAL_DIALOG     (EVR_CLEANUP +  7)
  #define EVR_MODELESS_DIALOG  (EVR_CLEANUP +  8)
  #define EVR_CONTROL          (EVR_CLEANUP +  9)
  #define EVR_REINIT           (EVR_CLEANUP + 10)

  /* request codes   -------------------------------------------------------- */

  #define EVR_BU_ISCHECKED            0
  #define EVR_BU_CHECK                1
  #define EVR_BU_INDEX                2
  #define EVR_BU_ISHILITE             3
  #define EVR_BU_HILITE               4
  #define EVR_BU_SETDEFAULT           5
  #define EVR_BU_ENABLE               6

  #define EVR_TXT_READ                7
  #define EVR_TXT_WRITE               8

  #define EVR_LB_CLEAR                9
  #define EVR_LB_DELETE              10
  #define EVR_LB_INSERT              11
  #define EVR_LB_SELECT              12
  #define EVR_LB_GETSELINDEX         13
  #define EVR_LB_GETSELTEXT          14

  #define EVR_EF_ISREADONLY          15
  #define EVR_EF_SETREADONLY         16
  #define EVR_EF_ISCHANGED           17

  #define EVR_CB_ISLISTSHOWING       18
  #define EVR_CB_SHOWLIST            19

  #define EVR_MLE_CURSOR             -1
  #define EVR_MLE_END                -2
  #define EVR_MLE_WRITELINES         20
  #define EVR_MLE_DELETELINES        21
  #define EVR_MLE_READLINES          22
  #define EVR_MLE_COUNTLINES         23
  #define EVR_MLE_ISREADONLY         24
  #define EVR_MLE_SETREADONLY        25
  #define EVR_MLE_ISCHANGED          26

#endif /* !defined (EVRexx_H) */
