compexec: compile execute

compile:
	gcc -std=c89 -Wpedantic Master.c -o Master
	gcc -std=c89 -Wpedantic Porti.c -o Porti
	gcc -std=c89 -Wpedantic Navi.c -o Navi

execute:
	./Master

gdb:
	gcc -std=c89 -Wpedantic -g -00 Master.c -o Master
	gcc -std=c89 -Wpedantic -g -00 Porti.c -o Porti
	gcc -std=c89 -Wpedantic -g -00 Navi.c -o Navi
	gdb Master