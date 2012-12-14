/*
    rexxUtils.c: RexxVariablePool utility functions
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
#include <malloc.h>

#define INCL_RXSHV
#include <os2.h>

#define ULONG_DIG     12
#define NO_MEMORY     1000

/* Set a REXX stem in the function pool with an array of shorts   ----------- */

ULONG shortArrayToRexxStem(const CHAR* stemName, SHORT* values, ULONG size,
                                                          ULONG symbolicSubst) {
   SHVBLOCK *block;
   CHAR     **varNames, **charValues, buffer[ULONG_DIG];
   ULONG    rc, i;
   UCHAR    subst;

   if (symbolicSubst)
      subst = RXSHV_SYSET;                                /* symbolic set     */
   else
      subst = RXSHV_SET;                                  /* direct set       */

   /* allocate resources   -------------------------------------------------- */

   block = (SHVBLOCK*) malloc((size+1)*sizeof(SHVBLOCK));
   if (block == NULL)
      return NO_MEMORY;

   charValues = (CHAR**) malloc((size+1)*sizeof(CHAR*)); /* pointer to        */
   if (charValues == NULL) {                             /* array of values   */
      free(block);
      return NO_MEMORY;
   }

   varNames = (CHAR**) malloc((size+1)*sizeof(CHAR*));   /* pointer to        */
   if (varNames == NULL) {                               /* array of varNames */
      free(block);
      free(charValues);
      return NO_MEMORY;
   }

   _ultoa(size,buffer,10);                               /* charValues        */
   for (i=0; i<=size; ++i) {
      charValues[i] = (CHAR*) malloc(ULONG_DIG*sizeof(CHAR));
      if (charValues[i] == NULL) {
         for (i=i-1; i>=0; --i)
            free(charValues[i]);
         free(charValues);
         free(varNames);
         free(block);
         return NO_MEMORY;
      }
   }
   for (i=0; i<=size; ++i) {                             /* varNames          */
      varNames[i] =
               (CHAR*) malloc((strlen(stemName)+strlen(buffer)+1)*sizeof(CHAR));
      if (varNames[i] == NULL) {
         for (i=i-1; i>=0; --i)
            free(varNames[i]);
         free(varNames);
         for (i=0; i<=size; ++i)
            free(charValues[i]);
         free(charValues);
         free(block);
         return NO_MEMORY;
      }
   }

   /* fill linked list of SHVBLOCK structures   ----------------------------- */

   for (i=0; i<=size; ++i) {
      if (i<size)
         block[i].shvnext = &block[i+1];           /* create links            */
      else
         block[i].shvnext = NULL;

      block[i].shvcode = subst;                    /* type of substitution    */
      block[i].shvret  = 0;                        /* clear return code       */

      strcpy(varNames[i],stemName);                /* stem with running index */
      strcat(varNames[i],_ultoa(i,buffer,10));
      MAKERXSTRING(block[i].shvname,varNames[i],strlen(varNames[i]));

      if (i==0)                                    /* stem.0 holds the size   */
         _ultoa(size,charValues[i],10);
      else
         _itoa(values[i-1],charValues[i],10);
      MAKERXSTRING(block[i].shvvalue,charValues[i],strlen(charValues[i]));
      block[i].shvvaluelen = strlen(charValues[i]);
   }

   /* set variables, free resources   --------------------------------------- */

   rc = RexxVariablePool(block);

   for (i=0; i<=size; ++i) {
      free(charValues[i]);
      free(varNames[i]);
   }
   free(charValues);
   free(varNames);
   free(block);
   return rc;
}
/* Set a REXX variable in the function pool with a short   ------------------ */

ULONG intToRexxVar(const CHAR* varName, INT value, ULONG symbolicSubst) {

   char     buffer[ULONG_DIG];
   SHVBLOCK block;

   block.shvnext = 0;                                     /* only one request */
   if (symbolicSubst)
      block.shvcode = RXSHV_SYSET;                        /* symbolic set     */
   else
      block.shvcode = RXSHV_SET;                          /* direct set       */
   block.shvret  = 0;
   MAKERXSTRING(block.shvname,varName,strlen(varName));   /* name of var      */
   _itoa(value,buffer,10);                                /* convert value to */
   MAKERXSTRING(block.shvvalue,buffer,strlen(buffer));    /* RXSTRING         */
   block.shvvaluelen = strlen(buffer);

   return RexxVariablePool(&block);                       /* that's it!       */
}


/* Set a REXX variable in the function pool with an ulong   ----------------- */

ULONG ulongToRexxVar(const CHAR* varName, ULONG value, ULONG symbolicSubst) {

   char     buffer[ULONG_DIG];
   SHVBLOCK block;

   block.shvnext = 0;                                     /* only one request */
   if (symbolicSubst)
      block.shvcode = RXSHV_SYSET;                        /* symbolic set     */
   else
      block.shvcode = RXSHV_SET;                          /* direct set       */
   block.shvret  = 0;
   MAKERXSTRING(block.shvname,varName,strlen(varName));   /* name of var      */
   _ultoa(value,buffer,10);                               /* convert value to */
   MAKERXSTRING(block.shvvalue,buffer,strlen(buffer));    /* RXSTRING         */
   block.shvvaluelen = strlen(buffer);

   return RexxVariablePool(&block);                       /* that's it!       */
}


