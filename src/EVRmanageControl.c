/*
    EVRmanageControl.c: read, query, set controls of a dialog window
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
#include <memory.h>

#include "EVRexx.h"
#include "xternDLL.h"
#define  _DEBUG_LEVEL  4
#include "trace.h"

#define ULONG_DIG   12         /* maximum number of digits in ULONG           */
#define REXX_VAR_LENGTH 252L   /* maximum length of rexx variable names: 250  */

VOID iNull(CHAR*,PVOID*);
VOID tNull(CHAR*,PVOID*);
/* APIRET APIENTRY EVRguardPageHandler(PEXCEPTIONREPORTRECORD pX); */

/* -------------------------------------------------------------------------- */
/* External function EVRmanageControl: Get or set values of dialog controls.  */
/*                                                                            */
/* CALL EVRmanageControl handle,control-id,request-code<,parms><,ret-var>     */
/*                                                                            */
/* control-id, request-code and parms can be either values or stem variable-  */
/* names. In the latter case control-id.0 contains the number of values       */
/* and stem.i contains the appropriate value. ret-var is the name of a        */
/* variable or a stem variable to receive the result.                         */
/*                                                                            */
/* Result: NULL (the result of the request is returned in ret-var).           */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Layout of shared memory object:                                            */
/*                                                                            */
/* CALL: size              (ULONG)                                            */
/*       handle            (ULONG)                                            */
/*       requests          (ULONG)                                            */
/*       data request 1 ÄÂ requestCode (ULONG)                                */
/*        .     .     .  Ã controlId   (ULONG)                                */
/*        .     .     .  À request specific data                              */
/*       data request n                                                       */
/*                                                                            */
/* RETURN: data request 1 ÄÂ requestCode (ULONG)                              */
/*          .     .     .  À request specific data                            */
/*         data request n                                                     */
/* -------------------------------------------------------------------------- */

