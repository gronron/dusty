CPP		=	g++
AR		=	ar rcs
CP		=	cp -r
RM		=	rm -rf
FIND	=	find
CFLAGS	=	-O2 -W -Wall
NAME	=	libstream.a
SRC		=	ioxstream.cpp \
			fstream.cpp \
			tcpstream.cpp \
			spipe.cpp
OBJ		=	$(SRC:.cpp=.o)

all: $(OBJ)
	$(AR) $(NAME) $(OBJ)

$(OBJ):
	$(CPP) -c $(CFLAGS) $(SRC)

clean:
	$(RM) $(OBJ) ; $(FIND) ./ -name '*~' -delete

fclean: clean
	$(RM) $(NAME)

re: fclean all
