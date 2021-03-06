/*
    EVRversion.c: source of EVRversion external function
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
#include <stdio.h>

#include "EVRexx.h"

/* -------------------------------------------------------------------------- */
/* External function EVRversion: Return version number of EVRexx              */
/*                                                                            */
/* CALL EVRversion                                                            */
/*                                                                            */
/* (all arguments are ignored)                                                */
/*                                                                            */
/* Result: current version nummber                                            */
/* -------------------------------------------------------------------------- */

ULONG EVRversion(CHAR *name, ULONG numargs, RXSTRING args[],
                                            CHAR *queuename, RXSTRING *retstr) {


  sprintf(retstr->strptr,"%4.2f",EVR_VERSION);
  retstr->strlength = strlen(retstr->strptr);
  return VALID_ROUTINE;
}
