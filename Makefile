BIN=120++
SOURCES=main.c list.c token.c lex.yy.c
TESTS=test/lex/test.c test/lex/test.cpp
OBJECTS=$(SOURCES:.c=.o)

LEX=flex
LEXFLAGS=--header-file=clex.h
CC=gcc
CFLAGS=-g -Wall -std=gnu99
LDFLAGS=
RM=rm -f

all: $(BIN)

test: $(BIN)
	./$(BIN) $(TESTS)

$(BIN): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

main.c: clex.h

clex.h: lex.yy.c

lex.yy.c: clex.l cgram.tab.h
	$(LEX) $(LEXFLAGS) $<

clean:
	$(RM) $(BIN) $(OBJECTS) lex.yy.c
