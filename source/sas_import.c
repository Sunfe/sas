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
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <mysql.h>  
#include "sas_common.h"
#include "sas_db.h"
#include "sas_shell.h"
#include "sas_dbg_print.h"

/***********************************************
 macro definition
***********************************************/
#undef SAS_DEBUG
#define MAX_FILE_NUM        (4096)
#define VALID_FILE_OFFSET   (2)


/***********************************************
 datatype definition
***********************************************/
typedef struct _PROC_SHARE_MEM
{
    int imported_file_num;
    int is_finished;
}PROC_SHARE_MEM;


/***********************************************
 global variables
***********************************************/
const char *data_path_index = "/home/stock/data_stock/";
const char *db_table_name_index = "orig_stock_2010_2015";

const char *data_path_stock = "/home/stock/data_index/";
const char *db_table_name_stock = "orig_index_2010_2015";


/***********************************************
 static variables 
***********************************************/


/***********************************************
 extern functions 
***********************************************/
extern MYSQL *get_global_mysql();
extern const char *orig_data_path;
extern const char *import_table_name;


/***********************************************
 static  function declarations
***********************************************/
static int sas_import_data(MYSQL *mysql);
static int sas_create_table(MYSQL*mysql, const char *tablename);

static int construct_create_table_sql(char *sqlexec);
static int construct_insert_table_sql(char *values,char *sqlexec);

/***********************************************
 global functions declarations
***********************************************/
int import_data(int type);


/***********************************************
 global functions definition
***********************************************/
int sas_import(char *arg)
{
    unsigned int type = 0;
    int idx = 0;
    
    if(!scanf_int(arg, &idx, &type))
    {
        printf("  import <type>:1 index,2 stock\n");
        return SAS_OK;
    }

    import_data(type);

    return SAS_OK;
}



int import_data(int type)
{
    MYSQL *mysql = get_global_mysql();

    if(SAS_IMPORT_TYPE_INDEX == type)
    {
        orig_data_path = data_path_index;
        import_table_name = db_table_name_index;

        if(SAS_DB_OK != sas_create_table(mysql,import_table_name))
            return SAS_ERROR;

        if(SAS_OK != sas_import_data(mysql))
            return SAS_ERROR;
    }
    else if(SAS_IMPORT_TYPE_STOCK == type)
    {
        orig_data_path = data_path_stock;
        import_table_name = db_table_name_stock;

        if(SAS_DB_OK != sas_create_table(mysql,import_table_name))
            return SAS_ERROR;

        if(SAS_OK != sas_import_data(mysql))
            return SAS_ERROR;
    }


}


static int sas_import_data_from_files(char **files,int file_start, int file_end)
{
    MYSQL *mysql = get_global_mysql();
    int file_pos=0; 
    int imported_file_num = 0;

    int total_file = file_end - file_start;
    for(file_pos = file_start; file_pos < file_end ; file_pos++)
    {
        if(SAS_OK != commit_data_from_file(files[file_pos], mysql))
        {
            continue;
        }

        double export_progress =imported_file_num/(double)(total_file - VALID_FILE_OFFSET)*100;

        if(file_pos == file_start)
        {
            printf("\n\r%4.1f%%",export_progress);
        }
        else if(file_end-1 == file_pos)
        {
        
            printf("\ndone!\n");
            fflush(stdout);
        }
        else
        {
            printf("\b\b\b\b\b\b\b%4.1f%%",export_progress);
            fflush(stdout);
        }

        imported_file_num++;
    }

    SAS_PRINT_INFO("total %d file skipped",total_file - imported_file_num);
    fflush(stdout);

    return SAS_OK;

}