/* Set REXX variables in the function pool with an array of ULONGs   -------- */

ULONG ulongArrayToRexxVar(const CHAR* varNames[], ULONG values[], ULONG size,
                                                          ULONG symbolicSubst) {
   SHVBLOCK *block;
   CHAR     **buffer;
   ULONG    rc, i;
   UCHAR    subst;

   if (symbolicSubst)
      subst = RXSHV_SYSET;                                /* symbolic set     */
   else
      subst = RXSHV_SET;                                  /* direct set       */

   /* allocate resources   -------------------------------------------------- */

   block = (SHVBLOCK*) malloc(size*sizeof(SHVBLOCK));
   if (block == NULL)
      return NO_MEMORY;

   buffer = (CHAR**) malloc(size*sizeof(CHAR*));    /* pointer to        */
   if (buffer == NULL) {                            /* array of values   */
      free(block);
      return NO_MEMORY;
   }

   for (i=0; i<size; ++i) {
      buffer[i] = (CHAR*) malloc(ULONG_DIG*sizeof(CHAR));
      if (buffer[i] == NULL) {
         for (i=i-1; i>=0; --i)
            free(buffer[i]);
         free(buffer);
         free(block);
         return NO_MEMORY;
      }
   }

   /* fill linked list of SHVBLOCK structures   ----------------------------- */

   for (i=0; i<size; ++i) {
      if (i<size-1)
         block[i].shvnext = &block[i+1];           /* create links            */
      else
         block[i].shvnext = NULL;

      block[i].shvcode = subst;                    /* type of substitution    */
      block[i].shvret  = 0;                        /* clear return code       */

      MAKERXSTRING(block[i].shvname,varNames[i],strlen(varNames[i]));

      _ultoa(values[i],buffer[i],10);
      MAKERXSTRING(block[i].shvvalue,buffer[i],strlen(buffer[i]));
      block[i].shvvaluelen = strlen(buffer[i]);
   }

   /* set variables, free resources   --------------------------------------- */

   rc = RexxVariablePool(block);

   for (i=0; i<size; ++i)
      free(buffer[i]);
   free(buffer);
   free(block);
   return rc;
}


/* Set an ulong to the value of a REXX variable   --------------------------- */

ULONG rexxVarToUlong(const CHAR* varName, ULONG* value, ULONG symbolicSubst) {

   char     buffer[ULONG_DIG];
   ULONG    rc;
   SHVBLOCK block;

   block.shvnext = 0;                                     /* only one request */
   if (symbolicSubst)
      block.shvcode = RXSHV_SYFET;                        /* symbolic fetch   */
   else
      block.shvcode = RXSHV_FETCH;                        /* direct fetch     */
   block.shvret  = 0;
   MAKERXSTRING(block.shvname,varName,strlen(varName));   /* name of var      */
   MAKERXSTRING(block.shvvalue,buffer,ULONG_DIG);     /* buffer for value */
   block.shvvaluelen = ULONG_DIG;

   rc = RexxVariablePool(&block);
   if (rc == 0) {
      buffer[block.shvvaluelen + 1] = '\0';               /* add NULL         */
      *value = strtoul(buffer,NULL,10);
   }
   return rc;
}


/* Set a long to the value of a REXX variable   ----------------------------- */

ULONG rexxVarToLong(const CHAR* varName, LONG* value, ULONG symbolicSubst) {

   char     buffer[ULONG_DIG];
   ULONG    rc;
   SHVBLOCK block;

   block.shvnext = 0;                                     /* only one request */
   if (symbolicSubst)
      block.shvcode = RXSHV_SYFET;                        /* symbolic fetch   */
   else
      block.shvcode = RXSHV_FETCH;                        /* direct fetch     */
   block.shvret  = 0;
   MAKERXSTRING(block.shvname,varName,strlen(varName));   /* name of var      */
   MAKERXSTRING(block.shvvalue,buffer,ULONG_DIG);     /* buffer for value */
   block.shvvaluelen = ULONG_DIG;

   rc = RexxVariablePool(&block);
   if (rc == 0) {
      buffer[block.shvvaluelen + 1] = '\0';               /* add NULL         */
      *value = strtol(buffer,NULL,10);
   }
   return rc;
}


/* Set a short to the value of a REXX variable   ---------------------------- */

