CFLAGS = -c -Wall --std=c99

LDFLAG = -pthread

INCLUDES = -I .

CC = gcc

node: main.o 
	$(CC) $(LDFLAG) $^ -o $@ 

%.o : %.c
	$(CC) -c $^ $(INCLUDES) $(CFLAGS) 
        
clean:
	rm *.o
