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

    if(SAS_DB_OK != mysql_db_connect(&mysql))
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

extern char* index_basic_table;
extern char* index_derived_table;
int show_stock_increase_num_ratio(MYSQL*mysql,const char *src_table)
{
    MYSQL_ROW row;
    MYSQL_RES *rs = NULL;

    if(SAS_OK != get_mysql_table_info(mysql,index_basic_table,"000001",&rs))
        return SAS_ERROR;

    int date = 0;
    int total_trade_date = 0;
    memset(&row, 0, sizeof row);
    while((row = mysql_fetch_row(rs)) != NULL)
    {
        strcpy(daily_trade_info[date][TRADE_DATE_POS],row[TRADE_DATE_POS]);
        strcpy(daily_trade_info[date][CLOSE_PRICE_POS],row[CLOSE_PRICE_POS]);
        date++;
    }
    total_trade_date = date;
    mysql_free_result(rs);

    //store index increase_ratio
    if(SAS_OK != get_mysql_table_info(mysql,index_derived_table,"000001",&rs))
        return SAS_ERROR;

    date = 0;
    while((row = mysql_fetch_row(rs)) != NULL)
    {
        strcpy(daily_trade_info[date][3],row[3]);
        date++;
    }
    mysql_free_result(rs);

    int ratio_span = 0;
    double price_incr_ratio[30];
    int left = 0;
    int right = 0;
    char left_str[10];
    char right_str[10];
    printf("\n-------------------------------------------------------------------------------"\
            "-------------------------------------------------------------------------\n");
    printf("           "); // 8 spaces
    printf("  ratio "); // 8 spaces

    for(ratio_span = 0;ratio_span < MAX_RATIO_SPAN; ratio_span++)
    {
        left = ratio_span - 11;
        right = ratio_span - 10;
        printf("  <%2d ",right);
    }
    printf("\n-------------------------------------------------------------------------------"\
            "-------------------------------------------------------------------------\n");

    for(date = 0; date < total_trade_date ;  date++)
    {
        char sqlexec[MAX_STR_LEN];
        strcpy(sqlexec, "select count(*) from ");
        strcat(sqlexec, src_table);
        strcat(sqlexec, " where trade_date='");
        strcat(sqlexec, daily_trade_info[date][TRADE_DATE_POS]);
        strcat(sqlexec,"'");

        if(SAS_DB_OK != mysql_exe_select(mysql, sqlexec, &rs))
            return SAS_ERROR;

        int daily_total =atoi((mysql_fetch_row(rs))[0]);
        mysql_free_result(rs);

        memset(left_str, 0, sizeof left_str);
        memset(right_str, 0, sizeof right_str);
        for(ratio_span = 0; ratio_span < MAX_RATIO_SPAN; ratio_span++)
        {
            left = ratio_span - 11;
            right = ratio_span - 10;
            sprintf(left_str,  "%d", left);
            sprintf(right_str, "%d", right);

            memset(sqlexec, 0, sizeof sqlexec);
            strcpy(sqlexec, "select count(*) from ");
            strcat(sqlexec, src_table);
            strcat(sqlexec, " where trade_date='");
            strcat(sqlexec, daily_trade_info[date][TRADE_DATE_POS]);
            strcat(sqlexec, "' and increase_ratio >= ");
            strcat(sqlexec, left_str);
            strcat(sqlexec, " and increase_ratio < ");
            strcat(sqlexec, right_str);

            if(SAS_DB_OK != mysql_exe_select(mysql, sqlexec, &rs))
                return SAS_ERROR;

            double price_incr_count = atof((mysql_fetch_row(rs))[0]);
            price_incr_ratio[ratio_span] = price_incr_count/daily_total;

            mysql_free_result(rs);

        }

        if(0 == date)
            printf("%8s: %6.2f%%", daily_trade_info[date][TRADE_DATE_POS], atof(daily_trade_info[date][3]));
        else
            printf("\n%8s: %6.2f%%", daily_trade_info[date][TRADE_DATE_POS], atof(daily_trade_info[date][3]));

        for(ratio_span = 0;ratio_span < MAX_RATIO_SPAN;ratio_span++)
        {
            printf("%6.1f", price_incr_ratio[ratio_span]*100);
        }
        
        fflush(stdout);

    }

    return SAS_OK;
}


