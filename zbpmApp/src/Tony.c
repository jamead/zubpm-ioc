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

#include <aSubRecord.h>
#include <registryFunction.h>
#include <epicsExport.h>

#define BUFMAX 115220

void linear_regression(float x[], float y[], int n, double* a, double* b, double* r, float* Tmax, float* Tmin) {
    double sum_x = 0.0, sum_y = 0.0;
    double sum_xy = 0.0, sum_x2 = 0.0, sum_y2=0.0;
    float xmin=999,xmax=0;

    for (int i = 0; i < n; i++) {
        if(x[i]>xmax) xmax = x[i];
        if(x[i]<xmin) xmin = x[i];
        sum_x += (double)x[i];
        sum_y += (double)y[i];
        sum_xy += (double)x[i] * (double)y[i];
        sum_x2 += (double)x[i] * (double)x[i];
        sum_y2 += (double)y[i] * (double)y[i];
    }

    *b = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x * sum_x);
    *a = (sum_y - (*b) * sum_x) / n;
// Correlation coefficient (r)
    double corr_den = sqrt((n * sum_x2 - sum_x * sum_x) * (n * sum_y2 - sum_y * sum_y));
    *r = (n * sum_xy - sum_x * sum_y) / corr_den;
    *Tmax = xmax;
    *Tmin = xmin;
}

