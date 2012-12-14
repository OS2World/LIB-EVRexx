/*
    PMCon.cmd: sample EVRexx program: collects output of (PM-) programs to
                                      stdout/stderr
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

CALL processArguments 'Cmd' ARG(1)        /* syntax check, options, arguments */

/* select actions to perform depending on command   ------------------------- */

SELECT

   WHEN command = 'STOP' THEN
         '@ECHO +++STOP' options filename '| RXQUEUE' queuename
   WHEN command = 'CLEAR' THEN
      '@ECHO +++CLEAR | RXQUEUE' queuename
   WHEN command = 'EDIT' THEN
      '@ECHO +++EDIT | RXQUEUE' queuename
   WHEN command = 'NOEDIT' THEN
      '@ECHO +++NOEDIT | RXQUEUE' queuename
   WHEN command = 'SAVE' | command = 'APPEND' THEN
      '@ECHO +++'command filename '| RXQUEUE' queuename

   /* this is the interesting part of the program   ------------------------- */

   WHEN command = 'START' THEN DO
      initialStart = 1                                /* default flag         */
      IF POS('p',options) > 0 THEN                    /* private queue        */
         queuename = RXQUEUE('Create')
      ELSE DO
         queuename = RXQUEUE('Create','PMCon')
         IF queuename <> 'PMCON' THEN DO           /* queue already exists    */
            CALL RXQUEUE 'Delete',queuename        /* don't need this queue   */
            initialStart = 0                       /* change flag             */
            queuename = 'PMCon'
         END
      END

      IF initialStart THEN DO

         /* install exeption-handler and prepare environment   -------------- */

         IF command = 'START' THEN DO
            SIGNAL ON FAILURE NAME freeResources
            SIGNAL ON HALT    NAME freeResources
            SIGNAL ON SYNTAX  NAME freeResources

            CALL RxFuncAdd 'EVRLoadFuncs','EVRexx','EVRLoadFuncs'
            CALL EVRloadFuncs
            CALL EVRinit 'PMCon'
            IF RESULT > 0 THEN DO
              SAY 'Could not initialize EVRexx (RC =' RESULT'). Aborting...'
              CALL RXQUEUE 'Delete', queuename
              EXIT 3
            END
            CALL EVRh2Rex 'PMCon.h'
            IF RESULT > 0 THEN DO
               CALL EVRmessageBox 'Could not read PMCon.h','Fatal error!',,
                                                                  MB.OK,MB.ERROR
               CALL freeResources
            END
         END

         /* set queue, load dialog and change title ------------------------- */

         CALL RXQUEUE 'Set',queuename
         handle = EVRloadDialog(PMCON_DLG)
         IF handle = 0 THEN DO
           CALL EVRmessageBox 'Could not load dialog','Fatal error!',,
                                                                  MB.OK,MB.ERROR
           CALL freeResources
         END
         CALL EVRmanageControl handle,PMCON_DLG,TXT.WRITE,,
                                            'PMCon - Queue:' queuename,'success'
         CALL EVRmodelessDialog handle
      END

      IF POS('n',options) > 0 THEN           /* start program in new session  */
          '@START "'program'" /c/f "'program '| RXQUEUE' queuename '2>&1"'
      ELSE
         program '| RXQUEUE' queuename  '2>&1'   /* pass program to cmd.exe   */

      IF initialStart THEN
         CALL processData                    /* enter data processing loop    */
   END

   OTHERWISE
      NOP                              /* already checked in processArguments */

END  /* SELECT */

RETURN 0

/*----------------------------------------------------------------------------*/
/* processArguments: parse arguments into options, queuename, filename etc.   */
/*----------------------------------------------------------------------------*/
processArguments:

PARSE ARG type command argString
command = TRANSLATE(command)

IF type = 'Queue' & WORDPOS(command,'+++STOP +++CLEAR +++EDIT +++NOEDIT',
                                                   '+++SAVE +++APPEND') = 0 THEN
   RETURN
ELSE IF type = 'Cmd' & WORDPOS(command,'START STOP CLEAR EDIT NOEDIT',
                                                         'SAVE APPEND') = 0 THEN
   CALL usage

/* collect option flags   --------------------------------------------------- */

options = '-'
DO WHILE LEFT(argString,1) = '-'
   PARSE VAR argString '-' opt argString
   options = options||opt
END

/* parse remaining argument string   ---------------------------------------- */

