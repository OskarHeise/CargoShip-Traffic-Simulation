run: compile execute

compile:
	gcc -std=c89 -Wpedantic master.c -o master -lpthread -lm 
	gcc -std=c89 -Wpedantic porti.c -o porti -lpthread -lm
	gcc -std=c89 -Wpedantic navi.c -o navi -lpthread -lm
	gcc -std=c89 -Wpedantic print.c -o print -lpthread -lm
	gcc -std=c89 -Wpedantic meteo.c -o meteo -lpthread -lm

execute:
	./master 

cleanup:
	rm -f *.o master porti navi print meteo