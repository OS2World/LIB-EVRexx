/*
    EVRh2Rex.cmd: internal procedure to define #defined constants as
                  rexx variables. Include it in your EVRexx applications
                  to facilitate the access to controls of dialog windows.
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
/*----------------------------------------------------------------------------*/
/* EVRh2Rex: internal proc to define #defined values as rexx variables        */
/*           (not 100% save, but works for reasonable #include-files)         */
/*----------------------------------------------------------------------------*/
EVRh2Rex:

ARG filename                                  /* name of #include-file as arg */

/* check filename   --------------------------------------------------------- */

IF STREAM(filename,'C','query exists') = '' THEN DO
   IF POS('\',filename) > 0 THEN
      RETURN 1

   /* second try: look for it in the same place as REXX   ------------------- */

   PARSE SOURCE . . thisProc
   file = SUBSTR(thisProc,1,LASTPOS('\',thisProc)) || filename
   IF STREAM(file,'C','query exists') = '' THEN DO

      /* third try: look for it in DPATH and INCLUDE   ---------------------- */

      IF RxFuncQuery('SysSearchPath') THEN
        CALL RxFuncAdd 'SysSearchPath','RexxUtil','SysSearchPath'
      file = SysSearchPath('DPATH',filename)
      IF file = '' THEN DO
         file = SysSearchPath('INCLUDE',filename)
         IF file = '' THEN
            RETURN 1
      END
   END
   filename = file
END

/* read line and filter #defines   ------------------------------------------ */

DO WHILE LINES(filename) > 0
   line = LINEIN(filename)                    /* get next line                */
   IF WORD(line,1) = '#define' THEN DO        /* assume #define is first word */
      PARSE VALUE line WITH . line '/*' .     /* remove #define and comments  */
      INTERPRET WORD(line,1) '=' SUBWORD(line,2)
   END
END
CALL LINEOUT filename                         /* close #include-file          */
RETURN 0                                      /* that's it!                   */
