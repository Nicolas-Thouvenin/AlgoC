CC ?= gcc

COBJS ?= client.o bmp.o couleur.o json.o validator.o usefulFcts.o
SOBJS ?= serveur.o json.o validator.o usefulFcts.o

.SUFFIXES: .c .o

SERVER = serveur
CLIENT = client
CFLAGS = -Wall -Wextra -g

all: $(SERVER) $(CLIENT)

$(SERVER): $(SOBJS)
	$(CC) -o $(SERVER) $(SOBJS) -lm

$(CLIENT): $(COBJS)
	$(CC) -o $(CLIENT) $(COBJS) -lm

.c.o: 
	$(CC) -c $*.c -lm
