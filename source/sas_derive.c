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
#include "sas_file.h"
#include "sas_db.h"
#include "sas_shell.h"


/***********************************************
  macro definition
 ***********************************************/
#define MYSQL_CLOSE_PRICE_ROW  (6)
#define MAX_ITEM_NUM           (10)
#define MAX_TRADE_DATE         (2000)
#define MAX_FIELD_NUM          (10)
#define MAX_FILED_STR_LEN      (20)
    
/***********************************************
  datatype definition
 ***********************************************/


/***********************************************
  global variables
 ***********************************************/
const char *index_basic_table = "composite_index_2013_2015";
const char *index_derived_table = "index_info_derived_2013_2015";

const char *stock_basic_table  = "stock_trade_basic_2013_2015";
const char *stock_derived_table = "stock_info_derived_2013_2015";

#if 0
const char *index_basic_table = "composite_index_2006_2013";
const char *index_derived_table = "index_info_derived_2006_2013";

const char *stock_basic_table  = "stock_trade_basic_2006_2013";
const char *stock_derived_table = "stock_info_derived_2006_2013";
#endif


/***********************************************
  static variables 
 ***********************************************/
static char trade_date[MAX_TRADE_DATE][MAX_STR_LEN];
static char stock_basic_info[MAX_TRADE_DATE][MAX_FIELD_NUM][MAX_FILED_STR_LEN];

/***********************************************
  extern variables
 ***********************************************/
MYSQL *get_global_mysql();


/***********************************************
  extern functions 
 ***********************************************/


/***********************************************
  static functions declarations
 ***********************************************/
static int sas_create_table(MYSQL *mysql, const char *tablename);
static int construct_create_table_sql(const char *tablename, char *sqlexec);


/***********************************************
  global functions declarations
 ***********************************************/
int derive_data(int type);
int derive_stock_info(MYSQL*mysql, const char *src_table, const char *dest_table);
int get_basic_trade_info(MYSQL *mysql, const char *table_name, char *stock_code, int *total_trade_date);


/***********************************************
  global functions definition
 ***********************************************/
int sas_derive(char *arg)
{
    unsigned int type = 0;
    int idx = 0;
    
    if(!scanf_int(arg, &idx, &type))
    {
        printf("  derive <type>:0 all\n");
        return SAS_OK;
    }

    return derive_data(type);
}


int derive_data(int type)
{
    MYSQL *mysql = get_global_mysql();

    if(SAS_OK != sas_create_table(mysql, stock_derived_table))
        return SAS_ERROR;

    if(SAS_OK != derive_stock_info(mysql,stock_basic_table, stock_derived_table ))
        return SAS_ERROR;

    if(SAS_OK != sas_create_table(mysql, index_derived_table))
        return SAS_ERROR;

    printf("begin to derive data...\n");
    if(SAS_OK != derive_stock_info(mysql,index_basic_table,index_derived_table ))
        return SAS_ERROR;

    return SAS_OK;
}


