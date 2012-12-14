/*
    rexxUtils.c: prototypes for functions from rexxUtils.c
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
#if !defined (rexxUtils_H)
  #define rexxUtils_H

  ULONG rexxVarToShort(const CHAR* varName, SHORT* value, ULONG symbolicSubst);
  ULONG rexxVarToLong(const CHAR* varName, LONG* value, ULONG symbolicSubst);
  ULONG rexxVarToUlong(const CHAR* varName, ULONG* value, ULONG symbolicSubst);
  ULONG rexxVarToChar(const CHAR* varName, CHAR* value, ULONG length,
                                                           ULONG symbolicSubst);

  ULONG intToRexxVar(const CHAR* varName, INT value, ULONG symbolicSubst);
  ULONG ulongToRexxVar(const CHAR* varName, ULONG value, ULONG symbolicSubst);
  ULONG charToRexxVar(const char* varName, char* value, ULONG symbolicSubst);

  ULONG ulongArrayToRexxVar(const CHAR* varNames[], ULONG values[],
                                               ULONG size, ULONG symbolicSubst);

  ULONG shortArrayToRexxStem(const CHAR* stemName, SHORT* values,
                                               ULONG size, ULONG symbolicSubst);
  ULONG charArrayToRexxStem(const CHAR* stemName, CHAR* values[],
                                               ULONG size, ULONG symbolicSubst);
#endif
