/******************************************************************************/
/*                 Copyright C 1998-2008 Broadcom Corporation                 */
/*                                                                            */
/* This material is the confidential trade secret and proprietary information */
/* of Broadcom Corporation.  It may not be reproduced, used, sold or          */
/* transferred to any third party without the prior written consent of        */
/* Broadcom Corporation.   All rights reserved.                               */
/******************************************************************************/
/*   FileName        : bcm_timer.h                                            */
/*   Purpose         : Usefull time related functions                         */
/*   Limitations     : None                                                   */
/*   Creation Date   : 20-Sep-2001                                            */
/*   Current Version : 1.17                                      */
/*   Last Modified   : 2010/04/13 14:39:29                           */
/*   History         : Compiled by CVS                                        */
/******************************************************************************/
#ifndef SAS_TIMER_H
# define SAS_TIMER_H

#include "sas_basic_type.h"
void sas_sleep(uint32 timeToSleepMS);
char *getCurrentTimeStr(char *s);
uint32 getCurrentTime(void);

#endif
