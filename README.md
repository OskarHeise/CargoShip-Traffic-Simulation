Progetto Sistemi Operativi UniTO A.A. 2022/2023 

/* I SEGNALI */ 

I segnali su Linux e C sono meccanismi utilizzati per notificare un processo dell'arrivo di un evento o di un'eccezione. I segnali possono essere inviati da un processo a un altro o da un sistema operativo a un processo e sono identificati da un numero intero.
Un handler è una funzione che viene chiamata quando un processo riceve un segnale, e consente di gestire il segnale in modo appropriato. In C, per impostare un handler per un segnale, si utilizza la funzione "signal()", che permette di specificare il segnale che si vuole gestire e l'handler che si vuole associare.
La funzione "sigaction()" è una variante più avanzata di "signal()", che consente di specificare un insieme di opzioni per la gestione dei segnali, come la possibilità di bloccare altri segnali durante l'esecuzione dell'handler o di impostare un tempo massimo di esecuzione dell'handler. In generale, sigaction è preferibile a signal per la maggior flessibilità che offre.
Una maschera di segnali è un insieme di segnali che un processo ha deciso di ignorare o bloccare temporaneamente. È possibile impostare una maschera di segnali con la funzione "sigprocmask()", che consente di bloccare temporaneamente l'arrivo di alcuni segnali durante l'esecuzione di una sezione critica del codice.
Per inviare un segnale a un processo, si utilizza la funzione "kill()", che richiede il PID del processo destinatario e il numero del segnale da inviare. Per ricevere un segnale, invece, si utilizza la funzione "sigwait()", che sospende l'esecuzione del processo fino all'arrivo di un segnale specificato, e restituisce il numero del segnale ricevuto.
In sintesi, i segnali su Linux e C sono un meccanismo importante per la gestione degli eventi e delle eccezioni in un sistema. La loro gestione richiede una buona conoscenza della sincronizzazione dei processi, della gestione degli errori e della programmazione concorrente. La scelta tra "signal()" e "sigaction()" dipende dalle esigenze specifiche del programma, mentre l'utilizzo di una maschera di segnali consente di controllare l'arrivo dei segnali in modo più preciso.

/* LA SHARED MEMORY */ 

La shared memory è una tecnica utilizzata nella programmazione concorrente per consentire a più processi di accedere alla stessa area di memoria condivisa. In pratica, la shared memory consente a più processi di condividere dati senza doverli copiare da un processo all'altro, riducendo così i tempi di elaborazione e migliorando le prestazioni del sistema.
In C su Linux, la shared memory viene implementata attraverso un insieme di funzioni che consentono di creare, attaccare e staccare una regione di memoria condivisa. La funzione "shmget()" permette di creare un'area di memoria condivisa specificando la dimensione e le autorizzazioni di accesso. La funzione "shmat()" consente di attaccare un processo all'area di memoria condivisa, restituendo un puntatore alla zona di memoria condivisa. La funzione "shmdt()" consente invece di staccare un processo dall'area di memoria condivisa.
Una volta creata l'area di memoria condivisa, i processi possono accedervi tramite il puntatore restituito dalla funzione "shmat()". È importante notare che l'accesso alla memoria condivisa deve essere sincronizzato tra i processi, per evitare problemi di race condition o di inconsistenza dei dati. A tal fine, si utilizzano spesso le tecniche di locking come i semafori o i mutex.
Tuttavia, l'uso della shared memory può comportare rischi come il deadlock o la starvation. Il deadlock si verifica quando due o più processi rimangono bloccati in attesa di risorse che sono occupate da altri processi, impedendo così l'avanzamento del sistema. La starvation, invece, si verifica quando un processo viene continuamente escluso dall'accesso alle risorse condivise, a causa di un errato utilizzo dei meccanismi di sincronizzazione.
In generale, la shared memory può essere utilizzata per migliorare le prestazioni dei sistemi concorrenti, ma richiede una progettazione attenta e una gestione corretta per evitare problemi di sicurezza e di stabilità del sistema.

/* CODE DI MESSAGGI, PIPE, FIFO */
 