ULONG EVRmanageControl(CHAR *name, ULONG numargs, RXSTRING args[],
                                            CHAR *queuename, RXSTRING *retstr) {
  ULONG   pcount, handle;
  BOOL    rc;

  ULONG   nRequests, i, controlId, requestCode, length;
  CHAR    lastChar, *varName, *buffer,
          parmVar[REXX_VAR_LENGTH], retVar[REXX_VAR_LENGTH];
  UCHAR   empty[]="";
  VOID    *memptr;

  /* EXCEPTIONREGISTRATIONRECORD guardPageHandler = {0,&EVRguardPageHandler}; */

  if (numargs < 3 || numargs > 5 || !RXVALIDSTRING(args[0]) ||
                            !RXVALIDSTRING(args[1]) || !RXVALIDSTRING(args[2]) )
     return INVALID_ROUTINE;
  handle = strtoul(args[0].strptr,NULL,10);
  if (handle == 0)
     return INVALID_ROUTINE;


  if (numargs > 3 && args[3].strptr == NULL) /* some library functions don't  */
     args[3].strptr = empty;                 /* like NULL pointers!           */
  if (numargs > 4 && args[4].strptr == NULL)
     args[4].strptr = empty;

  /* copy arguments to shared memory object   ------------------------------- */

  length = max(args[1].strlength,args[2].strlength);
  if (numargs > 3)
     length = max(length,args[3].strlength);
  if (numargs > 4)
     length = max(length,args[4].strlength) + ULONG_DIG;
  length += ULONG_DIG;
  lastChar = *(args[1].strptr+args[1].strlength-1);
  memptr   = EVRsharedMem + 3*sizeof(ULONG);

  /* DosSetExceptionHandler(&guardPageHandler);                               */
  /* TRACE(3,"%x:    RC of DosSetExceptionHandler: %lu\n",EVRpid,rc);         */

  if (lastChar != '.') {                                 /* no stem variable! */
     nRequests   = 1;
     controlId   = strtoul(args[1].strptr,NULL,10);
     requestCode = strtoul(args[2].strptr,NULL,10);
     *(ULONG*) memptr = requestCode;
     memptr += sizeof(ULONG);
     *(ULONG*) memptr = controlId;
     memptr += sizeof(ULONG);
     if (initControl[requestCode] != &iNull)    /* request code specific data */
        if (numargs < 4 || args[3].strptr == empty)
           return INVALID_ROUTINE;
        else
           (*initControl[requestCode])(args[3].strptr,&memptr);

  } else {
     varName = (CHAR*) malloc(length*sizeof(CHAR)); /* fetch controlid.0      */
     buffer  = (CHAR*) malloc(ULONG_DIG*sizeof(CHAR));
     strcpy(varName,args[1].strptr);
     strcat(varName,"0");
     rexxVarToUlong(varName,&nRequests,1);
     TRACE(3,"%x:    nRequests: %lu\n",EVRpid,nRequests);

     for (i=1; i<=nRequests; ++i) {
        strcpy(varName,args[1].strptr);             /* fetch controlid.i      */
        strcat(varName,_ultoa(i,buffer,10));
        rexxVarToUlong(varName,&controlId,1);
        TRACE(3,"%x:    controlId: %lu\n",EVRpid,controlId);

        strcpy(varName,args[2].strptr);             /* fetch requestCode.i    */
        strcat(varName,buffer);
        rexxVarToUlong(varName,&requestCode,1);
        TRACE(3,"%x:    requestCode: %lu\n",EVRpid,requestCode);
        *(ULONG*) memptr = requestCode;
        memptr += sizeof(ULONG);
        *(ULONG*) memptr = controlId;
        memptr += sizeof(ULONG);

        if (initControl[requestCode] != &iNull) {          /* fetch parm.i    */
           if (numargs < 4 || args[3].strptr == empty)
              return INVALID_ROUTINE;
           else {
              strcpy(varName,args[3].strptr);
              strcat(varName,buffer);
              rexxVarToChar(varName,parmVar,REXX_VAR_LENGTH,1);
              TRACE(4,"%x:    parm[%lu]: %s\n",EVRpid,i,parmVar);
           }
           (*initControl[requestCode])(parmVar,&memptr);
        }
     }
  }
  /* DosUnsetExceptionHandler(&guardPageHandler);                             */

  TRACE(3,"%x:    size of Data area: %lu\n",EVRpid,memptr-EVRsharedMem);
  *(ULONG*)  EVRsharedMem                  = memptr - EVRsharedMem;
  *(ULONG*) (EVRsharedMem+sizeof(ULONG))   = handle;
  *(ULONG*) (EVRsharedMem+2*sizeof(ULONG)) = nRequests;

  /* reset semaphore, broadcast message and wait until shadow program is      */
  /* finished processing all requests.   ------------------------------------ */

  SetBroadcastWait(EVR_CONTROL,"EVR_CONTROL");

  /* transfer results   ----------------------------------------------------- */

  memptr = EVRsharedMem;
  if (lastChar != '.') {                                 /* no stem variable! */
     memptr += sizeof(ULONG);
     if (termControl[requestCode] != &tNull)
        if (numargs < 5 || args[4].strptr == empty)
           return INVALID_ROUTINE;
        else
           (*termControl[requestCode])(args[4].strptr,&memptr);
  } else {
     for (i=1; i<=nRequests; ++i) {
        requestCode = *(ULONG*) memptr;             /* fetch requestCode.i    */
        memptr += sizeof(ULONG);
        if (termControl[requestCode] != &tNull)           /* fetch retvar.i   */
           if (numargs < 5 || args[4].strptr == empty)
              return INVALID_ROUTINE;
           else {
              strcpy(varName,args[4].strptr);
              strcat(varName,_ultoa(i,buffer,10));
              rexxVarToChar(varName,retVar,REXX_VAR_LENGTH,1);
              (*termControl[requestCode])(retVar,&memptr);
           }
     }
     free(varName);
     free(buffer);
  }

  /* return result   -------------------------------------------------------- */

  retstr->strptr = NULL;
  retstr->strlength = 0;
  return VALID_ROUTINE;
}
