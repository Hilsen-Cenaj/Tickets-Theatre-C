#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "res2.h"


pthread_mutex_t lockTel, lockCalc, lockPay, lockCash; //Mutexs
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int Earnings=0; //Total income
int seatsArray[n]; //Seats array
int Aseats[row][Aline+1],Bseats[row][Bline+1],Cseats[row][Bline+1];
double accum=0, accum1=0, accum2=0;
struct timespec start, stop, start1, stop1;

/**
* Synchronize Customers with Telef.
*/
void *customer(void *x) {

    int *id = (int *) x;
    int seats, cost, zone = 0;
    unsigned int timeT,timeC;
    int rc;

    printf("Customer with id: %d\n", *id);

    rc = pthread_mutex_lock(&lockTel); // Lock Tel

    while (Ntel == 0) {

        if (clock_gettime(CLOCK_REALTIME, &start) == -1) {
            perror("clock gettime\n");
            exit(EXIT_FAILURE);
        }

        printf("O pelatis %d, den brike diathesimo tilefoniti. Blocked...\n", *id);
        rc = pthread_cond_wait(&cond, &lockTel);

        if (clock_gettime(CLOCK_REALTIME, &stop) == -1) {
            perror("clock gettime\n");
            exit(EXIT_FAILURE);
        }
    }

    long seconds = stop.tv_sec - start.tv_sec;
    long ns = stop.tv_nsec - start.tv_nsec;

    if (start.tv_nsec > stop.tv_nsec) { // clock underflow
        --seconds;
        ns += 1000000000;
    }

    accum += (double) seconds;

    printf("O pelatis %d, eksipiretite.\n", *id);
    Ntel--;

    rc = pthread_mutex_unlock(&lockTel); // Unlock Tel

    rc = pthread_mutex_lock(&lockCalc); // Lock Calc
    /*dimiourgia tyxaiwn arithmwn*/

    /**
     * Epilogi zonis(A,B,C)
     */
	while (zone == 0) {
        if (((double) rand() / RAND_MAX) < PzoneA) {	
			zone = 1;
			printf("Customer %d needs A zone\n", *id);
			break;
        }

        if (((double) rand() / RAND_MAX) < PzoneB && zone==0) {
            zone = 2;
            printf("Customer %d needs B zone\n", *id);
			break;
        }

        if (((double) rand() / RAND_MAX) < PzoneC && zone==0) {
            zone = 3;
            printf("Customer %d needs C zone\n", *id);
			break;
        }
    }

	seats = rand() % Nseathigh + Nseatlow; //SEATS
	printf("Customer %d needs %d seats\n", *id, seats);

	timeT = rand() % Tseathigh + Tseatlow; // time for telephone
	timeC = rand() % Tcashlow + Tcashhigh; // time for cashier
	
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
			rc = pthread_mutex_lock(&lockCash);
			while (Ncash == 0) {
			
				printf("O pelatis %d, den brike diathesimo cashier. Blocked...\n", *id);
				rc = pthread_cond_wait(&cond, &lockCash);
			
			}
			Nseat = Nseat - seats;

			printf("The booking is complete\n");
			printf("Your id number is: %d\n", *id);
			printf("Your seats are: ");
			
			/**
			* Calculate Cost for zone A seats
			* and put customers in their seats
			* accordingly
			*/
			if(zone == 1){
				Azoneseat = Azoneseat - seats;
				cost = seats * CzoneA;
				Earnings += cost;

				int j=0,i=0,counter=0, line=5;
				while(j<A && counter<seats){
					
					while(i<Aline && counter<seats){
						
						/** 
						* Check line if it has available seats
						* if not go the next line
						*/
						if(Aseats[j][Aline+1] < seats && Aseats[j][Aline+1] != 0){
							break;
						}
						
						if(Aseats[j][i]==0){
							Aseats[j][i]=*id;
							printf("%d,%d | ", j, i);
							counter++;							
							
						}
						i++;
					}//while
					Aseats[j][Aline+1]=line-seats; // Keep track of seats available in each line
					j++;
				}//while
			} // zone A
			
			/**
			* Calculate Cost for zone B seats
			* and put customers in their seats
			* accordingly
			*/
			if(zone == 2){
				Bzoneseat = Bzoneseat - seats;
				cost = seats * CzoneB;
				Earnings += cost;

				int j=0,i=0,counter=0,line=10;
				while(j<B && counter<seats){
					
					while(i<Bline && counter<seats){
						
						/** 
						* Check line if it has available seats
						* if not go the next line
						*/
						if(Bseats[j][Aline+1] < seats && Bseats[j][Bline+1] != 0){
							break;
						}
						
						if(Bseats[j][i]==0){
							Bseats[j][i]=*id;
							printf("%d,%d | ", j, i);
							counter++;							
							
						}
						i++;
					}//while
					Bseats[j][Bline+1]=line-seats; // Keep track of seats available in each line
					j++;
				}//while
			} // zone B

			/**
			* Calculate Cost for zone C seats
			* and put customers in their seats
			* accordingly
			*/
			if(zone == 3){
				Czoneseat = Czoneseat - seats;
				cost = seats * CzoneC;
				Earnings += cost;

				int j=0,i=0,counter=0,line=10;
				while(j<B && counter<seats){
					
					while(i<Bline && counter<seats){
						
						/** 
						* Check line if it has available seats
						* if not go the next line
						*/
						if(Cseats[j][Bline+1] < seats && Cseats[j][Bline+1] != 0){
							break;
						}
						
						if(Cseats[j][i]==0){
							Cseats[j][i]=*id;
							printf("%d,%d | ", j, i);
							counter++;							
							
						}
						i++;
					} //while
					Cseats[j][Bline+1]=line-seats; // Keep track of seats available in each line
					j++;
				} //while
			} // zone C
			

			if( clock_gettime( CLOCK_REALTIME, &start1) == -1 ) {
	      		perror( "clock gettime\n" );
	      		exit( EXIT_FAILURE );
	    	}
		
			sleep(timeC);

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
		
			accum2 += (double)seconds1;
			

			printf("\nCost will be %d\n", cost);
		}

	//printf("O pelatis %d, eksipiretite apo ton cashier.\n", *id);
    Ncash--;

	rc=pthread_mutex_unlock(&lockCash); // Unlock Cash
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
	rc=pthread_mutex_unlock(&lockCalc); // Unlock Calc
	
	rc=pthread_mutex_lock(&lockPay); // Lock Pay
	printf("O pelatis %d eksipiretithike epitixos!\n", *id);
	printf("---------------\n");
	Ntel++;
	Ncash++;

	rc=pthread_cond_signal(&cond);
	rc=pthread_mutex_unlock(&lockPay); // Unlock Pay

	if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
		pthread_exit(&rc);
	}

	pthread_exit(x);
}

