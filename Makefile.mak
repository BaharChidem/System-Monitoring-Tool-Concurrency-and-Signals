# Makefile for Assignment 3

CC = gcc
CFLAGS = -Wall -Werror

all: program

program: a3.o a3_concurrency.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

.%o:%.c a3.h
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	rm *.o
