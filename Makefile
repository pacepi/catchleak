all : lib

lib :
	@gcc catchleak.c -fPIC -shared -g -o catchleak.so

install :
	install catchleak /usr/bin
	install catchleak.so /lib
clean :
	@rm -rf catchleak.so
