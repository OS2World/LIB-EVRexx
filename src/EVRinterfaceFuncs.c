/*
    EVRinterfaceFuncs.c: source of EVRloadFuncs and EVRdropFuncs
                         external functions
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

#include "EVRexx.h"
#include "xternDLL.h"
#define  _DEBUG_LEVEL  4
#include "trace.h"

/* -------------------------------------------------------------------------- */
/* External function EVRloadFuncs: Load all functions of the EVRexx package   */
/*                                                                            */
/* CALL EVRloadFuncs                                                          */
/*                                                                            */
/* Result: 0 all functions were loaded                                        */
/*         > 0 not every function was loaded                                  */
/* -------------------------------------------------------------------------- */

ULONG EVRloadFuncs(CHAR *name, ULONG numargs, RXSTRING args[],
                                            CHAR *queuename, RXSTRING *retstr) {

  INT    j;                                     /* Counter                    */
  ULONG  rc;                                    /* return code                */

  if (numargs > 0)                              /* check arguments            */
    return INVALID_ROUTINE;

  for (j = 0; j < EVRfuncTableSize; j++) {
     if (strcmp("EVRloadFuncs",EVRfuncTable[j])) {
        rc = RexxRegisterFunctionDll(EVRfuncTable[j],"EVRexx",EVRfuncTable[j]);
        TRACE(4,"RC of registering %s: %ul\n",EVRfuncTable[j],rc);
     }
  }

  _ultoa(rc,retstr->strptr,10);                 /* set return value           */
  retstr->strlength = strlen(retstr->strptr);

  return VALID_ROUTINE;
}

/* -------------------------------------------------------------------------- */
/* External function EVRdropFuncs: Drop all functions of the EVRexx package   */
/*                                                                            */
/* CALL EVRdropFuncs                                                          */
/*                                                                            */
/* Result: null string                                                        */
/* -------------------------------------------------------------------------- */

ULONG EVRdropFuncs(CHAR *name, ULONG numargs, RXSTRING args[],
                                            CHAR *queuename, RXSTRING *retstr) {

  INT     j;                                   /* Counter                     */

  if (numargs != 0)                            /* no arguments for this       */
    return INVALID_ROUTINE;                    /* raise an error              */

  retstr->strlength = 0;                       /* return a null string result */

  for (j = 0; j < EVRfuncTableSize; j++)
    RexxDeregisterFunction(EVRfuncTable[j]);

  return VALID_ROUTINE;                        /* no error on call            */
}