ULONG rexxVarToShort(const CHAR* varName, SHORT* value, ULONG symbolicSubst) {

   char     buffer[ULONG_DIG];
   ULONG    rc;
   SHVBLOCK block;

   block.shvnext = 0;                                     /* only one request */
   if (symbolicSubst)
      block.shvcode = RXSHV_SYFET;                        /* symbolic fetch   */
   else
      block.shvcode = RXSHV_FETCH;                        /* direct fetch     */
   block.shvret  = 0;
   MAKERXSTRING(block.shvname,varName,strlen(varName));   /* name of var      */
   MAKERXSTRING(block.shvvalue,buffer,ULONG_DIG);     /* buffer for value */
   block.shvvaluelen = ULONG_DIG;

   rc = RexxVariablePool(&block);
   if (rc == 0) {
      buffer[block.shvvaluelen + 1] = '\0';               /* add NULL         */
      *value = (SHORT) atoi(buffer);
   }
   return rc;
}


/* Set a REXX variable in the function pool with a char   ------------------- */

ULONG charToRexxVar(const CHAR* varName, CHAR* value, ULONG symbolicSubst) {

   SHVBLOCK block;

   block.shvnext = 0;                                     /* only one request */
   if (symbolicSubst)
      block.shvcode = RXSHV_SYSET;                        /* symbolic set     */
   else
      block.shvcode = RXSHV_SET;                          /* direct set       */
   block.shvret  = 0;
   MAKERXSTRING(block.shvname,varName,strlen(varName));   /* name of var      */
   MAKERXSTRING(block.shvvalue,value,strlen(value));      /* value as RXSTRING*/
   block.shvvaluelen = strlen(value);

   return RexxVariablePool(&block);                       /* that's it!       */
}


/* Set a char to the value of a REXX variable   ----------------------------- */

ULONG rexxVarToChar(const CHAR* varName, CHAR* value, ULONG length,
                                                          ULONG symbolicSubst) {
   ULONG    rc;
   SHVBLOCK block;

   block.shvnext = 0;                                     /* only one request */
   if (symbolicSubst)
      block.shvcode = RXSHV_SYFET;                        /* symbolic fetch   */
   else
      block.shvcode = RXSHV_FETCH;                        /* direct fetch     */
   block.shvret  = 0;
   MAKERXSTRING(block.shvname,varName,strlen(varName));   /* name of var      */
   MAKERXSTRING(block.shvvalue,value,length);             /* buffer for value */
   block.shvvaluelen = length;

   rc = RexxVariablePool(&block);
   if (rc == 0 && length > block.shvvaluelen)
      value[block.shvvaluelen] = '\0';                    /* add NULL         */
   return rc;
}


/* Set a REXX stem in the function pool with an array of chars   ------------ */

ULONG charArrayToRexxStem(const CHAR* stemName, CHAR* values[], ULONG size,
                                                          ULONG symbolicSubst) {
   SHVBLOCK *block;
   CHAR     **varNames, buffer[ULONG_DIG], value0[ULONG_DIG];
   ULONG    rc, i;
   UCHAR    subst;

   if (symbolicSubst)
      subst = RXSHV_SYSET;                                /* symbolic set     */
   else
      subst = RXSHV_SET;                                  /* direct set       */

   /* allocate resources   -------------------------------------------------- */

   block = (SHVBLOCK*) malloc((size+1)*sizeof(SHVBLOCK));
   if (block == NULL)
      return NO_MEMORY;

   varNames = (CHAR**) malloc((size+1)*sizeof(CHAR*));   /* pointer to        */
   if (varNames == NULL) {                               /* array of varNames */
      free(block);
      return NO_MEMORY;
   }

   _ultoa(size,buffer,10);                               /* varNames          */
   for (i=0; i<=size; ++i) {
      varNames[i] =
               (CHAR*) malloc((strlen(stemName)+strlen(buffer)+1)*sizeof(CHAR));
      if (varNames[i] == NULL) {
         for (i=i-1; i>=0; --i)
            free(varNames[i]);
         free(varNames);
         free(block);
         return NO_MEMORY;
      }
   }

   /* fill linked list of SHVBLOCK structures   ----------------------------- */

   for (i=0; i<=size; ++i) {
      if (i<size)
         block[i].shvnext = &block[i+1];           /* create links            */
      else
         block[i].shvnext = NULL;

      block[i].shvcode = subst;                    /* type of substitution    */
      block[i].shvret  = 0;                        /* clear return code       */

      strcpy(varNames[i],stemName);                /* stem with running index */
      strcat(varNames[i],_ultoa(i,buffer,10));
      MAKERXSTRING(block[i].shvname,varNames[i],strlen(varNames[i]));

      if (i==0) {                                  /* stem.0 holds the size   */
         _ultoa(size,value0,10);
         MAKERXSTRING(block[i].shvvalue,value0,strlen(value0));
         block[i].shvvaluelen = strlen(value0);
      } else {
         MAKERXSTRING(block[i].shvvalue,values[i-1],strlen(values[i-1]));
         block[i].shvvaluelen = strlen(values[i-1]);
      }
   }

   /* set variables, free resources   --------------------------------------- */

   rc = RexxVariablePool(block);

   for (i=0; i<=size; ++i)
      free(varNames[i]);
   free(varNames);
   free(block);
   return rc;
}
