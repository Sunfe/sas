
/*********************************************************
 * File Name:          
 * Description:       
 * Others:      
 * Author: lzc
 * Date: 2015-07-28
 **********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>  
#include "sas_common.h"
#include "sas_file.h"
#include "sas_db.h"


/***********************************************
  macro definition
 ***********************************************/
#define MYSQL_CLOSE_PRICE_ROW  (6)
#define MAX_ITEM_NUM           (10)
#define MAX_TRADE_DATE         (2000)
#define MAX_FIELD_NUM          (10)
#define MAX_FILED_STR_LEN      (20)
#define MAX_RATIO_SPAN         (22)
    
/***********************************************
  datatype definition
 ***********************************************/


/***********************************************
  global variables
 ***********************************************/


/***********************************************
  static variables 
 ***********************************************/
static char daily_trade_info[MAX_TRADE_DATE][MAX_ITEM_NUM][MAX_STR_LEN];

/***********************************************
  extern variables
 ***********************************************/
extern char* index_basic_table;
extern char* index_derived_table;


/***********************************************
  extern functions 
 ***********************************************/


/***********************************************
  static functions declarations
 ***********************************************/

/***********************************************
  global functions declarations
 ***********************************************/

/***********************************************
  global functions definition
 ***********************************************/
int show_info_main(int cmd)
{
    MYSQL mysql;

    if(MYSQL_OK != mysql_db_connect(&mysql))
        return 0;

#if 0
static char trade_date[MAX_TRADE_DATE][MAX_ITME_NUM][MAX_STR_LEN];

char *daily_trade_info[MAX_ITME_NUM][MAX_STR_LEN]
    int date = 0;
    for(date = 0; date< MAX_TRADE_DATE; date++)
    {
        daily_trade_info
    
    }
#endif

    extern char * stock_derived_table;
    extern char * stock_basic_table;
    if(1 == cmd)
    {
        if(SAS_OK != show_stock_increase_num_ratio(&mysql,stock_derived_table))
            return 0;
    }
    else if(2 == cmd)
    {
        if(SAS_OK != show_stock_ma_num_ratio(&mysql,stock_derived_table))
            return 0;
    
    }
    else if(3 == cmd)
    {
        if(SAS_OK != show_low_evalued_stocks(&mysql,stock_basic_table))
            return 0;
    
    }

    mysql_close(&mysql);

    return 0;
}

int show_low_evalued_stocks(MYSQL*mysql,const char *src_table)
{
    const char date_range_start[]="2014-07-01";
    const char date_range_end[]="2014-08-01";
    return get_stocks_by_price_in_index_range(mysql,src_table,date_range_start, date_range_end);
}


static char trade_date[MAX_TRADE_DATE][MAX_STR_LEN];
int get_stocks_by_price_in_index_range(MYSQL*mysql,const char *src_table,const char *date_start, const char *date_end)
{
    MYSQL_ROW row;
    MYSQL_RES *rs = NULL;

    if(SAS_OK != get_mysql_table_info(mysql,index_basic_table, "000001", &rs))
        return SAS_ERROR;

    int date = 0;
    memset(&row, 0, sizeof row);
    memset(trade_date, 0, sizeof trade_date);
    while((row = mysql_fetch_row(rs)) != NULL)
    {
        strcpy(trade_date[date],row[TRADE_DATE_POS]);
        date++;
    }
    mysql_free_result(rs);

    char sql[MAX_STR_LEN];
    memset(sql, 0, MAX_STR_LEN);
    strcpy(sql, "select stock_code from ");
    strcat(sql, src_table);
    strcat(sql, " group by stock_code");
    strcat(sql, " order by trade_date DESC");
    if(MYSQL_OK != mysql_exe_select(mysql, sql, &rs))
        return SAS_ERROR;

    int stock_num = 0;
    int maxStockNum = 0;
    char stock_code[MAX_STOCK_NUM][MAX_STOCK_CODE_STR];
    memset(stock_code, 0 , MAX_STOCK_NUM * MAX_STOCK_CODE_STR);
    while ((row = mysql_fetch_row(rs)))
    {
        if(NULL != row[0])
        {
            strcpy(stock_code[stock_num], row[0]);
            stock_num++;
        }
    }
    maxStockNum = stock_num;
    mysql_free_result(rs);

#if 0
    strcpy(sql, "select min(close_price), max(close_price) from ");
    strcat(sql, index_basic_table);
    strcat(sql, " where trade_date > '");
    strcat(sql, date_start);
    strcat(sql, "' and trade_date < '");
    strcat(sql, date_end);
    strcat(sql, "' and stock_code='000001'");
#endif

    
    double min_price = 0;
    double max_price = 0;
    char min_price_str[10];
    char max_price_str[10];
    for(stock_num = 0; stock_num < maxStockNum; stock_num++)
    {
        strcpy(sql, "select min(close_price), max(close_price) from ");
        strcat(sql, src_table);
        strcat(sql, " where trade_date > '");
        strcat(sql, date_start);
        strcat(sql, "' and trade_date < '");
        strcat(sql, date_end);
        strcat(sql, "' and stock_code='");
        strcat(sql, stock_code[stock_num]);
        strcat(sql, "'");

        if(MYSQL_OK != mysql_exe_select(mysql, sql, &rs))
            return SAS_ERROR;

        row = mysql_fetch_row(rs);
        if(NULL == row[0] || NULL == row[1])
            continue;
        mysql_free_result(rs);

        min_price = atof(row[0]);
        max_price = atof(row[1]);
        sprintf(min_price_str, "%.2f", min_price);
        sprintf(max_price_str, "%.2f", max_price);

        strcpy(sql, "select stock_code from ");
        strcat(sql, src_table);
        strcat(sql, " where close_price >= '");
        strcat(sql, min_price_str);
        strcat(sql, "' and close_price <= '");
        strcat(sql, max_price_str);
        strcat(sql, "' and stock_code='");
        strcat(sql, stock_code[stock_num]);
        strcat(sql, "' and trade_date='2015-08-19'");

        if(MYSQL_OK != mysql_exe_select(mysql, sql, &rs))
            return SAS_ERROR;

        row = mysql_fetch_row(rs);
        if(NULL == row)
        {
            printf("\nnot selected:%s", stock_code[stock_num]);
            continue;
        }

        if(NULL == row[0])
        {
            printf("\nnot selected:%s", stock_code[stock_num]);
            continue;
        }


        printf("\nselected:%s", stock_code[stock_num]);
        mysql_free_result(rs);
    }

    return SAS_OK;
}
