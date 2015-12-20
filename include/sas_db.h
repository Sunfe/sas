/*********************************************************
* File Name:          
* Description:       
* Others:      
* Author: lzc
* Date: 2015-07-12
**********************************************************/
#include "mysql.h"  

/***********************************************
 macro definitions
***********************************************/
#define SAS_DB_OK                    (0x0)
#define SAS_DB_ERROR                 (0x1 << 0)
#define SAS_DB_CONNECT_ERROR         (0x1 << 1)
#define SAS_DB_QUERRY_EXE_ERROR      (0x1 << 2)
#define SAS_DB_TABLE_CREATE_ERROR    (0x1 << 3)


/***********************************************
 datatype definitions
***********************************************/


/***********************************************
 global variable declarations
***********************************************/


/***********************************************
 global function declarations
***********************************************/
int mysql_db_connect( MYSQL *mysql);
int mysql_exe_select(MYSQL* mysql, char *sql, MYSQL_RES **rs);
int get_mysql_table_info(MYSQL*mysql,const char *table_name, const char * stock_code,  MYSQL_RES**rs);

