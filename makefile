CC=g++
SOURCE=lexer.cpp automata.cpp main.cpp parser.cpp
TARGET=bin

_start:
	${CC} -o ${TARGET} ${SOURCE}

clean:
	rm  ${TARGET}
