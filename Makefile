CROSS=
CC=$(CROSS)gcc

EsDialer : main.o md5.o str_extract.o http_req.o codec.o auth.o
	$(CC) -o EsDialer main.o md5.o str_extract.o http_req.o codec.o auth.o -lcurl
	strip EsDialer

main.o : main.c str_extract.h dbgout.h auth.h
	$(CC) -c main.c

md5.o : md5.c md5.h
	$(CC) -c md5.c

str_extract.o : str_extract.c str_extract.h
	$(CC) -c str_extract.c

http_req.o : http_req.c http_req.h
	$(CC) -c http_req.c

codec.o : codec.c codec.h md5.h
	$(CC) -c codec.c

auth.o : auth.c auth.h codec.h http_req.h md5.h dbgout.h str_extract.h
	$(CC) -c auth.c

clean :
	rm -f EsDialer main.o md5.o str_extract.o http_req.o codec.o auth.o
