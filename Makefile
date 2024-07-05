CFLAGS=-Wall -Wextra -std=c99

build:
	gcc $(CFLAGS) *.c -o tema2 

pack:
	zip -FSr 312CA_DumitrascuFilipTeodor_Tema2.zip README.md Makefile *.c *.h

clean:
	rm -rf tema2

.PHONY: pack clean