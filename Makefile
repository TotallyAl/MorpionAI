SRCS = main.c agent.c board.c rlagent.c aiagent.c Dict.c LinkedList.c
OBJS = $(SRCS:%.c=%.o)
TARGET = tictactoe
CC = gcc
CFLAGS = -std=c99 --pedantic -Wall -Wextra -Wmissing-prototypes -g3
LDFLAGS = -lm

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

debug: CFLAGS += -g

debug: $(TARGET)

run : $(TARGET)
	./$(TARGET) -s human human

clean:
	rm -f $(OBJS) $(TARGET)

Dict.o: Dict.c Dict.h
LinkedList.o: LinkedList.c LinkedList.h
board.o: board.c board.h
agent.o: agent.c board.h agent.h
main.o: main.c board.h agent.h aiagent.h
aiagent.o: aiagent.c aiagent.h board.h agent.h Dict.h
rlagent.o: rlagent.c rlagent.h board.h agent.h Dict.h LinkedList.h
