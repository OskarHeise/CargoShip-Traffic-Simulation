run: compile execute

compile:
	gcc -std=c89 -Wpedantic Master.c -o Master -pthread
	gcc -std=c89 -Wpedantic Porti.c -o Porti -pthread
	gcc -std=c89 -Wpedantic Navi.c -o Navi -pthread

execute:
	./Master

cleanup:
	echo cleanup 