Le code di messaggio, le pipe e le FIFO (First In First Out) sono meccanismi utilizzati nella programmazione concorrente per consentire a più processi di comunicare tra di loro.
Le code di messaggio sono una forma di comunicazione asincrona, in cui i processi possono inviare e ricevere messaggi in modo indipendente e senza dover attendere una risposta immediata. I messaggi vengono memorizzati in una coda di messaggi, e possono essere letti e scritti dai processi che ne hanno accesso. Le code di messaggio sono utilizzate soprattutto in contesti di comunicazione tra processi a lungo termine, in cui i messaggi sono trasmessi in modo affidabile e garantiscono l'ordine di arrivo.
Le pipe sono invece una forma di comunicazione sincrona, in cui i processi comunicanti sono collegati tramite un unico canale di comunicazione. Una pipe è costituita da due estremità, una di scrittura e una di lettura, e i processi possono utilizzare la pipe per inviare e ricevere dati in modo sequenziale. Una delle estremità della pipe viene utilizzata dal processo mittente, mentre l'altra estremità viene utilizzata dal processo destinatario. Le pipe sono utilizzate soprattutto per la comunicazione tra processi generati dalla stessa applicazione.
Le FIFO sono simili alle pipe, ma sono basate su un file system virtuale. Le FIFO consentono ai processi di comunicare in modo asincrono, senza dover attendere una risposta immediata. Le FIFO sono utilizzate soprattutto in contesti di comunicazione tra processi a lungo termine, in cui la comunicazione è affidabile e garantisce l'ordine di arrivo.
In generale, ogni meccanismo di comunicazione ha i propri vantaggi e svantaggi. Le code di messaggio sono utilizzate soprattutto in contesti di comunicazione tra processi a lungo termine e garantiscono l'ordine di arrivo dei messaggi, ma richiedono una gestione più complessa rispetto alle pipe e alle FIFO. Le pipe sono utilizzate soprattutto per la comunicazione tra processi generati dalla stessa applicazione, ma richiedono che i processi siano in esecuzione contemporaneamente. Le FIFO sono simili alle pipe, ma utilizzano un file system virtuale, e sono utilizzate soprattutto in contesti di comunicazione tra processi a lungo termine.

/* MAKEFILE */

Il Makefile è uno strumento utilizzato nella programmazione per automatizzare il processo di compilazione di un programma. In particolare, il Makefile consente di specificare le dipendenze tra i file sorgente e di compilare solo i file che hanno subito modifiche, riducendo così i tempi di compilazione e semplificando la gestione del codice.
La linea di codice che hai fornito è un comando di compilazione per un file sorgente chiamato "master.c". Di seguito ti spiego il significato di ogni flag presente nella linea di codice:

"gcc" è il compilatore C utilizzato per compilare il codice sorgente.
"-std=c89" specifica il livello di conformità al linguaggio C, in questo caso la versione del 1989.
"-Wpedantic" attiva un elevato livello di avvisi del compilatore, segnalando potenziali problemi di conformità al linguaggio.
"master.c" è il nome del file sorgente che si vuole compilare.
"-o master" specifica il nome dell'eseguibile che verrà generato.
"-pthread" attiva il supporto ai thread POSIX, necessario se il programma utilizza i thread.
"-lm" indica di linkare la libreria math.h, necessaria se il programma utilizza le funzioni matematiche.
"cleanup" semplicemente rimuove i file oggetto

In sintesi, la linea di codice che hai fornito compila il file sorgente "master.c" con il compilatore C, attivando un elevato livello di avvisi, specificando la versione del linguaggio C, il nome dell'eseguibile da generare, attivando il supporto ai thread POSIX e linkando la libreria math.h se necessario.
I thread sono processi leggeri che condividono lo stesso spazio di indirizzamento del processo principale e possono eseguire in modo concorrente rispetto al processo principale o ad altri thread. I thread sono utili in molte situazioni in cui è necessario gestire più attività contemporaneamente, senza dover creare processi separati e senza dover condividere dati tramite la memoria condivisa.
Inoltre, i thread sono utilizzati spesso per sfruttare al massimo le capacità delle CPU multicore, consentendo di eseguire più attività contemporaneamente su diversi core della CPU.
In generale, i thread sono utili in situazioni in cui è necessario gestire più attività contemporaneamente, senza dover creare processi separati e senza dover condividere dati tramite la memoria condivisa. I thread consentono di utilizzare le risorse del sistema in modo più efficiente, migliorando le prestazioni dell'applicazione e migliorando l'esperienza dell'utente finale.
