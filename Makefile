OSTYPE := $(shell uname -m)

all:
ifeq ($(OSTYPE), x86_64)
	cc -g -I./include -DUNIX -c -o getpass.o getpass.c
	cc -o getpass getpass.o -L./lib64 -lldapsdk -lpthread -lresolv -ldl -lldapssl -lcrypto	
else
	cc -g -I./include -DUNIX -c -o getpass.o getpass.c
	cc -o getpass getpass.o -L./lib -lldapsdk -lpthread -lresolv -ldl -lldapssl -lcrypto
endif
