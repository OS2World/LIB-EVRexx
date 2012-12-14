/*
    processControl.c: source code of functions processing individual
                      request-codes during calls to EVRmanageControl
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
#include <stdlib.h>
#include "EVRexx.h"
#include "xternEXE.h"

#define  _DEBUG_LEVEL  4
#include "trace.h"

/* -------------------------------------------------------------------------- */
/* processControl                                                             */
/*   Array of pointers to functions which process the shared memory           */
/*   object with request specific data.                                       */
/* -------------------------------------------------------------------------- */

typedef  VOID (*procFkt)(HWND, ULONG, PVOID*, PVOID*);

VOID pNull(HWND,ULONG,PVOID*,PVOID*);

VOID pButtonIsChecked(HWND,ULONG,PVOID*,PVOID*);
VOID pButtonCheck(HWND,ULONG,PVOID*,PVOID*);
VOID pButtonIndex(HWND,ULONG,PVOID*,PVOID*);
VOID pButtonIsHilite(HWND,ULONG,PVOID*,PVOID*);
VOID pButtonHilite(HWND,ULONG,PVOID*,PVOID*);
VOID pButtonSetDefault(HWND,ULONG,PVOID*,PVOID*);
VOID pButtonEnable(HWND,ULONG,PVOID*,PVOID*);

VOID pTextRead(HWND,ULONG,PVOID*,PVOID*);
VOID pTextWrite(HWND,ULONG,PVOID*,PVOID*);

VOID pListBoxClear(HWND,ULONG,PVOID*,PVOID*);
VOID pListBoxDelete(HWND,ULONG,PVOID*,PVOID*);
VOID pListBoxInsert(HWND,ULONG,PVOID*,PVOID*);
VOID pListBoxSelect(HWND,ULONG,PVOID*,PVOID*);
VOID pListBoxGetSelIndex(HWND,ULONG,PVOID*,PVOID*);
VOID pListBoxGetSelText(HWND,ULONG,PVOID*,PVOID*);

VOID pEntryFieldIsReadOnly(HWND,ULONG,PVOID*,PVOID*);
VOID pEntryFieldSetReadOnly(HWND,ULONG,PVOID*,PVOID*);
VOID pEntryFieldIsChanged(HWND,ULONG,PVOID*,PVOID*);

VOID pComboBoxIsListShowing(HWND,ULONG,PVOID*,PVOID*);
VOID pComboBoxShowList(HWND,ULONG,PVOID*,PVOID*);

VOID pMLEWriteLines(HWND,ULONG,PVOID*,PVOID*);
VOID pMLEDeleteLines(HWND,ULONG,PVOID*,PVOID*);
VOID pMLEReadLines(HWND,ULONG,PVOID*,PVOID*);
VOID pMLECountLines(HWND,ULONG,PVOID*,PVOID*);
VOID pMLEIsReadOnly(HWND,ULONG,PVOID*,PVOID*);
VOID pMLESetReadOnly(HWND,ULONG,PVOID*,PVOID*);
VOID pMLEIsChanged(HWND,ULONG,PVOID*,PVOID*);

