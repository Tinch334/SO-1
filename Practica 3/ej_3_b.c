#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <assert.h>

/* Producer/consumer program illustrating conditional variables */

/* Size of shared buffer */
#define BUF_SIZE 5

int *buffer[BUF_SIZE];					/* shared buffer */
int ptrBuff = 0;

pthread_cond_t canConsume;
pthread_cond_t canProduce;

pthread_mutex_t ptrMutex = PTHREAD_MUTEX_INITIALIZER;

/* Produce value(s) */
void *producer(void *param)
{
	int i;
	for (i=1; i<=20; i++) {
		/* Insert into buffer */
        pthread_mutex_lock(&ptrMutex);

        while(ptrBuff >= BUF_SIZE)
            pthread_cond_wait(&canProduce, &ptrMutex);

		int *ptr = malloc(sizeof(int));
        assert(ptr != NULL);
		*ptr = i;
        //printf("i:%d\n", i);
		buffer[ptrBuff] = ptr;
		ptrBuff++;

        printf ("producer: inserted %d\n", *ptr);  fflush (stdout);

        //Theres an element, can consume
        pthread_cond_signal(&canConsume);
        pthread_mutex_unlock(&ptrMutex);        
	}
	printf ("producer quiting\n");  fflush (stdout);

    return NULL;
}

/* Consume value(s); Note the consumer never terminates */
void *consumer(void *param)
{
	int i;
	while (1) {
        pthread_mutex_lock(&ptrMutex);
        while(ptrBuff < 1)
            pthread_cond_wait(&canConsume, &ptrMutex);
            
        i = *buffer[ptrBuff - 1];
		free(buffer[ptrBuff - 1]);
		ptrBuff--;

        printf ("Consume value %d\n", i);  fflush(stdout);

        pthread_cond_signal(&canProduce);
        pthread_mutex_unlock(&ptrMutex);
	}

    return NULL;
}

int main (int argc, char *argv[])
{
	pthread_t tid1, tid2;		/* thread identifiers */


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