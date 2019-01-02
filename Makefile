TARGET1=server
TARGET2=client
OBJ1+=server.o
OBJ2+=client.o

TARGETS=$(TARGET1) $(TARGET2)
OBJS=$(OBJ1) $(OBJ2)

CFLAGS+=-g -Wall
LDFLAGS+=-I. -lubox

all: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET1) $(OBJ1) $(LDFLAGS)
	$(CC) $(CFLAGS) -o $(TARGET2) $(OBJ2) $(LDFLAGS)
clean:
	rm -rf $(TARGETS) $(OBJS) *.bak
    
%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