procFkt processControl[] =
   {
      &pButtonIsChecked,          /*  "BU.ISCHECKED",                         */
      &pButtonCheck,              /*  "BU.CHECK",                             */
      &pButtonIndex,              /*  "BU.INDEX",                             */
      &pButtonIsHilite,           /*  "BU.ISHILITE",                          */
      &pButtonHilite,             /*  "BU.HILITE",                            */
      &pButtonSetDefault,         /*  "BU.SETDEFAULT",                        */
      &pButtonEnable,             /*  "BU.ENABLE",                            */

      &pTextRead,                 /*  "TXT.READ",                             */
      &pTextWrite,                /*  "TXT.WRITE",                            */

      &pListBoxClear,             /*  "LB.CLEAR"                              */
      &pListBoxDelete,            /*  "LB.DELETE"                             */
      &pListBoxInsert,            /*  "LB.INSERT"                             */
      &pListBoxSelect,            /*  "LB.SELECT"                             */
      &pListBoxGetSelIndex,       /*  "LB.GETSELINDEX"                        */
      &pListBoxGetSelText,        /*  "LB.GETSELTEXT"                         */

      &pEntryFieldIsReadOnly,     /*  "EF.ISREADONLY"                         */
      &pEntryFieldSetReadOnly,    /*  "EF.SETREADONLY"                        */
      &pEntryFieldIsChanged,      /*  "EF.ISCHANGED"                          */

      &pComboBoxIsListShowing,    /*  "CB.ISLISTSHOWING"                      */
      &pComboBoxShowList,         /*  "CB.SHOWLIST"                           */

      &pMLEWriteLines,            /*  "MLE.WRITELINES"                        */
      &pMLEDeleteLines,           /*  "MLE.DELETELINES"                       */
      &pMLEReadLines,             /*  "MLE.READLINES"                         */
      &pMLECountLines,            /*  "MLE.COUNTLINES"                        */
      &pMLEIsReadOnly,            /*  "MLE.ISREADONLY"                        */
      &pMLESetReadOnly,           /*  "MLE.SETREADONLY"                       */
      &pMLEIsChanged              /*  "MLE.ISCHANGED"                         */
   };


/* -------------------------------------------------------------------------- */
/* p*: Functions which process an individual request.                         */
/* On entry, requestData points to the request specific data, and memptr      */
/* points to the next free memory location in the shared memory object.       */
/* After each of these functions return, the memptr must again point to the   */
/* next free memory location, and requestData must point to the next data     */
/* item.                                                                      */
/* -------------------------------------------------------------------------- */

VOID pNull(HWND hwndDlg, ULONG controlId, PVOID* requestData, PVOID* memptr) {}

VOID pButtonIsChecked(HWND hwndDlg, ULONG controlId, PVOID* requestData,
                                                                PVOID* memptr) {
   ULONG  msgId;
   USHORT checkState;

   msgId     = BM_QUERYCHECK;

   checkState = (USHORT) WinSendDlgItemMsg(hwndDlg,controlId,msgId,NULL,NULL);
   *(USHORT*)*memptr = checkState;
   *memptr += sizeof(USHORT);
}

VOID pButtonCheck(HWND hwndDlg, ULONG controlId, PVOID* requestData,
                                                                PVOID* memptr) {
   ULONG  msgId;
   USHORT checkState;

   msgId     = BM_SETCHECK;
   checkState = *(USHORT*)*requestData;       /* new checkstate               */
   *requestData += sizeof(USHORT);

   checkState = (USHORT) WinSendDlgItemMsg(hwndDlg,controlId,msgId,
                                                 MPFROMSHORT(checkState), NULL);
   *(USHORT*)*memptr = checkState;            /* return old checkState        */
   *memptr += sizeof(USHORT);
}

VOID pButtonIndex(HWND hwndDlg, ULONG controlId, PVOID* requestData,
                                                                PVOID* memptr) {
   ULONG  msgId;
   SHORT  index;

   msgId     = BM_QUERYCHECKINDEX;

   index = (SHORT) WinSendDlgItemMsg(hwndDlg,controlId,msgId,NULL,NULL);
   *(SHORT*)*memptr = index;
   *memptr += sizeof(SHORT);
}

VOID pButtonIsHilite(HWND hwndDlg, ULONG controlId, PVOID* requestData,
                                                                PVOID* memptr) {
   ULONG  msgId;
   BOOL   state;

   msgId     = BM_QUERYHILITE;

   state = (BOOL) WinSendDlgItemMsg(hwndDlg,controlId,msgId,NULL,NULL);
   *(BOOL*)*memptr = state;
   *memptr += sizeof(BOOL);
}

VOID pButtonHilite(HWND hwndDlg, ULONG controlId, PVOID* requestData,
                                                                PVOID* memptr) {
   ULONG  msgId;
   USHORT newState;
   BOOL   oldState;

   msgId = BM_SETHILITE;
   newState = *(USHORT*)*requestData;
   *requestData += sizeof(USHORT);

   oldState = (BOOL) WinSendDlgItemMsg(hwndDlg,controlId,msgId,
                                                   MPFROMSHORT(newState), NULL);
   *(BOOL*)*memptr = oldState;
   *memptr += sizeof(BOOL);
}

