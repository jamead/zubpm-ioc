from cothread.catools import caget
from time import sleep
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import TextBox

props = dict(boxstyle='round', facecolor='wheat', alpha=0.7)


Frf = 499.68         #MHz
Fturn = Frf/1320.0   #MHZ
Fadc = Fturn*310.0   #MHz
Ffa = Fturn/38.0     #MHz

Apv = "lab-BI{BPM:2}ADC:A-Wfm"
Bpv = "lab-BI{BPM:2}ADC:B-Wfm"
Cpv = "lab-BI{BPM:2}ADC:C-Wfm"
Dpv = "lab-BI{BPM:2}ADC:D-Wfm"

A = caget(Apv)
B = caget(Bpv)
C = caget(Cpv)
D = caget(Dpv)

A = A[0:8192]
B = B[0:8192]
C = C[0:8192]
D = D[0:8192]

L = len(A)
df = Fadc/L #MHz

F = []
for i in range(0,int(L/2)):
   F.append(i*df)

H = np.hanning(L)

AH = np.multiply(A,H)  
BH = np.multiply(B,H)
CH = np.multiply(C,H)  
DH = np.multiply(D,H)

Afft = np.fft.fft(AH)/L
Afft = Afft[0:int(L/2)]
Afft = 2.0*np.abs(Afft)
maxA = max(Afft)
Afft = np.multiply(np.log10(np.divide(Afft,maxA)),20)

Bfft = np.fft.fft(BH)/L
Bfft = Bfft[0:int(L/2)]
Bfft = 2.0*np.abs(Bfft)
maxB = max(Bfft)
Bfft = np.multiply(np.log10(np.divide(Bfft,maxB)),20)

Cfft = np.fft.fft(CH)/L
Cfft = Cfft[0:int(L/2)]
Cfft = 2.0*np.abs(Cfft)
maxC = max(Cfft)
Cfft = np.multiply(np.log10(np.divide(Cfft,maxC)),20)

Dfft = np.fft.fft(DH)/L
Dfft = Dfft[0:int(L/2)]
Dfft = 2.0*np.abs(Dfft)
maxD = max(Dfft)
Dfft = np.multiply(np.log10(np.divide(Dfft,maxD)),20)

f,ax = plt.subplots(2,2,figsize=(11,11))

ax[0][0].plot(F,Afft)
ax[0][0].grid(True)
ax[0][0].set_xlabel("Frequency (MHz)")
ax[0][0].set_ylabel("A FFT")

ax[0][1].plot(F,Bfft)
ax[0][1].grid(True)
ax[0][1].set_xlabel("Frequency (MHz)")
ax[0][1].set_ylabel("B FFT")

ax[1][0].plot(F,Dfft)
ax[1][0].grid(True)
ax[1][0].set_xlabel("Frequency (Hz)")
ax[1][0].set_ylabel("D FFT")

ax[1][1].plot(F,Cfft)
ax[1][1].grid(True)
ax[1][1].set_xlabel("Frequency (Hz)")
ax[1][1].set_ylabel("C FFT")

plt.show()