static long Tony(aSubRecord *precord) {
    static float Abuff[BUFMAX],Bbuff[BUFMAX],Cbuff[BUFMAX],Dbuff[BUFMAX];
    static float ACbuff[BUFMAX],BDbuff[BUFMAX],Xbuff[BUFMAX],Ybuff[BUFMAX];

    float Awfm[15000],Bwfm[15000],Cwfm[15000],Dwfm[15000];
    float ACwfm[15000],BDwfm[15000],Twfm[15000],Xwfm[15000],Ywfm[15000];
    int i,wfmCnt;
    float Xsum,Ysum,Xavg,Yavg,Xsig,Ysig,Tmax,Tmin;
    double m,b,r;

    static int topIndx=0,Count=0,PVset=0;

    double A = *(double *)precord->a;
    double B = *(double *)precord->b;
    double C = *(double *)precord->c;
    double D = *(double *)precord->d;
    double AC = *(double *)precord->e;
    double BD = *(double *)precord->f;
    int length = *(int *)precord->g;
    int decimate = *(int *)precord->h;
    int reset = *(int *)precord->i;
    float X = *(float *)precord->j;
    float Y = *(float *)precord->k;
    int sensor = *(int *)precord->l;
    int axis = *(int *)precord->m;

    printf("%5.3f %5.3f %5.3f %5.3f\n",A,B,C,D);
//    printf("%d %d %d\n",length,decimate,reset);

    if(A>0){
        Abuff[topIndx] = (float)A;
        Bbuff[topIndx] = (float)B;
        Cbuff[topIndx] = (float)C;
        Dbuff[topIndx] = (float)D;
        ACbuff[topIndx] = (float)AC;
        BDbuff[topIndx] = (float)BD;
        Xbuff[topIndx] = X;
        Ybuff[topIndx] = Y;

        wfmCnt = 0;
        Xsum = 0;
        Ysum = 0;
        if(Count<BUFMAX){
            Count = Count + 1;
            int endIndx = topIndx - topIndx%decimate;
            if((length*decimate)>endIndx) length = endIndx/decimate;
            int startIndx = endIndx - (length)*decimate;
//            printf("startIndx=%d endIndx=%d\n",startIndx,endIndx);
            for(i=startIndx;i<=endIndx;i=i+decimate){
//                printf("Awfm[%d]=Abuff[%d]=%5.3f\n",wfmCnt,i,Abuff[i]);
                Awfm[wfmCnt] = Abuff[i];
                Bwfm[wfmCnt] = Bbuff[i];
                Cwfm[wfmCnt] = Cbuff[i];
                Dwfm[wfmCnt] = Dbuff[i];
                ACwfm[wfmCnt] = ACbuff[i];
                BDwfm[wfmCnt] = BDbuff[i];
                Xwfm[wfmCnt] = Xbuff[i];
                Ywfm[wfmCnt] = Ybuff[i];
                Xsum = Xsum + Xbuff[i];
                Ysum = Ysum + Ybuff[i];
                Twfm[wfmCnt] = (float)wfmCnt/60.0;
                wfmCnt = wfmCnt + 1;
            }
        }else{
            int endIndx = topIndx - topIndx%decimate;
            int startIndx = endIndx - (length-1)*decimate;
//            printf("startIndx=%d endIndx=%d\n",startIndx,endIndx);
            if(topIndx<length*decimate){
                for(i=(BUFMAX+topIndx-length*decimate+1);i<BUFMAX;i=i+decimate){
//                    printf("Awfm[%d]=Abuff[%d]=%5.3f\n",wfmCnt,i,Abuff[i]);
                    Awfm[wfmCnt] = Abuff[i];
                    Bwfm[wfmCnt] = Bbuff[i];
                    Cwfm[wfmCnt] = Cbuff[i];
                    Dwfm[wfmCnt] = Dbuff[i];
                    ACwfm[wfmCnt] = ACbuff[i];
                    BDwfm[wfmCnt] = BDbuff[i];
                    Xwfm[wfmCnt] = Xbuff[i];
                    Ywfm[wfmCnt] = Ybuff[i];
                    Xsum = Xsum + Xbuff[i];
                    Ysum = Ysum + Ybuff[i];
                    Twfm[wfmCnt] = (float)wfmCnt/60.0;
                    wfmCnt = wfmCnt + 1;
                }
                startIndx = 0;
            }
            for(i=startIndx;i<=endIndx;i=i+decimate){
//                printf("Awfm[%d]=Abuff[%d]=%5.3f\n",wfmCnt,i,Abuff[i]);
                Awfm[wfmCnt] = Abuff[i];
                Bwfm[wfmCnt] = Bbuff[i];
                Cwfm[wfmCnt] = Cbuff[i];
                Dwfm[wfmCnt] = Dbuff[i];
                ACwfm[wfmCnt] = ACbuff[i];
                BDwfm[wfmCnt] = BDbuff[i];
                Twfm[wfmCnt] = (float)wfmCnt/60.0;
                Xwfm[wfmCnt] = Xbuff[i];
                Ywfm[wfmCnt] = Ybuff[i];
                Xsum = Xsum + Xbuff[i];
                Ysum = Ysum + Ybuff[i];
                wfmCnt = wfmCnt + 1;
            }
        }

        topIndx = topIndx + 1;
        if(topIndx==BUFMAX) topIndx = 0;

        Xavg = Xsum/wfmCnt;
        Yavg = Ysum/wfmCnt;
        if(wfmCnt>5){
            Xsum = 0;
            Ysum = 0;
            for(i=0;i<wfmCnt;i++){
                Xsum = Xsum + (Xwfm[i]-Xavg)*(Xwfm[i]-Xavg);
                Ysum = Ysum + (Ywfm[i]-Yavg)*(Ywfm[i]-Yavg);
            }
            Xsig = sqrt(Xsum/wfmCnt);
            Ysig = sqrt(Ysum/wfmCnt);
            if(topIndx%10==0){
                if(sensor==0 && axis==0) linear_regression(Awfm,Xwfm,wfmCnt,&b,&m,&r,&Tmax,&Tmin);
                if(sensor==0 && axis==1) linear_regression(Awfm,Ywfm,wfmCnt,&b,&m,&r,&Tmax,&Tmin);
                if(sensor==1 && axis==0) linear_regression(Bwfm,Xwfm,wfmCnt,&b,&m,&r,&Tmax,&Tmin);
                if(sensor==1 && axis==1) linear_regression(Bwfm,Ywfm,wfmCnt,&b,&m,&r,&Tmax,&Tmin);
                if(sensor==2 && axis==0) linear_regression(Cwfm,Xwfm,wfmCnt,&b,&m,&r,&Tmax,&Tmin);
                if(sensor==2 && axis==1) linear_regression(Cwfm,Ywfm,wfmCnt,&b,&m,&r,&Tmax,&Tmin);
                if(sensor==3 && axis==0) linear_regression(Dwfm,Xwfm,wfmCnt,&b,&m,&r,&Tmax,&Tmin);
                if(sensor==3 && axis==1) linear_regression(Dwfm,Ywfm,wfmCnt,&b,&m,&r,&Tmax,&Tmin);
                if(sensor==4 && axis==0) linear_regression(ACwfm,Xwfm,wfmCnt,&b,&m,&r,&Tmax,&Tmin);
                if(sensor==4 && axis==1) linear_regression(ACwfm,Ywfm,wfmCnt,&b,&m,&r,&Tmax,&Tmin);
                if(sensor==5 && axis==0) linear_regression(ACwfm,Xwfm,wfmCnt,&b,&m,&r,&Tmax,&Tmin);
                if(sensor==5 && axis==1) linear_regression(ACwfm,Ywfm,wfmCnt,&b,&m,&r,&Tmax,&Tmin);
//                printf("m=%.4f  b=%.3f  r=%.3f  Tmax=%.3f  Tmin=%.3f\n",m,b,r,Tmax,Tmin);
                float dT = (Tmax - Tmin)/100.0;
                float Tfit[101],Pfit[101];
                for(i=0;i<=100;i=i+1){
                    Tfit[i] = Tmin + i*dT;
                    Pfit[i] = (float)m*Tfit[i] + (float)b;
                }
                *(float *)precord->valn = (float)m;
                *(float *)precord->valo = (float)b;
                *(float *)precord->valp = (float)r;
                memcpy((float *)precord->valq,Tfit,101*sizeof(float));
                memcpy((float *)precord->valr,Pfit,101*sizeof(float));
            }
        }else{
            Xsig = 0.0;
            Ysig = 0.0;
        }

//  Calculate linear regression lines for X and Y versus T



        precord->neva = wfmCnt;
        precord->nevb = wfmCnt;
        precord->nevc = wfmCnt;
        precord->nevd = wfmCnt;
        precord->neve = wfmCnt;
        precord->nevf = wfmCnt;
        precord->nevg = wfmCnt;
        precord->nevh = wfmCnt;
        precord->nevi = wfmCnt;
        memcpy((float *)precord->vala,Awfm,wfmCnt*sizeof(float));
        memcpy((float *)precord->valb,Bwfm,wfmCnt*sizeof(float));
        memcpy((float *)precord->valc,Cwfm,wfmCnt*sizeof(float));
        memcpy((float *)precord->vald,Dwfm,wfmCnt*sizeof(float));
        memcpy((float *)precord->vale,ACwfm,wfmCnt*sizeof(float));
        memcpy((float *)precord->valf,BDwfm,wfmCnt*sizeof(float));
        memcpy((float *)precord->valg,Twfm,wfmCnt*sizeof(float));
        memcpy((float *)precord->valh,Xwfm,wfmCnt*sizeof(float));
        memcpy((float *)precord->vali,Ywfm,wfmCnt*sizeof(float));
        *(float *)precord->valj = Xavg;
        *(float *)precord->valk = Xsig;
        *(float *)precord->vall = Yavg;
        *(float *)precord->valm = Ysig;
    }
    return 0;
}
epicsRegisterFunction(Tony);
