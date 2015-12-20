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
#include <mysql.h>  
#include "sas_common.h"
#include "sas_db.h"
#include "sas_dbg_print.h"


/***********************************************
  macro definition
 ***********************************************/
#define DB_HOST      "localhost"
#define DB_USER      "root"
#define DB_PASSWORD  "Line0907"
#define DB_NAME      "stock"


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
const char *import_table_name ;


/***********************************************
  extern functions 
 ***********************************************/


/***********************************************
 global functions declarations
***********************************************/
int mysql_create_table(MYSQL *mysql, const char *tablename, const char *sqlexec);


/***********************************************
  global functions definition
 ***********************************************/
int mysql_db_connect( MYSQL *mysql)
{
    mysql_init(mysql); 
    const char *host = DB_HOST;
    const char *user = DB_USER;
    const char *pass = DB_PASSWORD;
    const char *db   = DB_NAME;

    if (!mysql_real_connect(mysql, host, user, pass, db, 0, NULL, 0))
    {
        printf("error at %s,%d:database failed to connect:%s\n",__FILE__,
                __LINE__, mysql_error(mysql));
        return SAS_DB_CONNECT_ERROR;
    } 

    return SAS_DB_OK;
}


int mysql_create_table(MYSQL *mysql, const char *tablename, const char *sqlexec)
{
    MYSQL_RES *rs = NULL;

    if(NULL == tablename)
        return SAS_DB_ERROR;

    if(NULL == sqlexec)
        return SAS_DB_ERROR;

    char sql[MAX_STR_LEN] ;
    strcpy(sql, "show tables like '");
    strcat(sql, tablename);
    strcat(sql, "'");

    if(SAS_DB_OK != mysql_exe_select(mysql, sql, &rs))
        return SAS_DB_QUERRY_EXE_ERROR;

    int row_counts = mysql_num_rows(rs);
    if(0 != row_counts)  //table already exists
    {
        memset(sql, 0, sizeof sql);
        strcat(sql, "ALTER TABLE `");
        strcat(sql, tablename);
        strcat(sql, "` ADD INDEX idx(`stock_code`, `trade_date`)");
        mysql_query(mysql, sql);

        return SAS_DB_OK;
    }


    if (0 != mysql_query(mysql, sqlexec))
    {
#ifdef SAS_DEBUG
        printf( "\nerror at %s,%d:new table creation not successful," \
                "error info:%s", __FILE__,__LINE__, mysql_error(mysql));
        fflush(stdout);
#endif
        return SAS_DB_TABLE_CREATE_ERROR;
    }
    else
    {
        memset(sql, 0, sizeof sql);
        strcat(sql, "ALTER TABLE `");
        strcat(sql, tablename);
        strcat(sql, "` ADD INDEX idx(`stock_code`, `trade_date`)");
        puts(sql);
        mysql_query(mysql, sql);

        SAS_PRINT_INFO("new table %s created!",__FILE__,__LINE__,tablename);
        fflush(stdout);

        return SAS_DB_OK;
    }


}


int mysql_exe_select(MYSQL* mysql, char *sql, MYSQL_RES **rs)
{
    if (mysql_query(mysql, sql) != 0)
    {
        printf( "error at %s,%d:%s\n",__FILE__,__LINE__, mysql_error(mysql));
        return SAS_DB_QUERRY_EXE_ERROR;
    }

    *rs = mysql_store_result(mysql); 
    return SAS_DB_OK;
}


int get_mysql_table_info(MYSQL*mysql,const char *table_name, const char * stock_code,  MYSQL_RES**rs)
{
    if(mysql == NULL || table_name == NULL)
        return SAS_ERROR;

    char sql[MAX_STR_LEN];
    memset(sql, 0, MAX_STR_LEN);
    strcpy(sql, "select * from ");
    strcat(sql, table_name);
    strcat(sql, " where stock_code='");
    strcat(sql, stock_code);
    strcat(sql, "' order by trade_date DESC");

    if(SAS_DB_OK != mysql_exe_select(mysql, sql, rs))
        return SAS_ERROR;

    return SAS_OK;
}


int sas_init_db(MYSQL *mysql)
{
    if(NULL == mysql)
    {
        printf("\nerror at %s,%d:null mysql object",__FILE__,__LINE__);
        return SAS_ERROR;
    }

    memset(mysql, 0 ,sizeof(MYSQL));
    mysql_init(mysql); 
    return SAS_OK;
}


int  sas_close_db(MYSQL *mysql)
{
    if(NULL == mysql)
    {
        printf("\nerror at %s,%d:null mysql object",__FILE__,__LINE__);
        return SAS_ERROR;
    }
    mysql_close(mysql);
    return SAS_OK;
}


/***********************************************
  static functions definition
 ***********************************************/
/*to be added*/