VOID pButtonSetDefault(HWND hwndDlg, ULONG controlId, PVOID* requestData,
                                                                PVOID* memptr) {
   ULONG  msgId;
   USHORT newState;
   BOOL   fSuccess;

   msgId = BM_SETDEFAULT;
   newState = *(USHORT*)*requestData;
   *requestData += sizeof(USHORT);

   fSuccess = (BOOL) WinSendDlgItemMsg(hwndDlg,controlId,msgId,
                                                   MPFROMSHORT(newState), NULL);
   *(BOOL*)*memptr = fSuccess;
   *memptr += sizeof(BOOL);
}


VOID pButtonEnable(HWND hwndDlg, ULONG controlId, PVOID* requestData,
                                                                PVOID* memptr) {
   ULONG  msgId;
   USHORT newState;

   msgId = WM_ENABLE;
   newState = *(USHORT*)*requestData;
   *requestData += sizeof(USHORT);

   WinSendDlgItemMsg(hwndDlg,controlId,msgId,MPFROMSHORT(newState),NULL);
}

/* -------------------------------------------------------------------------- */

VOID pTextRead(HWND hwndDlg, ULONG controlId, PVOID* requestData,
                                                                PVOID* memptr) {
   HWND hwndControl;
   LONG textLength;

   hwndControl = WinWindowFromID(hwndDlg,controlId);
   if (hwndControl == 0)
      hwndControl = hwndDlg;
   textLength  = WinQueryWindowTextLength(hwndControl) +1;
   WinQueryWindowText(hwndControl, textLength, (CHAR*)*memptr);
   *memptr += textLength;
}

VOID pTextWrite(HWND hwndDlg, ULONG controlId, PVOID* requestData,
                                                                PVOID* memptr) {
   HWND hwndControl;
   BOOL fSuccess;

   hwndControl = WinWindowFromID(hwndDlg,controlId);
   if (hwndControl == 0)
      hwndControl = hwndDlg;
   fSuccess = WinSetWindowText(hwndControl, (CHAR*)*requestData);
   *requestData += strlen((CHAR*)*requestData) + 1;

   *(BOOL*)*memptr = fSuccess;
   *memptr += sizeof(BOOL);
}

/* -------------------------------------------------------------------------- */

VOID pListBoxClear(HWND hwndDlg, ULONG controlId, PVOID* requestData,
                                                                PVOID* memptr) {
   ULONG  msgId;
   BOOL   fSuccess;

   msgId     = LM_DELETEALL;

   fSuccess = (BOOL) WinSendDlgItemMsg(hwndDlg,controlId,msgId,NULL,NULL);
   *(BOOL*)*memptr = fSuccess;
   *memptr += sizeof(BOOL);
}

VOID pListBoxDelete(HWND hwndDlg, ULONG controlId, PVOID* requestData,
                                                                PVOID* memptr) {
   ULONG  msgId;
   SHORT  index, remaining;

   msgId = LM_DELETEITEM;
   index = *(SHORT*)*requestData;
   if (index > 0)
      index--;                            /* REXX' indices are not offsets!   */
   *requestData += sizeof(SHORT);

   remaining = (BOOL) WinSendDlgItemMsg(hwndDlg,controlId,msgId,
                                                   MPFROMSHORT(index), NULL);
   *(SHORT*)*memptr = remaining;
   *memptr += sizeof(SHORT);
}

VOID pListBoxSelect(HWND hwndDlg, ULONG controlId, PVOID* requestData,
                                                                PVOID* memptr) {
   ULONG  msgId;
   USHORT state;
   SHORT  index;

   msgId = LM_SELECTITEM;
   state = *(USHORT*)*requestData;
   *requestData += sizeof(USHORT);
   index = *(SHORT*)*requestData;
   *requestData += sizeof(SHORT);

   WinSendDlgItemMsg(hwndDlg,controlId,msgId, MPFROMSHORT(index),
                                                            MPFROMSHORT(state));
}

