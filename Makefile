BIN=120++
SOURCES=main.c list.c lex.yy.c
OBJECTS=$(SOURCES:.c=.o)

LEX=flex
CC=gcc
CFLAGS=-g -Wall
LDFLAGS=
RM=rm -f

all: $(BIN)

$(BIN): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

lex.yy.c: clex.l cgram.tab.h
	$(LEX) $<

clean:
	$(RM) $(BIN) $(OBJECTS) lex.yy.c
