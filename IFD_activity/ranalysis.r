library(reshape2)
library(ggplot2)
setwd("C:/Users/P285100/Desktop/IFD_activity/IFD_activity")
data <- read.table("activity-evol2.txt", sep="\t", header = F)

df <- (t(subset(data, select = -c(V1, V1002))))
colnames(df) <- data$V1

length(df[1,])

minwv = 0.0;     # minimal (weight) value
maxwv = 1.0;     # maximal (weight) value
steps = 101;  # num. of bins across range
stepsize = (maxwv - minwv)/steps  # bin range size


#Frequency matrix:

mtrxwP1 <- matrix(nrow = steps, ncol = length(df[1,]), dimnames=list(seq(minwv+stepsize, maxwv, stepsize)))  # Frequency matrix


start.time <- Sys.time()
for (i in 1:length(df[1,])){
  #Weights
  mtrxwP1[,i] = table(cut(df[,i], seq(minwv, maxwv, stepsize), right=T))/1000
}

colnames(mtrxwP1) <- data$V1


P2P <- ggplot(data = melt(t(mtrxwP1)), aes(x=Var1, y=Var2, fill=value)) + labs(x="", y="activity") + 
  geom_tile() + scale_fill_gradientn(colours = colorRampPalette(c("white", "red", "blue"))(3), 
                                     values = c(0, 0.3 , 1), space = "Lab", guide = FALSE) + geom_hline(yintercept = 0)+ theme_bw() +
  theme(axis.title.x=element_text(size=14), axis.title.y=element_text(size=12), panel.grid.major = element_blank(), panel.grid.minor = element_blank(),axis.line = element_line(colour = "black"), legend.position = "none")

P2P

plot(rowMeans(df) ~ data$V1)


#####
# IFD measures

idf_data <- read.table("IDF.txt", header = T)

plot(idf_data$G, idf_data$avg_ttifd)




