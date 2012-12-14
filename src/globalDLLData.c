/*
    globalDLLData.c: DLL-data shared by all process using EVRexx.dll
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
#include "EVRexx.h"


/* -------------------------------------------------------------------------- */
/* EVRfuncTable                                                               */
/*   Array of names of the EVR functions.                                     */
/*   This list is used for registration and deregistration.                   */
/* -------------------------------------------------------------------------- */

ULONG EVRfuncTableSize = 13;

PSZ  EVRfuncTable[] =
   {
     "EVRversion",
     "EVRloadFuncs",
     "EVRdropFuncs",
     "EVRinit",
     "EVRsetVars",
     "EVRcleanup",
     "EVRmessageBox",
     "EVRfileDialog",
     "EVRloadDialog",
     "EVRmodalDialog",
     "EVRmodelessDialog",
     "EVRdestroyDialog",
     "EVRmanageControl"
   };

/* -------------------------------------------------------------------------- */
/* EVRnameTable                                                               */
/*   Array of names of REXX variables which are set during initialization.    */
/*   The corresponding values are in EVRvalueTable.                           */
/* -------------------------------------------------------------------------- */

PSZ EVRnameTable[] =
   {
      /* buttons for message boxes (9/9 entries):   ------------------------- */

      "MB.OK",
      "MB.OKCANCEL",
      "MB.RETRYCANCEL",
      "MB.ABORTRETRYIGNORE",
      "MB.YESNO",
      "MB.YESNOCANCEL",
      "MB.CANCEL",
      "MB.ENTER",
      "MB.ENTERCANCEL",

      /* return codes from message boxes (10/19 entries):   ----------------- */

      "MB.RC_OK",
      "MB.RC_CANCEL",
      "MB.RC_ABORT",
      "MB.RC_RETRY",
      "MB.RC_IGNORE",
      "MB.RC_YES",
      "MB.RC_NO",
      "MB.RC_HELP",
      "MB.RC_ENTER",
      "MB.RC_ERROR",

      /* icons for message boxes (4/23 entries):   -------------------------- */

      "MB.QUERY",
      "MB.WARNING",
      "MB.INFORMATION",
      "MB.ERROR",

      /* options for file dialog (2/25 entries):   -------------------------- */

      "FD.OPEN",
      "FD.SAVEAS",

      /* standard dialog IDs (2/27 entries):   ------------------------------ */

      "DID_OK",
      "DID_CANCEL",

      /* request codes for buttons: (7/34 entries):   ----------------------- */

      "BU.ISCHECKED",
      "BU.CHECK",
      "BU.INDEX",
      "BU.ISHILITE",
      "BU.HILITE",
      "BU.SETDEFAULT",
      "BU.ENABLE",

      /* request codes for text (2/36 entries):   --------------------------- */

      "TXT.READ",
      "TXT.WRITE",

      /* request codes for listboxes (9/45 entries):   ---------------------- */

      "LB.CLEAR",
      "LB.DELETE",
      "LB.INSERT",
      "LB.SELECT",
      "LB.GETSELINDEX",
      "LB.GETSELTEXT",
      "LB.END",
      "LB.ASCENDING",
      "LB.DESCENDING",

      /* request codes for entryfields (3/48 entries):   -------------------- */

      "EF.ISREADONLY",
      "EF.SETREADONLY",
      "EF.ISCHANGED",

      /* request codes for combo-boxes (2/50 entries):   -------------------- */

      "CB.ISLISTSHOWING",
      "CB.SHOWLIST",

      /* request codes for MLEs (9/59 entries):   --------------------------- */

    "MLE.CURSOR",
    "MLE.END",
    "MLE.WRITELINES",
    "MLE.DELETELINES",
    "MLE.READLINES",
    "MLE.COUNTLINES",
    "MLE.ISREADONLY",
    "MLE.SETREADONLY",
    "MLE.ISCHANGED"

   };

ULONG EVRnameTableSize = 59;


/* -------------------------------------------------------------------------- */
/* EVRvalueTable                                                              */
/*   Array of values for REXX variables.                                      */
/*   The corresponding names are in EVRnameTable.                             */
/* -------------------------------------------------------------------------- */

