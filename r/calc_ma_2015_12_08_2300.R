########################################################################
#by liaozhicheng@20151206
########################################################################
conn <- dbConnect(MySQL(), dbname = "stock", username="root", password="Line0907");

getMa<-function(cur_date,prev_date,stock)
{
  sqlstr<-NA;
  res<-NA;
  
  sqlstr=paste0("select avg(close_price) from orig_stock_2010_2015 where trade_date >'",prev_date,"' and trade_date <= '",cur_date,"' and stock_code='",stock,"'")
  res<-dbGetQuery(conn, sqlstr);
#   if(!dbHasCompleted(res))
#   {
#     return(-1);
#   }
  #print(res);
  names(res)<-c("avg")
  ma<-format(round(res$avg,2),nsmall = 2);    

  return(ma);
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
  prev_price <- res$close_price;

  ratio <- format(round(((cur_price-prev_price)/prev_price)*100,2),nsmall=2);

  return(ratio);
}

get_cur_close_price <- function(cur_date)
{
  sqlstr<-NA;
  res<-NA;
  
  sqlstr     <- paste0("select close_price from orig_stock_2010_2015 where trade_date ='",cur_date,"' and stock_code='",stock,"'");
  res        <- dbGetQuery(conn, sqlstr);
  names(res) <- c("close_price");
  cur_price  <- res$close_price; 
  
  return(cur_price);
}

#######################
#main
#######################

prev_date         <- NA;
g_stock_code      <- unique(orig_stock_info$stock_code);
#basic_index_info  <- subset(orig_index_info,stock_code=="000001");
#max_calc_date_num <- length(basic_index_info$trade_date);
max_calc_date_num <- 240;

# df_stock_ma_info  <- data.frame();
stock_num         <- 1;
total_stock_num   <- length(g_stock_code);
# ma5               <- rep(0,total_stock_num * max_calc_date_num);
# ma10              <- rep(0,total_stock_num * max_calc_date_num);
# ma20              <- rep(0,total_stock_num * max_calc_date_num);
stock_code        <- NA;
trade_date        <- NA;

#########################
#START
#########################
time_start <- Sys.time();
# dim(ma5)   <- c(total_stock_num , max_calc_date_num);
# dim(ma10)  <- c(total_stock_num , max_calc_date_num);
# dim(ma20)  <- c(total_stock_num , max_calc_date_num);
df_stock_ma_info=data.frame(1,2,3,4,5,6);
names(df_stock_ma_info)=c("stock","close_price","ratio","ma5","ma10","ma20");
for(stock in g_stock_code)
{
  time_start_stock <- Sys.time();
  
  stock_code[stock_num] <- stock;
  for(i in 1:max_calc_date_num)
  {
    cur_date  <- g_trade_date[i];    
    # trade_date[stock_num,i] <- cur_date;
    close_price <- get_cur_close_price(cur_date);
    
    date_offset <- 5;
    prev_date <- g_trade_date[i + date_offset];
    MA5<-getMa(cur_date,prev_date,stock);
    # ma5[stock_num,i] <-MA5;

    # MA5 <-mean(subset(basic_stock_info,trade_date>prev_date & trade_date<=cur_date & stock_code==stock)$close_price)
    # MA5 <-round(MA5,2);
    
    date_offset <- 10;    
    prev_date <- g_trade_date[i + date_offset];    
    MA10 <- getMa(cur_date,prev_date,stock);
    # ma10[stock_num,i] <- MA10;
   
    date_offset <- 20;    
    prev_date <- g_trade_date[i + date_offset];    
    MA20 <- getMa(cur_date,prev_date,stock);
    # ma20[stock_num,i] <- MA20;


#     time_end <- Sys.time();
#     print(time_end - time_start);
#     ma_info <- cbind(stock,cur_date,MA5,MA10,MA20);
#     df_stock_ma_info <-rbind(df_stock_ma_info,ma_info);
    
    date_offset <- 1;
    prev_date   <- g_trade_date[i + date_offset];
    ratio       <- get_incr_ratio(cur_date,prev_date);
    
 
    df_stock_ma_info[(stock_num-1)*max_calc_date_num + i,1] <- stock;
    df_stock_ma_info[(stock_num-1)*max_calc_date_num + i,2] <- close_price;
    df_stock_ma_info[(stock_num-1)*max_calc_date_num + i,3] <- ratio;   
    df_stock_ma_info[(stock_num-1)*max_calc_date_num + i,4] <- MA5;
    df_stock_ma_info[(stock_num-1)*max_calc_date_num + i,5] <- MA10;   
    df_stock_ma_info[(stock_num-1)*max_calc_date_num + i,6] <- MA20; 
    df_stock_ma_info
  }
  

  output <- sprintf("progress:%4.2f%%,current stock:%s",(stock_num/total_num)*100,stock);
  print(output);
  
  stock_num      <- stock_num + 1;
  time_end_stock <- Sys.time();
  print(time_end_stock - time_start_stock);
}
time_end <-Sys.time();
print(time_end - time_start)