VOID pListBoxInsert(HWND hwndDlg, ULONG controlId, PVOID* requestData,
                                                                PVOID* memptr) {
   ULONG  msgId;
   SHORT  itemIndex, indexInserted, nLines, success = 1;
   CHAR   *item;
   INT    i;

   msgId = LM_INSERTITEM;
   itemIndex = *(SHORT*)*requestData;
   *requestData += sizeof(SHORT);
   nLines = *(SHORT*)*requestData;
   *requestData += sizeof(SHORT);

   TRACE(4,"%x:              itemIndex: %hd\n",EVRpid,itemIndex);
   TRACE(4,"%x:              nLines   : %hd\n",EVRpid,nLines);
   for (i=1; i<=nLines; ++i) {
      item = (CHAR*)*requestData;
      *requestData += strlen(item) + 1;
      TRACE(4,"%x:              item[%d]: %s\n",EVRpid,i,item);
      if (success) {
         indexInserted = (SHORT) WinSendDlgItemMsg(hwndDlg,controlId,msgId,
                                                   MPFROMSHORT(itemIndex), item);
         if (indexInserted == LIT_ERROR || indexInserted == LIT_MEMERROR)
            success = 0;
      }                                 /* can't break because of requestData */
   }
   *(SHORT*)*memptr = success;
   *memptr += sizeof(SHORT);
}

VOID pListBoxGetSelIndex(HWND hwndDlg, ULONG controlId, PVOID* requestData,
                                                                PVOID* memptr) {
   ULONG  msgId;
   SHORT  itemIndex, firstItemIndex, *nSelected;

   msgId = LM_QUERYSELECTION;
   nSelected  = (SHORT*)*memptr;
   *memptr   += sizeof(SHORT);
   *nSelected = 0;

   firstItemIndex = (SHORT) WinSendDlgItemMsg(hwndDlg,controlId,msgId,
                                                  MPFROMSHORT(LIT_FIRST), NULL);
   itemIndex = firstItemIndex;
   while (itemIndex != LIT_NONE) {
      *(SHORT*)*memptr = itemIndex + 1;     /* REXX' indices are not offsets! */
      *memptr    += sizeof(SHORT);
      *nSelected += 1;
      itemIndex = (SHORT) WinSendDlgItemMsg(hwndDlg,controlId,msgId,
                                                  MPFROMSHORT(itemIndex), NULL);
      TRACE(4,"%x:              itemIndex: %hd\n",EVRpid,itemIndex);
      if (itemIndex == firstItemIndex)      /* in case of single selection    */
         return;                            /* listbox!                       */
   }
}

VOID pListBoxGetSelText(HWND hwndDlg, ULONG controlId, PVOID* requestData,
                                                                PVOID* memptr) {
   SHORT  itemIndex, firstItemIndex, count;
   LONG   *nSelected;

   nSelected  = (LONG*)*memptr;
   *memptr   += sizeof(LONG);
   *nSelected = 0;

   firstItemIndex = (SHORT) WinSendDlgItemMsg(hwndDlg,controlId,
                                LM_QUERYSELECTION,MPFROMSHORT(LIT_FIRST), NULL);
   itemIndex = firstItemIndex;
   while (itemIndex != LIT_NONE) {
      count = (SHORT) WinSendDlgItemMsg(hwndDlg,controlId,LM_QUERYITEMTEXT,
                                  MPFROM2SHORT(itemIndex,4096), (CHAR*)*memptr);
      *memptr    += count + 1;
      *nSelected += 1;
      itemIndex = (SHORT) WinSendDlgItemMsg(hwndDlg,controlId,LM_QUERYSELECTION,
                                                  MPFROMSHORT(itemIndex), NULL);
      if (itemIndex == firstItemIndex)      /* in case of single selection    */
         return;                            /* listbox!                       */
   }
}

/* -------------------------------------------------------------------------- */

VOID pEntryFieldIsReadOnly(HWND hwndDlg, ULONG controlId, PVOID* requestData,
                                                                PVOID* memptr) {
   ULONG  msgId;
   BOOL   fReadOnly;

   msgId     = EM_QUERYREADONLY;

   fReadOnly = (BOOL) WinSendDlgItemMsg(hwndDlg,controlId,msgId,NULL,NULL);
   *(BOOL*)*memptr = fReadOnly;
   *memptr += sizeof(BOOL);
}

