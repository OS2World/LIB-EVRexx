/*
    EVRexx.c: source of EVRexx.exe, the shadow PM-program
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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "EVRexx.h"
#define  _DEBUG_LEVEL  4
#include "trace.h"

VOID cleanup(USHORT);

/* Private data used by EVRexx.exe   ---------------------------------------- */

HEV      EVRsemaphore;
PVOID    EVRsharedMem;
ATOM     EVRactivateShadow;
PID      EVRpid;
HMODULE  EVRdefaultResourceDllHandle;

HAB   hab;
HMQ   hmq;
QMSG  qmsg;
HWND  hwndObject;

/* prototypes   ------------------------------------------------------------- */

MRESULT shadowWndProc(HWND,ULONG,MPARAM,MPARAM);

int main(int argc, char *argv[]) {


   HATOMTBL hAtomTable;
   APIRET   RC;
   CHAR     semName[EVR_SEM_NAME_LEN] = EVR_SEMAPHORE_NAME,
            memName[EVR_MEM_NAME_LEN] = EVR_MEMORY_NAME;
   PVOID    memptr;

   /* initialize variables   ------------------------------------------------ */

   EVRpid = atoi(argv[1]);                          /* parent process id      */
   TRACE(3,"%x:           Initializing EVRexx.exe:\n",EVRpid);

   hAtomTable = WinQuerySystemAtomTable();
   EVRactivateShadow = WinFindAtom(hAtomTable,EVR_ACTIVATE_SHADOW);
   TRACE(3,"%x:              Atom from WinFindAtom: %lu\n",
                                                      EVRpid,EVRactivateShadow);

   sprintf(semName+strlen(semName),"%x",EVRpid);    /* append pid to semName  */
   RC = DosOpenEventSem(semName, &EVRsemaphore);
   TRACE(3,"%x:              RC of DosOpenEventSem: %lu\n",EVRpid,RC);

   sprintf(memName+strlen(memName), "%x", EVRpid);  /* append pid to memName  */
   RC = DosGetNamedSharedMem(&EVRsharedMem, memName, PAG_READ|PAG_WRITE);
   TRACE(3,"%x:              RC of DosGetNamedSharedMem: %lu\n",EVRpid,RC);

   memptr = EVRsharedMem;
   TRACE(4,"%x:              Drive: %lu\n",EVRpid,*(ULONG*)memptr);
   DosSetDefaultDisk(*(ULONG*) memptr);             /* change drive           */

   memptr += sizeof(ULONG);
   TRACE(4,"%x:              Dir: %s\n",EVRpid,(CHAR*)memptr);
   DosSetCurrentDir((CHAR*) memptr);                /* and directory          */

   memptr += strlen((CHAR*)memptr)+1;
   TRACE(4,"%x:              DLL: %s\n",EVRpid,(CHAR*)memptr);

   *(ULONG *) EVRsharedMem = 0;
   if (*(CHAR*)memptr != '\0') {
      RC = DosLoadModule(NULL,0L,(CHAR*)memptr,&EVRdefaultResourceDllHandle);
      if (RC != 0) {
         TRACE(4,"%x:              DosLoadModule failed, RC = %lu\n",EVRpid,RC);
         *(ULONG *) EVRsharedMem = RC;
         DosFreeMem(EVRsharedMem);
         TRACE(4,"%x:              Posting event semaphore\n",EVRpid);
         DosPostEventSem(EVRsemaphore);
         return 3;
      }
   } else
      EVRdefaultResourceDllHandle = 0;

   /* initialize PM environment   ------------------------------------------- */

   hab = WinInitialize(0);
   hmq = WinCreateMsgQueue(hab, 0);
   DosExitList(EXLST_ADD, (PFNEXITLIST) cleanup);

   /* register shadow window class   ---------------------------------------- */

   WinRegisterClass(hab,EVR_SHADOW_CLASS, (PFNWP) shadowWndProc, 0L, 0L);

   /* create the shadow window (an invisible object window)   --------------- */

   hwndObject = WinCreateWindow(HWND_OBJECT,EVR_SHADOW_CLASS,
                                                  (PSZ)NULL, 0L, 0L, 0L, 0L, 0L,
                                       (HWND)NULLHANDLE, HWND_TOP,EVR_SHADOW_ID,
                                                      (PVOID)NULL, (PVOID)NULL);

   /* start message loop   -------------------------------------------------- */

   while(WinGetMsg(hab, &qmsg, (HWND)NULLHANDLE, 0L, 0L))
         WinDispatchMsg(hab, &qmsg);

   return 0;
}

/* Exit-list to cleanup resources   ----------------------------------------- */

VOID cleanup(USHORT code) {
   APIRET   RC;

   TRACE(3,"%x:           Termination of EVRexx.exe:\n",EVRpid);
   if (WinIsWindow(hab,hwndObject))
      WinDestroyWindow (hwndObject);
   WinDestroyMsgQueue (hmq);
   WinTerminate (hab);

   if (EVRdefaultResourceDllHandle != NULLHANDLE) {
      RC = DosFreeModule(EVRdefaultResourceDllHandle);
      TRACE(3,"%x:              RC of DosFreeModule: %lu\n",EVRpid,RC);
   }

   RC = DosCloseEventSem(EVRsemaphore);
   TRACE(3,"%x:              RC of DosCloseEventSem: %lu\n",EVRpid,RC);

   RC = DosFreeMem(EVRsharedMem);
   TRACE(3,"%x:              RC of DosFreeMem: %lu\n",EVRpid,RC);

   DosExitList(EXLST_EXIT, (PFNEXITLIST) NULL);
}
