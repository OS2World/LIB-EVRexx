/*
    init_termControl.c: functions for initial and final
                        manipulation of the shared memory object during
                        request-code processing with EVRmanageControl
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
#define  _DEBUG_LEVEL  3
#include "trace.h"

#define ULONG_DIG   12         /* maximum number of digits in ULONG           */


/* -------------------------------------------------------------------------- */
/* i*: Functions which initialize the shared memory object with request       */
/*     specific data.                                                         */
/* t*: Functions which read the shared memory object after processing by the  */
/*     shadow program.                                                        */
/* On entry, the memptr points to the request specific data. After each of    */
/* these functions return, the memptr must point to the next data item.       */
/* -------------------------------------------------------------------------- */

VOID iNull(CHAR* parm,PVOID* memptr) {}

VOID iUShort(CHAR* parm,PVOID* memptr) {
   *(USHORT*) *memptr = (USHORT) strtoul(parm,NULL,10);
   *memptr += sizeof(USHORT);
}

VOID iShort(CHAR* parm,PVOID* memptr) {
   *(SHORT*) *memptr = (SHORT) atoi(parm);
   *memptr += sizeof(SHORT);
}

VOID iULong(CHAR* parm,PVOID* memptr) {
   *(ULONG*) *memptr = strtoul(parm,NULL,10);
   *memptr += sizeof(ULONG);
}

VOID iString(CHAR* parm,PVOID* memptr) {
   strcpy((CHAR*)*memptr,parm);
   *memptr += strlen(parm)+1;
}

VOID iListBoxInsert(CHAR* parm,PVOID* memptr) {

   CHAR   *varName, buffer[ULONG_DIG];
   SHORT  location, nLines;
   INT    i;

   varName = (CHAR*) malloc((strlen(parm)+ULONG_DIG)*sizeof(CHAR));

   strcpy(varName,parm);                  /* get insertion point              */
   strcat(varName,"LOC");
   rexxVarToShort(varName,&location,1);
   if (location > 0)
      location--;                         /* REXX' indices are not offsets!   */
   *(SHORT*)*memptr = location;
   *memptr += sizeof(SHORT);

   strcpy(varName,parm);                  /* get number of lines to insert    */
   strcat(varName,"0");
   rexxVarToShort(varName,&nLines,1);
   TRACE(4,"%x:    nLines: %hd\n",EVRpid,nLines);
   *(SHORT*)*memptr = nLines;
   *memptr += sizeof(SHORT);

   if (location == LIT_END || location == LIT_SORTASCENDING ||
                                              location == LIT_SORTDESCENDING ) {
      for (i=1; i<=nLines; ++i) {
         strcpy(varName,parm);
         _itoa(i,buffer,10);
         strcat(varName,buffer);
         TRACE(4,"%x:    varName: %s\n",EVRpid,varName);
         rexxVarToChar(varName,(CHAR*)*memptr,4096,1);
         TRACE(4,"%x:    varValue: %s\n",EVRpid,(CHAR*)*memptr);
         *memptr += strlen((CHAR*)*memptr) + 1;
      }
   } else {                                /* flip order                      */
      for (i=nLines; i>0; --i) {
         strcpy(varName,parm);
         _itoa(i,buffer,10);
         strcat(varName,buffer);
         rexxVarToChar(varName,(CHAR*)*memptr,4096,1);
         *memptr += strlen((CHAR*)*memptr) + 1;
      }
   }

   free(varName);
}

VOID iListBoxSelect(CHAR* parm,PVOID* memptr) {

   CHAR    *varName;
   SHORT   state, index;

   varName = (CHAR*) malloc((strlen(parm)+6)*sizeof(CHAR));

   strcpy(varName,parm);                  /* get selection state              */
   strcat(varName,"STATE");
   rexxVarToShort(varName,&state,1);
   *(USHORT*)*memptr = (USHORT) state;
   *memptr += sizeof(USHORT);

   strcpy(varName,parm);                  /* get line to select               */
   strcat(varName,"INDEX");
   rexxVarToShort(varName,&index,1);
   if (index > 0)
      index--;                            /* REXX' indices are not offsets!   */
   else
      index = 0;                          /* error in user parm!              */
   *(SHORT*)*memptr = index;
   *memptr += sizeof(SHORT);

   free(varName);
}