int show_stock_ma_num_ratio(MYSQL*mysql,const char *src_table)
{
    MYSQL_ROW row;
    MYSQL_RES *rs = NULL;
    
    puts(index_basic_table);
    puts(index_derived_table);

    if(SAS_OK != get_mysql_table_info(mysql,index_basic_table,"000001",&rs))
        return SAS_ERROR;

    int date = 0;
    int total_trade_date = 0;
    memset(&row, 0, sizeof row);
    while((row = mysql_fetch_row(rs)) != NULL)
    {
        strcpy(daily_trade_info[date][TRADE_DATE_POS],row[TRADE_DATE_POS]);
        strcpy(daily_trade_info[date][CLOSE_PRICE_POS],row[CLOSE_PRICE_POS]);
        date++;
    }
    total_trade_date = date;
    mysql_free_result(rs);

    /*store index increase_ratio*/
    if(SAS_OK != get_mysql_table_info(mysql,index_derived_table,"000001",&rs))
        return SAS_ERROR;

    date = 0;
    while((row = mysql_fetch_row(rs)) != NULL)
    {
        strcpy(daily_trade_info[date][3],row[3]);//3: close_price
        date++;
    }
    mysql_free_result(rs);


    printf("\n-----------------------------------------------\n");
    printf("              index     MA5     MA10    MA20");
    printf("\n-----------------------------------------------\n");

    for(date = 0; date < total_trade_date ;  date++)
    {
        char sqlexec[MAX_STR_LEN];
        strcpy(sqlexec, "select count(*) from ");
        strcat(sqlexec, src_table);
        strcat(sqlexec, " where trade_date='");
        strcat(sqlexec, daily_trade_info[date][TRADE_DATE_POS]);
        strcat(sqlexec,"'");

        if(SAS_DB_OK != mysql_exe_select(mysql, sqlexec, &rs))
            return SAS_ERROR;

        int daily_total = atoi((mysql_fetch_row(rs))[0]);
        mysql_free_result(rs);

        memset(sqlexec, 0, sizeof sqlexec);
        strcat(sqlexec, "select count(*) from ");
        strcat(sqlexec, src_table);
        strcat(sqlexec, " where trade_date='");
        strcat(sqlexec, daily_trade_info[date][TRADE_DATE_POS]);
        strcat(sqlexec,"' and ma5 <= close_price;");
        
        strcat(sqlexec, "select count(*) from ");
        strcat(sqlexec, src_table);
        strcat(sqlexec, " where trade_date='");
        strcat(sqlexec, daily_trade_info[date][TRADE_DATE_POS]);
        strcat(sqlexec,"' and ma10 <= close_price;");

        strcat(sqlexec, "select count(*) from ");
        strcat(sqlexec, src_table);
        strcat(sqlexec, " where trade_date='");
        strcat(sqlexec, daily_trade_info[date][TRADE_DATE_POS]);
        strcat(sqlexec,"' and ma20 <= close_price;");


        mysql_set_server_option(mysql, MYSQL_OPTION_MULTI_STATEMENTS_ON);
        if (mysql_query(mysql, sqlexec)!=0)
        {
            printf( "error at %s,%d:%s", __FILE__, __LINE__,mysql_error(mysql));
            exit(0);
        }

        int res_next = 0;
        int ma_id = 0;
        char cur_ma_str[MAX_MA_NUM][MAX_FILED_STR_LEN];
        double cur_ma[MAX_MA_NUM];
        memset(cur_ma_str, 0, sizeof cur_ma_str);
        do
        {
            rs = mysql_store_result(mysql);
            if(rs)
            {
                while(NULL != (row = mysql_fetch_row(rs)))
                {
                    if(NULL != row[0])
                    {
                        //strcpy(cur_ma_str[ma_id],row[0]);
                        cur_ma[ma_id] = atof(row[0]);
                    }
                    else
                    {
                        printf("\ninfo at %s,%d,get MA info failed!", __FILE__, __LINE__ );
                        exit(0);
                    }
                }
                mysql_free_result(rs);
            }
            ma_id++;
            res_next = mysql_next_result(mysql);
        }while(res_next == 0);

        if(0 == date)
            printf("%8s: %6.2f%%", daily_trade_info[date][TRADE_DATE_POS], atof(daily_trade_info[date][3]));
        else
            printf("\n%8s: %6.2f%%", daily_trade_info[date][TRADE_DATE_POS], atof(daily_trade_info[date][3]));

        //printf(" %6.2f   %6.2f  %6.2f", cur_ma[0], cur_ma[1], cur_ma[2]);
        printf("   %6.2f  %6.2f  %6.2f", 100*cur_ma[0]/daily_total, 100*cur_ma[1]/daily_total, 100*cur_ma[2]/daily_total);
        fflush(stdout);

    }

    return SAS_OK;
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
    if(SAS_DB_OK != mysql_exe_select(mysql, sql, &rs))
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

        if(SAS_DB_OK != mysql_exe_select(mysql, sql, &rs))
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

        if(SAS_DB_OK != mysql_exe_select(mysql, sql, &rs))
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
