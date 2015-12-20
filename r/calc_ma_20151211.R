########################################################################
#by liaozhicheng@20151206
########################################################################
library(RMySQL);
options(width = 200);
conn <- dbConnect(MySQL(), dbname = "stock", username="root", password="Line0907");

ERROR <- -1;
#######################
#FUNCTIONS
#######################
get_cur_close_price <- function(cur_date)
{
  sqlstr<-NA;
  res<-NA;

  sqlstr     <- paste0("select close_price from orig_stock_2010_2015 where trade_date ='",cur_date,"' and stock_code='",stock,"'");
  res        <- dbGetQuery(conn, sqlstr);
  names(res) <- c("close_price");
  if(!is.na(res$close_price))
  {
    cur_price  <- res$close_price; 
    return(as.double(cur_price));
  }else {
    return(ERROR);
  }
}


get_prev_date <- function(cur_date,date_offset,stock)
{
  sqlstr  <- NA;
  res     <- NA;
  i       <- 0;
  cur_idx <- 0;

  cur_idx <- which(g_trade_date == cur_date);
  
#   sqlstr=paste0("select count(close_price) from orig_stock_2010_2015 where  trade_date < '",cur_date,"' and stock_code='",stock,"'");
#   res <- dbGetQuery(conn, sqlstr); 
#   total_count_from_cur_date <- res[1,1];
#   if(total_count_from_cur_date < date_offset)
#   {
#     next_prev_idx <- cur_idx + total_count_from_cur_date;
#     prev_date     <- g_trade_date[next_prev_idx];
#     return(prev_date);
#   }
  
  prev_date <- g_trade_date[cur_idx + date_offset];
  start_trade_date <- g_start_trade_date[g_start_trade_date$stock_code==stock,2]
  if(prev_date < start_trade_date)
  {
    prev_date <- start_trade_date;
    return(prev_date);
  }
  
  sqlstr=paste0("select count(close_price) from orig_stock_2010_2015 where trade_date >='",prev_date,"' and trade_date < '",cur_date,"' and stock_code='",stock,"'");
  res <- dbGetQuery(conn, sqlstr); 
#   if(res[1,1] < date_offset )
#   {
#     next_prev_idx <- cur_idx + date_offset;
#     prev_date     <- g_trade_date[next_prev_idx];
#     return(prev_date);
#   }
  while(res[1,1] != (date_offset))
  {
    i <- i + 1;
    next_prev_idx <-cur_idx + date_offset + i;
    cur_idx <- which(g_trade_date == cur_date);
    prev_date <- g_trade_date[next_prev_idx];
    sqlstr=paste0("select count(close_price) from orig_stock_2010_2015 where trade_date >='",prev_date,"' and trade_date < '",cur_date,"' and stock_code='",stock,"'");
    res <- dbGetQuery(conn, sqlstr);  
  }
  
  return(prev_date);

}


get_incr_ratio <- function(cur_date,prev_date)
{
  sqlstr<-NA;
  res<-NA;
  
  sqlstr     <- paste0("select close_price from orig_stock_2010_2015 where trade_date ='",cur_date,"' and stock_code='",stock,"'");
  res        <- dbGetQuery(conn, sqlstr);
  names(res) <- c("close_price");
  cur_price  <- res$close_price;    
  
  sqlstr     <- paste0("select close_price from orig_stock_2010_2015 where trade_date ='",prev_date,"' and stock_code='",stock,"'");
  res        <- dbGetQuery(conn, sqlstr);
  names(res) <- c("close_price");
  
  if(!is.na(res))
  {
    prev_price <- res$close_price;
    ratio <- format(round(((cur_price-prev_price)/prev_price)*100,2),nsmall=2);  
    return(as.double(ratio));
  }else {
    return(ERROR);
  }
}


getMa<-function(cur_date,prev_date,stock)
{
  sqlstr <- NA;
  res    <- NA;
  ma     <- 0;

  sqlstr=paste0("select avg(close_price) from orig_stock_2010_2015 where trade_date >'",prev_date,"' and trade_date <= '",cur_date,"' and stock_code='",stock,"'")
  res<-dbGetQuery(conn, sqlstr);
  names(res)<-c("avg")
  if(!is.na(res))
  {
    ma<-format(round(res$avg,2),nsmall = 2); 
    return(as.double(ma));
  }else{
    return(ERROR);
  }
}


