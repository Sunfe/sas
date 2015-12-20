/*********************************************************
* File Name:          
* Description:       
* Others:      
* Author: lzc
* Date: 2015-07-12
**********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sas_common.h"

/***********************************************
 macro definition
***********************************************/


/***********************************************
 datatype definition
***********************************************/


/***********************************************
 global variables
***********************************************/


/***********************************************
 static variables 
***********************************************/


/***********************************************
 extern variables
***********************************************/


/***********************************************
 extern functions 
***********************************************/


/***********************************************
 global functions definition
***********************************************/
char * mallocMem(int bufferLen)
{
    char *buffer = (char*)malloc(bufferLen);
    if(NULL==buffer)
    {
        printf("\n%s,%d:malloc not successful!",__FILE__, __LINE__); 
        return NULL;
    }
    memset(buffer, 0,bufferLen);
    return buffer;
}

void freeMem(char *buffer, int len)
{
    memset(buffer, 0, len);
    free(buffer);
}


/***********************************************
 static functions definition
***********************************************/
