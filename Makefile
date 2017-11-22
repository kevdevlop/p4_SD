LIBDIR=/home/kevdev/Tmk/lib.i386_linux2


INCLUDE_DIR=/home/kevdev/Tmk/include

LIB=Tmk_udp

test_tmk:
	gcc -m32 test_tmk.c -o test_tmk -L$(LIBDIR) -I$(INCLUDE_DIR) -l$(LIB)
	
hola_tmk:
	gcc -m32 hola_tmk.c -o hola_tmk -L$(LIBDIR) -I$(INCLUDE_DIR) -l$(LIB)
	
suma_tmk:
	gcc -m32 suma_tmk.c -o suma_tmk -L$(LIBDIR) -I$(INCLUDE_DIR) -l$(LIB)

boletos:
	gcc -m32 boletos.c -o boletos -L$(LIBDIR) -I$(INCLUDE_DIR) -l$(LIB)  -std=c99
	
clean:
	rm -f test_tmk hola_tmk suma_tmk boletos