int derive_stock_info(MYSQL*mysql, const char *src_table, const char *dest_table)
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

    for(stock_num = 0; stock_num < maxStockNum; stock_num++)
    {
        char sqlexec[MAX_STR_LEN];
        int date_offset = 0;
        int total_trade_date = 0;
        int date = 0;
        double close_price_cur = 0;
        double close_price_pre = 0;
        char cur_trade_date[20];
        char pre_trade_date[20];

        memset(cur_trade_date, 0, 20);
        memset(pre_trade_date, 0, 20);

        if(SAS_OK != get_basic_trade_info(mysql,src_table,stock_code[stock_num], &total_trade_date))
            return SAS_ERROR;

        for(date = 0; date < total_trade_date; date++)
        {
            strcpy(cur_trade_date, stock_basic_info[date][TRADE_DATE_POS]);
            if(!strcmp(cur_trade_date,""))
                break;

            close_price_cur = atof(stock_basic_info[date][CLOSE_PRICE_POS]);

            date_offset = 1;
            get_prev_trade_date(cur_trade_date,pre_trade_date,date_offset);

            if(date+1 < MAX_TRADE_DATE)
            {
                strcpy(pre_trade_date, stock_basic_info[date+1][TRADE_DATE_POS]);
                close_price_pre = atof(stock_basic_info[date+1][CLOSE_PRICE_POS]);
            }
            else
            {
                strcpy(pre_trade_date, stock_basic_info[date][TRADE_DATE_POS]);
                close_price_pre = atof(stock_basic_info[date][CLOSE_PRICE_POS]);
            }

            double increase_ratio = 0;
            if(0 != close_price_pre)
                increase_ratio =((close_price_cur - close_price_pre) / close_price_pre)*100;

            char close_price_cur_str[50];
            char increase_ratio_str[50];

            memset(close_price_cur_str, 0, 50);
            memset(increase_ratio_str, 0, 50);

            sprintf(increase_ratio_str, "%6.2f", increase_ratio);
            sprintf(close_price_cur_str, "%6.2f", close_price_cur);

            memset(sqlexec, 0,MAX_STR_LEN);
            strcpy(sqlexec,"insert ignore into `");
            strcat(sqlexec,dest_table);
            strcat(sqlexec,"`(`stock_code`,`trade_date`, `close_price`,`increase_ratio`)values");
            strcat(sqlexec, "('");
            strcat(sqlexec, stock_code[stock_num]);
            strcat(sqlexec, "','");
            strcat(sqlexec, cur_trade_date);
            strcat(sqlexec, "',");
            strcat(sqlexec, close_price_cur_str);
            strcat(sqlexec, ",");
            strcat(sqlexec, increase_ratio_str);
            strcat(sqlexec, ")");

            if (mysql_query(mysql, sqlexec)!=0)
            {
                printf( "error at %s,%d:insertion for stockcode %s failed,errorinfo: %s\n",
                        __FILE__, __LINE__, stock_code[stock_num],mysql_error(mysql));
                exit(0);
            }
        }

        for(date = 0; date < MAX_TRADE_DATE; date++)
        {
            strcpy(cur_trade_date, stock_basic_info[date][TRADE_DATE_POS]);
            if(!strcmp(cur_trade_date,""))
                break;

            date_offset = 5;
            get_prev_trade_date(cur_trade_date,pre_trade_date,date_offset);
            strcpy(sqlexec,"select avg(close_price) from ");
            strcat(sqlexec, dest_table);
            strcat(sqlexec, " where trade_date > '");
            strcat(sqlexec, pre_trade_date);
            strcat(sqlexec, "' and trade_date <= '");
            strcat(sqlexec, cur_trade_date);
            strcat(sqlexec, "' and stock_code='");
            strcat(sqlexec, stock_code[stock_num]);
            strcat(sqlexec, "';");

            date_offset = 10;
            get_prev_trade_date(cur_trade_date,pre_trade_date,date_offset);
            strcat(sqlexec,"select avg(close_price) from ");
            strcat(sqlexec, dest_table);
            strcat(sqlexec, " where trade_date > '");
            strcat(sqlexec, pre_trade_date);
            strcat(sqlexec, "' and trade_date <= '");
            strcat(sqlexec, cur_trade_date);
            strcat(sqlexec, "' and stock_code='");
            strcat(sqlexec, stock_code[stock_num]);
            strcat(sqlexec, "';");

            date_offset = 20;
            get_prev_trade_date(cur_trade_date,pre_trade_date,date_offset);
            strcat(sqlexec,"select avg(close_price) from ");
            strcat(sqlexec, dest_table);
            strcat(sqlexec, " where trade_date > '");
            strcat(sqlexec, pre_trade_date);
            strcat(sqlexec, "' and trade_date <= '");
            strcat(sqlexec, cur_trade_date);
            strcat(sqlexec, "' and stock_code='");
            strcat(sqlexec, stock_code[stock_num]);
            strcat(sqlexec, "';");

            mysql_set_server_option(mysql, MYSQL_OPTION_MULTI_STATEMENTS_ON);
            if (mysql_query(mysql, sqlexec)!=0)
            {
                printf( "error at %s,%d:%s", __FILE__, __LINE__,mysql_error(mysql));
                exit(0);
            }

            int res_next = 0;
            int ma_index = 0;
            char cur_ma[MAX_MA_NUM][MAX_FILED_STR_LEN];
            memset(cur_ma, 0, sizeof cur_ma);
            do
            {
                rs = mysql_store_result(mysql);
                if(rs)
                {
                    while(NULL != (row = mysql_fetch_row(rs)))
                    {
                        if(NULL != row[0])
                            strcpy(cur_ma[ma_index],row[0]);
                        else
                            strcpy(cur_ma[ma_index],"0.0");
                        break;
                    }
                    mysql_free_result(rs);
                }
                ma_index++;
                res_next = mysql_next_result(mysql);
            }while(res_next == 0);

            memset(sqlexec, 0,MAX_STR_LEN);
            strcpy(sqlexec,"update ");
            strcat(sqlexec,dest_table);
            strcat(sqlexec," set ma5 = ");
            strcat(sqlexec,cur_ma[0]);
            strcat(sqlexec,", ma10 = ");
            strcat(sqlexec,cur_ma[1]);
            strcat(sqlexec,",ma20 = ");
            strcat(sqlexec,cur_ma[2]);
            strcat(sqlexec," where stock_code='");
            strcat(sqlexec,stock_code[stock_num]);
            strcat(sqlexec,"' and trade_date = '");
            strcat(sqlexec,cur_trade_date);
            strcat(sqlexec,"'");

            if (mysql_query(mysql, sqlexec)!=0)
            {
                printf( "error at %s,%d:update for stockcode %s failed,errorinfo: %s\n",
                        __FILE__, __LINE__, stock_code[stock_num],mysql_error(mysql));
                exit(0);
            }

        }

        fflush(stdout);
        printf("\n%s done!",stock_code[stock_num]);
    }

    return SAS_OK;
}


