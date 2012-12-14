/*
    EVRloadDialog.c: source of EVRloadDialog external function
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
#include "xternDLL.h"
#define  _DEBUG_LEVEL  4
#include "trace.h"

/* -------------------------------------------------------------------------- */
/* External function EVRloadDialog: Load a dialog from a resource file.       */
/*                                                                            */
/* CALL EVRloadDialog id,<dllname>,<rexx-dialog-proc>                         */
/*                                                                            */
/* Result: Handle to dialog (0 in case of failure)                            */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Layout of shared memory object:                                            */
/*                                                                            */
/* CALL: id (ULONG)                                                           */
/*       dllname (null terminated string)                                     */
/*       rexxproc (null terminated string)                                    */
/*                                                                            */
/* RETURN: handle (ULONG)                                                     */
/* -------------------------------------------------------------------------- */

ULONG EVRloadDialog(CHAR *name, ULONG numargs, RXSTRING args[],
                                            CHAR *queuename, RXSTRING *retstr) {
  ULONG   pcount;
  CHAR    *pChar;
  APIRET  RC;
  BOOL    rc;

  if (numargs < 1 || numargs > 3 || !RXVALIDSTRING(args[0]))
    return INVALID_ROUTINE;

  /* copy arguments to shared memory object   ------------------------------- */

  *(ULONG *) EVRsharedMem = strtoul(args[0].strptr,NULL,10);
  pChar    = (CHAR*) (EVRsharedMem + sizeof(ULONG));

  if (numargs>1 && RXVALIDSTRING(args[1]))
     strcpy(pChar,args[1].strptr);
  else
     *pChar = '\0';
  pChar += strlen(pChar)+1;

  *pChar = '\0';
  if (numargs>2 && RXVALIDSTRING(args[2])) {
     RC = DosSearchPath(SEARCH_CUR_DIRECTORY|SEARCH_ENVIRONMENT,"PATH",
                                               args[2].strptr,pChar,CCHMAXPATH);
     if (RC == 0)
        TRACE(4,"%x:    REXX dialog-proc name: %s\n",EVRpid,pChar);
  }

  /* reset semaphore, broadcast message and wait until user responded   ----- */

  SetBroadcastWait(EVR_LOAD_DIALOG,"EVR_LOAD_DIALOG");

  /* return result   -------------------------------------------------------- */

  return_rc(*(ULONG*) EVRsharedMem);
}
