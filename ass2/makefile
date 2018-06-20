default: ngbr.buildrooms ngbr.adventure
buildrooms : ngbr.buildrooms.c
	gcc -std -o ngbr.buildrooms ngbr.buildrooms.c
adventure : ngbr.adventure.c
	gcc -std -lpthread -o ngbr.adventure ngbr.adventure.c
	gcc -std -o ngbr.adventure ngbr.adventure.c

clean:
	rm ngbr.buildrooms
	rm ngbr.adventure
