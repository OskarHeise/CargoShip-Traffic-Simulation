------------- SEGNALI -------------

- sono dei meccanismi utilizzati per notificare un processo dell'arrivo di un determinato evento o di un'eccezione.

- i segnali possono essere inviati da un processo a un altro o da un sistema operativo a un processo e sono identificati da un numero intero.

- l'handler è una funzione che viene chiamata quando un processo riceve un segnale e consente di gestire il segnale nel modo che il programmatore preferisce.

- in C, si utilizza la funzione "signal()" per impostare un handler per un segnale.

- la funzione "sigaction()" è una variante più avanzata di "signal()" che consente di specificare un insieme di opzioni per la gestione dei segnali.

- una maschera di segnali è un insieme di segnali che un processo ha deciso di ignorare o bloccare temporaneamente.

- è anche possibile impostare una maschera di segnali con la funzione "sigprocmask()".

- per inviare un segnale a un processo, si utilizza la funzione "kill()".


------------- MEMORIA CONDIVISA -------------


- è una tecnica utilizzata nella programmazione per consentire a più processi di accedere alla stessa area di memoria condivisa.

- grazie a lei più processi possono condividere dati senza doverli copiare da un processo all'altro.

- i vantaggi sono che riduce i tempi di elaborazione e migliora le prestazioni del sistema.

- in C su Linux, la shared memory viene implementata attraverso un insieme di funzioni.

-La funzione "shmget()" permette di creare un'area di memoria condivisa specificando la dimensione e le autorizzazioni di accesso.

-La funzione "shmat()" consente di attaccare un processo all'area di memoria condivisa, restituendo un puntatore alla zona di memoria condivisa.

-La funzione "shmdt()" consente di staccare un processo dall'area di memoria condivisa.

-L'accesso alla memoria condivisa deve essere sincronizzato tra i processi per evitare problemi di race condition o di inconsistenza dei dati.

-Si utilizzano spesso le tecniche di locking come i semafori o i mutex.

-L'uso della shared memory può comportare rischi come il deadlock o la starvation.

-La shared memory può essere utilizzata per migliorare le prestazioni dei sistemi concorrenti, ma richiede una progettazione attenta e una gestione corretta per evitare problemi di sicurezza e di stabilità del sistema.


------ CODE DI MESSAGGI, PIPE, FIFO ----------

- sono meccanismi di comunicazione utilizzati nella programmazione concorrente per permettere a più processi di comunicare tra di loro.

- code di messaggio sono una forma di comunicazione asincrona e permettono ai processi di inviare e ricevere messaggi in modo indipendente e senza dover attendere una risposta immediata.

- pipe sono una forma di comunicazione sincrona e collegano due processi tramite un unico canale di comunicazione, con una estremità di scrittura e una di lettura.

- FIFO sono simili alle pipe, ma utilizzano un file system virtuale e consentono ai processi di comunicare in modo asincrono.

---------- MAKEFILE -------------
- Makefile è uno strumento utilizzato nella programmazione per semplificare e automatizzare la compilazione di un programma.
- La linea di compilazione "gcc -std=c89 -Wpedantic master.c -o master -pthread -lm" compila il file sorgente "master.c" con il compilatore C, attivando un elevato livello di avvisi (wpedantic), specificando la versione del linguaggio C (1989), il nome dell'eseguibile da generare, attivando il supporto ai semafori POSIX e linkando la libreria math.h.

--------- THREAD ------------
- sono processi leggeri che condividono lo stesso spazio di indirizzamento (spazio di memoria virtuale assegnato al processo dal sistema operativo) del processo principale e possono eseguire in modo concorrente rispetto al processo principale o ad altri thread.
- thread sono utili in molte situazioni in cui è necessario gestire più attività contemporaneamente, senza dover creare processi separati e senza dover condividere dati tramite la memoria condivisa.
- thread consentono di utilizzare le risorse del sistema in modo più efficiente, migliorando le prestazioni dell'applicazione e migliorando l'esperienza dell'utente finale.
