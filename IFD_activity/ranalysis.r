library(reshape2)
library(ggplot2)
setwd("C:/Users/P285100/Desktop/IFD_activity/IFD_activity")

str1 <- "01_6-11activities"
data <- read.table(paste0(str1, ".txt"), sep="\t", header = F)

df <- (t(subset(data, select = -c(V1, V1002))))
colnames(df) <- data$V1

length(df[1,])

minwv = 0.0;     # minimal (weight) value
maxwv = 0.5;     # maximal (weight) value
steps = 101;  # num. of bins across range
stepsize = (maxwv - minwv)/steps  # bin range size


#Frequency matrix:

mtrxwP1 <- matrix(nrow = steps, ncol = length(df[1,]), dimnames=list(seq(minwv+stepsize, maxwv, stepsize)))  # Frequency matrix
vecFpos <- vector()

for (i in 1:length(df[1,])){
  
  vecFpos[i] = length(which(df[,i] > 0.05))/length(df[,1])
  
  mtrxwP1[,i] = table(cut(df[,i], seq(minwv, maxwv, stepsize), right=T))/1000
  
}

colnames(mtrxwP1) <- data$V1


P2P <- ggplot(data = melt(t(mtrxwP1)), aes(x=Var1, y=Var2, fill=value)) + labs(x="generations", y="activity") + 
  geom_tile() + scale_fill_gradientn(colours = colorRampPalette(c("white", "red", "blue"))(3), 
                                     values = c(0, 0.3 , 1), space = "Lab", guide = FALSE) + geom_hline(yintercept = 0)+ theme_bw() +
  theme(axis.title.x=element_text(size=14), axis.title.y=element_text(size=12), panel.grid.major = element_blank(), panel.grid.minor = element_blank(),axis.line = element_line(colour = "black"), legend.position = "none")

P2P

ggsave(paste0(str1, ".png"), P2P)




#frequencies of morphs, custom
df_H = data.frame(data$V1, vecFpos, 1 - vecFpos)
colnames(df_H) <- c("gens", "highact", "lowact")

frequencies <- ggplot(data=df_H, aes(x=gens, y=highact, colour="weight > 0")) +
  geom_line(colour="darkgreen")  + theme(axis.title=element_text(size=12))+ 
  geom_line(data = df_H, aes(x = gens, y = lowact, colour="weight < 0"), colour="purple")  + 
  labs(x="", y="Herbivore frequency", colour="")+ theme(legend.position='none') +
  theme(panel.background = element_blank(), panel.border = element_rect(colour = "black", fill=NA),  
        axis.line = element_line(colour = "black"), axis.title.y = element_text(size = 9), axis.text=element_text(size=8))


#####
# IFD measures

idf_data <- read.table("IDF.txt", header = T)

plot(idf_data$G, idf_data$avg_ttifd)


###########
library(ggplot2)

df1 <-read.table("17-11-1ifd.txt", header=T)
#View(df1)


ggplot(data = df1)+
  geom_jitter(mapping = aes(x = pop_size, y = time_to_IFD, shape = as.factor(act), color = as.factor(act)))



p <- ggplot(data = df1[df1$pop_size==50,], aes(x = time_to_IFD, color = as.factor(act), linetype = as.factor(pop_size)))+
  geom_density()

df1 <- df1[df1$act>0.05,]

p <- ggplot(data = df1[df1$pop_size<2000 & df1$pop_size != 400,], aes(x = time_to_IFD, color = as.factor(act)))+
  geom_density(key_glyph = draw_key_path) + facet_wrap(~pop_size, scales = "free_y") + 
  ylab("Density") + xlab("Time-to-IFD") + guides(colour = guide_legend(override.aes = list(size = 6)))+ theme(legend.key = element_rect(fill = NA))+
  scale_color_discrete( name = "activity", labels = paste(as.character(0.5 - c(0.4, 0.3, 0.2, 0.1, 0.0)), "||", as.character(0.5+c(0.4, 0.3, 0.2, 0.1, 0.0))))


ggsave("dist_time-to-IFD.png", p)

######
df2 <-read.table("3-1_contin_ifd.txt", header=T)

View(df1)
length(df1$act)
df2 <- df2[df2$act>0.05,]

# p <- ggplot(data = df1[df1$pop_size<2000,], aes(x = time, y = ifd_prop, color = as.factor(act)))+
#   geom_path(aes(group = interaction(iter, act)))+facet_wrap(~pop_size, scales = "free")+
#   coord_cartesian(xlim = c(0, 100))+ ylab("Proportion IDF")+ 
#   scale_color_discrete(name = "activity", labels = paste(as.character(c(0.1, 0.2, 0.3, 0.4, 0.5)), "||", as.character(1-c(0.1, 0.2, 0.3, 0.4, 0.5))))
# 
# p <- ggplot(data = df1[df1$pop_size==4000,], aes(x = time, y = ifd_prop, color = as.factor(act)))+
#   geom_path(aes(group = interaction(iter, act)))+facet_wrap(~act, scales = "free")+
#   coord_cartesian(xlim = c(0, 0.1), ylim = c(0, 0.4))+ylab("Proportion IDF")+ 
#   scale_color_discrete(name = "activity", labels = paste(as.character(c(0.05, 0.1, 0.2, 0.3, 0.4, 0.5)), "||", as.character(1-c(0.05, 0.1, 0.2, 0.3, 0.4, 0.5))))
# 



p2 <- ggplot(data = df2[df2$pop_size<2000 & df2$pop_size != 400,], aes(x = time, y = ifd_prop, color = as.factor(act)))+
  geom_path(aes(group = interaction(iter, act)))+facet_wrap(~pop_size, scales = "free")+
  ylab("Proportion IDF") + xlab("Time")+ theme(legend.position = "bottom") + 
  scale_color_discrete(name = "activity", labels = paste(as.character(0.5 - c(0.4, 0.3, 0.2, 0.1, 0.0)), "||", as.character(0.5+c(0.4, 0.3, 0.2, 0.1, 0.0))))




Combined_P <- ggarrange(p, p2, ncol = 1, common.legend = T, legend="bottom", labels = c("A", "B"))
ggsave("twomorphs_betterlandscapeifd_contin.png", p)

ggsave("Combined.png", Combined_P)



## Variance

p2 <- ggplot(data = df2[df2$pop_size<2000 & df2$pop_size != 200,], aes(x = time, y = stddev, color = as.factor(act)))+
  geom_path(aes(group = interaction(iter, act)))+facet_wrap(~pop_size)+
  ylab("SD intake rates") + xlab("Time")+ theme(legend.position = "bottom") + 
  scale_color_discrete(name = "activity", labels = paste(as.character(0.5 - c(0.4, 0.3, 0.2, 0.1, 0.0)), "||", as.character(0.5+c(0.4, 0.3, 0.2, 0.1, 0.0))))




