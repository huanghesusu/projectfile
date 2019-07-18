all:
	gcc server.c -o server -pthread -lsqlite3
	gcc client.c -o client -pthread

.PHONY:clean
clean:
	rm -rf *.o server client 
