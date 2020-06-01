CC := gcc
NAME := minecweeper
PREFIX := /usr

minecweeper: main.c util2d.h input.h
	${CC} -o ${NAME} main.c

install: minecweeper
	@mkdir -p ${PREFIX}
	@cp ${NAME} ${PREFIX}/bin/${NAME}
	@chmod 755 ${PREFIX}/bin/${NAME}

uninstall:
	@rm ${NAME} ${PREFIX}/bin/${NAME}

