run: compile execute

compile:
	gcc -std=c89 -Wpedantic master.c -o master -lpthread
	gcc -std=c89 -Wpedantic porti.c -o porti -lpthread
	gcc -std=c89 -Wpedantic navi.c -o navi -lpthread
	gcc -std=c89 -Wpedantic output.c -o output -lpthread
	gcc -std=c89 -Wpedantic controlli.c -o controlli -lpthread

execute:
	./master

cleanup:
	echo cleanup 