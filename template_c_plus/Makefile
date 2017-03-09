# Makefile 

PACKAGE = aco
SRCS    = $(PACKAGE).cpp
# HEADS = $(PACKAGE).h
OBJS    = $(SRCS:.cpp=.o)

FILES   = $(SRCS) 
VER     = `date +%Y%m%d`

### command and flags ###
# uncomment when debugging
#DEBUG  = -ggdb -pg # -lefence

# common (*.o)
LD      = g++
LDFLAGS = -g $(DEBUG)
LDLIBS  = -lm

# C (*.c)
CC      = gcc
CFLAGS  = -g -O2 -Wall $(DEBUG)
CPPFLAGS= -I.

# command
#RM = rm

### rules ###
.SUFFIXES:
.SUFFIXES: .o .c .cc .cpp .f .p

all: $(PACKAGE)

$(PACKAGE): $(OBJS)
	$(LD) $(LDFLAGS) $(LDLIBS) $(OBJS) -o $@

$(OBJS): $(HEADS) Makefile

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

### useful commands ###

clean:
	$(RM) $(PACKAGE) $(OBJS)
	$(RM) *~ #*#

tar:
	@echo $(PACKAGE)-$(VER) > .package
	@$(RM) -r `cat .package`
	@mkdir `cat .package`
	@ln $(FILES) `cat .package`
	tar cvf - `cat .package` | gzip -9 > `cat .package`.tar.gz
	@$(RM) -r `cat .package` .package

zip:
	zip -9 $(PACKAGE)-$(VER).zip $(FILES)


prof: run
	$(PROF) $(PACKAGE) | less

run: all
	./$(PACKAGE) --tours 1000 --ants 5 --instance eil51.tsp 

