/*
    EVRprocessDialog.c: source of EVRmodalDialog external function
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
/* External function EVRmodalDialog: Process modal dialog.                    */
/*                                                                            */
/* CALL EVRmodalDialog handle<,window-list-title>                             */
/*                                                                            */
/* Result: RC of dialog (selected button)                                     */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Layout of shared memory object:                                            */
/*                                                                            */
/* CALL: handle  (ULONG)                                                      */
/*       window-list-title (null terminated string)                           */
/*                                                                            */
/* RETURN: button id (ULONG)                                                  */
/* -------------------------------------------------------------------------- */

ULONG EVRprocessDialog(CHAR *name, ULONG numargs, RXSTRING args[],
                                            CHAR *queuename, RXSTRING *retstr) {
  ULONG   pcount, handle;
  BOOL    rc;

  if (numargs > 2 || !RXVALIDSTRING(args[0]))
     return INVALID_ROUTINE;

  /* copy arguments to shared memory object   ------------------------------- */

  handle = strtoul(args[0].strptr,NULL,10);
  if (handle == 0)
     return INVALID_ROUTINE;
  *(ULONG*) EVRsharedMem = handle;
  if (numargs == 2)
     strcpy((CHAR*) (EVRsharedMem+sizeof(ULONG)),args[1].strptr);
  else
     *(CHAR*) (EVRsharedMem+sizeof(ULONG)) = '\0';

  /* reset semaphore, broadcast message and wait until user responded   ----- */

  SetBroadcastWait(EVR_PROC_DIALOG,"EVR_PROC_DIALOG");

  /* return result   -------------------------------------------------------- */

  return_rc(*(ULONG*) EVRsharedMem);
}
