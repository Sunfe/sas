########################################################################
#by liaozhicheng@20151206
########################################################################

get_start_date <- function()
{
  stocks          <- NA;
  trade_date      <- NA;
  num             <- 1;
  total_stock_num <- length(g_stock_code);
  sqlstr          <- NA;
  res             <- NA;

  for(stock in g_stock_code)
  {
    stocks[num]     <- stock;
    sqlstr=paste0("select min(trade_date) from orig_stock_2010_2015 where stock_code='",stock,"'");
    res <- dbGetQuery(conn, sqlstr); 
    trade_date[num] <- res[1,1];

    num             <- num + 1;
  }

  return(cbind(stocks,trade_date));
}


#################
# main
#################
time_start <- Sys.time();
g_start_trade_date <- data.frame(get_start_date(),stringsAsFactors=FALSE);
names(g_start_trade_date) <- c("stock_code","trade_date")
time_end <-Sys.time();
print(time_end - time_start)


