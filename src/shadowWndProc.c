/*
    shadowWndProc.c: window procedure for EVRexx.c
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
#include <memory.h>
#include "EVRexx.h"
#include "xternEXE.h"

#define  _DEBUG_LEVEL  4
#include "trace.h"

typedef  VOID (*procFkt)(HWND, ULONG, PVOID*, PVOID*);
extern procFkt  processControl[];

#if defined(REXX_DLG_PROC_IMPLEMENTED)
   rexxDlgProc(HWND,ULONG,MPARAM,MPARAM);
#endif

MRESULT shadowWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {

   if (msg == EVRactivateShadow) {   /* this might be for us                  */
      TRACE(4,"%x:           EVRactivateShadow received\n",EVRpid);
      if ((ULONG) mp1 == EVRpid) {   /* this is for us!                       */
         TRACE(4,"%x:           mp1 == EVRpid!\n",EVRpid);
         switch ((ULONG) mp2) {

         case EVR_REINIT:
           {
            PVOID memptr;
            APIRET   RC;

            memptr = EVRsharedMem;
            TRACE(4,"%x:              Drive: %lu\n",EVRpid,*(ULONG*)memptr);
            DosSetDefaultDisk(*(ULONG*) memptr);           /* change drive    */

            memptr += sizeof(ULONG);
            TRACE(4,"%x:              Dir: %s\n",EVRpid,(CHAR*)memptr);
            DosSetCurrentDir((CHAR*) memptr);               /* and directory  */

            memptr += strlen(memptr)+1;
            TRACE(4,"%x:              DLL: %s\n",EVRpid,(CHAR*)memptr);
            *(ULONG *) EVRsharedMem = 0;
            if (*(CHAR*)memptr != '\0') {
               RC = DosLoadModule(NULL,0L,(CHAR*)memptr,
                                                  &EVRdefaultResourceDllHandle);
               if (RC != 0) {
                  *(ULONG *) EVRsharedMem = RC;
                  TRACE(4,"%x:              DosLoadModule failed, RC = %lu\n",
                                                                    EVRpid, RC);
               }
            } else
               EVRdefaultResourceDllHandle = 0;

            TRACE(3,"%x:              Posting event semaphore\n",EVRpid);
            DosPostEventSem(EVRsemaphore);
            break;
           }
         case EVR_CLEANUP:
            TRACE(3,"%x:           Posting WM_QUIT ...\n",EVRpid);
            WinPostMsg(hwnd,WM_QUIT,(MPARAM) 0, (MPARAM) 0);
            break;

         case EVR_MESSAGE_BOX:
           {
            USHORT rc;
            PSZ    text, title;
            ULONG  flags;

            flags = *(ULONG *) EVRsharedMem;
            text  = (PSZ) (EVRsharedMem+sizeof(ULONG));
            title =  text + strlen(text) + 1;

            TRACE(3,"%x:           Displaying message box ...\n",EVRpid);
            rc = WinMessageBox(HWND_DESKTOP,hwnd,text,title,0,flags);
            *(USHORT*) EVRsharedMem = rc;

            TRACE(3,"%x:              Posting event semaphore\n",EVRpid);
            DosPostEventSem(EVRsemaphore);
            break;
           }
         case EVR_FILE_DIALOG:
           {
            HWND    hwndDlg;
            FILEDLG *pFileDlg;
            LONG    rc;

            pFileDlg = (FILEDLG*) EVRsharedMem;
            TRACE(3,"%x:           Displaying file dialog ...\n",EVRpid);
            hwndDlg  = WinFileDlg(HWND_DESKTOP,hwnd,pFileDlg);
            rc = pFileDlg->lReturn;
            if (hwndDlg && (rc == DID_OK)) {
               ULONG *pUlong;
               CHAR  *pChar;
               pUlong = (ULONG*) (EVRsharedMem+sizeof(LONG));
               pChar  = (CHAR*)  (EVRsharedMem+sizeof(LONG)+sizeof(ULONG));
               if (pFileDlg->ulFQFCount == 1) {         /* only a single file */
                  *pUlong = 1;
                  memmove(pChar,pFileDlg->szFullFile,
                                                strlen(pFileDlg->szFullFile)+1);
               } else {
                  PAPSZ filenames;
                  ULONG i;
                  *pUlong = pFileDlg->ulFQFCount;
                  filenames = pFileDlg->papszFQFilename;
                  for (i=0; i<*pUlong; ++i) {
                     strcpy(pChar,(*filenames)[i]);
                     pChar += strlen(pChar) + 1;
                  }
                  WinFreeFileDlgList(filenames);
               }
            }
            *(LONG*) EVRsharedMem = rc;
            TRACE(3,"%x:              Posting event semaphore\n",EVRpid);
            DosPostEventSem(EVRsemaphore);
            break;
           }
         case EVR_LOAD_DIALOG:
           {
            HWND    hwndDlg;
            HMODULE hmoduleResource;
            ULONG   dialogID;
            CHAR    *dllName, *rexxProcName, *buffer;
            APIRET  RC;

            dialogID     = *(ULONG*) EVRsharedMem;
            dllName      = (CHAR*) (EVRsharedMem+sizeof(ULONG));
            rexxProcName = dllName + strlen(dllName) + 1;

            if (*dllName == '\0')
               hmoduleResource = EVRdefaultResourceDllHandle;
            else {
               RC = DosLoadModule(NULL,0L,dllName,&hmoduleResource);
               TRACE(3,"%x:              RC of DosLoadModule: %lu\n",EVRpid,RC);
            }

            TRACE(3,"%x:           Loading dialog ...\n",EVRpid);
            if (*rexxProcName == '\0')
               hwndDlg = WinLoadDlg(HWND_DESKTOP,hwnd,WinDefDlgProc,
                                                 hmoduleResource,dialogID,NULL);
#if defined(REXX_DLG_PROC_IMPLEMENTED)
            else {
               /* Allocate memory for dialog window parms. The rexxDlgProc    */
               /* will free this memory on WM_DESTROY.                        */

               buffer = (CHAR*) malloc((strlen(rexxProcName)+1)*sizeof(CHAR));
               strcpy(buffer,rexxProcName);
               hwndDlg = WinLoadDlg(HWND_DESKTOP,hwnd,rexxDlgProc,
                                         hmoduleResource,dialogID,buffer);
            }
#else
            else
               hwndDlg = NULLHANDLE;
#endif
            *(ULONG *) EVRsharedMem = hwndDlg;
            TRACE(3,"%x:              Posting event semaphore\n",EVRpid);
            DosPostEventSem(EVRsemaphore);
            break;
           }
         case EVR_PROC_DIALOG:
           {
            HWND  hwndDlg;
            ULONG button, titleLength;
            CHAR* title;
            SWCNTRL swctl;                             /* switch control data */
            HSWITCH hswitch;                           /* switch handle       */

            hwndDlg = *(HWND*) EVRsharedMem;
            title   = (CHAR*) (EVRsharedMem+sizeof(ULONG));
            titleLength = strlen(title);
            if (*title != '\0') {
               strcat(title,": ");
               titleLength += 2;
               WinQueryWindowText(hwndDlg, MAXNAMEL+4-titleLength,
                                                           title+titleLength+1);
               *(title+titleLength) = ' ';
            } else
               WinQueryWindowText(hwndDlg, MAXNAMEL+4,title);

            swctl.hwnd = hwndDlg;                      /* window handle       */
            swctl.hwndIcon = NULLHANDLE;               /* icon handle         */
            swctl.hprog = NULLHANDLE;                  /* program handle      */
            swctl.idProcess = EVRpid;                  /* process identifier  */
            swctl.idSession = 0;                       /* session identifier  */
            swctl.uchVisibility = SWL_VISIBLE;         /* visibility          */
            swctl.fbJump = SWL_JUMPABLE;               /* jump indicator      */
            strcpy(swctl.szSwtitle,title);             /* program name        */
            swctl.bProgType    = PROG_DEFAULT;         /* program type        */

            TRACE(3,"%x:           Processing dialog ...\n",EVRpid);
            hswitch = WinAddSwitchEntry(&swctl);
            button  = WinProcessDlg(hwndDlg);
            WinRemoveSwitchEntry(hswitch);
            *(ULONG*) EVRsharedMem = button;
            TRACE(3,"%x:              Posting event semaphore\n",EVRpid);
            DosPostEventSem(EVRsemaphore);
            break;
           }
         case EVR_MODELESS_DIALOG:
           {
            HWND  hwndDlg;
            ULONG titleLength;
            CHAR* title;
            SWCNTRL swctl;                             /* switch control data */
            HSWITCH hswitch;                           /* switch handle       */

            hwndDlg = *(HWND*) EVRsharedMem;
            title   = (CHAR*) (EVRsharedMem+sizeof(ULONG));
            titleLength = strlen(title);
            if (*title != '\0') {
               strcat(title,": ");
               titleLength += 2;
               WinQueryWindowText(hwndDlg, MAXNAMEL+4-titleLength,
                                                           title+titleLength+1);
               *(title+titleLength) = ' ';
            } else
               WinQueryWindowText(hwndDlg, MAXNAMEL+4,title);

            swctl.hwnd = hwndDlg;                      /* window handle       */
            swctl.hwndIcon = NULLHANDLE;               /* icon handle         */
            swctl.hprog = NULLHANDLE;                  /* program handle      */
            swctl.idProcess = EVRpid;                  /* process identifier  */
            swctl.idSession = 0;                       /* session identifier  */
            swctl.uchVisibility = SWL_VISIBLE;         /* visibility          */
            swctl.fbJump = SWL_JUMPABLE;               /* jump indicator      */
            strcpy(swctl.szSwtitle,title);             /* program name        */
            swctl.bProgType    = PROG_DEFAULT;         /* program type        */

            TRACE(3,"%x:           Processing modeless dialog ...\n",EVRpid);
            hswitch = WinAddSwitchEntry(&swctl);
            WinShowWindow(hwndDlg,TRUE);
            TRACE(3,"%x:              Posting event semaphore\n",EVRpid);
            DosPostEventSem(EVRsemaphore);
            break;
           }
         case EVR_DEST_DIALOG:
           {
            HWND  hwndDlg;
            BOOL fSuccess;
            HSWITCH hswitch;

            hwndDlg = *(HWND*) EVRsharedMem;

            TRACE(3,"%x:           Processing dialog ...\n",EVRpid);
            hswitch = WinQuerySwitchHandle(hwndDlg,0);
            WinRemoveSwitchEntry(hswitch);
            fSuccess = WinDestroyWindow(hwndDlg);
            *(BOOL*) EVRsharedMem = fSuccess;
            TRACE(3,"%x:              Posting event semaphore\n",EVRpid);
            DosPostEventSem(EVRsemaphore);
            break;
           }
         case EVR_CONTROL:
           {
            HWND hwndDlg;
            ULONG nRequests, size, i, requestCode, controlId;
            VOID  *requestData, *memptr;

            size      = *(ULONG*) EVRsharedMem-3*sizeof(ULONG);
            nRequests = *(ULONG*) (EVRsharedMem+2*sizeof(ULONG));
            hwndDlg   = *(ULONG*) (EVRsharedMem+  sizeof(ULONG));

            /* save request data (shared mem-object is needed for results!)   */

            requestData = malloc(size);
            memcpy(requestData,EVRsharedMem+3*sizeof(ULONG),size);
            memptr = EVRsharedMem;

            TRACE(3,"%x:           Processing requests ...\n",EVRpid);
            for (i=1; i<=nRequests; ++i) {
               requestCode = *(ULONG*) memptr = *(ULONG*) requestData;
               memptr      += sizeof(ULONG);
               requestData += sizeof(ULONG);
               controlId   = *(ULONG*) requestData;
               requestData += sizeof(ULONG);
               TRACE(3,"%x:            requestCode: %lu \n",EVRpid,requestCode);
               (*processControl[requestCode])(hwndDlg,controlId,
                                                          &requestData,&memptr);
            }
            TRACE(3,"%x:           ... and finished\n",EVRpid);
            free(requestData);

            TRACE(3,"%x:              Posting event semaphore\n",EVRpid);
            DosPostEventSem(EVRsemaphore);
            break;
           }
        default:
           return WinDefWindowProc(hwnd,msg,mp1,mp2);
           break;
         } /* endswitch */
      }

   } else {                          /* standard messages                     */

      switch (msg) {
      case WM_CREATE:
         TRACE(4,"%x:           WM_CREATE received\n",EVRpid);
         TRACE(4,"%x:              Posting event semaphore\n",EVRpid);
         DosPostEventSem(EVRsemaphore);
         break;
      case WM_DESTROY:
         TRACE(4,"%x:           WM_DESTROY received\n",EVRpid);
         break;
      default:
         return WinDefWindowProc(hwnd,msg,mp1,mp2);
        break;
      } /* endswitch */

   } /* endif */
   return (MRESULT) FALSE;
}
