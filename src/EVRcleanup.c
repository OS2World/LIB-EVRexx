/*
    EVRcleanup.c: source of EVRcleanup external function
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
#include <string.h>
#include <process.h>

#include "EVRexx.h"
#include "xternDLL.h"
#define  _DEBUG_LEVEL  4
#include "trace.h"

/* -------------------------------------------------------------------------- */
/* External function EVRcleanup: Free resources                               */
/*                                                                            */
/* CALL EVRcleanup                                                            */
/*                                                                            */
/* (all arguments are ignored)                                                */
/*                                                                            */
/* Result: NULL                                                               */
/* -------------------------------------------------------------------------- */

ULONG EVRcleanup(CHAR *name, ULONG numargs, RXSTRING args[],
                                            CHAR *queuename, RXSTRING *retstr) {

  APIRET RC;
  HATOMTBL hAtomTable;

  /* notify shadow program   ------------------------------------------------ */

  TRACE(4,"%x: Broadcasting EVR_CLEANUP message\n",EVRpid);
  RC = WinBroadcastMsg(HWND_OBJECT,EVRactivateShadow,(MPARAM) EVRpid,
                                                (MPARAM) EVR_CLEANUP,BMSG_POST);
  TRACE(4,"%x: RC of WinBroadcastMsg: %lu\n",EVRpid,RC);

  /* return resources to the system   --------------------------------------- */

  hAtomTable = WinQuerySystemAtomTable();

  TRACE(3,"%x: Cleanup of resources:\n",EVRpid);

  RC = WinDeleteAtom(hAtomTable,EVRactivateShadow);
  TRACE(3,"%x:    RC of WinDeleteAtom: %lu\n",EVRpid,RC);

  RC = DosCloseEventSem(EVRsemaphore);
  TRACE(3,"%x:    RC of DosCloseEventSem: %lu\n",EVRpid,RC);

  RC = DosFreeMem(EVRsharedMem);
  TRACE(3,"%x:    RC of DosFreeMem: %lu\n",EVRpid,RC);

  /* set defaults   --------------------------------------------------------- */

  EVRactivateShadow          = 0;
  EVRsemaphore               = (HEV) 0;
  EVRsharedMem               = (PVOID) 0;

  retstr->strptr = NULL;
  return VALID_ROUTINE;
}
