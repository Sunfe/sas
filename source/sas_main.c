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
#include "sas_db.h"


/***********************************************
  macro definition
 ***********************************************/
    
/***********************************************
  datatype definition
 ***********************************************/


/***********************************************
  global variables
 ***********************************************/
MYSQL mysql;

const char *orig_data_path = NULL;
const char *import_table_name = NULL;

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
  static functions declarations
 ***********************************************/


/***********************************************
  global functions declarations
 ***********************************************/
MYSQL *get_global_mysql();
int sas_init_shell();


/***********************************************
  global functions definition
 ***********************************************/
int main()
{
    MYSQL *db = get_global_mysql();
    sas_activate_db(db);
    sas_init_shell();
    return 0;
}


int sas_activate_db(MYSQL *mysql)
{
    if(SAS_DB_OK != sas_init_db(mysql))
        return SAS_ERROR;

    if(SAS_DB_OK != mysql_db_connect(mysql))
        return SAS_ERROR;

    return SAS_OK;
}


MYSQL *get_global_mysql()
{
  return &mysql;
}


int sas_init_shell()
{
    return shell_main();
}


/***********************************************
  static functions definition
 ***********************************************/




