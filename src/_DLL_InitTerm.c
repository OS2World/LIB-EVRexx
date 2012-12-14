/*
    _DLL_InitTerm.c: initialization and termination of EVRexx.dll
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
#define  _DEBUG_LEVEL  4
#include "trace.h"

int  _CRT_init(void);
void _CRT_term(void);

/* Private DLL data used by DLL --------------------------------------------- */

HEV    EVRsemaphore = 0;
PVOID  EVRsharedMem = 0;
ATOM   EVRactivateShadow = 0;
PID    EVRpid = 0;

unsigned long _System _DLL_InitTerm(unsigned long handle, unsigned long flag) {

  if (flag == 0UL) {                    /* initialization   ----------------- */
     if (_CRT_init() == -1)
        return 0UL;
  } else {                              /* termination: the API-calls insure  */
                                        /* proper termination if the process  */
     APIRET RC;                         /* didn't call EVRcleanup             */
     HATOMTBL hAtomTable;
     hAtomTable = WinQuerySystemAtomTable();

     TRACE(3,"%x: Termination of DLL:\n",EVRpid);

     RC = WinDeleteAtom(hAtomTable,EVRactivateShadow);
     TRACE(3,"%x:    RC of WinDeleteAtom: %lu\n",EVRpid,RC);

     RC = DosCloseEventSem(EVRsemaphore);
     TRACE(3,"%x:    RC of DosCloseEventSem: %lu\n",EVRpid,RC);

     RC = DosFreeMem(EVRsharedMem);
     TRACE(3,"%x:    RC of DosFreeMem: %lu\n",EVRpid,RC);

     _CRT_term();
  }
  return 1UL;
}
