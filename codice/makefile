run: compile execute

compile:
	gcc -std=c89 -Wall -Wpedantic master.c -o master -lpthread -lm 
	gcc -std=c89 -Wall -Wpedantic porti.c -o porti -lpthread -lm
	gcc -std=c89 -Wall -Wpedantic navi.c -o navi -lpthread -lm
	gcc -std=c89 -Wall -Wpedantic print.c -o print -lpthread -lm
	gcc -std=c89 -Wall -Wpedantic meteo.c -o meteo -lpthread -lm

execute:
	./master

cleanup:
	echo cleanup 