#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include "res1.h"


pthread_mutex_t lockTel, lockCalc, lockPay;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int Earnings=0;
int seatsArray[n];
double accum=0, accum1=0;
struct timespec start, stop, start1, stop1;

/**
* Synchronize Customers with Telef.
*/
void *customer(void *x){

	int *id = (int*)x;
	int seats, cost;
	unsigned int timeT;
	int rc;

	printf("Customer with id: %d\n",*id);
	rc=pthread_mutex_lock(&lockTel);

	while (Ntel == 0) {

		if( clock_gettime( CLOCK_REALTIME, &start) == -1 ) {
	      		perror( "clock gettime\n" );
	      		exit( EXIT_FAILURE );
	    	}	
		
		printf("O pelatis %d, den brike diathesimo tilefoniti. Blocked...\n",*id);
		rc=pthread_cond_wait(&cond, &lockTel);
		if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) {
      			perror( "clock gettime\n" );
      			exit( EXIT_FAILURE );
    		}
	}

	long seconds = stop.tv_sec - start.tv_sec; 
    	long ns = stop.tv_nsec - start.tv_nsec; 
    
    	if (start.tv_nsec > stop.tv_nsec) { // clock underflow 
		--seconds; 
		ns += 1000000000; 
   	}
	
	accum += (double)seconds ;
	
	printf("O pelatis %d, eksipiretite.\n",*id);
	Ntel--;
	
	rc=pthread_mutex_unlock(&lockTel);

	rc=pthread_mutex_lock(&lockCalc);
	/*dimiourgia tyxaiwn arithmwn*/
	seats = rand() % Nseathigh + Nseatlow; //SEATS
	printf("Customer %d needs %d seats\n", *id, seats);

	timeT = rand() % Tseathigh + Tseatlow; //TIME

	
	
	if(Nseat==0){
		printf("Theater is full\n");
		rc=pthread_mutex_unlock(&lockCalc);
		printf("O pelatis %d den eksipiretithike!\n", *id);
		printf("---------------\n");
		pthread_exit(&id);
	}
	
	/**
	* Seats are available	
	*/
	if(seats <= Nseat){
		printf("Searching for seats...\n");
	
		if( clock_gettime( CLOCK_REALTIME, &start1) == -1 ) {
	      		perror( "clock gettime\n" );
	      		exit( EXIT_FAILURE );
	    	}
		
		sleep(timeT);

		if( clock_gettime( CLOCK_REALTIME, &stop1) == -1 ) {
      			perror( "clock gettime\n" );
      			exit( EXIT_FAILURE );
    		}
		
		long seconds1 = stop1.tv_sec - start1.tv_sec; 
	    	long ns1 = stop1.tv_nsec - start1.tv_nsec; 
	    
	    	if (start1.tv_nsec > stop1.tv_nsec) { // clock underflow 
			--seconds1; 
			ns1 += 1000000000; 
	   	}
		
		accum1 += (double)seconds1;

		printf("Seats are available\n");
		/**
		* Credit card Error	
		*/
		if(((double)rand() / RAND_MAX) > cardsuccess){
			printf("Credit card ERROR for Customer %d\n", *id);
			rc=pthread_mutex_unlock(&lockCalc);
			printf("O pelatis %d den eksipiretithike!\n", *id);
			printf("---------------\n");
			pthread_exit(&id);
		}
		/**
		* Booking accomplished
		* Calculate Earnings, put Customer in seat accordingly	
		*/
		else {
			Nseat = Nseat - seats;
			cost = seats * Cseat;
			Earnings += cost;

			printf("The booking is complete\n");
			printf("Your id number is: %d\n", *id);
			printf("Your seats are: ");

			int j=0, counter=0;
			while(j<n && counter<seats){
				
				if(seatsArray[j]==0){
					seatsArray[j]=*id;
					printf("%d ", j);
					counter++;				
				}
				j++;
			}
			printf("\nCost will be %d\n", cost);
		}
	}

	/**
	* Seats are not available	
	*/
	else {
		printf("Seats not available\n");
		rc=pthread_mutex_unlock(&lockCalc);
		printf("O pelatis %d den eksipiretithike!\n", *id);
		printf("---------------\n");
		pthread_exit(&id);	
	}
	rc=pthread_mutex_unlock(&lockCalc);
	
	rc=pthread_mutex_lock(&lockPay);
	printf("O pelatis %d eksipiretithike epitixos!\n", *id);
	printf("---------------\n");
	Ntel++;

	rc=pthread_cond_signal(&cond);
	rc=pthread_mutex_unlock(&lockPay);

	if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
		pthread_exit(&rc);
	}

	pthread_exit(x);
}

/**
* Last Print
*/
void allPrint(int Customers){
	int i=0;

	printf("Seats are booked as follows\n");
	while(seatsArray[i] != 0) {
		printf("Seat %d / Customer %d \n",i ,seatsArray[i]);
		i++;
	}
	
	printf("Total earnings are: %d\n", Earnings);

	printf("Average time customer spent waiting: %f seconds\n", accum/(double)Customers);

	printf("Average time customer was serviced: %f seconds\n", accum1/(double)Customers);
}


int main(int argc, char *argv[]) {

	int rc;

	if (argc != 3) {
		printf("ERROR: the program should take two arguments, the number of costumers and the random number\n");
		exit(-1);
	}
		
	 
	int Customers = atoi(argv[1]);
	srand(atoi(argv[2]));

	if (Customers <= 0) {
		printf("ERROR: the number of costumers should be a positive number\n");
		exit(-1);
	}

	printf("Main: We will create %d threads\n", Customers);
	
	
	pthread_t threads[Customers];

	if (threads == NULL) {
		printf("NOT ENOUGH MEMORY!\n");
		return -1;
	}

	for(int i=0; i<n; i++){
		seatsArray[i] = 0;
	}
	int i;
	int id[Customers];

	//arxikopoiisi tou mutex.
	rc = pthread_mutex_init(&lockTel, NULL);
	if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_init() is %d\n", rc);
		exit(-1);
	}
	
	rc = pthread_mutex_init(&lockCalc, NULL);
	if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_init() is %d\n", rc);
		exit(-1);
	}
	
	
	rc = pthread_mutex_init(&lockPay, NULL);
	if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_init() is %d\n", rc);
		exit(-1);
	}
	
/*for loop gia na dimioyrghsw threads osa einai ta Customers*/
	for(i=0; i<Customers; i++){
		//printf("Main: creating thread %d\n", i);
		id[i] = i + 1;
		/*dimiourgia tou thread*/
    		rc = pthread_create(&threads[i], NULL, customer, &id[i]);

		/*elegxos oti to thread dimiourgithike swsta.*/
    		if (rc != 0) {
    			printf("ERROR: return code");
       			exit(-1);
       		}
	}

	
	void *status;
	/*Aparaitito gia na stamatisei to thread*/
	for (i = 0; i < Customers; i++) {
		rc = pthread_join(threads[i], &status);
		
		if (rc != 0) {
			printf("ERROR: return code from pthread_join() is %d\n", rc);
			exit(-1);		
		}
	}
	
	//"katastrofi" tou mutex.
	rc = pthread_mutex_destroy(&lockTel);
	if (rc != 0) {
   		printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
      		exit(-1);
	}

	rc = pthread_mutex_destroy(&lockCalc);
	if (rc != 0) {
   		printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
      		exit(-1);
	}

	rc = pthread_mutex_destroy(&lockPay);
	if (rc != 0) {
   		printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
      		exit(-1);
	}

	allPrint(Customers);
	
	return 1;
}
