/*
    EVRmkdll.cmd: REXX-script to create a resource DLL
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

/* required utility functions   --------------------------------------------- */

IF RxFuncQuery('SysSearchPath') THEN
  CALL RxFuncAdd 'SysSearchPath','RexxUtil','SysSearchPath'

/* parse arguments and prompt if necessary   -------------------------------- */

PARSE ARG resourceName dllName .
IF resourceName = '' THEN
  CALL usage
ELSE DO
   IF POS('.',resourceName) > 0 THEN DO
      resourceType = TRANSLATE(SUBSTR(resourceName,LASTPOS('.',resourceName)+1))
      IF WORDPOS(resourceType,'RC RES') = 0 THEN DO
         SAY ''
         SAY 'Invalid file extension!'
         CALL usage
      END
   END
   ELSE IF STREAM(resourceName'.rc','C','query exists') <> '' THEN DO
         resourceName = resourceName'.rc'
         resourceType = 'RC'
   END
   ELSE IF STREAM(resourceName'.res','C','query exists') <> '' THEN DO
      resourceName = resourceName'.res'
      resourceType = 'RES'
   END
   ELSE DO
      SAY 'Neither' resourceName'.rc nor' resourceName'.res exists!'
      EXIT 2
   END
   basename = SUBSTR(resourceName,1,LASTPOS('.',resourceName)-1)
END

IF dllName = '' THEN DO
   CALL CHAROUT ,'Enter name of DLL ('basename'.dll):'
   PULL dllName
   IF dllName = '' THEN
      dllName = basename'.dll'
END
IF TRANSLATE(RIGHT(dllName,4)) <> '.DLL' THEN
   dllName = dllName'.dll'

/* locate empty.obj and empty.def   ----------------------------------------- */

emptyObject = SysSearchPath('DPATH','EVRempty.obj')
IF emptyObject = '' THEN
   emptyObject = SysSearchPath('PATH','EVRempty.obj')
IF emptyObject = '' THEN DO
   SAY 'EVRempty.obj not in DPATH and PATH'
   EXIT 3
END

emptyDef = SysSearchPath('DPATH','EVRempty.def')
IF emptyDef = '' THEN
   emptyDef = SysSearchPath('PATH','EVRempty.def')
IF emptyDef = '' THEN DO
   SAY 'EVRempty.def not in DPATH and PATH'
   EXIT 3
END

/* link and add resources   ------------------------------------------------- */

ADDRESS CMD 'link386' emptyObject','dllName',,,'emptyDef
IF resourceType <> 'RES' THEN
   ADDRESS CMD 'rc -r' resourceName
ADDRESS CMD 'rc' basename'.res' dllName
RETURN

/* internal subroutines   --------------------------------------------------- */

usage:

PARSE SOURCE . . thisProc
thisProc = SUBSTR(thisProc,LASTPOS('\',thisProc)+1)
PARSE VAR thisProc thisProc '.' .

SAY ' '
SAY 'usage:' thisProc 'res-filename <dll-filename>'
SAY ' '
SAY 'res-filename: either a resource script file (extension .RC)'
SAY '              or a compiled resource (extension .RES).'
SAY '              If no extension is supplied,' thisProc 'will look for the'
SAY '              .RC-file first.'
SAY ' '
SAY 'dll-filename: name of the DLL (extension .DLL will be added if omitted)'
EXIT 1
