NAME=myselect
SRC=myselect.c
OBJ=$(SRC:.c=.o)
LIBPATH=../lib
RM=rm -f

myselect: $(OBJ)
	cc -L../lib $(OBJ) -lmy -lreadline -lncurses -o $(NAME)

all:myselect

clean:
	-$(RM) *.o
	-$(RM) ../*.o
	-$(RM) *~
	-$(RM) \#*
	-$(RM) *.swp
	-$(RM) *.core
fclean:clean
	-$(RM) $(NAME)
re:fclean all
