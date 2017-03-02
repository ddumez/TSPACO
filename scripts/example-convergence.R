#
# Example algorithms
#
# Change for the your filenames and the names of your tests
file.names <- c("data/convergence-algo/data-convergence-as.txt", "data/convergence-algo/data-convergence-eas.txt", "data/convergence-algo/data-convergence-ras.txt", "data/convergence-algo/data-convergence-mmas.txt", "data/convergence-algo/data-convergence-acs.txt")
test.names <- c("as", "eas", "ras", "mmas", "acs")

# Read data in a list with "names" as elements
data <- list() 
for(i in 1:length(file.names)){
  data[[test.names[i]]] <- read.table(file=file.names[i], header=TRUE, sep=":")
}


source("R-scripts/convergence.R")
do.lines.plot(data)

#
# Example ants
#
# Change for the your filenames and the names of your tests
file.names <- c("data/convergence-ants/data-convergence-m5-1234.txt", "data/convergence-ants/data-convergence-m10-1234.txt", "data/convergence-ants/data-convergence-m20-1234.txt", "data/convergence-ants/data-convergence-m50-1234.txt", "data/convergence-ants/data-convergence-m100-1234.txt")
test.names <- c("m5", "m10", "m20", "m50", "m100")

# Read data in a list with "names" as elements
data <- list() 
for(i in 1:length(file.names)){
  data[[test.names[i]]] <- read.table(file=file.names[i], header=TRUE, sep=":")
}


source("R-scripts/convergence.R")
do.lines.plot(data)
