LIBFTPRINTF_DIR = ../
ERROR_LIMIT = 0

SHELL = /bin/sh

# I'm not proud of this
TESTS = $(shell for ((i=1;i<=2000;i++)); do echo "$$i "; done)

NAME = tester
LIBTEST = libtest/libtest.a
LIBFTPRINTF = ${LIBFTPRINTF_DIR}/libftprintf.a

SRC_DIR = ./src
OBJ_DIR = ./obj

SRCS_FILES = main.c \
			 tests.c \
			 get_next_line.c \
			 get_next_line_utils.c \
			 utils.c

HEADERS_FILES = helpers.h
HEADERS = ${addprefix ${SRC_DIR}/, ${HEADERS_FILES}}

SRCS = ${addprefix ${SRC_DIR}/, ${SRCS_FILES}}

OBJS_FILES = ${SRCS_FILES:.c=.o}
OBJS = ${addprefix ${OBJ_DIR}/, ${OBJS_FILES}}

CFLAGS = -Wall -Wextra -g3

PRINTF_FLAGS = ${CFLAGS} -Werror

SANITIZE = -fsanitize=address

UNAME = ${shell uname -s}
ifeq (${UNAME}, Darwin)
	SRCS_FILES := ${SRCS_FILES} malloc_count.c 
endif

CC = clang ${CFLAGS}

export LSAN_OPTIONS=exitcode=30

all: ${NAME}
	@echo ""

nosan: SANITIZE :=
nosan: ${NAME}
	@echo ""

${NAME}: ${LIBFTPRINTF} ${LIBTEST} ${HEADERS} ${OBJS}
	${CC} ${SANITIZE} -L./libtest -L${LIBFTPRINTF_DIR} ${OBJS} -o ${NAME} -ltest -lftprintf -ldl
	mkdir -p files

${LIBFTPRINTF}:
	make -C ${LIBFTPRINTF_DIR}

${LIBTEST}:
	make -C libtest CFLAGS="${CFLAGS}"

${OBJ_DIR}/%.o: ${SRC_DIR}/%.c ${HEADERS} Makefile
	${CC} -DERROR_LIMIT=${ERROR_LIMIT} -DBUFFER_SIZE=32 -c $< -o $@

r: run
run:
	./${NAME} 2>myleaks.txt

${TESTS}: SANITIZE := -fsanitize=address
${TESTS}: ${NAME}
	./${NAME} $@ 2>myle

_bonus:
	make CFLAGS="${PRINTF_FLAGS}" -C ${LIBFTPRINTF_DIR} bonus

bonus: _bonus ${NAME}

bonusnosan: SANITIZE :=
bonusnosan: _bonus ${NAME}

push:
	git add -A
	git commit -m "chore: automated commit"
	git push

clean:
	@echo cleaning...
	@make -C ./libtest clean
	make -C ${LIBFTPRINTF_DIR} clean
	@${RM} ${OBJS}

fclean: clean
	@make -C ./libtest fclean
	make -C ${LIBFTPRINTF_DIR} fclean
	@${RM} ${NAME}

re: fclean all

.PHONY: ${NAME} ${LIBTEST} ${LIBFTPRINTF}