VOID pEntryFieldSetReadOnly(HWND hwndDlg, ULONG controlId, PVOID* requestData,
                                                                PVOID* memptr) {
   ULONG  msgId;
   USHORT newState;
   BOOL   oldState;

   msgId = EM_SETREADONLY;
   newState = *(USHORT*)*requestData;
   *requestData += sizeof(USHORT);

   oldState = (BOOL) WinSendDlgItemMsg(hwndDlg,controlId,msgId,
                                                   MPFROMSHORT(newState), NULL);
   *(BOOL*)*memptr = oldState;
   *memptr += sizeof(BOOL);
}

VOID pEntryFieldIsChanged(HWND hwndDlg, ULONG controlId, PVOID* requestData,
                                                                PVOID* memptr) {
   ULONG  msgId;
   BOOL   fIsChanged;

   msgId     = EM_QUERYCHANGED;

   fIsChanged = (BOOL) WinSendDlgItemMsg(hwndDlg,controlId,msgId,NULL,NULL);
   *(BOOL*)*memptr = fIsChanged;
   *memptr += sizeof(BOOL);
}

/* -------------------------------------------------------------------------- */

VOID pComboBoxIsListShowing(HWND hwndDlg, ULONG controlId, PVOID* requestData,
                                                                PVOID* memptr) {
   ULONG  msgId;
   BOOL   fIsShowing;

   msgId     = CBM_ISLISTSHOWING;

   fIsShowing = (BOOL) WinSendDlgItemMsg(hwndDlg,controlId,msgId,NULL,NULL);
   *(BOOL*)*memptr = fIsShowing;
   *memptr += sizeof(BOOL);
}

VOID pComboBoxShowList(HWND hwndDlg, ULONG controlId, PVOID* requestData,
                                                                PVOID* memptr) {
   ULONG  msgId;
   USHORT newState;
   BOOL   oldState;

   msgId = CBM_SHOWLIST;
   newState = *(USHORT*)*requestData;
   *requestData += sizeof(USHORT);

   oldState = (BOOL) WinSendDlgItemMsg(hwndDlg,controlId,msgId,
                                                   MPFROMSHORT(newState), NULL);
   *(BOOL*)*memptr = oldState;
   *memptr += sizeof(BOOL);
}

/* -------------------------------------------------------------------------- */

VOID pMLEWriteLines(HWND hwndDlg, ULONG controlId, PVOID* requestData,
                                                                PVOID* memptr) {

   LONG  location, nLines, i;
   ULONG nChars = 0, rc;
   CHAR  *line;
   IPT   start, cursorPos;

   location = *(LONG*)*requestData;
   *requestData += sizeof(LONG);
   nLines   = *(LONG*)*requestData;
   *requestData += sizeof(LONG);

   TRACE(4,"%x:              location : %ld\n",EVRpid,location);
   TRACE(4,"%x:              nLines   : %ld\n",EVRpid,nLines);

   /* change \0 to \n in memory object   ------------------------------------ */

   line = (CHAR*)*requestData;
   for (i=0; i<nLines; ++i) {
      nChars += strlen(line) + 1;
      line   += strlen(line);
      *line   = '\n';
      line++;
   }
   line = (CHAR*)*requestData;
   *requestData += nChars;
   TRACE(4,"%x:              nChars: %lu\n",EVRpid,nChars);

   /* set environment for import   ------------------------------------------ */

   rc = WinSendDlgItemMsg(hwndDlg,controlId,MLM_SETIMPORTEXPORT,
                                                       line, nChars);
   TRACE(4,"%x:              rc of SETIMPORTEXPORT: %lu\n",EVRpid,rc);
   rc = WinSendDlgItemMsg(hwndDlg,controlId,MLM_FORMAT,
                                               MPFROMSHORT(MLFIE_NOTRANS),NULL);
   TRACE(4,"%x:              rc of MLM_FORMAT: %lu\n",EVRpid,rc);
   WinSendDlgItemMsg(hwndDlg,controlId,MLM_DISABLEREFRESH,NULL,NULL);

   /* import text   --------------------------------------------------------- */


   if (location == EVR_MLE_END)
      start = (IPT) WinSendDlgItemMsg(hwndDlg,controlId,MLM_QUERYTEXTLENGTH,
                                                                    NULL, NULL);
   else
      start = (IPT) WinSendDlgItemMsg(hwndDlg,controlId,MLM_CHARFROMLINE,
                                                       (MPARAM) location, NULL);
   TRACE(4,"%x:              start: %lu\n",EVRpid,start);
   cursorPos = start;

   rc = WinSendDlgItemMsg(hwndDlg,controlId,MLM_IMPORT,&start, (MPARAM) nChars);
   TRACE(4,"%x:              nImported: %lu\n",EVRpid,rc);
   WinSendDlgItemMsg(hwndDlg,controlId,MLM_ENABLEREFRESH,NULL,NULL);
   WinSendDlgItemMsg(hwndDlg,controlId,MLM_SETSEL,(MPARAM) cursorPos,
                                                            (MPARAM) cursorPos);
}