IF type = 'Cmd' THEN DO
   IF command = 'START' THEN DO
     IF argString = '' THEN
        CALL usage
     ELSE
        program = argString
   END
   ELSE DO
      PARSE VAR argString queuename filename
      IF queuename = '' THEN
         queuename = 'PMCon'
   END
END
ELSE
   filename = argString

RETURN

/*----------------------------------------------------------------------------*/
/* processData: wait for data and process requests                            */
/*----------------------------------------------------------------------------*/
processData:

line.0   = 1                                        /* add a single line      */
line.loc = MLE.END                                  /* at the end of the text */

DO FOREVER
   line.1 = LINEIN('QUEUE:')                        /* parse from queue       */
   CALL processArguments 'Queue' line.1
   SELECT
      WHEN command = '+++STOP' THEN DO
         IF POS('a',options) > 0 THEN
            CALL saveData 'APPEND'
         ELSE IF POS('s',options) > 0 THEN
            CALL saveData 'SAVE'
         CALL EVRdestroyDialog handle                   /* destroy window     */
         CALL RXQUEUE 'Delete', queuename               /* delete queue and   */
         CALL freeResources                             /* return resources   */
      END

      WHEN command = '+++CLEAR' THEN DO
         clear.start = 1
         clear.count = MLE.END
         CALL EVRmanageControl handle, PMCON_MLE, MLE.DELETELINES, 'clear.'
      END

      WHEN command = '+++EDIT' THEN
         CALL EVRmanageControl handle, PMCON_MLE, MLE.SETREADONLY, 0, 'oldStat'
      WHEN command = '+++NOEDIT' THEN
         CALL EVRmanageControl handle, PMCON_MLE, MLE.SETREADONLY, 1, 'oldStat'

      WHEN command = '+++SAVE' THEN
         CALL saveData 'SAVE'
      WHEN command = '+++APPEND' THEN
         CALL saveData 'APPEND'

      OTHERWISE                                       /* add line to console  */
         CALL EVRmanageControl handle, PMCON_MLE, MLE.WRITELINES,'line.'
   END  /* SELECT */

END /* DO FOREVER */
RETURN

/*----------------------------------------------------------------------------*/
/* saveData: query filename and save or append data                           */
/*----------------------------------------------------------------------------*/
saveData:

ARG saveOption

IF filename = '' THEN DO                                  /* query filename  */
   IF saveOption = 'SAVE'  THEN
      CALL EVRfileDialog 'Select file','*.*',FD.SAVEAS,'filename'
   ELSE
      CALL EVRfileDialog 'Select file','*.*',FD.OPEN,'filename'
   IF RESULT = DID_CANCEL THEN
      RETURN
END

read.start = 1
read.count = MLE.END
CALL EVRmanageControl handle,PMCON_MLE,MLE.READLINES,'read.','read.'
IF saveOption = 'SAVE'  THEN
   '@DEL' filename '2>NUL'                            /* delete existing file */
DO i=1 TO read.0
   CALL LINEOUT filename, read.i
END
CALL LINEOUT filename                                 /* close file           */
RETURN

/*----------------------------------------------------------------------------*/
/* freeResources: return system resources and terminate                       */
/*----------------------------------------------------------------------------*/
freeResources:

IF CONDITION() <> '' THEN DO
   PARSE SOURCE . . thisProc
   SAY 'Internal error in' thisProc
   SAY 'Error in line' SIGL '(RC='RC'):' ERRORTEXT(RC)
   SAY 'line('SIGL'):'SOURCELINE(SIGL)
   SAY 'Condition:  ' CONDITION('C')
   SAY 'Description:' CONDITION('D')
   TRACE ?R
END
CALL RXQUEUE 'Delete', queuename
CALL EVRcleanup
EXIT

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

/*----------------------------------------------------------------------------*/
/* usage: summary of commands and parameters                                  */
/*----------------------------------------------------------------------------*/
usage:

IF RxFuncQuery('SysSearchPath') THEN
  CALL RxFuncAdd 'SysSearchPath','RexxUtil','SysSearchPath'

PMConDoc = SysSearchPath('DPATH','PMCon.doc')
IF PMConDoc = '' THEN
   PMConDoc = SysSearchPath('HELP','PMCon.doc')
IF PMConDoc = '' THEN
   PMConDoc = SysSearchPath('BOOK','PMCon.doc')
IF PMConDoc = '' THEN DO
   SAY 'PMCon.doc not in DPATH, HELP or BOOK-path'
   EXIT 3
END
'@type' PMConDoc '|more'
EXIT 1
