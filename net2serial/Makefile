
OBJECTS = main.o uart.o net.o
SRC = main.c uart.c net.c

net2serial : $(OBJECTS)
	$(CC) $(LDFLAGS) -o net2serial $(OBJECTS)
	
main.o : uart.h net.h
uart.o : uart.h net.h
net.o : uart.h net.h

.PHONY : clean
clean:
	rm *.o net2serial