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
#include <ctype.h>
#include <sys/time.h> 
#include <unistd.h> 
#include "sas_common.h"
#include "sas_file.h"

/***********************************************
 macro definition
***********************************************/
#define MAX_TRADE_INDEX_LEN (64)
#define MAX_ITEM_NUM        (10)
#define STOCK_CODE_LEN      (7)
#define STOCK_DATE_ITEM_NUM (7)
#define MAX_FILE_LINE       (4096)


/***********************************************
 datatype definition
***********************************************/


/***********************************************
 global variables
***********************************************/


/***********************************************
 static variables 
***********************************************/
static void format_sql_values_str(char **content,int total_line, char *sqlstr);
static int get_file_content(FILE *fp, char **content, int *total_line);


/***********************************************
 extern variables
***********************************************/


/***********************************************
 extern functions 
***********************************************/


/***********************************************
 global function declarations
***********************************************/
void splitStr(char *src, const char *sep, char**des,  int *total);


/***********************************************
 global functions definition
***********************************************/
int parzeFileContentToSqlStr(const char*file, char *sqlstr)
{
    FILE *fp = NULL; 
    int i=0;
    char *content[MAX_FILE_LINE];
    int total_line = 0;

    if((fp = fopen(file,"r")) == NULL) 
    { 
        printf("\n%s,%d:failed to open file!",__FILE__, __LINE__); 
        return FILE_READ_ERROR; 
    }

    for(i = 0; i < MAX_FILE_LINE; i++)
        content[i] = mallocMem(MAX_STR_LEN);

    if(SAS_OK != get_file_content(fp, content, &total_line))
        return SAS_ERROR;

    format_sql_values_str(content, total_line, sqlstr);

    for(i = 0; i < MAX_FILE_LINE; i++)
        freeMem(content[i],MAX_STR_LEN);

    fclose(fp);
    return SAS_OK;
}


void splitStr(char *src, const char *sep, char*des[], int *total)
{
    int i = 0;
    char* token = NULL;

    i=0;
    token = strtok(src,sep);   
    while(token != NULL)
    {   
        strcpy(des[i],token);
        token = strtok(NULL,sep);   
        i++;
    }       

    *total = i;
}


int get_useconds()
{
    struct timeval tv; 
    struct timezone tz; 
    gettimeofday (&tv , &tz); 
    return tv.tv_usec;
}


/***********************************************
 static functions definition
***********************************************/
static int get_file_content(FILE *fp, char **content, int *total_line) 
{
    char fileline[MAX_STR_LEN];
    char *str[MAX_ITEM_NUM];
    int line_num = 0;
    int total_item = 0;
    int i = 0;

    for(i = 0; i < MAX_ITEM_NUM; i++)
        str[i] = mallocMem(MAX_STR_LEN);

    while (!feof(fp)) 
    { 
        memset(fileline, 0,MAX_STR_LEN );
        fgets(fileline, MAX_STR_LEN, fp);  

        int line_len = strlen(fileline);
        if(line_len > 2)
            fileline[strlen(fileline) - 2] = '\0'; /*remove ^M*/

        /*the first contains the stock code*/
        if(0 == line_num)
        {
            splitStr(fileline, " ", str, &total_item);

            if(!isdigit(fileline[0]))
                return FILE_FORMAT_ERROR ;

            if(strlen(str[0]) > STOCK_CODE_LEN ) 
                return FILE_FORMAT_ERROR ;

            strcpy(content[line_num++], str[0]);
            continue;
        }

        if(NULL == strchr(fileline, ',') || !isdigit(fileline[0]))
            continue;

        strcpy(content[line_num++],fileline);
    } 
    *total_line = line_num;

    for(i = 0;i < MAX_ITEM_NUM; i++)
       freeMem(str[i],MAX_STR_LEN);

    if(*total_line < 2)
        return FILE_FORMAT_ERROR;
    else
        return SAS_OK;
}

static void format_sql_values_str(char **content,int total_line, char *sqlstr)
{
    int trade_index=0;
    int total_item = 0;
    int line_num = 0;
    int i = 0;
    int j = 0;
    char *values = sqlstr;
    char *str[MAX_ITEM_NUM];
    char stock_code[STOCK_CODE_LEN];
    char trade_index_str[MAX_TRADE_INDEX_LEN];

    for(i = 0; i < MAX_ITEM_NUM; i++)
        str[i] = mallocMem(MAX_STR_LEN);

    if(0 == line_num)
        strcpy(stock_code, content[0]);

    for(i = 1; i < total_line; i++)
    {
        splitStr(content[i], ",", str, &total_item);
        if(total_item != STOCK_DATE_ITEM_NUM)
            continue;

        /*format like: "(1,'600000'," */
        strcat(values,"(");
        sprintf(trade_index_str, "%d", trade_index);
        strcat(values+strlen(values),trade_index_str);
        strcat(values+strlen(values),",'");
        strcat(values+strlen(values),stock_code);
        strcat(values+strlen(values),"',");

        /*date*/
        strcat(values + strlen(values), "'");
        strcat(values + strlen(values), str[0]);
        strcat(values + strlen(values), "'");
        /*prices*/
        for(j = 1; j < total_item; j++)
        {
            strcat(values+strlen(values), ",");
            strcat(values + strlen(values), str[j]);
        }

        strcat(values+strlen(values), "),");

        trade_index++;
    }

    /*format like:
     * "(1,'600000','2006/01/04', 1.70, 1.82, 1.70, 1.81, 16333311, 165103152.000) " 
     * */
    values[strlen(values) - 1]=' ';

    for(i = 0;i < MAX_ITEM_NUM; i++)
       freeMem(str[i],MAX_STR_LEN);

    return ;
}
