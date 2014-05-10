#Makefile
#________
NAME= minishell
SRC=  myminishell.c history.c
RM= rm -f
LIBPATH = ../lib

#LIBS= -lmy
#TESTNAME= test
#TESTSRC=testmy.c
#TESTOBJ=$(TESTSRC:.c=.o)

#target: dependencies
#--tab---command
#--tab---command
#--tab---command
#must have all, clean, fclean, and re
#all: makes "name"
#clean: clean up files
#fclean: clean + delete "name"
#re: fclean + all
all: $(OBJ)
	cc -L$(LIBPATH) $(SRC) -lmy -lncurses -o $(NAME)
 
clean:
	-$(RM) *.o
	-$(RM) *~
	-$(RM) \#*
	-$(RM) *.swp
	-$(RM) *.core
fclean: clean
	-$(RM) $(NAME)
re: fclean all
#test: $(TESTOBJ)
#	-L$(LIBPATH) $(TESTOBJ) $(LIBS) -o $(TESTNAME)
