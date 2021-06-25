PRINTER_SYSTEM = system_printer
PRINTER_USER = user_printer

LIBFTPRINTF_DIR = ../

# I'm not proud of this
TESTS = $(shell for ((i=1;i<=200;i++)); do echo "$$i "; done)

NAME = tester
LIBTEST = libtest/libtest.a
LIBFTPRINTF = ${LIBFTPRINTF_DIR}/libftprintf.a

LIB_DIR = ../

# Uncomment the line bellow to run the program under valgrind's monitoring
#VALGRIND = valgrind -q --leak-check=full --show-leak-kinds=all --suppressions=./tester.supp

SRC_DIR = ./src
OBJ_DIR = ./obj

SRCS_FILES = main.c \
			 get_next_line.c \
			 get_next_line_utils.c \
			 malloc_count.c

SRCS = ${addprefix ${SRC_DIR}/, ${SRCS_FILES}}

OBJS_FILES = ${SRCS_FILES:.c=.o}
OBJS = ${addprefix ${OBJ_DIR}/, ${OBJS_FILES}}

CFLAGS = -Wall -Werror -Wextra -g3 #-fsanitize=address

CC = clang ${CFLAGS}

all: ${NAME} run
	@echo ""

${NAME}: ${LIBFTPRINTF} ${LIBTEST} ${OBJS}
	${CC} -L./libtest -L${LIBFTPRINTF_DIR} ${OBJS} -o ${NAME} -ltest -lftprintf -ldl
	mkdir -p files

${LIBFTPRINTF}:
	make -C ${LIBFTPRINTF_DIR}

${LIBTEST}:
	make -C libtest

${OBJ_DIR}/%.o: ${SRC_DIR}/%.c
	${CC} -DBUFFER_SIZE=32 -c $< -o $@

run: #${STRINGS} ${STRINGS_FLAGS}
	${VALGRIND} ./${NAME}

${TESTS}: ${NAME}
	${VALGRIND} ./${NAME} $@

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
