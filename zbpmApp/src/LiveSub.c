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

#define ADCMAX 8000
#define TBTMAX 8000
#define PREFIX "lab-BI{BPM:2}"

static int LiveSub(aSubRecord *precord) {

    int i;
    char pvname[100];

    float *Abuff = (float *)precord->a;
    float *Bbuff = (float *)precord->b;
    float *Cbuff = (float *)precord->c;
    float *Dbuff = (float *)precord->d;
    float *Xbuff = (float *)precord->i;
    float *Ybuff = (float *)precord->j;

    int ADCwfmLen = *(int *)precord->e;
    int ADCwfmOff = *(int *)precord->f;
    int ADCnord = *(int *)precord->h;
    char *tstr = (char *)precord->g;
    int TBTwfmLen = *(int *)precord->k;
    int TBTwfmOff = *(int *)precord->l;
    int TBTnord = *(int *)precord->m;

    if(ADCwfmLen>ADCMAX) ADCwfmLen=ADCMAX;
    if(ADCwfmLen<50) ADCwfmLen=50;
    if(ADCwfmOff>(ADCMAX-ADCwfmLen)) ADCwfmOff=(ADCMAX-ADCwfmLen);
    if(ADCwfmOff<0) ADCwfmOff=0;
    *(int *)precord->vale = ADCwfmLen;
    *(int *)precord->valf = ADCwfmOff;

    if(TBTwfmLen>TBTMAX) TBTwfmLen=TBTMAX;
    if(TBTwfmLen<50) TBTwfmLen=50;
    if(TBTwfmOff>(TBTMAX-TBTwfmLen)) TBTwfmOff=(TBTMAX-TBTwfmLen);
    if(TBTwfmOff<0) TBTwfmOff=0;
    *(int *)precord->valg = TBTwfmLen;
    *(int *)precord->valh = TBTwfmOff;

    float Awfm[8000],Bwfm[8000],Cwfm[8000],Dwfm[8000],Twfm[8000];
    chid Apv,Bpv,Cpv,Dpv,Tpv;

    ca_context_create(ca_enable_preemptive_callback);
    strcpy(pvname,PREFIX);
    strcat(pvname,"Live:ADC:A-Wfm");
    ca_create_channel(pvname,NULL,NULL,0,&Apv);
    strcpy(pvname,PREFIX);
    strcat(pvname,"Live:ADC:B-Wfm");
    ca_create_channel(pvname,NULL,NULL,0,&Bpv);
    strcpy(pvname,PREFIX);
    strcat(pvname,"Live:ADC:C-Wfm");
    ca_create_channel(pvname,NULL,NULL,0,&Cpv);
    strcpy(pvname,PREFIX);
    strcat(pvname,"Live:ADC:D-Wfm");
    ca_create_channel(pvname,NULL,NULL,0,&Dpv);
    strcpy(pvname,PREFIX);
    strcat(pvname,"Live:ADC:Time-Wfm");
    ca_create_channel(pvname,NULL,NULL,0,&Tpv);
    ca_pend_io(5);

    for(i=ADCwfmOff;i<ADCwfmOff+ADCwfmLen;i++){
       Awfm[i-ADCwfmOff] = Abuff[i];
       Bwfm[i-ADCwfmOff] = Bbuff[i];
       Cwfm[i-ADCwfmOff] = Cbuff[i];
       Dwfm[i-ADCwfmOff] = Dbuff[i];
       Twfm[i-ADCwfmOff] = (i-ADCwfmOff)*0.008521583;
    }
    ca_array_put(DBR_FLOAT,ADCwfmLen,Apv,Awfm);
    ca_array_put(DBR_FLOAT,ADCwfmLen,Bpv,Bwfm);
    ca_array_put(DBR_FLOAT,ADCwfmLen,Cpv,Cwfm);
    ca_array_put(DBR_FLOAT,ADCwfmLen,Dpv,Dwfm);
    ca_array_put(DBR_FLOAT,ADCwfmLen,Tpv,Twfm);
    ca_pend_io(5);

// TBT Live:

    float Xwfm[8000],Ywfm[8000],Xsum=0,Ysum=0,Xavg,Yavg,Xsig,Ysig;
    chid Xpv,Ypv,TMpv;

    ca_context_create(ca_enable_preemptive_callback);
    strcpy(pvname,PREFIX);
    strcat(pvname,"Live:TBT:X-Wfm");
    ca_create_channel(pvname,NULL,NULL,0,&Xpv);
    strcpy(pvname,PREFIX);
    strcat(pvname,"Live:TBT:Y-Wfm");
    ca_create_channel(pvname,NULL,NULL,0,&Ypv);
    strcpy(pvname,PREFIX);
    strcat(pvname,"Live:TBT:Time-Wfm");
    ca_create_channel(pvname,NULL,NULL,0,&TMpv);
    ca_pend_io(5);

    for(i=TBTwfmOff;i<TBTwfmOff+TBTwfmLen;i++){
       Xwfm[i-TBTwfmOff] = Xbuff[i];
       Xsum = Xsum + Xbuff[i];
       Ywfm[i-TBTwfmOff] = Ybuff[i];
       Ysum = Ysum + Ybuff[i];
       Twfm[i-TBTwfmOff] = (i-TBTwfmOff)*0.002641691;
    }

    Xavg = Xsum/TBTwfmLen;
    Yavg = Ysum/TBTwfmLen;
    *(float *)precord->vala = Xavg;
    *(float *)precord->valb = Yavg;

    Xsum=0;
    Ysum=0;
    for(i=0;i<TBTwfmLen;i++){
        Xsum = Xsum + (Xwfm[i]-Xavg)*(Xwfm[i]-Xavg);
        Ysum = Ysum + (Ywfm[i]-Yavg)*(Ywfm[i]-Yavg);
    }

    Xsig = sqrt(Xsum/TBTwfmLen);
    Ysig = sqrt(Ysum/TBTwfmLen);
    *(float *)precord->valc = Xsig;
    *(float *)precord->vald = Ysig;

    ca_array_put(DBR_FLOAT,TBTwfmLen,Xpv,Xwfm);
    ca_array_put(DBR_FLOAT,TBTwfmLen,Ypv,Ywfm);
    ca_array_put(DBR_FLOAT,TBTwfmLen,TMpv,Twfm);
    ca_pend_io(5);

    return(0);
}
// Note the function must be registered at the end!
epicsRegisterFunction(LiveSub);
