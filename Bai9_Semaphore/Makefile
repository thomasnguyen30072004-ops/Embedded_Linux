CC = gcc
CFLAGS = -Wall -Wextra -pthread
LDFLAGS = -lrt

all: launcher producer consumer

launcher: launcher.c shared.h
	$(CC) $(CFLAGS) -o launcher launcher.c $(LDFLAGS)

producer: producer.c shared.h
	$(CC) $(CFLAGS) -o producer producer.c $(LDFLAGS)

consumer: consumer.c shared.h
	$(CC) $(CFLAGS) -o consumer consumer.c $(LDFLAGS)

clean:
	rm -f launcher producer consumer
