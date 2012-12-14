/*
    EVRzip.cmd: sample EVRexx program: list contents of a ZIP-archive and unzip
                    selected files. Note: This is only a sample program. ZIPME
                    is a better choice for daily work!
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

/* install exeption-handler   ----------------------------------------------- */

SIGNAL ON ERROR   NAME freeResources
SIGNAL ON FAILURE NAME freeResources
SIGNAL ON HALT    NAME freeResources
SIGNAL ON SYNTAX  NAME freeResources

/* prepare environment   ---------------------------------------------------- */

CALL RxFuncAdd 'EVRLoadFuncs','EVRexx','EVRLoadFuncs'
CALL EVRloadFuncs
CALL EVRinit 'EVRzip'
IF RESULT > 0 THEN DO
   SAY 'Could not initialize EVRexx (RC =' RESULT'). Aborting...'
   EXIT 3
END
CALL EVRh2Rex 'EVRzip.h'
IF RESULT > 0 THEN DO
   CALL EVRmessageBox 'Could not read EVRzip.h','Fatal error!',MB.OK,MB.ERROR
   CALL freeResources
END

/* read arguments and set default values   ---------------------------------- */

ARG archive filespecs
IF archive <> '' THEN
  IF STREAM(archive,'C','query exists') = '' THEN
     archive = ''
IF archive = '' THEN DO
   CALL EVRfileDialog 'Select ZIP-File','*.zip',FD.OPEN,'archive'
   IF RESULT = DID_CANCEL THEN
      CALL freeResources
END
CALL setDefaults

/* enter display-loop   ----------------------------------------------------- */

DO FOREVER
   CALL getZipInfo
   handle = EVRloadDialog(ZIPINFO_DIALOG)
   IF handle = 0 THEN DO
      CALL EVRmessageBox 'Could not load dialog','Fatal error!',MB.OK,MB.ERROR
      CALL freeResources
   END

   /* set title and values of controls, then display dialog   --------------- */

   CALL EVRmanageControl handle,ZIPINFO_DIALOG,TXT.WRITE,,
                                                     'EVRzip -' archive,'dummy'
   CALL EVRmanageControl handle,'controlID.','requestCode.','parm.','retVal.'
   CALL EVRmodalDialog handle

   /* process result (selected button)   ------------------------------------ */

   SELECT
      WHEN RESULT = CHANGE_OPTIONS THEN DO
        CALL refreshOptions
        CALL EVRdestroyDialog handle
        ITERATE
      END
      WHEN RESULT = UNZIP_SELECTED THEN DO
        CALL unzipArchive
        IF RESULT = 0 THEN
           LEAVE
        ELSE
           ITERATE
      END
      OTHERWISE DO
        CALL EVRdestroyDialog handle
        LEAVE
      END
   END
END
CALL freeResources
RETURN

/*----------------------------------------------------------------------------*/
/* setDefaults: define values                                                 */
/*----------------------------------------------------------------------------*/
setDefaults:

controlId.0 = 7
controlId.1 = OPTION_1;      requestCode.1 = BU.CHECK;  parm.1 = 0
controlId.2 = OPTION_SHORT;  requestCode.2 = BU.CHECK;  parm.2 = 0
controlId.3 = OPTION_MEDIUM; requestCode.3 = BU.CHECK;  parm.3 = 1
controlId.4 = OPTION_LONG;   requestCode.4 = BU.CHECK;  parm.4 = 0
controlId.5 = ARCH_HEADER;   requestCode.5 = TXT.WRITE;
controlId.6 = ARCH_TOTALS;   requestCode.6 = TXT.WRITE;
controlId.7 = ARCH_INFO;     requestCode.7 = LB.INSERT; parm.7 = 'files.'
files.loc = LB.END
RETURN

/*----------------------------------------------------------------------------*/
/* getZipInfo: run zipinfo and collect information                            */
/*----------------------------------------------------------------------------*/
getZipInfo:

optionString = '-ht'
IF parm.1 = 1 THEN
  optionString = optionString'2'
ELSE IF parm.2 = 1 THEN
  optionString = optionString's'
ELSE IF parm.3 = 1 THEN
  optionString = optionString'm'
ELSE IF parm.4 = 1 THEN
  optionString = optionString'l'

'@unzip -Z' optionString archive filespecs '|RXQUEUE'
IF QUEUED() > 0 THEN DO
   PARSE PULL parm.5
   files.0 = QUEUED() - 1
   DO i=1 TO files.0
      PARSE PULL files.i
   END
   PARSE PULL parm.6
