CROSS=
CC=gcc

EsDialer : main.o md5.o str_extract.o http_req.o codec.o auth.o
	$(CROSS)$(CC) -std=c99 -o EsDialer main.o md5.o str_extract.o http_req.o codec.o auth.o -lcurl
	$(CROSS)strip EsDialer

main.o : main.c str_extract.h dbgout.h auth.h
	$(CROSS)$(CC) -std=c99 -c main.c

md5.o : md5.c md5.h
	$(CROSS)$(CC) -std=c99 -c md5.c

str_extract.o : str_extract.c str_extract.h
	$(CROSS)$(CC) -std=c99 -c str_extract.c

http_req.o : http_req.c http_req.h
	$(CROSS)$(CC) $(LDFLAGS) -std=c99 -c http_req.c

codec.o : codec.c codec.h md5.h
	$(CROSS)$(CC) -std=c99 -c codec.c

auth.o : auth.c auth.h codec.h http_req.h md5.h dbgout.h str_extract.h
	$(CROSS)$(CC) -std=c99 -c auth.c

clean :
	rm -f EsDialer main.o md5.o str_extract.o http_req.o codec.o auth.o
