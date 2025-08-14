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
#define BUFMAX 576000

static int SAsub(aSubRecord *precord) {

    static float Abuff[BUFMAX],Bbuff[BUFMAX],Cbuff[BUFMAX],Dbuff[BUFMAX];
    static float Sbuff[BUFMAX],Xbuff[BUFMAX],Ybuff[BUFMAX];
    static chid Apv,Bpv,Cpv,Dpv,Spv,Xpv,Ypv,Tpv,TSpv;

    static int topIndx=0,Count=0,PVset=0;

    int i,wfmCnt;
    float Awfm[10000],Bwfm[10000],Cwfm[10000],Dwfm[10000];
    float Swfm[10000],Xwfm[10000],Ywfm[10000],Twfm[10000];
    float XMwfm[100],XSwfm[10],YMwfm[100],YSwfm[10];
    float Aavg,Bavg,Cavg,Davg,Xavg,Yavg,Savg;
    float Asig,Bsig,Csig,Dsig,Xsig,Ysig,Ssig;
    double Asum=0,Bsum=0,Csum=0,Dsum=0,Ssum=0,Xsum=0,Ysum=0;

    double A = *(double *)precord->a;
    double B = *(double *)precord->b;
    double C = *(double *)precord->c;
    double D = *(double *)precord->d;
    double S = *(double *)precord->e;
    double X = *(double *)precord->f;
    double Y = *(double *)precord->g;
    int TrigCnt = *(int *)precord->h;
    int SAlen = *(int *)precord->i;
    int SAdec = *(int *)precord->j;
    int reset = *(int *)precord->k;
    int tsec = *(int *)precord->l;

    time_t evrTime;
    struct tm ts;
    char tmstr[80];

//    if(TrigCnt%10==0) printf("TrigCnt = %d\n",TrigCnt);
    evrTime = (time_t)tsec;
    ts = *localtime(&evrTime);
    strftime(tmstr,sizeof(tmstr), "%b %d %Y %H:%M:%S %Z", &ts);

    if(reset==1){
        printf("Resetting...\n");
        topIndx = 0;
        Count = 0;
        reset = 0;
        *(int *)precord->vals = reset;
    }

    Abuff[topIndx] = (float)A;
    Bbuff[topIndx] = (float)B;
    Cbuff[topIndx] = (float)C;
    Dbuff[topIndx] = (float)D;
    Sbuff[topIndx] = (float)S;
    Xbuff[topIndx] = (float)X;
    Ybuff[topIndx] = (float)Y;

    if(Count<BUFMAX){
        int endIndx = topIndx - topIndx%SAdec;
        if((SAlen*SAdec)>endIndx) SAlen = endIndx/SAdec;
        int startIndx = endIndx - (SAlen-1)*SAdec;
        wfmCnt=0;
        for(i=startIndx;i<=endIndx;i=i+SAdec){
            Awfm[wfmCnt] = Abuff[i];
            Asum = Asum + Abuff[i];
            Bwfm[wfmCnt] = Bbuff[i];
            Bsum = Bsum + Bbuff[i];
            Cwfm[wfmCnt] = Cbuff[i];
            Csum = Csum + Cbuff[i];
            Dwfm[wfmCnt] = Dbuff[i];
            Dsum = Dsum + Dbuff[i];
            Swfm[wfmCnt] = Sbuff[i];
            Ssum = Ssum + Sbuff[i];
            Xwfm[wfmCnt] = Xbuff[i];
            Xsum = Xsum + Xbuff[i];
            Ywfm[wfmCnt] = Ybuff[i];
            Ysum = Ysum + Ybuff[i];
            Twfm[wfmCnt] = 0.001673071*wfmCnt*SAdec;  //Time in Minutes
            wfmCnt = wfmCnt + 1;
        }
    }else{
        int endIndx = topIndx - topIndx%SAdec;
        int startIndx = endIndx - (SAlen-1)*SAdec;
        wfmCnt = 0;
        if(topIndx<SAdec*SAlen){
            for(i=(BUFMAX+topIndx-SAdec*SAlen+1);i<BUFMAX;i=i+SAdec){
                Awfm[wfmCnt] = Abuff[i];
                Asum = Asum + Abuff[i];
                Bwfm[wfmCnt] = Bbuff[i];
                Bsum = Bsum + Bbuff[i];
                Cwfm[wfmCnt] = Cbuff[i];
                Csum = Csum + Cbuff[i];
                Dwfm[wfmCnt] = Dbuff[i];
                Dsum = Dsum + Dbuff[i];
                Swfm[wfmCnt] = Sbuff[i];
                Ssum = Ssum + Sbuff[i];
                Xwfm[wfmCnt] = Xbuff[i];
                Xsum = Xsum + Xbuff[i];
                Ywfm[wfmCnt] = Ybuff[i];
                Ysum = Ysum + Ybuff[i];
                Twfm[wfmCnt] = 0.001673071*wfmCnt*SAdec;  //Time in Minutes
                wfmCnt = wfmCnt + 1;
            }
            startIndx = 0;
        }
        for(i=startIndx;i<=endIndx;i=i+SAdec){
            Awfm[wfmCnt] = Abuff[i];
            Asum = Asum + Abuff[i];
            Bwfm[wfmCnt] = Bbuff[i];
            Bsum = Bsum + Bbuff[i];
            Cwfm[wfmCnt] = Cbuff[i];
            Csum = Csum + Cbuff[i];
            Dwfm[wfmCnt] = Dbuff[i];
            Dsum = Dsum + Dbuff[i];
            Swfm[wfmCnt] = Sbuff[i];
            Ssum = Ssum + Sbuff[i];
            Xwfm[wfmCnt] = Xbuff[i];
            Xsum = Xsum + Xbuff[i];
            Ywfm[wfmCnt] = Ybuff[i];
            Ysum = Ysum + Ybuff[i];
            Twfm[wfmCnt] = 0.001673071*wfmCnt*SAdec;  //Time in Minutes
            wfmCnt = wfmCnt + 1;
        }
    }

    if(wfmCnt>100){
        int indx=0;
        for(i=wfmCnt-100;i<wfmCnt;i++){
            XMwfm[indx] = Xwfm[i];
            YMwfm[indx] = Ywfm[i];
            indx = indx + 1;
        }
        indx=0;
        for(i=wfmCnt-10;i<wfmCnt;i++){
            XSwfm[indx] = Xwfm[i];
            YSwfm[indx] = Ywfm[i];
            indx = indx + 1;
        }
        memcpy((float *)precord->valh,XMwfm,100*sizeof(float));
        memcpy((float *)precord->vali,YMwfm,100*sizeof(float));
        memcpy((float *)precord->valj,XSwfm,10*sizeof(float));
        memcpy((float *)precord->valk,YSwfm,10*sizeof(float));
    }

    Aavg =(float)(Asum/(double)wfmCnt);
    Bavg =(float)(Bsum/(double)wfmCnt);
    Cavg =(float)(Csum/(double)wfmCnt);
    Davg =(float)(Dsum/(double)wfmCnt);
    Savg =(float)(Ssum/(double)wfmCnt);
    Xavg =(float)(Xsum/(double)wfmCnt);
    Yavg =(float)(Ysum/(double)wfmCnt);

    *(float *)precord->vala = Aavg;
    *(float *)precord->valb = Bavg;
    *(float *)precord->valc = Cavg;
    *(float *)precord->vald = Davg;
    *(float *)precord->vale = Savg;
    *(float *)precord->valf = Xavg;
    *(float *)precord->valg = Yavg;

    Asum = 0;
    Bsum = 0;
    Csum = 0;
    Dsum = 0;
    Ssum = 0;
    Xsum = 0;
    Ysum = 0;
    for(i=0;i<wfmCnt;i++){
        Asum = Asum + (Awfm[i]-Aavg)*(Awfm[i]-Aavg);
        Bsum = Bsum + (Bwfm[i]-Bavg)*(Bwfm[i]-Bavg);
        Csum = Csum + (Cwfm[i]-Cavg)*(Cwfm[i]-Cavg);
        Dsum = Dsum + (Dwfm[i]-Davg)*(Dwfm[i]-Davg);
        Ssum = Ssum + (Swfm[i]-Savg)*(Swfm[i]-Savg);
        Xsum = Xsum + (Xwfm[i]-Xavg)*(Xwfm[i]-Xavg);
        Ysum = Ysum + (Ywfm[i]-Yavg)*(Ywfm[i]-Yavg);
    }
    Asig = sqrt(Asum/wfmCnt);
    Bsig = sqrt(Bsum/wfmCnt);
    Csig = sqrt(Csum/wfmCnt);
    Dsig = sqrt(Dsum/wfmCnt);
    Ssig = sqrt(Ssum/wfmCnt);
    Xsig = sqrt(Xsum/wfmCnt);
    Ysig = sqrt(Ysum/wfmCnt);

    *(float *)precord->vall = Asig;
    *(float *)precord->valm = Bsig;
    *(float *)precord->valn = Csig;
    *(float *)precord->valo = Dsig;
    *(float *)precord->valp = Ssig;
    *(float *)precord->valq = Xsig;
    *(float *)precord->valr = Ysig;

    topIndx = topIndx + 1;
    if(Count<BUFMAX) Count = Count + 1;
    if(topIndx==BUFMAX) topIndx = 0;

    if(PVset==0){
        char pvname[100];
        ca_context_create(ca_enable_preemptive_callback);
        strcpy(pvname,PREFIX);
        strcat(pvname,"SA:A-Wfm");
        ca_create_channel(pvname,NULL,NULL,0,&Apv);
        strcpy(pvname,PREFIX);
        strcat(pvname,"SA:B-Wfm");
        ca_create_channel(pvname,NULL,NULL,0,&Bpv);
        strcpy(pvname,PREFIX);
        strcat(pvname,"SA:C-Wfm");
        ca_create_channel(pvname,NULL,NULL,0,&Cpv);
        strcpy(pvname,PREFIX);
        strcat(pvname,"SA:D-Wfm");
        ca_create_channel(pvname,NULL,NULL,0,&Dpv);
        strcpy(pvname,PREFIX);
        strcat(pvname,"SA:Sum-Wfm");
        ca_create_channel(pvname,NULL,NULL,0,&Spv);
        strcpy(pvname,PREFIX);
        strcat(pvname,"SA:X-Wfm");
        ca_create_channel(pvname,NULL,NULL,0,&Xpv);
        strcpy(pvname,PREFIX);
        strcat(pvname,"SA:Y-Wfm");
        ca_create_channel(pvname,NULL,NULL,0,&Ypv);
        strcpy(pvname,PREFIX);
        strcat(pvname,"SA:Time-Wfm");
        ca_create_channel(pvname,NULL,NULL,0,&Tpv);
        strcpy(pvname,PREFIX);
        strcat(pvname,"SA:TrigDate-I");
        ca_create_channel(pvname,NULL,NULL,0,&TSpv);
        ca_pend_io(5);
        PVset = 1;
        printf("Initializing PVs...\n");
    }

    ca_array_put(DBR_FLOAT,wfmCnt,Apv,Awfm);
    ca_array_put(DBR_FLOAT,wfmCnt,Bpv,Bwfm);
    ca_array_put(DBR_FLOAT,wfmCnt,Cpv,Cwfm);
    ca_array_put(DBR_FLOAT,wfmCnt,Dpv,Dwfm);
    ca_array_put(DBR_FLOAT,wfmCnt,Spv,Swfm);
    ca_array_put(DBR_FLOAT,wfmCnt,Xpv,Xwfm);
    ca_array_put(DBR_FLOAT,wfmCnt,Ypv,Ywfm);
    ca_array_put(DBR_FLOAT,wfmCnt,Tpv,Twfm);
    ca_put(DBR_STRING,TSpv,(void *)&tmstr);
    ca_pend_io(5);

    return(0);
}
// Note the function must be registered at the end!
epicsRegisterFunction(SAsub);
