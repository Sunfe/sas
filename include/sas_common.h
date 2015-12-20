/*********************************************************
* File Name:          
* Description:       
* Others:      
* Author: lzc
* Date: 2015-07-12
**********************************************************/
#ifndef SAS_COMMON_H
#define SAS_COMMON_H

#include "sas_basic_type.h"

/***********************************************
 macro definitions
***********************************************/
#define SAS_DEBUG                     (0x0)
#define SAS_OK                        (0x0)
#define SAS_ERROR                     (0x1<<0)
#define SAS_CREATE_MYSQL_TABLE_ERROR  (0x1<<1)

#define MAX_BUFFER_LEN      (1024000)
#define MAX_STR_LEN         (1024)
#define MAX_FILE_NAME_LEN   (256)

#define MAX_STOCK_NUM       (5000)
#define MAX_STOCK_CODE_STR  (8)

#define MAX_MA_NUM          (3)  //MA5, MA10, MA20

#define CLOSE_PRICE_POS     (6)
#define TRADE_DATE_POS      (2)

#define PARA_NOT_USED(p)    (p = p)

#define SAS_IMPORT_TYPE_INDEX   1
#define SAS_IMPORT_TYPE_STOCK   2


/***********************************************
 datatype definitions
***********************************************/



/***********************************************
 global variable declarations
***********************************************/


/***********************************************
 global function declarations
***********************************************/
char * mallocMem(int bufferLen);
void freeMem(char *buffer, int len);

#endif
