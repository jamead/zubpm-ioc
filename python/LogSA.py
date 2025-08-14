from cothread.catools import caget
from time import sleep

PVs = []
PVs.append("lab-BI{BPM:2}Ampl:ASA-I")
PVs.append("lab-BI{BPM:2}Ampl:BSA-I")
PVs.append("lab-BI{BPM:2}Ampl:CSA-I")
PVs.append("lab-BI{BPM:2}Ampl:DSA-I")
PVs.append("lab-BI{BPM:2}Pos:X-I")
PVs.append("lab-BI{BPM:2}Pos:Y-I")
PVs.append("lab-BI{BPM:2}Temp:Thermistor:A-I")
PVs.append("lab-BI{BPM:2}Temp:Thermistor:B-I")
PVs.append("lab-BI{BPM:2}Temp:Thermistor:C-I")
PVs.append("lab-BI{BPM:2}Temp:Thermistor:D-I")
PVs.append("lab-BI{BPM:2}Temp:Thermistor:AC-I")
PVs.append("lab-BI{BPM:2}Temp:Thermistor:BD-I")

f = open("SALog.txt","w")

while True:
   Dat = caget(PVs)
   dstr = str(round(Dat[0],2))+","+str(round(Dat[1],2))+","+str(round(Dat[2],2))+","
   dstr = dstr + str(round(Dat[3],2))+","+str(round(Dat[4],3))+","+str(round(Dat[5],3))+","
   dstr = dstr + str(round(Dat[6],3))+","+str(round(Dat[7],3))+","+str(round(Dat[8],3))+","
   dstr = dstr + str(round(Dat[9],3))+","+str(round(Dat[10],3))+","+str(round(Dat[11],3))
   print(dstr)
   f.write("%s\n" % (dstr))
   sleep(1)