VOID iMLEDeleteLines(CHAR* parm,PVOID* memptr) {

   CHAR   *varName;
   ULONG  nLines, start;

   varName = (CHAR*) malloc((strlen(parm)+ULONG_DIG)*sizeof(CHAR));

   strcpy(varName,parm);                  /* get first line                   */
   strcat(varName,"START");
   rexxVarToUlong(varName,&start,1);      /* must be ULONG to translate       */
   if ((LONG) start > 0)                  /* cursor-position correctly        */
      start--;                            /* REXX' indices are not offsets!   */
   *(LONG*)*memptr = (LONG) start;
   *memptr += sizeof(LONG);

   strcpy(varName,parm);                  /* get number of lines to delete    */
   strcat(varName,"COUNT");
   rexxVarToUlong(varName,&nLines,1);
   *(LONG*)*memptr = (LONG) nLines;
   *memptr += sizeof(LONG);

   free(varName);
}

VOID iMLEWriteLines(CHAR* parm,PVOID* memptr) {

   CHAR   *varName, buffer[ULONG_DIG];
   LONG   i, nLines;
   ULONG  location;

   varName = (CHAR*) malloc((strlen(parm)+ULONG_DIG)*sizeof(CHAR));

   strcpy(varName,parm);                  /* get insertion point              */
   strcat(varName,"LOC");
   rexxVarToUlong(varName,&location,1);
   if ((LONG) location > 0)
      location--;                         /* REXX' indices are not offsets!   */
   *(LONG*)*memptr = (LONG) location;
   *memptr += sizeof(LONG);

   strcpy(varName,parm);                  /* get number of lines to insert    */
   strcat(varName,"0");
   rexxVarToLong(varName,&nLines,1);
   *(LONG*)*memptr = nLines;
   *memptr += sizeof(LONG);

   for (i=1; i<=nLines; ++i) {
      strcpy(varName,parm);
      _ltoa(i,buffer,10);
      strcat(varName,buffer);
      rexxVarToChar(varName,(CHAR*)*memptr,4096,1);
      *memptr += strlen((CHAR*)*memptr) + 1;
   }
   free(varName);
}

VOID iMLEReadLines(CHAR* parm,PVOID* memptr) {
   ULONG start;
   start = strtoul(parm,NULL,10);
   if ((LONG) start > 0)
      start--;                            /* REXX' indices are not offsets!   */
   *(LONG*) *memptr = (LONG) start;
   *memptr += sizeof(LONG);
}

/* -------------------------------------------------------------------------- */

VOID tNull(CHAR* retVar,PVOID* memptr) {}

VOID tULong(CHAR* retVar,PVOID* memptr) {
   ulongToRexxVar(retVar,*(ULONG*)*memptr,1);
   *memptr += sizeof(ULONG);
}

VOID tUShort(CHAR* retVar,PVOID* memptr) {
   ulongToRexxVar(retVar,*(USHORT*)*memptr,1);
   *memptr += sizeof(USHORT);
}

VOID tShort(CHAR* retVar,PVOID* memptr) {
   intToRexxVar(retVar,*(SHORT*)*memptr,1);
   *memptr += sizeof(SHORT);
}

VOID tShortArray(CHAR* retVar,PVOID* memptr) {
   SHORT *array, length;
   length   = *(SHORT*)*memptr;
   *memptr += sizeof(SHORT);
   array    = (SHORT*)*memptr;
   *memptr += length*sizeof(SHORT);
   shortArrayToRexxStem(retVar,array,length,1);
}

VOID tBool(CHAR* retVar,PVOID* memptr) {
   ulongToRexxVar(retVar,*(BOOL*)*memptr,1);    /* BOOL is type-defined ULONG */
   *memptr += sizeof(BOOL);
}

VOID tString(CHAR* retVar,PVOID* memptr) {
   charToRexxVar(retVar,(CHAR*)*memptr,1);
   *memptr += strlen((CHAR*)*memptr)+1;
}

VOID tStrings(CHAR* retVar,PVOID* memptr) {
   LONG i, size;
   CHAR  **line;

   size = *(LONG*)*memptr;
   *memptr += sizeof(LONG);

   if (size > 0) {
      line = (CHAR**) malloc(size*sizeof(CHAR*));
      line[0] = (CHAR*)*memptr;
      *memptr += strlen(line[0]) + 1;
      for (i=1; i<size; ++i) {
         line[i] = line[i-1] + strlen(line[i-1]) + 1;
         *memptr += strlen(line[i]) + 1;
      }
   }
   charArrayToRexxStem(retVar,line,size,1);
   if (size > 0)
      free(line);
}
