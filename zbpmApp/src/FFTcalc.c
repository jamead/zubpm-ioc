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
#include <fftw3.h>
#include <time.h>

#include <aSubRecord.h>
#include <registryFunction.h>
#include <epicsExport.h>

// Apply Hanning window to input data
void apply_hanning(double *data, int N) {
    for (int n = 0; n < N; n++) {
        double w = 0.5 * (1 - cos(2 * M_PI * n / (N - 1)));
        data[n] *= w;
    }
}

float* calc_FFT(double* InWfm, int L){
    float *OutWfm = malloc((L/2 + 1)*sizeof(float));
    
    fftw_complex out[L/2 + 1];  // For real-to-complex transform
    fftw_plan plan;

    // Create plan for forward FFT
    plan = fftw_plan_dft_r2c_1d(L, InWfm, out, FFTW_ESTIMATE);
    
    // Execute the FFT
    fftw_execute(plan);
    
    for (int i = 0; i < L/2 + 1; i++){
       OutWfm[i] = 2*sqrt(out[i][0]*out[i][0] + out[i][1]*out[i][1])/L;
    }
    // Clean up
    fftw_destroy_plan(plan);
    fftw_cleanup();

    return(OutWfm);
}

double* floatToDouble(const float *fArray, int length) {
    if (fArray == NULL || length <= 0) {
        return NULL;
    }

    double *dArray = malloc(length * sizeof(double));
    if (dArray == NULL) {
        // Allocation failed
        return NULL;
    }

    for (int i = 0; i < length; i++) {
        dArray[i] = (double)fArray[i];
    }
    return dArray;
}

static int FFTcalc(aSubRecord *precord) {

    printf("FFTcalc\n");
    int L=0,i;
    int chan = *(int *)precord->a;
    int stream = *(int *)precord->b;
    int unit = *(int *)precord->c;
    float sum,avg,FFT0Hz,df,*Raw,*Input,*Freq,maxFFTout;
    double maxFFTin;

    switch(stream){
        case 0:
            L = *(int *)precord->k;
            if(L>0){
                L = (int)pow(2.0,(int)log2(L));
                Raw = (float *)precord->g;
                Input = malloc(L*sizeof(float));
                Freq = malloc((L/2 +1)*sizeof(float));
                memcpy(Input,Raw,L*sizeof(float));
                df = 117.3491/L; //MHz
                for(i=0;i<(L/2+1);i++){
                    Freq[i] = i*df;
                }
                df = 1000*df; //convert df to KHz for GUI
            }
            break;
        case 1:
            L = *(int *)precord->j;
            if(L>0){
                L = (int)pow(2.0,(int)log2(L));
                Raw = (float *)precord->f;
                Input = malloc(L*sizeof(float));
                Freq = malloc((L/2 +1)*sizeof(float));
                memcpy(Input,Raw,L*sizeof(float));
                sum = 0;
                for(i=0;i<L;i++){
                    sum = sum + Input[i];
                }                
                avg = sum/L;
                for(i=0;i<L;i++){
                    Input[i] = Input[i]-avg;
                }
                df = 378.54545/L;  //KHz
                for(i=0;i<(L/2+1);i++){
                    Freq[i] = i*df;
                }
                df = 1000*df; //Hz for GUI
            }
            break;
        case 2:
            L = *(int *)precord->i;
            if(L>0){
                L = (int)pow(2.0,(int)log2(L));
                Raw = (float *)precord->e;
                Input = malloc(L*sizeof(float));
                Freq = malloc((L/2 +1)*sizeof(float));
                memcpy(Input,Raw,L*sizeof(float));
                sum = 0;
                for(i=0;i<L;i++){
                    sum = sum + Input[i];
                }                
                avg = sum/L;
                for(i=0;i<L;i++){
                    Input[i] = Input[i] - avg;
                }
                df = 9.961722488/L;  //KHz
                for(i=0;i<(L/2+1);i++){
                    Freq[i] = i*df;
                }
                df = 1000*df;  //Hz for GUI
            }
            break;
        case 3:
            L = *(int *)precord->h;
            int decimate = *(int *)precord->l;
            if(L>0){
                L = (int)pow(2.0,(int)log2(L));
                Raw = (float *)precord->d;
                Input = malloc(L*sizeof(float));
                Freq = malloc((L/2 +1)*sizeof(float));
                memcpy(Input,Raw,L*sizeof(float));
                sum = 0;
                for(i=0;i<L;i++){
                    sum = sum + Input[i];
                }                
                avg = sum/L;
                for(i=0;i<L;i++){
                    Input[i] = Input[i] - avg;
                }                
                df = 9.9617222488/(L*decimate);  //Hz
                for(i=0;i<(L/2+1);i++){
                    Freq[i] = i*df;
                }
            }
            break;
        default:
            printf("FFT Stream Error!\n");
            break;
    }
    if(L>0){
        printf("\nStream=%d  Length=%d\n",stream,L);
        double *FFTin = floatToDouble(Input, L);
        float *FFTout;      
        apply_hanning(FFTin,L);
        free(Input);
        maxFFTin=0;
        for(i=0;i<L;i++){
            if(abs(FFTin[i])>0) maxFFTin = abs(FFTin[i]);
        }
        if(maxFFTin>0){
            FFTout = calc_FFT(FFTin,L);
            printf("unit=%d  chan=%d\n",unit,chan);
            if(unit==1){
                for(i=0;i<(L/2+1);i++){
                    FFTout[i] = 20.0*log10(FFTout[i]/32767.0);
                }
            }else if(unit==2){
                maxFFTout=0;
                for(i=0;i<(L/2+1);i++){
                    if(FFTout[i]>maxFFTout) maxFFTout = FFTout[i];
                }
                for(i=0;i<(L/2+1);i++){
                    FFTout[i] = 20.0*log10(FFTout[i]/maxFFTout);
                }
            }
            memcpy((float *)precord->vala,FFTout,(L/2 + 1)*sizeof(float));
            memcpy((float *)precord->valb,Freq,(L/2 + 1)*sizeof(float));
            precord->neva = (L/2 + 1);
            precord->nevb = (L/2 + 1);
            *(float *)precord->valc = df;
            *(float *)precord->vald = avg;
            *(int *)precord->vale = L;
            free(FFTout);
        }            
        free(FFTin);
        free(Freq);
    }
    return(0);
}
// Note the function must be registered at the end!
epicsRegisterFunction(FFTcalc);