END
ELSE DO
   CALL EVRmessageBox 'Problems with unzip','Fatal error!',MB.OK,MB.ERROR
   CALL freeResources
END
RETURN

/*----------------------------------------------------------------------------*/
/* refreshOptions: query selected options                                     */
/*----------------------------------------------------------------------------*/
refreshOptions:

controlId.0 = 4
requestCode.1 = BU.ISCHECKED; retVar.1 = 'parm.1'  /* return result in        */
requestCode.2 = BU.ISCHECKED; retVar.2 = 'parm.2'  /* specified variables     */
requestCode.3 = BU.ISCHECKED; retVar.3 = 'parm.3'
requestCode.4 = BU.ISCHECKED; retVar.4 = 'parm.4'
CALL EVRmanageControl handle, 'controlId.', 'requestCode.',,'retVar.'
controlId.0 = 7
requestCode.1 = BU.CHECK; requestCode.2 = BU.CHECK
requestCode.3 = BU.CHECK; requestCode.4 = BU.CHECK
RETURN

/*----------------------------------------------------------------------------*/
/* unzipArchive:  query selected files and unzip                              */
/*----------------------------------------------------------------------------*/
unzipArchive:

CALL EVRmanageControl handle, ARCH_INFO, LB.GETSELTEXT,,'selectedFiles.'
CALL EVRdestroyDialog handle
IF selectedFiles.0 = 0 THEN DO
   CALL EVRmessageBox 'No files selected','User error!',MB.OK,MB.INFORMATION
   RETURN 1
END

handle = EVRloadDialog(UNZIP_DIALOG)
IF handle = 0 THEN DO
   CALL EVRmessageBox 'Could not load dialog','Fatal error!',MB.OK,MB.ERROR
   CALL freeResources
END
CALL EVRmanageControl handle, OPTION_PROMPT,BU.CHECK,1,'dummy'
CALL EVRmodalDialog handle
IF RESULT = DID_OK THEN DO
   unzipId.0 = 7;
   unzipId.1 = UNZIP_DIR;        unzipCode.1 = TXT.READ;
   unzipId.2 = OPTION_PROMPT;    unzipCode.2 = BU.ISCHECKED;
   unzipId.3 = OPTION_OVERWRITE; unzipCode.3 = BU.ISCHECKED;
   unzipId.4 = OPTION_NEVEROVWR; unzipCode.4 = BU.ISCHECKED;
   unzipId.5 = OPTION_JUNK;      unzipCode.5 = BU.ISCHECKED;
   unzipId.6 = OPTION_FRESHEN;   unzipCode.6 = BU.ISCHECKED;
   unzipId.7 = OPTION_UPDATE;    unzipCode.7 = BU.ISCHECKED;
   unzipRetVar.1 = 'unzipDir'
   CALL EVRmanageControl handle, 'unzipId.','unzipCode.',,'unzipRetVar.'

   IF unzipRetVar.2 = 1 THEN
      unzipOptions = ''
   ELSE IF unzipRetVar.3 = 1 THEN
      unzipOptions = '-o'
   ELSE IF unzipRetVar.4 = 1 THEN
      unzipOptions = '-n'
   IF unzipRetVar.5 = 1 THEN
      unzipOptions = unzipOptions '-j'
   IF unzipRetVar.6 = 1 THEN
      unzipOptions = unzipOptions '-f'
   ELSE IF unzipRetVar.7 = 1 THEN
      unzipOptions = unzipOptions '-u'

   filespecs = ''
   DO i = 1 TO selectedFiles.0
      filespecs = filespecs WORD(selectedFiles.i,WORDS(selectedFiles.i))
   END
   IF unzipDir <> '' THEN
      filespecs = filespecs '-d' unzipDir

   CALL EVRdestroyDialog handle
   'unzip' unzipOptions archive filespecs
   RETURN 0
END
ELSE
   RETURN 1

/*----------------------------------------------------------------------------*/
/* freeResources: return system resources and terminate                       */
/*----------------------------------------------------------------------------*/
freeResources:

IF CONDITION() <> '' THEN DO
   SAY 'Internal error in EVRzip.cmd'
   SAY 'Error in line' SIGL '(RC='RC'):' ERRORTEXT(RC)
   SAY 'line('SIGL'):'SOURCELINE(SIGL)
   SAY 'Condition:  ' CONDITION('C')
   SAY 'Description:' CONDITION('D')
   TRACE ?R
END
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