VOID pMLEDeleteLines(HWND hwndDlg, ULONG controlId, PVOID* requestData,
                                                                PVOID* memptr) {

   LONG  startLine, nLines;
   ULONG delChars;
   IPT   start, end;

   startLine = *(LONG*)*requestData;
   *requestData += sizeof(LONG);
   nLines   = *(LONG*)*requestData;
   *requestData += sizeof(LONG);

   TRACE(4,"%x:              startLine : %ld\n",EVRpid,startLine);
   TRACE(4,"%x:              nLines    : %ld\n",EVRpid,nLines);

   /* collect IPT values   -------------------------------------------------- */

   start = (IPT) WinSendDlgItemMsg(hwndDlg,controlId,MLM_CHARFROMLINE,
                                                      (MPARAM) startLine, NULL);
   TRACE(4,"%x:              start : %lu\n",EVRpid,start);
   if (startLine == EVR_MLE_CURSOR)
      startLine = (IPT) WinSendDlgItemMsg(hwndDlg,controlId,MLM_LINEFROMCHAR,
                                                          (MPARAM) start, NULL);
   if (nLines == EVR_MLE_END)
      end = (IPT) WinSendDlgItemMsg(hwndDlg,controlId,MLM_QUERYTEXTLENGTH,
                                                                    NULL, NULL);
   else
      end = (IPT) WinSendDlgItemMsg(hwndDlg,controlId,MLM_CHARFROMLINE,
                                             (MPARAM) (startLine+nLines), NULL);
   TRACE(4,"%x:              end  : %lu\n",EVRpid,end);

   /* delete characters   --------------------------------------------------- */

   WinSendDlgItemMsg(hwndDlg,controlId,MLM_DISABLEREFRESH,NULL,NULL);
   delChars = WinSendDlgItemMsg(hwndDlg,controlId,MLM_DELETE,(MPARAM) start,
                                                          (MPARAM) (end-start));
   TRACE(4,"%x:              delChars: %lu\n",EVRpid,delChars);
   WinSendDlgItemMsg(hwndDlg,controlId,MLM_ENABLEREFRESH,NULL,NULL);
}