ULONG EVRvalueTable[] =
   {
      /* buttons for message boxes:   --------------------------------------- */

      MB_OK,                      /*  "MB.OK",                                */
      MB_OKCANCEL,                /*  "MB.OKCANCEL",                          */
      MB_RETRYCANCEL,             /*  "MB.RETRYCANCEL",                       */
      MB_ABORTRETRYIGNORE,        /*  "MB.ABORTRETRYIGNORE",                  */
      MB_YESNO,                   /*  "MB.YESNO",                             */
      MB_YESNOCANCEL,             /*  "MB.YESNOCANCEL",                       */
      MB_CANCEL,                  /*  "MB.CANCEL",                            */
      MB_ENTER,                   /*  "MB.ENTER",                             */
      MB_ENTERCANCEL,             /*  "MB.ENTERCANCEL",                       */

      /* return codes from message boxes:   --------------------------------- */

      MBID_OK,                    /*  "MB.RC_OK",                             */
      MBID_CANCEL,                /*  "MB.RC_CANCEL",                         */
      MBID_ABORT,                 /*  "MB.RC_ABORT",                          */
      MBID_RETRY,                 /*  "MB.RC_RETRY",                          */
      MBID_IGNORE,                /*  "MB.RC_IGNORE",                         */
      MBID_YES,                   /*  "MB.RC_YES",                            */
      MBID_NO,                    /*  "MB.RC_NO",                             */
      MBID_HELP,                  /*  "MB.RC_HELP",                           */
      MBID_ENTER,                 /*  "MB.RC_ENTER",                          */
      MBID_ERROR,                 /*  "MB.RC_ERROR",                          */

      /* icons for message boxes:   ----------------------------------------- */

      MB_QUERY,                   /*  "MB.QUERY",                             */
      MB_WARNING,                 /*  "MB.WARNING",                           */
      MB_INFORMATION,             /*  "MB.INFORMATION",                       */
      MB_ERROR,                   /*  "MB.ERROR",                             */

      /* options for file dialog:   ----------------------------------------- */

      FDS_OPEN_DIALOG,            /*  "FD.OPEN",                              */
      FDS_SAVEAS_DIALOG,          /*  "FD.SAVEAS",                            */

      /* standard dialog IDs:   --------------------------------------------- */

      DID_OK,                     /*  "DID_OK",                               */
      DID_CANCEL,                 /*  "DID_CANCEL",                           */

      /* request codes for buttons:   --------------------------------------- */

      EVR_BU_ISCHECKED,           /*  "BU.ISCHECKED",                         */
      EVR_BU_CHECK,               /*  "BU.CHECK",                             */
      EVR_BU_INDEX,               /*  "BU.INDEX",                             */
      EVR_BU_ISHILITE,            /*  "BU.ISHILITE",                          */
      EVR_BU_HILITE,              /*  "BU.HILITE",                            */
      EVR_BU_SETDEFAULT,          /*  "BU.SETDEFAULT",                        */
      EVR_BU_ENABLE,              /*  "BU.ENABLE",                            */

      /* request codes for text:   ------------------------------------------ */

      EVR_TXT_READ,               /*  "TXT.READ",                             */
      EVR_TXT_WRITE,              /*  "TXT.WRITE",                            */

      /* request codes for listboxes (+ insert locations):   ---------------- */

      EVR_LB_CLEAR,               /*  "LB.CLEAR"                              */
      EVR_LB_DELETE,              /*  "LB.DELETE"                             */
      EVR_LB_INSERT,              /*  "LB.INSERT"                             */
      EVR_LB_SELECT,              /*  "LB.SELECT"                             */
      EVR_LB_GETSELINDEX,         /*  "LB.GETSELINDEX"                        */
      EVR_LB_GETSELTEXT,          /*  "LB.GETSELTEXT"                         */
      LIT_END,                    /*  "LB.END"                                */
      LIT_SORTASCENDING,          /*  "LB.ASCENDING"                          */
      LIT_SORTDESCENDING,         /*  "LB.DESCENDING"                         */

      /* request codes for entryfields:   ----------------------------------- */

      EVR_EF_ISREADONLY,          /*  "EF.ISREADONLY"                         */
      EVR_EF_SETREADONLY,         /*  "EF.SETREADONLY"                        */
      EVR_EF_ISCHANGED,           /*  "EF.ISCHANGED"                          */

      /* request codes for combo-boxes:   ----------------------------------- */

      EVR_CB_ISLISTSHOWING,       /*  "CB.ISLISTSHOWING"                      */
      EVR_CB_SHOWLIST,            /*  "CB.SHOWLIST"                           */

      /* request codes for MLEs:   ------------------------------------------ */

      EVR_MLE_CURSOR,             /*  "MLE.CURSOR"                            */
      EVR_MLE_END,                /*  "MLE.END"                               */
      EVR_MLE_WRITELINES,         /*  "MLE.WRITELINES"                        */
      EVR_MLE_DELETELINES,        /*  "MLE.DELETELINES"                       */
      EVR_MLE_READLINES,          /*  "MLE.READLINES"                         */
      EVR_MLE_COUNTLINES,         /*  "MLE.COUNTLINES"                        */
      EVR_MLE_ISREADONLY,         /*  "MLE.ISREADONLY"                        */
      EVR_MLE_SETREADONLY,        /*  "MLE.SETREADONLY"                       */
      EVR_MLE_ISCHANGED           /*  "MLE.ISCHANGED"                         */

   };




/* -------------------------------------------------------------------------- */
/* initControl                                                                */
/*   Array of pointers to functions which initialize the shared memory        */
/*   object with request specific data.                                       */
/* -------------------------------------------------------------------------- */

typedef VOID (*initFkt)(CHAR*,PVOID*);

