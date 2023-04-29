------------- I segnali -------------

-Meccanismi utilizzati per notificare un processo dell'arrivo di un evento o di un'eccezione.

-Possono essere inviati da un processo a un altro o da un sistema operativo a un processo.

-Identificati da un numero intero.

-L'handler è una funzione che viene chiamata quando un processo riceve un segnale e consente di gestire il segnale in modo appropriato.

-In C, si utilizza la funzione "signal()" per impostare un handler per un segnale.

-La funzione "sigaction()" è una variante più avanzata di "signal()" che consente di specificare un insieme di opzioni per la gestione dei segnali.

-Una maschera di segnali è un insieme di segnali che un processo ha deciso di ignorare o bloccare temporaneamente.

-È possibile impostare una maschera di segnali con la funzione "sigprocmask()".

-Per inviare un segnale a un processo, si utilizza la funzione "kill()".

-Per ricevere un segnale, si utilizza la funzione "sigwait()".


------------- shared memory -------------


-Tecnica utilizzata nella programmazione concorrente per consentire a più processi di accedere alla stessa area di memoria condivisa.

-Consente a più processi di condividere dati senza doverli copiare da un processo all'altro.

-Riduce i tempi di elaborazione e migliora le prestazioni del sistema.

-In C su Linux, la shared memory viene implementata attraverso un insieme di funzioni.

-La funzione "shmget()" permette di creare un'area di memoria condivisa specificando la dimensione e le autorizzazioni di accesso.

-La funzione "shmat()" consente di attaccare un processo all'area di memoria condivisa, restituendo un puntatore alla zona di memoria condivisa.

-La funzione "shmdt()" consente di staccare un processo dall'area di memoria condivisa.

-L'accesso alla memoria condivisa deve essere sincronizzato tra i processi per evitare problemi di race condition o di inconsistenza dei dati.

-Si utilizzano spesso le tecniche di locking come i semafori o i mutex.

-L'uso della shared memory può comportare rischi come il deadlock o la starvation.

-La shared memory può essere utilizzata per migliorare le prestazioni dei sistemi concorrenti, ma richiede una progettazione attenta e una gestione corretta per evitare problemi di sicurezza e di stabilità del sistema.
