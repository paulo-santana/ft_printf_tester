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

CFLAGS = -Wall -Werror -Wextra -g3 -O0 -gdwarf-4

CC = gcc ${CFLAGS}

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
