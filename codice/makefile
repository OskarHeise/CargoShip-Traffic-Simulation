run: compile execute

compile:
	gcc -std=c89 -Wpedantic master.c -o master -pthread -lm
	gcc -std=c89 -Wpedantic porti.c -o porti -pthread -lm
	gcc -std=c89 -Wpedantic navi.c -o navi -pthread -lm
	gcc -std=c89 -Wpedantic print.c -o print -pthread -lm
	gcc -std=c89 -Wpedantic meteo.c -o meteo -pthread -lm

execute:
	./master

cleanup:
	rm -f *.o master porti navi print meteo