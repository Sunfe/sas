########################################################################
#by liaozhicheng@20151206
########################################################################
source(global.R);

ERROR <- -1;
#######################
#FUNCTIONS
#######################
get_cur_close_price <- function(cur_date,stock)
{
  sqlstr<-NA;
  res<-NA;
  
  sqlstr     <- paste0("select close_price from orig_index_2010_2015 where trade_date ='",cur_date,"' and stock_code='",stock,"'");
  res        <- dbGetQuery(conn, sqlstr);
  names(res) <- c("close_price");
  if(0 != length(res$close_price))
  {
    cur_price  <- res$close_price; 
    return(as.double(cur_price));
  }else {
    return(ERROR);
  }
}


get_prev_date <- function(cur_date,date_offset,stock)
{
  cur_idx <- which(g_trade_date == cur_date);
  prev_date <- g_trade_date[cur_idx + date_offset];
  sqlstr     <- paste0("select count(*) from orig_index_2010_2015 where trade_date ='",prev_date,"' and stock_code='",stock,"'");
  res        <- dbGetQuery(conn, sqlstr);
  if(0 != res[1,1])
  {
    return(prev_date);
  }else {
    return(ERROR);
  }
}


get_incr_ratio <- function(cur_date,prev_date,stock)
{
  sqlstr<-NA;
  res<-NA;
  
  sqlstr     <- paste0("select close_price from orig_index_2010_2015 where trade_date ='",cur_date,"' and stock_code='",stock,"'");
  res        <- dbGetQuery(conn, sqlstr);
  names(res) <- c("close_price");
  cur_price  <- res$close_price;    
  
  sqlstr     <- paste0("select close_price from orig_index_2010_2015 where trade_date ='",prev_date,"' and stock_code='",stock,"'");
  res        <- dbGetQuery(conn, sqlstr);
  names(res) <- c("close_price");
  
  if(0 != length(res$close_price))
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
  
  sqlstr=paste0("select avg(close_price) from orig_index_2010_2015 where trade_date >'",prev_date,"' and trade_date <= '",cur_date,"' and stock_code='",stock,"'");
  res<-dbGetQuery(conn, sqlstr);
  names(res)<-c("avg")
  if(0 != length(res$avg))
  {
    ma<-format(round(res$avg,2),nsmall = 2); 
    return(as.double(ma));
  }else{
    return(ERROR);
  }
}


is_trade_date_exists <- function(cur_date,stock)
{
  sqlstr <- NA;
  res    <- NA;
  
  sqlstr=paste0("select count(close_price) from orig_index_2010_2015 where trade_date ='",cur_date,"' and stock_code='",stock,"'")
  # print(sqlstr);
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
g_index_code      <- NA;
max_calc_date_num <- 240;
index_num         <- 1;
total_index_num   <- NA;
code              <- NA;
trade_date        <- NA;

#########################
#START
#########################
time_start <- Sys.time();
# sqlstr=paste0("select * from orig_index_2010_2015");
# orig_index_info  <- dbGetQuery(conn, sqlstr);
g_index_code     <- unique(orig_index_info$stock_code);
total_index_num  <- length(g_index_code);

for(code in g_index_code)
{
  
  time_start_stock <- Sys.time();
  
  df_basic_index_info=data.frame(1,2,3,4,5,6,7);
  names(df_basic_index_info)=c("index_code","trade_date","close_price","ratio","ma5","ma10","ma20");
  
  time_start_stock0 <- Sys.time();
  
  trade_date_idx <- 1;
  for(i in 1:max_calc_date_num)
  {
    
    cur_date    <- g_trade_date[i];
    # print(cur_date)
    if (!is_trade_date_exists(cur_date,code))
      next;
    
    close_price <- get_cur_close_price(cur_date,code);
    # print(close_price)
    
    date_offset <- 1;
    prev_date   <- get_prev_date(cur_date,date_offset,code);
    if(prev_date == ERROR)
      next;

    ratio       <- get_incr_ratio(cur_date,prev_date,code);

    date_offset <- 5;
    prev_date   <- get_prev_date(cur_date,date_offset,code);
    MA5 <- getMa(cur_date,prev_date,code);

    date_offset <- 10;
    prev_date   <- get_prev_date(cur_date,date_offset,code);
    MA10        <- getMa(cur_date,prev_date,code);
    

    date_offset <- 20;
    prev_date   <- get_prev_date(cur_date,date_offset,code);
    MA20        <- getMa(cur_date,prev_date,code);

    df_basic_index_info[trade_date_idx,1] <- code;
    df_basic_index_info[trade_date_idx,2] <- cur_date;    
    df_basic_index_info[trade_date_idx,3] <- close_price;
    df_basic_index_info[trade_date_idx,4] <- ratio;   
    df_basic_index_info[trade_date_idx,5] <- MA5;
    df_basic_index_info[trade_date_idx,6] <- MA10;   
    df_basic_index_info[trade_date_idx,7] <- MA20; 
    trade_date_idx <- trade_date_idx + 1;
    
  }
  
  dbWriteTable(conn, "basic_index_2010_2015" ,df_basic_index_info, row.names = F,append=TRUE);
  
  output <- sprintf("progress:%4.2f%%,current code:%s",(index_num/total_index_num)*100,code);
  print(output);
  
  index_num      <- index_num + 1;
  time_end_stock <- Sys.time();
  print(time_end_stock - time_start_stock);

}
time_end <-Sys.time();
print(time_end - time_start)


time_start <- Sys.time();
sqlstr=paste0("select * from basic_index_2010_2015");
res<-dbGetQuery(conn, sqlstr);
time_end <-Sys.time();
print(time_end - time_start)

