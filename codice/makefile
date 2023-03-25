run: compile execute

compile:
	gcc -std=c89 -Wpedantic master.c -o master -lpthread -lm
	gcc -std=c89 -Wpedantic porti.c -o porti -lpthread -lm
	gcc -std=c89 -Wpedantic navi.c -o navi -lpthread -lm
	gcc -std=c89 -Wpedantic output.c -o output -lpthread -lm
	gcc -std=c89 -Wpedantic controlli.c -o controlli -lpthread -lm

execute:
	./master

cleanup:
	echo cleanup 