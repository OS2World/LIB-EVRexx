/*
    EVRmessageBox.c: source of EVRmessageBox external function
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
/* External function EVRmessageBox: Display an application modal message box. */
/*                                                                            */
/* CALL EVRmessageBox text, title, buttons <,icon>                            */
/*                                                                            */
/* Result: RC of message box (selected button)                                */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Layout of shared memory object:                                            */
/*                                                                            */
/* CALL: flags (ULONG)                                                        */
/*       text  (null terminated string)                                       */
/*       title (null terminated string)                                       */
/*                                                                            */
/* RETURN: button (ULONG)                                                     */
/* -------------------------------------------------------------------------- */

ULONG EVRmessageBox(CHAR *name, ULONG numargs, RXSTRING args[],
                                            CHAR *queuename, RXSTRING *retstr) {
  ULONG *ulptr, pcount;
  CHAR  *charptr;
  BOOL  rc;

  if (numargs < 3 || numargs > 4)                  /* three or four arguments */
    return INVALID_ROUTINE;

  /* copy arguments to shared memory object   ------------------------------- */

  ulptr   = (ULONG*) EVRsharedMem;
  charptr = (CHAR*) (EVRsharedMem + sizeof(ULONG));

  *ulptr = strtoul(args[2].strptr,NULL,10) |                   /* buttons ... */
                          MB_APPLMODAL | MB_MOVEABLE;

  if (numargs == 4 && RXVALIDSTRING(args[3]))
     *ulptr |= strtoul(args[3].strptr,NULL,10);               /* icon ...     */
  else
     *ulptr |= MB_NOICON;

  memcpy(charptr,args[0].strptr,args[0].strlength+1);         /* message text */
  charptr += args[0].strlength + 1;
  memcpy(charptr,args[1].strptr,args[1].strlength+1);         /* title        */

  /* reset semaphore, broadcast message and wait until user responded   ----- */

  SetBroadcastWait(EVR_MESSAGE_BOX,"EVR_MESSAGE_BOX");

  /* return the selected button to the user   ------------------------------- */

  return_rc(*(ULONG*)EVRsharedMem);
}
