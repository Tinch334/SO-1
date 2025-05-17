#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

/* Producer/consumer program illustrating conditional variables */

/* Size of shared buffer */
#define BUF_SIZE 5

int *buffer[BUF_SIZE];					/* shared buffer */
int ptrBuff = 0;

sem_t semInsert;
sem_t semRemove;

pthread_mutex_t ptrMutex = PTHREAD_MUTEX_INITIALIZER;

/*
int sem_init(sem_t *sem, int pshared, unsigned int value);
int sem_destroy(sem_t *sem);
int sem_wait(sem_t *sem);
int sem_post(sem_t *sem);
*/

/* Produce value(s) */
void *producer(void *param)
{
	int i;
	for (i=1; i<=20; i++) {
        sem_wait(&semInsert);
		/* Insert into buffer */
		int *ptr = malloc(sizeof(int));
		*ptr = i;
        pthread_mutex_lock(&ptrMutex);
		buffer[ptrBuff] = ptr;
		ptrBuff++;
        pthread_mutex_unlock(&ptrMutex);
        sem_post(&semRemove);

    	printf ("producer: inserted %d\n", *ptr);  fflush (stdout);
	}
	printf ("producer quiting\n");  fflush (stdout);

    return NULL;
}

/* Consume value(s); Note the consumer never terminates */
void *consumer(void *param)
{
	int i;
	while (1) {
        sem_wait(&semRemove);

        pthread_mutex_lock(&ptrMutex);
        i = *buffer[ptrBuff - 1];
		free(buffer[ptrBuff - 1]);
		ptrBuff--;

        printf ("Consume value %d\n", i);  fflush(stdout);

        pthread_mutex_unlock(&ptrMutex);

        sem_post(&semInsert);
	}

    return NULL;
}

int main (int argc, char *argv[])
{
	pthread_t tid1, tid2;		/* thread identifiers */

    sem_init(&semInsert, 0, BUF_SIZE - 1);
    sem_init(&semRemove, 0, 0);

	/* create the threads; may be any number, in general */
	if (pthread_create(&tid1,NULL,producer,NULL) != 0) {
		fprintf (stderr, "Unable to create producer thread\n");
		exit (1);
	}
	if (pthread_create(&tid2,NULL,consumer,NULL) != 0) {
		fprintf (stderr, "Unable to create consumer thread\n");
		exit (1);
	}
	/* wait for created thread to exit */
	pthread_join(tid1,NULL);
	pthread_join(tid2,NULL);
	printf ("Parent quiting\n");
}