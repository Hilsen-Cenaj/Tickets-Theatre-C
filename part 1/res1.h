#include <stdio.h>
#define n 250

int Nseat=250, Ntel=8, Nseatlow=1, Nseathigh=5, Tseatlow=5, Tseathigh=10, Cseat=20;
double cardsuccess=0.9;

void *customer(void *x);
