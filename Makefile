#CC := arm-none-linux-gnueabi-gcc
CC := gcc
SRCS := cgiserver.c
OBJS := $(SRCS:%.c=%.o)
LIBS = -lcgic
DEBUG = -g -O2
CFLAGS = $(DEBUG) -Wall -c

.PHONY:all
all:login logout  index cgiserver

login:
	$(CC) -o output/login.cgi login.c $(SRCS) $(LIB) $(INCLUDE) $(LIBS)
logout:
	$(CC) -o output/logout.cgi logout.c $(SRCS) $(LIB) $(INCLUDE) $(LIBS)
index:
	$(CC) -o output/index.cgi index.c $(SRCS) $(LIB) $(INCLUDE) $(LIBS)
cgiserver:
	$(CC) -o output/cgiserver main.c $(SRCS)

.PHONY:clean
clean:
	rm -f output/*.cgi output/cgiserver
instal:
	sudo rm  -f /var/www/cgi-bin/*
	sudo cp output/*cgi /var/www/cgi-bin/

