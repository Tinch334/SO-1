OBJS	= servidor_epoll.o utils.o hash_table.o glist.o
SOURCE	= servidor_epoll.c utils.c hash_table.c glist.c
HEADER	= 
OUT	= servidor
FLAGS	= -g -c -Wall
LFLAGS	= -lpthread -lrt
CC	= gcc

all:	servidor

servidor: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

servidor.o: servidor.c
	$(CC) $(FLAGS) servidor.c 

utils.o: utils.c
	$(CC) $(FLAGS) utils.c

hash_table.o: hash_table.c
	$(CC) $(FLAGS) hash_table.c 

glist.o: glist.c
	$(CC) $(FLAGS) glist.c

clean:
	rm -f $(OBJS) $(OUT)

