TARGET=server
TARGET1=client

OBJS1+=server.o
OBJS2+=$(TARGET1).o

OBJS=$(OBJS1) $(OBJS2)

CFLAGS+=-g -Wall
LDFLAGS+=-lubox

all: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS1) $(LDFLAGS)
	$(CC) $(CFLAGS) -o $(TARGET1) $(OBJS2) $(LDFLAGS)


clean:
	rm -rf  $(TARGET) $(OBJS) *.bak

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<



