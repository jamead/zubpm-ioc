import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import TextBox

props = dict(boxstyle='round', facecolor='wheat', alpha=0.8)

A=[]
B=[]
C=[]
D=[]
X=[]
Y=[]
TA=[]
TB=[]
TC=[]
TD=[]
TAC=[]
TBD=[]
TM=[]
t=0
N=0

with open('SALogRFSWon2.txt', 'r') as file:
    for line in file:
        line = line.strip()
        Dat = [float(l) for l in line.split(",")]
        if(len(Dat)==12):
           A.append(Dat[0])
           B.append(Dat[1])
           C.append(Dat[2])
           D.append(Dat[3])
           X.append(Dat[4])
           Y.append(Dat[5])
           TA.append(Dat[6])
           TB.append(Dat[7])
           TC.append(Dat[8])
           TD.append(Dat[9])
           TAC.append(Dat[10])
           TBD.append(Dat[11])
           TM.append(t/3600.0)
           t = t + 0.25
           N = N+1

    TM = TM[N-230401:N-1]
    TM = np.subtract(TM,TM[0])
    X = X[N-230401:N-1]
    Y = Y[N-230401:N-1]
    Xsig = np.std(X)
    Ysig = np.std(Y)
    f,ax = plt.subplots(2,1,figsize=(8,8))
    ax[0].plot(TM,X)
    ax[0].grid(True)
    ax[0].set_xlabel("Time (Hours)")
    ax[0].set_ylabel("X Position (microns)")
    dstr = "Xsig = "+str(round(Xsig,3))+"um"
    ax[0].text(0.05, 0.95,dstr,transform=ax[0].transAxes, fontsize=12,verticalalignment='top', bbox=props)

    ax[1].plot(TM,Y)
    ax[1].grid(True)
    ax[1].set_xlabel("Time (Hours)")
    ax[1].set_ylabel("Y Position (microns)")
    dstr = "Ysig = "+str(round(Ysig,3))+"um"
    ax[1].text(0.05, 0.95,dstr,transform=ax[1].transAxes, fontsize=12,verticalalignment='top', bbox=props)

    TA = TA[N-230401:N-1]
    TC = TC[N-230401:N-1]
    TAC = TAC[N-230401:N-1]
    TBD = TBD[N-230401:N-1]

    px = np.polyfit(TA,X,1)
    py = np.polyfit(TA,Y,1)
    
    Xcorr = np.corrcoef(TA,X)[0][1]
    Ycorr = np.corrcoef(TA,Y)[0][1]
    
    Xfit=[]
    Yfit=[]
    for t in TA:
       Xfit.append(px[0]*t + px[1])
       Yfit.append(py[0]*t + py[1])
 
    f,ax = plt.subplots(2,1,figsize=(8,8))
    ax[0].plot(TA,X,'o',markersize=2)
    ax[0].plot(TA,Xfit)
    ax[0].grid(True)
    ax[0].set_xlabel("TA Temperature (C)")
    ax[0].set_ylabel("X Position (microns)")
    dstr = "Slope = "+str(round(px[0],3))+"um/C\nIntcp  = "+str(round(px[1],2))+"um\nCorr = "+str(round(Xcorr,3))
    ax[0].text(0.05, 0.95,dstr,transform=ax[0].transAxes, fontsize=12,verticalalignment='top', bbox=props)
    
    ax[1].plot(TA,Y,'o',markersize=2)
    ax[1].plot(TA,Yfit)
    ax[1].grid(True)
    ax[1].set_xlabel("TA Temperature (C)")
    ax[1].set_ylabel("Y Position (microns)")
    dstr = "Slope = "+str(round(py[0],3))+"um/C\nIntcp  = "+str(round(py[1],2))+"um\nCorr = "+str(round(Ycorr,3))
    ax[1].text(0.05, 0.95,dstr,transform=ax[1].transAxes, fontsize=12,verticalalignment='top', bbox=props)       

    A0 = TA[0]
    C0 = TC[0]
    dT = abs(A0-C0)
    TAo = np.subtract(TA,A0)
    TCo = np.subtract(TC,C0)
    
    f,ax = plt.subplots(2,1,figsize=(8,8))
    ax[0].plot(TM,TAo)
    ax[0].plot(TM,TCo)
    ax[0].grid(True)
    ax[0].set_xlabel("Time (Hours)")
    ax[0].set_ylabel("Thermistors A and C (C)")
    dstr = "TA[0] = "+str(round(A0,2))+"C\nTC[0]  = "+str(round(C0,2))+"C\ndT = "+str(round(dT,2))+"C"
    ax[0].text(0.75, 0.95,dstr,transform=ax[0].transAxes, fontsize=12,verticalalignment='top', bbox=props) 
    
    TAmC = np.subtract(TA,TC)
    
    ax[1].plot(TM,TAmC)
    ax[1].grid(True)
    ax[1].set_xlabel("Time (Hours)")
    ax[1].set_ylabel("Temperature Difference Between TA and TC (C)")
    
    AC0 = TAC[0]
    BD0 = TBD[0]
    dT = abs(AC0-BD0)
    TACo = np.subtract(TAC,AC0)
    TBDo = np.subtract(TBD,BD0)
    
    f,ax = plt.subplots(2,1,figsize=(8,8))
    ax[0].plot(TM,TACo)
    ax[0].plot(TM,TBDo)
    ax[0].grid(True)
    ax[0].set_xlabel("Time (Hours)")
    ax[0].set_ylabel("Thermistors AC and BD (C)")
    dstr = "TAC[0] = "+str(round(AC0,2))+"C\nTBD[0]  = "+str(round(BD0,2))+"C\ndT = "+str(round(dT,2))+"C"
    ax[0].text(0.7, 0.95,dstr,transform=ax[0].transAxes, fontsize=12,verticalalignment='top', bbox=props) 
    
    TACBD = np.subtract(TACo,TBDo)
    
    ax[1].plot(TM,TACBD)
    ax[1].grid(True)
    ax[1].set_xlabel("Time (Hours)")
    ax[1].set_ylabel("Temperature Difference Between TAC and TBD (C)")

    plt.show()

