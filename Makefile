CC = gcc
COPTS = -g -Wall -fPIC

############################### Various ##############################
### -rpath arguments for finding .so objects in pre-specified locations
MY_DIR = .
RPATH = -rpath=$(MY_DIR)
### decide on debugging level(s)
COPTS += -DNO_DEBUG_
COPTS += -DNO_DEBUG2_

############################### Target ##############################
all: libs
	$(CC) $(COPTS) -Wl,-rpath=. main.c -L. -lINfs

libs:
	$(CC) -c $(COPTS) infs.c
	$(CC) $(COPTS) -shared -Wl,-rpath=.,$(RPATH),-soname=libINfs.so \
           -o libINfs.so infs.o

clean:
	rm -f  *.o *.so a.out 

