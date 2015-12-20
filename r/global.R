########################################################################
#by liaozhicheng@20151206
########################################################################
library(RMySQL);
library(sqldf);

options(width = 151);
options(stringsAsFactors=F);
options(gsubfn.engine = "R");
options(sqldf.driver = "SQLite") 

conn <- dbConnect(MySQL(), dbname = "stock", username="root", password="Line0907")