int exe_import_job(MYSQL *mysql)
{
    const char *path= orig_data_path ;
    char *files[MAX_FILE_NUM];

    int i=0;
    for(; i < MAX_FILE_NUM; i++)
    {
        files[i] = mallocMem(MAX_FILE_NAME_LEN);
        if(NULL == files[i])
        {
            exit(0);
            return SAS_ERROR;
        }
    }

    int total_file=0;
    getFiles(path, files, &total_file);

    if(total_file < 3 )
    {
        SAS_PRINT_ERROR("no data found!");
        fflush(stdout);

        exit(0);
        return SAS_ERROR;
    }

    SAS_PRINT_INFO("total %d files found, prepare to import,wait..", total_file - VALID_FILE_OFFSET);

    sas_import_data_from_files(files, VALID_FILE_OFFSET, total_file);

    for(i=0; i < MAX_FILE_NUM; i++)
        freeMem(files[i],MAX_FILE_NAME_LEN);
    
    exit(0);
    return SAS_OK;
}


int commit_data_from_file(char *file, MYSQL *mysql)
{
    char values[MAX_BUFFER_LEN];
    char sqlexec[MAX_BUFFER_LEN];

    memset(values,0,MAX_BUFFER_LEN);
    memset(sqlexec,0,MAX_BUFFER_LEN);

    if(SAS_OK != parzeFileContentToSqlStr(file, values))
    {
        SAS_PRINT_INFO("%s format wrong,skipped",file);
        return SAS_ERROR;
    }

    if(SAS_OK != construct_insert_table_sql(values,sqlexec))
    {
        SAS_PRINT_ERROR("construct sql error,%s skipped.",file);
        return SAS_ERROR;
    }

    if (mysql_query(mysql,sqlexec)!=0)
    {
        SAS_PRINT_ERROR("record insertion not successful,errorinfo:%s",mysql_error(mysql));
        return SAS_ERROR;
    }
    
    return SAS_OK;
}

/***********************************************
  static functions definition
 ***********************************************/
static int sas_import_data(MYSQL *mysql)
{
    switch(fork())
    {
        case -1:
            SAS_PRINT_INFO("fail to fork process!");
        case 0:
            exe_import_job(mysql);
        default:
            break;
    }

    return SAS_OK;
}


static int sas_create_table(MYSQL*mysql, const char *tablename)
{
    char sqlexec[MAX_STR_LEN];
    memset(sqlexec, 0, MAX_STR_LEN);
    if(SAS_OK != construct_create_table_sql(sqlexec))
        return SAS_ERROR;

    if(SAS_DB_OK != mysql_create_table(mysql,tablename,sqlexec))
        return SAS_ERROR;

    return SAS_OK;
}


static int construct_create_table_sql(char *sqlexec)
{
    char sql[MAX_STOCK_NUM];
    memset(sql, 0, MAX_STOCK_NUM);

    if( NULL == sqlexec)
        return SAS_ERROR;

    strcpy(sql,"CREATE TABLE ");
    strcat(sql,import_table_name);
    char *str="(trade_index int(10) NOT NULL ," \
         " stock_code varchar(10) NOT NULL," \
         " trade_date varchar(10) NOT NULL," \
         " open_price float(7,2)  NULL," \
         " max_price float(7,2)    NULL," \
         " min_price float(7,2)    NULL," \
         " close_price float(7,2)  NULL," \
         " volume int(15) NULL," \
         " turnover float(15,1) NULL," \
         " PRIMARY KEY (`stock_code`, `trade_date`)" \
         " ) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=GB2312";
    strcat(sql,str);
    strcpy(sqlexec, sql);

    return SAS_OK;
}


static int construct_insert_table_sql(char *values,char *sqlexec)
{
    char *sql = NULL;
    if(NULL == values || NULL == sqlexec)
        return SAS_ERROR;

    strcpy(sqlexec,"insert ignore into `");
    strcat(sqlexec,import_table_name);
    sql = "`(`trade_index`,`stock_code`,"\
           "`trade_date`,`open_price`, `max_price`, `min_price`, `close_price`,"\
           "`volume`, `turnover`)values";

    strcat(sqlexec, sql);
    strcat(sqlexec, values);
    return SAS_OK;
}