/**
* Print the content of the arrays
*/
void arrayPrint(int* arr, int m, int k, char zone){
	int i=0,j=0;
	printf("Seats are booked as follows\n");
	for (i = 0; i < m; i++){
		for (j = 0;j < k; j++){
			if(*((arr+i*k) + j) == 0) break;
			printf("Zone %c / Seat %d,%d / Customer %d \n",zone,i,j,*((arr+i*k) + j));
		}
		if(*((arr+i*k) + j) == 0) break;
	}
}

/**
* Last Print
*/
void allPrint(int Customers){

	arrayPrint((int *)Aseats, row, Aline,'A');
	arrayPrint((int *)Bseats, row, Bline,'B');
	arrayPrint((int *)Cseats, row, Bline,'C');
	
	printf("Total earnings are: %d\n", Earnings);

	printf("Average time customer spent waiting: %f seconds\n", accum/(double)Customers);

	printf("Average time customer was serviced: %f seconds\n", accum1/(double)Customers);

	printf("Average time customer was waiting for cashier: %f seconds\n", accum2/(double)Customers);
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
	
	//printf("hello");	
	pthread_t threads[Customers];

	if (threads == NULL) {
		printf("NOT ENOUGH MEMORY!\n");
		return -1;
	}
	
	int iline,jline;

	for(iline=0; iline<row; iline++){
		jline=0;
		while(jline<Aline+1){
			Aseats[iline][jline] = 0;
			jline++;
		}
	}

	for(iline=0; iline<row; iline++){
		jline=0;
		while(jline<Bline+1){
			Bseats[iline][jline] = 0;
			jline++;
		}
	}

	for(iline=0; iline<row; iline++){
		jline=0;
		while(jline<Bline+1){
			Cseats[iline][jline] = 0;
			jline++;
		}
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

	rc = pthread_mutex_init(&lockCash, NULL);
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

	rc = pthread_mutex_destroy(&lockCash);
	if (rc != 0) {
   		printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
      		exit(-1);
	}

	allPrint(Customers);
	
	return 1;
}
