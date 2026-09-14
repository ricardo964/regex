CC=g++
SOURCE=lexer.cpp
TARGET=bin

_start:
	${CC} -o ${TARGET} ${SOURCE}

clean:
	rm  ${TARGET}