is_trade_date_exists <- function(cur_date)
{
  sqlstr <- NA;
  res    <- NA;

  sqlstr=paste0("select count(close_price) from orig_stock_2010_2015 where trade_date ='",cur_date,"' and stock_code='",stock,"'")
  res <- dbGetQuery(conn, sqlstr); 
  if(res[1,1] == 0)
  {
    return(FALSE);
  }else{
    return(TRUE);
  }
}


#######################
#main
#######################
prev_date         <- NA;
g_stock_code      <- unique(orig_stock_info$stock_code);
max_calc_date_num <- 240;
stock_num         <- 1;
total_stock_num   <- length(g_stock_code);
stock_code        <- NA;
trade_date        <- NA;

#########################
#START
#########################
time_start <- Sys.time();

for(stock in g_stock_code)
{
  time_start_stock <- Sys.time();
  
  df_basic_stock_info=data.frame(1,2,3,4,5,6,7);
  names(df_basic_stock_info)=c("stock","trade_date","close_price","ratio","ma5","ma10","ma20");
  
  time_start_stock0 <- Sys.time();
  
  trade_date_idx <- 1;
  for(i in 1:max_calc_date_num)
  {
    
    cur_date    <- g_trade_date[i];
    if (!is_trade_date_exists(cur_date))
      next;
    
    # print(stock);
    
    close_price <- get_cur_close_price(cur_date);
    # print(close_price);
    
    # time_start_stock <- Sys.time();
    
    date_offset <- 5;
    prev_date   <- get_prev_date(cur_date,date_offset,stock);
    MA5 <- getMa(cur_date,prev_date,stock);
    # print(MA5);
    
#     
#     time_end_stock <- Sys.time();
#     print(time_end_stock - time_start_stock);
#     
#     
#     time_start_stock <- Sys.time();
    
    date_offset <- 10;
    prev_date   <- get_prev_date(cur_date,date_offset,stock);
    MA10        <- getMa(cur_date,prev_date,stock);
    # print(MA10);
#     
#     time_end_stock <- Sys.time();
#     print(time_end_stock - time_start_stock);
#     
#     
#     time_start_stock <- Sys.time();
    date_offset <- 20;
    prev_date   <- get_prev_date(cur_date,date_offset,stock);
    MA20        <- getMa(cur_date,prev_date,stock);
    # print(MA20);
#     
#     time_end_stock <- Sys.time();
#     print(time_end_stock - time_start_stock);
#     time_start_stock <- Sys.time();
       
    date_offset <- 1;
    prev_date   <- get_prev_date(cur_date,date_offset,stock);
    ratio       <- get_incr_ratio(cur_date,prev_date);
    # print(ratio);
    
    
#     time_end_stock <- Sys.time();
#     print(time_end_stock - time_start_stock);
#     
#     time_start_stock <- Sys.time();
    
    df_basic_stock_info[trade_date_idx,1] <- stock;
    df_basic_stock_info[trade_date_idx,2] <- cur_date;    
    df_basic_stock_info[trade_date_idx,3] <- close_price;
    df_basic_stock_info[trade_date_idx,4] <- ratio;   
    df_basic_stock_info[trade_date_idx,5] <- MA5;
    df_basic_stock_info[trade_date_idx,6] <- MA10;   
    df_basic_stock_info[trade_date_idx,7] <- MA20; 
    trade_date_idx <- trade_date_idx + 1;
    #print(df_basic_stock_info);

#     time_end_stock <- Sys.time();
#     print(time_end_stock - time_start_stock);
    
  }

  dbWriteTable(conn, "basic_stock_2010_2015" ,df_basic_stock_info, row.names = F,append=TRUE);

  output <- sprintf("progress:%4.2f%%,current stock:%s",(stock_num/total_stock_num)*100,stock);
  print(output);
  
  stock_num      <- stock_num + 1;
  time_end_stock <- Sys.time();
  print(time_end_stock - time_start_stock);
}
time_end <-Sys.time();
print(time_end - time_start)
