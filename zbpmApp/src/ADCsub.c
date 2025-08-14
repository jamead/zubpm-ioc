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

#define PREFIX "lab-BI{BPM:2}"
#define BUFMAX 50000
#define SCALE 1

static int ADCsub(aSubRecord *precord) {

    static chid Apv,Bpv,Cpv,Dpv,Tpv;
    static int i,PVset=0,oldready=0,oldwfmLen=0,oldwfmOff=0;

    int ready = *(int *)precord->h;
    int wfmLen = *(int *)precord->e;
    int wfmOff = *(int *)precord->f;

    if((oldready==0 && ready==1) || wfmLen!=oldwfmLen || wfmOff!=oldwfmOff){
        printf("ADC\n");
        if(PVset==0){
            char pvname[100];
            ca_context_create(ca_enable_preemptive_callback);
            strcpy(pvname,PREFIX);
            strcat(pvname,"ADC:A-Wfm");
            ca_create_channel(pvname,NULL,NULL,0,&Apv);
            strcpy(pvname,PREFIX);
            strcat(pvname,"ADC:B-Wfm");
            ca_create_channel(pvname,NULL,NULL,0,&Bpv);
            strcpy(pvname,PREFIX);
            strcat(pvname,"ADC:C-Wfm");
            ca_create_channel(pvname,NULL,NULL,0,&Cpv);
            strcpy(pvname,PREFIX);
            strcat(pvname,"ADC:D-Wfm");
            ca_create_channel(pvname,NULL,NULL,0,&Dpv);
            strcpy(pvname,PREFIX);
            strcat(pvname,"ADC:Time-Wfm");
            ca_create_channel(pvname,NULL,NULL,0,&Tpv);
            ca_pend_io(5);
            PVset = 1;
            printf("Initializing PVs...\n");
        }

        int bufLen = *(int *)precord->g;

        if(wfmLen>10000) wfmLen = 10000;
        if(wfmLen<100) wfmLen = 100;
        if(wfmLen>bufLen) wfmLen = bufLen;
        if(wfmOff<0) wfmOff = 0;
        if(wfmOff>(bufLen-wfmLen)) wfmOff = bufLen - wfmLen;
        oldwfmLen = wfmLen;
        oldwfmOff = wfmOff;
        *(int *)precord->vala = wfmLen;
        *(int *)precord->valb = wfmOff;

        double *Abuff = (double *)precord->a;
        double *Bbuff = (double *)precord->b;
        double *Cbuff = (double *)precord->c;
        double *Dbuff = (double *)precord->d;

        short Awfm[10000],Bwfm[10000],Cwfm[10000],Dwfm[10000];
        float Twfm[10000];

        for(i=wfmOff;i<(wfmLen+wfmOff);i++){
            Awfm[i-wfmOff] = (short)Abuff[i];
            Bwfm[i-wfmOff] = (short)Bbuff[i];
            Cwfm[i-wfmOff] = (short)Cbuff[i];
            Dwfm[i-wfmOff] = (short)Dbuff[i];
            Twfm[i-wfmOff] = (i-wfmOff)*0.008521583;  //Time in microseconds
        }

        ca_array_put(DBR_SHORT,wfmLen,Apv,Awfm);
        ca_array_put(DBR_SHORT,wfmLen,Bpv,Bwfm);
        ca_array_put(DBR_SHORT,wfmLen,Cpv,Cwfm);
        ca_array_put(DBR_SHORT,wfmLen,Dpv,Dwfm);
        ca_array_put(DBR_FLOAT,wfmLen,Tpv,Twfm);
        ca_pend_io(5);
    }
    oldready = ready;
    return(0);
}
// Note the function must be registered at the end!
epicsRegisterFunction(ADCsub);