VOID iNull(CHAR*,PVOID*);
VOID iUShort(CHAR*,PVOID*);
VOID iULong(CHAR*,PVOID*);
VOID iShort(CHAR*,PVOID*);
VOID iString(CHAR*,PVOID*);
VOID iListBoxInsert(CHAR*,PVOID*);
VOID iListBoxSelect(CHAR*,PVOID*);
VOID iMLEWriteLines(CHAR*,PVOID*);
VOID iMLEDeleteLines(CHAR*,PVOID*);
VOID iMLEReadLines(CHAR*,PVOID*);

initFkt initControl[] =
   {
      &iNull,                     /*  "BU.ISCHECKED",                         */
      &iUShort,                   /*  "BU.CHECK",                             */
      &iNull,                     /*  "BU.INDEX",                             */
      &iNull,                     /*  "BU.ISHILITE",                          */
      &iUShort,                   /*  "BU.HILITE",                            */
      &iUShort,                   /*  "BU.SETDEFAULT",                        */
      &iUShort,                   /*  "BU.ENABLE",                            */

      &iNull,                     /*  "TXT.READ",                             */
      &iString,                   /*  "TXT.WRITE",                            */

      &iNull,                     /*  "LB.CLEAR"                              */
      &iShort,                    /*  "LB.DELETE"                             */
      &iListBoxInsert,            /*  "LB.INSERT"                             */
      &iListBoxSelect,            /*  "LB.SELECT"                             */
      &iNull,                     /*  "LB.GETSELINDEX"                        */
      &iNull,                     /*  "LB.GETSELTEXT"                         */

      &iNull,                     /*  "EF.ISREADONLY"                         */
      &iUShort,                   /*  "EF.SETREADONLY"                        */
      &iNull,                     /*  "EF.ISCHANGED"                          */

      &iNull,                     /*  "CB.ISLISTSHOWING"                      */
      &iUShort,                   /*  "CB.SHOWLIST"                           */

      &iMLEWriteLines,            /*  "MLE.WRITELINES"                        */
      &iMLEDeleteLines,           /*  "MLE.DELETELINES"                       */
      &iMLEDeleteLines,           /*  "MLE.READLINES"                         */
      &iNull,                     /*  "MLE.COUNTLINES"                        */
      &iNull,                     /*  "MLE.ISREADONLY"                        */
      &iUShort,                   /*  "MLE.SETREADONLY"                       */
      &iNull                      /*  "MLE.ISCHANGED"                         */
   };

/* -------------------------------------------------------------------------- */
/* termControl                                                                */
/*   Array of pointers to functions which read the shared memory object       */
/*   after processing by the shadow program.                                  */
/* -------------------------------------------------------------------------- */

typedef VOID (*termFkt)(CHAR*,PVOID*);

VOID tULong(CHAR*,PVOID*);
VOID tUShort(CHAR*,PVOID*);
VOID tShort(CHAR*,PVOID*);
VOID tShortArray(CHAR*,PVOID*);
VOID tBool(CHAR*,PVOID*);
VOID tString(CHAR*,PVOID*);
VOID tStrings(CHAR*,PVOID*);
VOID tNull(CHAR*,PVOID*);

termFkt termControl[] =
   {
      &tUShort,                   /*  "BU.ISCHECKED",                         */
      &tUShort,                   /*  "BU.CHECK",                             */
      &tShort,                    /*  "BU.INDEX",                             */
      &tBool,                     /*  "BU.ISHILITE",                          */
      &tBool,                     /*  "BU.HILITE",                            */
      &tBool,                     /*  "BU.SETDEFAULT",                        */
      &tNull,                     /*  "BU.ENABLE",                            */

      &tString,                   /*  "TXT.READ",                             */
      &tBool,                     /*  "TXT.WRITE",                            */

      &tBool,                     /*  "LB.CLEAR"                              */
      &tShort,                    /*  "LB.DELETE"                             */
      &tShort,                    /*  "LB.INSERT"                             */
      &tNull,                     /*  "LB.SELECT"                             */
      &tShortArray,               /*  "LB.GETSELINDEX"                        */
      &tStrings,                  /*  "LB.GETSELTEXT"                         */

      &tBool,                     /*  "EF.ISREADONLY"                         */
      &tBool,                     /*  "EF.SETREADONLY"                        */
      &tBool,                     /*  "EF.ISCHANGED"                          */

      &tBool,                     /*  "CB.ISLISTSHOWING"                      */
      &tBool,                     /*  "CB.SHOWLIST"                           */

      &tNull,                     /*  "MLE.WRITELINES"                        */
      &tNull,                     /*  "MLE.DELETELINES"                       */
      &tStrings,                  /*  "MLE.READLINES"                         */
      &tULong,                    /*  "MLE.COUNTLINES"                        */
      &tBool,                     /*  "MLE.ISREADONLY"                        */
      &tBool,                     /*  "MLE.SETREADONLY"                       */
      &tBool                      /*  "MLE.ISCHANGED"                         */
   };
