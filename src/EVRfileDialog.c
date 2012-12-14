/*
    EVRfileDialog.c: source of EVRfileDialog external function
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
#include <memory.h>
#include <stdlib.h>

#include "EVRexx.h"
#include "xternDLL.h"
#define  _DEBUG_LEVEL  4
#include "trace.h"

/* -------------------------------------------------------------------------- */
/* External function EVRfileDialog: Display an application modal file dialog. */
/*                                                                            */
/* CALL EVRfileDialog title, template, option, filenames                      */
/*                                                                            */
/* Result: RC of file dialog (selected button)  or                            */
/*         RC of REXX-variable-pool interface (RC<0)                          */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Layout of shared memory object:                                            */
/*                                                                            */
/* CALL: FILEDLG (predefined structure)                                       */
/*       title   (null terminated string)                                     */
/*                                                                            */
/* RETURN: return code (LONG)                                                 */
/*         number of filenames (ULONG)                                        */
/*         filename1 (null terminated string)                                 */
/*           . . .                                                            */
/*         filenameN (null terminated string)                                 */
/* -------------------------------------------------------------------------- */

ULONG EVRfileDialog(CHAR *name, ULONG numargs, RXSTRING args[],
                                            CHAR *queuename, RXSTRING *retstr) {
  ULONG   pcount;
  LONG    button;
  FILEDLG *pFileDlg;
  CHAR    *pChar, lastChar;
  BOOL    rc;

  if (numargs != 4 || !RXVALIDSTRING(args[0]) || !RXVALIDSTRING(args[1]) ||
                             !RXVALIDSTRING(args[2]) || !RXVALIDSTRING(args[3]))
    return INVALID_ROUTINE;

  /* copy arguments to shared memory object   ------------------------------- */

  pFileDlg = (FILEDLG*) EVRsharedMem;
  pChar    = (CHAR*) (EVRsharedMem + sizeof(FILEDLG));

  memset(pFileDlg,0,sizeof(FILEDLG));           /* initialize fields to zero  */
  pFileDlg->cbSize = sizeof(FILEDLG);
  pFileDlg->fl     = FDS_CENTER | strtoul(args[2].strptr,NULL,10);
  lastChar = *(args[3].strptr+strlen(args[3].strptr)-1);
  if (lastChar == '.')                          /* stem var for filenames!    */
     pFileDlg->fl |= FDS_MULTIPLESEL;
  strcpy(pFileDlg->szFullFile,args[1].strptr);  /* template                   */
  strcpy(pChar,args[0].strptr);                 /* title                      */
  pFileDlg->pszTitle = pChar;

  /* reset semaphore, broadcast message and wait until user responded   ----- */

  SetBroadcastWait(EVR_FILE_DIALOG,"EVR_FILE_DIALOG");

  /* analyze result   ------------------------------------------------------- */

  button = *(LONG*) EVRsharedMem;
  if (button != DID_CANCEL) {

     if (lastChar == '.') {                     /* stem var for filenames!    */
        ULONG  i, n;
        LONG   rc;
        CHAR   **filename;

        n = *(ULONG*) (EVRsharedMem+sizeof(LONG));
        filename = (CHAR**) malloc(n*sizeof(CHAR*));
        filename[0] = (CHAR*) (EVRsharedMem + sizeof(LONG) + sizeof(ULONG));
        for (i=1; i<n; ++i)
           filename[i] = filename[i-1] + strlen(filename[i-1]) +1;

        if ((rc = charArrayToRexxStem(args[3].strptr,filename,n,1)) != 0 &&
                                                            rc != RXSHV_NEWV) {
           free(filename);
           _ltoa(-rc,retstr->strptr,10);
           retstr->strlength = strlen(retstr->strptr);
           return VALID_ROUTINE;
        } else
           free(filename);
     } else {
        pChar = (CHAR*) (EVRsharedMem+sizeof(LONG) + sizeof(ULONG));
        charToRexxVar(args[3].strptr,pChar,1);
     }

  } /* endif (if (button != DID_CANCEL) { ...   */

  _ltoa(button,retstr->strptr,10);
  retstr->strlength = strlen(retstr->strptr);
  return VALID_ROUTINE;
}
