#CC := arm-none-linux-gnueabi-gcc
CC := gcc
SRCS := cgiserver.c
OBJS := $(SRCS:%.c=%.o)
LIBS = -lcgic
DEBUG = -g -O2
CFLAGS = $(DEBUG) -Wall -c

.PHONY:all
all:login index cgiserver

login:
	$(CC) -o login.cgi login.c $(SRCS) $(LIB) $(INCLUDE) $(LIBS)
index:
	$(CC) -o index.cgi index.c $(SRCS) $(LIB) $(INCLUDE) $(LIBS)
cgiserver:
	$(CC) -o cgiserver main.c $(SRCS)

.PHONY:clean
clean:
	rm -f *.cgi cgiserver
instal:
	sudo rm  -f /var/www/cgi-bin/*
	sudo cp *cgi /var/www/cgi-bin/

