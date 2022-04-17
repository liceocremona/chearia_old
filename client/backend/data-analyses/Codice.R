#install.packages("rjson")
library("rjson")
setwd("/cloud/project")

url <- 'https://olimpiadirobotica-2021-default-rtdb.europe-west1.firebasedatabase.app/db1.json'
db1 <- fromJSON(url)
View(db1)

L <- length(as.numeric(db1$humidity$data$`22-03-2022`))
humidity <- as.numeric(db1$humidity$data$`22-03-2022`)
temperature <- as.numeric(db1$temperature$data$`22-03-2022`)
CO <- as.numeric(db1$CO$data$`22-03-2022`)
#ozone <- as.numeric(db1$ozone$data$`22-03-2022`)

plot(1:L, humidity, xlab = "tempo", ylab = "Umidità", type = "l", col = 2)
plot(1:L, temperature, xlab = "tempo", ylab = "Temperatura", type = "l")
plot(1:L, CO)
#plot(1:L, ozone)

data <- cbind(humidity, temperature, CO)

plot(temperature, humidity)

modello <- lm(temperature ~ humidity)
summary(modello)