int  get_prev_trade_date(char *cur_trade_date, char* pre_trade_date, int date_offset)
{
    if(NULL == cur_trade_date)
        return SAS_ERROR;

    int date = 0;
    for(; date < MAX_TRADE_DATE; date++)
    {
        if(!strcmp(cur_trade_date, trade_date[date]))
            break;
    }

    if(date + date_offset < MAX_TRADE_DATE)
        strcpy(pre_trade_date, trade_date[date+date_offset] );
    else
        strcpy(pre_trade_date, trade_date[MAX_TRADE_DATE-1] );

    return SAS_OK;
}


int get_date_trade_price(MYSQL *mysql, char *table_name, char *stock_code, char *date, double* price)
{
    char sql[MAX_STR_LEN];
    memset(sql, 0, MAX_STR_LEN);

    strcpy(sql, "select close_price from ");
    strcat(sql, table_name);
    strcat(sql, " where stock_code='");
    strcat(sql, stock_code);
    strcat(sql, "'");
    strcat(sql, " and trade_date='");
    strcat(sql, date);
    strcat(sql, "'");

    MYSQL_RES *rs = NULL;
    if(SAS_DB_OK != mysql_exe_select(mysql, sql, &rs))
    {
        printf( "error at %s,%d:mysql_exe_select failed.\n",__FILE__, __LINE__);
        return SAS_ERROR;
    }

    MYSQL_ROW row;
    memset(&row, 0, sizeof row);
    while( NULL != (row= mysql_fetch_row(rs)))
    {
        *price = atof(row[0]);
    }

    return SAS_OK;
}


 int get_basic_trade_info(MYSQL *mysql,const char *table_name,  char *stock_code, int *total_trade_date)
 {
     MYSQL_RES *rs = NULL;
     if(SAS_OK != select_basic_stock_info(mysql,table_name, stock_code, &rs) )
         return SAS_ERROR;

     MYSQL_ROW row;
     int row_num = 0;
     memset(&row, 0, sizeof row);
     memset(stock_basic_info,  0,  MAX_TRADE_DATE * MAX_FIELD_NUM * MAX_FIELD_NUM);
     while(NULL != (row = mysql_fetch_row(rs)))
     {
         int fields_num=0;
         int fields_num_total = mysql_num_fields(rs);
         while(fields_num < fields_num_total)
         {
             strcpy(stock_basic_info[row_num][fields_num], row[fields_num]);
             fields_num++;
         }
         row_num++;
     }

     *total_trade_date = row_num;

     return SAS_OK;
 }


int select_basic_stock_info(MYSQL*mysql,const char *table_name, char *stock_code, MYSQL_RES **rs)
{
    char sqlexec[MAX_STR_LEN];
    memset(sqlexec, 0,MAX_STR_LEN);

    strcpy(sqlexec,"select * from ");
    strcat(sqlexec,table_name);
    strcat(sqlexec," where stock_code ='");
    strcat(sqlexec,stock_code);
    strcat(sqlexec,"' order by trade_date DESC");

    if(SAS_DB_OK != mysql_exe_select(mysql, sqlexec, rs))
    {
        printf( "error at %s,%d:mysql_exe_select for stockcode %s failed.\n",__FILE__,
                __LINE__,stock_code);
        return SAS_ERROR;
    }

    return SAS_OK;
}


/***********************************************
  static functions definition
 ***********************************************/
static int sas_create_table(MYSQL *mysql, const char *tablename)
{
    char sqlexec[MAX_STR_LEN];
    memset(sqlexec, 0, MAX_STR_LEN);

    construct_create_table_sql(tablename, sqlexec);

    if(SAS_DB_OK != mysql_create_table(mysql,tablename,sqlexec))
        return SAS_ERROR;

    return SAS_OK;
}


static int construct_create_table_sql(const char *tablename, char *sqlexec)
{
    char sql[MAX_STR_LEN];
    memset(sql, 0, MAX_STR_LEN);

    strcpy(sql, "CREATE TABLE ");
    strcat(sql, tablename);
    strcat(sql, "(");
    const char *str=
        " stock_code varchar(10) NOT NULL," \
        " trade_date varchar(10) NOT NULL," \
        " close_price float(7,2)  NULL," \
        " increase_ratio float(7,2) NULL," \
        " MA5 float(7,2) NULL," \
        " MA10 float(7,2) NULL," \
        " MA20 float(7,2) NULL," \
        " PRIMARY KEY (`stock_code`, `trade_date`)" \
        " ) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=GB2312";
    strcat(sql, str);
    strcpy(sqlexec, sql);

    return SAS_OK; 
}

