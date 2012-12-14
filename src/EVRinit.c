/*
    EVRinit.c: initialize EVRexx environment
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
#include <stdio.h>
#include <string.h>
#include <process.h>

#include "EVRexx.h"
#include "xternDLL.h"
#define  _DEBUG_LEVEL  4
#include "trace.h"

/* -------------------------------------------------------------------------- */
/* External function EVRinit: Initialize EVRexx environment                   */
/*                                                                            */
/* CALL EVRinit <default-resource-dll><,number of memory pages>               */
/*                                                                            */
/* Result: 0 successful initialization                                        */
/*         4 initialization failed: problems with system resources            */
/*         8 initialization failed  resource-dll does not exist               */
/*        12 initialization failed  couldn't find EVRexx.exe                  */
/*        16 initialization failed  problems starting EVRexx.exe              */
/* -------------------------------------------------------------------------- */

ULONG EVRinit(CHAR *name, ULONG numargs, RXSTRING args[],
                                            CHAR *queuename, RXSTRING *retstr) {

  APIRET      rc = 0;                              /* return code             */
  HATOMTBL    hAtomTable;
  CHAR        *pgmTitle = NULL, pgmName[CCHMAXPATH], objBuffer[100] = "",
              argBuffer[16],
              semName[EVR_SEM_NAME_LEN] = EVR_SEMAPHORE_NAME,
              memName[EVR_MEM_NAME_LEN] = EVR_MEMORY_NAME;
  STARTDATA   startData;
  ULONG       j, dirNameLength = CCHMAXPATH, dummy, sessID,
              resExist = 0, memSize;
  PID         pid;
  PVOID       memptr;

  if (numargs > 2)                              /* at most one argument       */
    return INVALID_ROUTINE;

  /* create system resources   ---------------------------------------------- */

  EVRpid = getpid();
  TRACE(3,"%x: EVRinit:\n",EVRpid);

  sprintf(semName+strlen(semName),"%x",EVRpid);     /* append pid to semName  */
  rc = DosCreateEventSem(semName, &EVRsemaphore, 0, 0);    /* create and set  */
  TRACE(3,"%x:    RC of DosCreateEventSem: %lu\n",EVRpid,rc);
  if (rc == ERROR_DUPLICATE_NAME) {                 /* verify sem-handle      */
     resExist = 1;
     if (DosResetEventSem(EVRsemaphore,&dummy) == ERROR_INVALID_HANDLE)
        DosOpenEventSem(semName,&EVRsemaphore);
  } else if (rc > 0) {
     return_rc(4UL);
  }

  sprintf(memName+strlen(memName), "%x", EVRpid);   /* append pid to memName  */
  if (numargs == 2)
     memSize = strtoul(args[1].strptr,NULL,10)*EVR_PAGE_SIZE;
  else
     memSize = 4*EVR_PAGE_SIZE;

  /* rc = DosAllocSharedMem(&EVRsharedMem, memName, 65536,                    */
  /*                                           PAG_READ|PAG_WRITE|PAG_GUARD); */
  /* rc = DosSetMem(EVRsharedMem,4096L,PAG_DEFAULT|PAG_COMMIT);               */
  /* TRACE(3,"%x:    RC of DosSetMem: %lu\n",EVRpid,rc);                      */

  rc = DosAllocSharedMem(&EVRsharedMem, memName, memSize,
                                                 PAG_COMMIT|PAG_READ|PAG_WRITE);
  TRACE(3,"%x:    RC of DosAllocSharedMem: %lu\n",EVRpid,rc);

  if (rc == ERROR_ALREADY_EXISTS) {                 /* verify mem-handle      */
     PVOID testMem = NULL;
     resExist = 1;
     TRACE(3,"%x:       Verifying mem-handle\n",EVRpid);
     DosGetNamedSharedMem(&testMem, memName, PAG_READ|PAG_WRITE);
     if (testMem != EVRsharedMem) {
        TRACE(3,"%x:       Mem-handle invalid, new DosGetNamedSharedMem\n",EVRpid);
        DosGetNamedSharedMem(&EVRsharedMem, memName, PAG_READ|PAG_WRITE);
     }
     DosFreeMem(testMem);
  } else if (rc > 0) {
     return_rc(4UL);
  }

  hAtomTable = WinQuerySystemAtomTable();
  if (!resExist ||
         (EVRactivateShadow = WinFindAtom(hAtomTable,EVR_ACTIVATE_SHADOW)) == 0)
     EVRactivateShadow = WinAddAtom(hAtomTable,EVR_ACTIVATE_SHADOW);
  TRACE(3,"%x:    Atom from Win-Add/Find-Atom: %lu\n",EVRpid,EVRactivateShadow);
  if (EVRactivateShadow == 0) {
     return_rc(4UL);
  }

  /* define REXX variables   ------------------------------------------------ */

  ulongArrayToRexxVar(EVRnameTable,EVRvalueTable,EVRnameTableSize,0);
  TRACE(3,"%x:    Defined variables in rexx pool\n",EVRpid);

  /* pass current drive, current directory and default DLL to shadow pgm   -- */

  memptr = EVRsharedMem;

  DosQueryCurrentDisk((ULONG*)memptr,&dummy);
  memptr += sizeof(ULONG);
  DosQueryCurrentDir(0,(CHAR *) memptr,&dirNameLength);
  memptr += strlen((CHAR*) memptr) + 1;
  if (numargs > 0 && RXVALIDSTRING(args[0]))
     strcpy((CHAR *)memptr, args[0].strptr);
  else
     *(CHAR*) memptr = '\0';

  /* check if program exists and reinitialize EVRexx.exe   ------------------ */

  if (resExist) {
     TRACE(3,"%x: Broadcasting EVR_REINIT message\n",EVRpid);
     rc = WinBroadcastMsg(HWND_OBJECT,EVRactivateShadow,(MPARAM) EVRpid,
                                              (MPARAM) EVR_REINIT,BMSG_POST);
     TRACE(3,"%x: RC of WinBroadcastMsg: %lu\n",EVRpid,rc);

     TRACE(3,"%x: Waiting on event semaphore ...\n",EVRpid);
     rc = DosWaitEventSem(EVRsemaphore,5000);               /* wait 5 seconds */
     TRACE(3,"%x: ... and continuing again (rc = %lu)\n",EVRpid,rc);
     if (rc == 0) {
        if (*(ULONG *) EVRsharedMem != 0) {    /* could not load DLL   ------ */
           return_rc(8UL);
        } else {
           return_rc(0UL);
        }
     }    /* semaphore not posted means EVRexx.exe does not exist anymore!    */
  }

  /* spawn shadow program   ------------------------------------------------- */

  if (_path(pgmName,"evrexx.exe")) {
     return_rc(12UL);
  } else {
    _fullpath(pgmName,pgmName,CCHMAXPATH);
    TRACE(3,"%x:    Full path of EVRexx.exe: %s\n",EVRpid,pgmName);
  }
  _ultoa(EVRpid,argBuffer,10);

  startData.Length        = sizeof(STARTDATA);
  startData.Related       = SSF_RELATED_CHILD;
  startData.FgBg          = SSF_FGBG_BACK;
  startData.TraceOpt      = SSF_TRACEOPT_NONE;
  startData.PgmTitle      = pgmTitle;
  startData.PgmName       = pgmName;
  startData.PgmInputs     = argBuffer;
  startData.TermQ         = 0;
  startData.Environment   = 0;
  startData.InheritOpt    = SSF_INHERTOPT_SHELL;
  startData.SessionType   = SSF_TYPE_DEFAULT;
  startData.IconFile      = 0;
  startData.PgmHandle     = 0;
  startData.PgmControl    = SSF_CONTROL_INVISIBLE;
  startData.InitXPos      = 0;
  startData.InitYPos      = 0;
  startData.InitXSize     = 0;
  startData.InitYSize     = 0;
  startData.Reserved      = 0;
  startData.ObjectBuffer  = objBuffer;
  startData.ObjectBuffLen = strlen(objBuffer);

  rc = DosStartSession(&startData, &sessID, &pid);
  if (rc == 0) {
     TRACE(3,"%x: Waiting on event semaphore ...\n",EVRpid);
     rc = DosWaitEventSem(EVRsemaphore,5000);               /* wait 5 seconds */
     TRACE(3,"%x: ... and continuing again (rc = %lu)\n",EVRpid,rc);
     if (rc == ERROR_TIMEOUT) {
        TRACE(3,"%x:    EVRexx.exe does not respond, aborting...\n", EVRpid);
        return_rc(16UL);
     } else if (*(ULONG *) EVRsharedMem != 0) { /* could not load DLL   ----- */
        return_rc(8UL);
     } else {
        return_rc(0UL);
     }
  } else {
     TRACE(3,"%x:    Could not start session, RC: %lu\n(ObjBuffer: %s)\n",
                                                           EVRpid,rc,objBuffer);
     return_rc(16UL);
  }
}
