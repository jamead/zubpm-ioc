#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include "cadef.h"
#include <aSubRecord.h>
#include <registryFunction.h>
#include <epicsExport.h>

#define BUFMAX 115220

static int TempSub(aSubRecord *precord) {

    printf("Hello\n");

//    static float Abuff[BUFMAX],Bbuff[BUFMAX],Cbuff[BUFMAX],Dbuff[BUFMAX];
//    static float ACbuff[BUFMAX],BDbuff[BUFMAX],Xbuff[BUFMAX],Ybuff[BUFMAX];

//    static int topIndx=0,Count=0,PVset=0;

//    float Awfm[15000],Bwfm[15000],Cwfm[15000],Dwfm[15000];
//    float ACwfm[15000],BDwfm[15000],Xwfm[15000],Ywfm[15000],Twfm[15000];

    double A = *(double *)precord->a;
    double B = *(double *)precord->b;
    double C = *(double *)precord->c;
    double D = *(double *)precord->d;
    double AC = *(double *)precord->e;
    double BD = *(double *)precord->f;

    printf("%5.3f  %5.3f  %5.3f  %5.3f  %5.3f  %5.3f\n",A,B,C,D,AC,BD);

    return(0);
}
// Note the function must be registered at the end!
epicsRegisterFunction(TempSub);