VOID pMLEReadLines(HWND hwndDlg, ULONG controlId, PVOID* requestData,
                                                                PVOID* memptr) {

   LONG  startLine, *nLines, count;
   ULONG nChars, rc;
   CHAR  *line;
   IPT   start, end;

   startLine = *(LONG*)*requestData;
   *requestData += sizeof(LONG);
   count    = *(LONG*)*requestData;          /* number of lines to read       */
   *requestData += sizeof(LONG);

   nLines   = (LONG*)*memptr;
   *nLines  = 0;                             /* number of lines actually read */
   *memptr += sizeof(LONG);
   line = (CHAR*)*memptr;

   /* collect IPT values   -------------------------------------------------- */

   start = (IPT) WinSendDlgItemMsg(hwndDlg,controlId,MLM_CHARFROMLINE,
                                                      (MPARAM) startLine, NULL);
   TRACE(4,"%x:              start : %lu\n",EVRpid,start);
   if (startLine == EVR_MLE_CURSOR)
      startLine = (IPT) WinSendDlgItemMsg(hwndDlg,controlId,MLM_LINEFROMCHAR,
                                                          (MPARAM) start, NULL);
   if (count == EVR_MLE_END)
      end = (IPT) WinSendDlgItemMsg(hwndDlg,controlId,MLM_QUERYTEXTLENGTH,
                                                                    NULL, NULL);
   else
      end = (IPT) WinSendDlgItemMsg(hwndDlg,controlId,MLM_CHARFROMLINE,
                                             (MPARAM) (startLine+count), NULL);
   TRACE(4,"%x:              end  : %lu\n",EVRpid,end);

   /* export text   --------------------------------------------------------- */

   rc = WinSendDlgItemMsg(hwndDlg,controlId,MLM_FORMAT,
                                               MPFROMSHORT(MLFIE_NOTRANS),NULL);
   TRACE(4,"%x:              rc of MLM_FORMAT: %lu\n",EVRpid,rc);
   rc = WinSendDlgItemMsg(hwndDlg,controlId,MLM_SETIMPORTEXPORT,line,end-start);
   TRACE(4,"%x:              rc of SETIMPORTEXPORT: %lu\n",EVRpid,rc);
   nChars = end - start;
   rc = (ULONG) WinSendDlgItemMsg(hwndDlg,controlId,MLM_EXPORT,&start,&nChars);
   TRACE(4,"%x:              nChars: %lu\n",EVRpid,rc);
   nChars = rc;

   /* change \n to \0 in memory object   ------------------------------------ */

   if (nChars > 0) {
      *(line+nChars) = '\0';
      if (*(line+nChars-1) == '\n')
         *nLines  = 0;
      else if (*(line+nChars-1) == '\0')
         *nLines  = 1;
      else {
         *nLines  = 1;
         nChars++;
      }

      while ((line = strchr(line,'\n')) != NULL) {
         *line    = '\0';
         *nLines += 1;
         line++;
      }
      *memptr += nChars;
   }
}

VOID pMLECountLines(HWND hwndDlg, ULONG controlId, PVOID* requestData,
                                                                PVOID* memptr) {
   ULONG  msgId;
   ULONG   ulLines;

   msgId     = MLM_QUERYLINECOUNT;

   ulLines = (ULONG) WinSendDlgItemMsg(hwndDlg,controlId,msgId,NULL,NULL);
   *(ULONG*)*memptr = ulLines;
   *memptr += sizeof(ULONG);
}

VOID pMLEIsReadOnly(HWND hwndDlg, ULONG controlId, PVOID* requestData,
                                                                PVOID* memptr) {
   ULONG  msgId;
   BOOL   fReadOnly;

   msgId     = MLM_QUERYREADONLY;

   fReadOnly = (BOOL) WinSendDlgItemMsg(hwndDlg,controlId,msgId,NULL,NULL);
   *(BOOL*)*memptr = fReadOnly;
   *memptr += sizeof(BOOL);
}

VOID pMLESetReadOnly(HWND hwndDlg, ULONG controlId, PVOID* requestData,
                                                                PVOID* memptr) {
   ULONG  msgId;
   USHORT newState;
   BOOL   oldState;

   msgId = MLM_SETREADONLY;
   newState = *(USHORT*)*requestData;
   *requestData += sizeof(USHORT);

   oldState = (BOOL) WinSendDlgItemMsg(hwndDlg,controlId,msgId,
                                                   MPFROMSHORT(newState), NULL);
   *(BOOL*)*memptr = oldState;
   *memptr += sizeof(BOOL);
}

VOID pMLEIsChanged(HWND hwndDlg, ULONG controlId, PVOID* requestData,
                                                                PVOID* memptr) {
   ULONG  msgId;
   BOOL   fIsChanged;

   msgId     = MLM_QUERYCHANGED;

   fIsChanged = (BOOL) WinSendDlgItemMsg(hwndDlg,controlId,msgId,NULL,NULL);
   *(BOOL*)*memptr = fIsChanged;
   *memptr += sizeof(BOOL);
}

