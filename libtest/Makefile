NAME = libtest.a

SRCS = string.c \
	   print_errors.c \
	   diff.c

OBJS = ${SRCS:.c=.o}

AR = ar rcs
CFLAGS = -Wall -Werror -Wextra
CC = clang ${CFLAGS}

all: ${NAME}

${NAME}: ${OBJS}
	${AR} ${NAME} ${OBJS}

%.o: %.c
	${CC} -c $< -o $@ 

clean:
	${RM} ${OBJS}

fclean: clean
	${RM} ${NAME}

re: fclean all
