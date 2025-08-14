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
#define BUFMAX 100000
#define SCALE 512.7435

static int TBTsub(aSubRecord *precord) {

    static chid Apv,Bpv,Cpv,Dpv,Spv,Xpv,Ypv,Tpv;
    static int i,PVset=0,oldready=0,oldwfmLen=0,oldwfmOff=0;

    int ready = *(int *)precord->k;
    int wfmLen = *(int *)precord->h;
    int wfmOff = *(int *)precord->i;

    if((oldready==0 && ready==1) || oldwfmLen!=wfmLen || oldwfmOff!=wfmOff){
        int bufLen = *(int *)precord->j;

        if(wfmLen>10000) wfmLen = 10000;
        if(wfmLen<100) wfmLen = 100;
        if(wfmLen>bufLen) wfmLen = bufLen;
        if(wfmOff<0) wfmOff = 0;
        if(wfmOff>(bufLen-wfmLen)) wfmOff = bufLen - wfmLen;
        oldwfmLen = wfmLen;
        oldwfmOff = wfmOff;
        *(int *)precord->vala = wfmLen;
        *(int *)precord->valb = wfmOff;

        printf("TBT\n");
        if(PVset==0){
            char pvname[100];
            ca_context_create(ca_enable_preemptive_callback);
            strcpy(pvname,PREFIX);
            strcat(pvname,"TBT:A-Wfm");
            ca_create_channel(pvname,NULL,NULL,0,&Apv);
            strcpy(pvname,PREFIX);
            strcat(pvname,"TBT:B-Wfm");
            ca_create_channel(pvname,NULL,NULL,0,&Bpv);
            strcpy(pvname,PREFIX);
            strcat(pvname,"TBT:C-Wfm");
            ca_create_channel(pvname,NULL,NULL,0,&Cpv);
            strcpy(pvname,PREFIX);
            strcat(pvname,"TBT:D-Wfm");
            ca_create_channel(pvname,NULL,NULL,0,&Dpv);
            strcpy(pvname,PREFIX);
            strcat(pvname,"TBT:Sum-Wfm");
            ca_create_channel(pvname,NULL,NULL,0,&Spv);
            strcpy(pvname,PREFIX);
            strcat(pvname,"TBT:X-Wfm");
            ca_create_channel(pvname,NULL,NULL,0,&Xpv);
            strcpy(pvname,PREFIX);
            strcat(pvname,"TBT:Y-Wfm");
            ca_create_channel(pvname,NULL,NULL,0,&Ypv);
            strcpy(pvname,PREFIX);
            strcat(pvname,"TBT:Time-Wfm");
            ca_create_channel(pvname,NULL,NULL,0,&Tpv);
            ca_pend_io(5);
            PVset = 1;
            printf("Initializing PVs...\n");
        }

        double *Abuff = (double *)precord->a;
        double *Bbuff = (double *)precord->b;
        double *Cbuff = (double *)precord->c;
        double *Dbuff = (double *)precord->d;
        double *Sbuff = (double *)precord->e;
        double *Xbuff = (double *)precord->f;
        double *Ybuff = (double *)precord->g;

        float Awfm[10000],Bwfm[10000],Cwfm[10000],Dwfm[10000];
        float Swfm[10000],Xwfm[10000],Ywfm[10000],Twfm[10000];
        double Asum=0,Bsum=0,Csum=0,Dsum=0,Ssum=0,Xsum=0,Ysum=0;
        float Aavg,Bavg,Cavg,Davg,Xavg,Yavg,Savg;
        float Asig,Bsig,Csig,Dsig,Xsig,Ysig,Ssig;

        for(i=wfmOff;i<(wfmLen+wfmOff);i++){
            Awfm[i-wfmOff] = Abuff[i]/SCALE;
            Asum = Asum + Abuff[i]/SCALE;
            Bwfm[i-wfmOff] = Bbuff[i]/SCALE;
            Bsum = Bsum + Bbuff[i]/SCALE;
            Cwfm[i-wfmOff] = Cbuff[i]/SCALE;
            Csum = Csum + Cbuff[i]/SCALE;
            Dwfm[i-wfmOff] = Dbuff[i]/SCALE;
            Dsum = Dsum + Dbuff[i]/SCALE;
            Swfm[i-wfmOff] = Sbuff[i]/SCALE;
            Ssum = Ssum + Sbuff[i]/SCALE;
            Xwfm[i-wfmOff] = Xbuff[i]/1000.0;      //X in microns
            Xsum = Xsum + Xbuff[i]/1000.0;
            Ywfm[i-wfmOff] = Ybuff[i]/1000.0;      //Y in microns
            Ysum = Ysum + Ybuff[i]/1000.0;
            Twfm[i-wfmOff] = (i-wfmOff)*0.002642;  //Time in milliseconds
        }

        Aavg = Asum/wfmLen;
        Bavg = Bsum/wfmLen;
        Cavg = Csum/wfmLen;
        Davg = Dsum/wfmLen;
        Savg = Ssum/wfmLen;
        Xavg = Xsum/wfmLen;
        Yavg = Ysum/wfmLen;
        *(float *)precord->valc = Aavg;
        *(float *)precord->vald = Bavg;
        *(float *)precord->vale = Cavg;
        *(float *)precord->valf = Davg;
        *(float *)precord->valg = Savg;
        *(float *)precord->valh = Xavg;
        *(float *)precord->vali = Yavg;

        Asum = 0;
        Bsum = 0;
        Csum = 0;
        Dsum = 0;
        Ssum = 0;
        Xsum = 0;
        Ysum = 0;
        for(i=0;i<wfmLen;i++){
            Asum = Asum + (Awfm[i]-Aavg)*(Awfm[i]-Aavg);
            Bsum = Bsum + (Bwfm[i]-Bavg)*(Bwfm[i]-Bavg);
            Csum = Csum + (Cwfm[i]-Cavg)*(Cwfm[i]-Cavg);
            Dsum = Dsum + (Dwfm[i]-Davg)*(Dwfm[i]-Davg);
            Ssum = Ssum + (Swfm[i]-Savg)*(Swfm[i]-Savg);
            Xsum = Xsum + (Xwfm[i]-Xavg)*(Xwfm[i]-Xavg);
            Ysum = Ysum + (Ywfm[i]-Yavg)*(Ywfm[i]-Yavg);
        }
        Asig = sqrt(Asum/wfmLen);
        Bsig = sqrt(Bsum/wfmLen);
        Csig = sqrt(Csum/wfmLen);
        Dsig = sqrt(Dsum/wfmLen);
        Ssig = sqrt(Ssum/wfmLen);
        Xsig = sqrt(Xsum/wfmLen);
        Ysig = sqrt(Ysum/wfmLen);

        *(float *)precord->valj = Asig;
        *(float *)precord->valk = Bsig;
        *(float *)precord->vall = Csig;
        *(float *)precord->valm = Dsig;
        *(float *)precord->valn = Ssig;
        *(float *)precord->valo = Xsig;
        *(float *)precord->valp = Ysig;

        ca_array_put(DBR_FLOAT,wfmLen,Apv,Awfm);
        ca_array_put(DBR_FLOAT,wfmLen,Bpv,Bwfm);
        ca_array_put(DBR_FLOAT,wfmLen,Cpv,Cwfm);
        ca_array_put(DBR_FLOAT,wfmLen,Dpv,Dwfm);
        ca_array_put(DBR_FLOAT,wfmLen,Spv,Swfm);
        ca_array_put(DBR_FLOAT,wfmLen,Xpv,Xwfm);
        ca_array_put(DBR_FLOAT,wfmLen,Ypv,Ywfm);
        ca_array_put(DBR_FLOAT,wfmLen,Tpv,Twfm);
        ca_pend_io(5);
    }
    oldready = ready;
    return(0);
}
// Note the function must be registered at the end!
epicsRegisterFunction(TBTsub);
