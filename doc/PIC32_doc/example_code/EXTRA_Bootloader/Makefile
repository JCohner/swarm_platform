# compile from separate files, store as nu32utility
CFLAGS=

all: nu32utility 

nu32utility: common/frame.o common/hex.o utility/nu32client.o \
	utility/uart_posix.o utility/uart_windows.o
	$(CC) -o nu32utility common/frame.o utility/nu32client.o \
		utility/uart_posix.o utility/uart_windows.o common/hex.o

%.o : %.c 
	$(CC) -c -o $@ -I./common -I./utility $(CFLAGS) $< 

.PHONY : clean
clean:
	$(RM) common/*.o utility/*.o nu32utility
