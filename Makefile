PRINTER_SYSTEM = system_printer
PRINTER_USER = user_printer

LIBFTPRINTF_DIR = ../

SHELL = /bin/sh

# I'm not proud of this
TESTS = $(shell for ((i=1;i<=900;i++)); do echo "$$i "; done)

NAME = tester
LIBTEST = libtest/libtest.a
LIBFTPRINTF = ${LIBFTPRINTF_DIR}/libftprintf.a

LIB_DIR = ../

SRC_DIR = ./src
OBJ_DIR = ./obj

SRCS_FILES = main.c \
			 get_next_line.c \
			 get_next_line_utils.c \
			 utils.c

HEADERS_FILES = helpers.h
HEADERS = ${addprefix ${SRC_DIR}/, ${HEADERS_FILES}}

SRCS = ${addprefix ${SRC_DIR}/, ${SRCS_FILES}}

OBJS_FILES = ${SRCS_FILES:.c=.o}
OBJS = ${addprefix ${OBJ_DIR}/, ${OBJS_FILES}}

CFLAGS = -Wall -Werror -Wextra -g3

#VALGRIND = valgrind -q --leak-check=full --show-leak-kinds=all
UNAME = ${shell uname -s}
ifeq (${UNAME}, Darwin)
	SRCS_FILES := ${SRCS_FILES} malloc_count.c 
endif

CC = clang ${CFLAGS}

export LSAN_OPTIONS=exitcode=30

all: update ${NAME} run
	@echo ""

san: CFLAGS := ${CFLAGS} -fsanitize=address
san: all

${NAME}: ${LIBFTPRINTF} ${LIBTEST} ${HEADERS} ${OBJS}
	${CC} -L./libtest -L${LIBFTPRINTF_DIR} ${OBJS} -o ${NAME} -ltest -lftprintf -ldl
	mkdir -p files

${LIBFTPRINTF}:
	make -C ${LIBFTPRINTF_DIR} CFLAGS="${CFLAGS}"

${LIBTEST}:
	make -C libtest CFLAGS="${CFLAGS}"

${OBJ_DIR}/%.o: ${SRC_DIR}/%.c ${HEADERS} Makefile
	${CC} -DBUFFER_SIZE=32 -c $< -o $@

run:
	./${NAME} 2>myleaks.txt

${TESTS}: ${NAME}
	./${NAME} $@  2>myleaks.txt

update:
	git pull

clean:
	make -C ./libtest clean
	make -C ${LIBFTPRINTF_DIR} clean
	${RM} ${OBJS}

fclean: clean
	make -C ./libtest fclean
	make -C ${LIBFTPRINTF_DIR} fclean
	${RM} ${NAME}

re: fclean all

.PHONY: ${NAME} ${LIBTEST} ${LIBFTPRINTF}
