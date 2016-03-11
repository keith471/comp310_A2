LIBS := -lrt -lpthread
CFLAGS := -g

all: printer client

printer: printer.c
		gcc $(CFLAGS) $^ -o $@ $(LIBS)

client: client.c
		gcc $(CFLAGS) $^ -o $@ $(LIBS)

clean:
		-rm printer client
