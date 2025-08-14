#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <stddef.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>
#include <unistd.h>
#include <aSubRecord.h>
#include <registryFunction.h>
#include <epicsExport.h>

static int TBTselect(aSubRecord *precord) {

    printf("TBTselect\n");

    int stream = *(int *)precord->b;
    if(stream==1){
        int chan = *(int *)precord->a;
        float *A = (float *)precord->c;
        float *B = (float *)precord->d;
        float *C = (float *)precord->e;
        float *D = (float *)precord->f;
        float *X = (float *)precord->g;
        float *Y = (float *)precord->h;
        float *selected = NULL;
        int len = *(int *)precord->i;
        switch(chan){
            case 0: selected = A; break;
            case 1: selected = B; break;
            case 2: selected = C; break;
            case 3: selected = D; break;
            case 4: selected = X; break;
            case 5: selected = Y; break;
            default: printf("Invalid FA Channel.\n");break;
        }
        memcpy((float *)precord->vala,selected,len*sizeof(float));
        precord->neva = len;
    }
    return(0);
}
// Note the function must be registered at the end!
epicsRegisterFunction(TBTselect);

