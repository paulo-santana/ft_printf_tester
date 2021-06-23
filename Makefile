PRINTER_SYSTEM = system_printer
PRINTER_USER = user_printer

LIBFTPRINTF_DIR = ../

NAME = tester
LIBTEST = libtest/libtest.a
LIBFTPRINTF = ${LIBFTPRINTF_DIR}/libftprintf.a

LIB_DIR = ../

GNL_FILES = get_next_line/get_next_line.c \
			get_next_line/get_next_line_utils.c \
			get_next_line/get_next_line.h

SRC_DIR = ./src
OBJ_DIR = ./obj

SRCS_FILES = main.c
SRCS = ${addprefix ${SRC_DIR}/, ${SRCS_FILES}}

OBJS_FILES = ${SRCS_FILES:.c=.o}
OBJS = ${addprefix ${OBJ_DIR}/, ${OBJS_FILES}}

CFLAGS = -Wall -Werror -Wextra -g3

CC = gcc ${CFLAGS}

STRINGS = 1 2 3 4 5 6 7 8 
STRINGS_FLAGS = 9 10 11 12 13 #14 15 16 17 18 19 20 21 22 23 24 25 26

all: ${NAME} run
	@echo ""

${NAME}: ${LIBFTPRINTF} ${LIBTEST} ${OBJS}
	${CC} -L./libtest -L${LIBFTPRINTF_DIR} ${OBJS} -DBUFFER_SIZE=32 ${GNL_FILES} -o ${NAME} -ltest -lftprintf

${LIBFTPRINTF}:
	make -C ${LIBFTPRINTF_DIR}

${LIBTEST}:
	make -C libtest

${OBJ_DIR}/%.o: ${SRC_DIR}/%.c
	${CC} -c $< -o $@

run: #${STRINGS} ${STRINGS_FLAGS}
	./${NAME}

${STRINGS}: DESCRIBE_STRING
	@./${NAME} $@

${STRINGS_FLAGS}: DESCRIBE_STRING_FLAGS
	@./${NAME} $@

DESCRIBE_STRING:
	@echo ""
	@./${NAME} describe "Testing with strings"

DESCRIBE_STRING_FLAGS:
	@echo ""
	@./${NAME} describe "Testing strings with flags"

clean:
	make -C ./libtest clean
	make -C ${LIBFTPRINTF_DIR} clean
	${RM} ${OBJS}

fclean:
	make -C ./libtest fclean
	make -C ${LIBFTPRINTF_DIR} fclean
	${RM} ${NAME}

re: fclean all

.PHONY: ${NAME} ${LIBTEST} ${LIBFTPRINTF}
