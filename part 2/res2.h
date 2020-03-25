#include <stdio.h>
#define n 250
#define A 50
#define B 100
#define row 10
#define Aline 5
#define Bline 10

int Nseat=250, Azoneseat=50, Bzoneseat=100, Czoneseat=100,
    Ntel=8, Ncash=4, Nrow=10, NzoneA=5, NzoneB=10, NzoneC=10,
    Nseatlow=1, Nseathigh=5, Tseatlow=5, Tseathigh=10,
    Cseat=20, CzoneA=30, CzoneB=25, CzoneC=20, Tcashlow=2, Tcashhigh=4;

double cardsuccess=0.9, PzoneA=0.2, PzoneB=0.4, PzoneC=0.4;

void *customer(void *x);
