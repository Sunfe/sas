########################################################################
#by liaozhicheng@20151212
########################################################################
source(global.R);

g_stock_code      <- unique(orig_stock_info$stock_code);
max_calc_date_num <- 240;
total_stock_num   <- length(g_stock_code);
df_ma_ratio       <- data.frame(1,2,3,4);
time_start <- Sys.time();
for(i in 1:max_calc_date_num)
{
  time_start_date <- Sys.time();
  
  cur_date    <- g_trade_date[i];
  df_ma_ratio[i,1]  <- cur_date;
  
  sqlstr=paste0("select count(*) from df_basic_ma_info where trade_date ='",cur_date,"' and close_price > ma5");
  res <- sqldf(sqlstr);
  df_ma_ratio[i,2] <- res[1,1];
  if(is.na(res))
  {
    print(sqlstr);
  }
  
  sqlstr=paste0("select count(*) from df_basic_ma_info where trade_date ='",cur_date,"' and close_price > ma10");
  res <- sqldf(sqlstr);
  df_ma_ratio[i,3] <- res[1,1];
  if(is.na(res))
  {
    print(sqlstr);
  }
  
  sqlstr=paste0("select count(*) from df_basic_ma_info where trade_date ='",cur_date,"' and close_price > ma20");
  res <- sqldf(sqlstr);
  df_ma_ratio[i,4] <- res[1,1];
  
  if(is.na(res))
  {
    print(sqlstr);
  }
  
  output <- sprintf("progress:%4.2f%%,current date:%s",(i/max_calc_date_num)*100,cur_date);
  print(output);
  
  time_end_date <- Sys.time();
  print(time_end_date - time_start_date);

}
time_end <-Sys.time();
print(time_end - time_start)

names(df_ma_ratio) <- c("trade_date","ma5","ma10","ma20");
total_stock_num   <- length(g_stock_code);
ma5_info  = format(round((df_ma_ratio$ma5/total_stock_num)*100,2),nsmall = 2); 
ma10_info = format(round((df_ma_ratio$ma10/total_stock_num)*100,2),nsmall = 2); 
ma20_info = format(round((df_ma_ratio$ma20/total_stock_num)*100,2),nsmall = 2)


























