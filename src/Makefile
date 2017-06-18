CFLAGS = -c -Wall --std=c99 -g

LDFLAG = -pthread -g

INCLUDES = -I .

CC = gcc

node: main.o 
	$(CC) $(LDFLAG) $^ -o $@ 

%.o : %.c
	$(CC) -c $^ $(INCLUDES) $(CFLAGS) 
        
clean:
	rm *.o node
