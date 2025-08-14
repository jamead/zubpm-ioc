#!../../bin/linux-x86_64/zbpm

#- SPDX-FileCopyrightText: 2005 Argonne National Laboratory
#-
#- SPDX-License-Identifier: EPICS

#- You may have to change zbpm to something else
#- everywhere it appears in this file

#< envPaths
epicsEnvSet("PX","lab-BI{BPM:2}")

epicsEnvSet("IOCNAME", "lab-BI")
epicsEnvSet("ALIVELEN", "8000");   # ADC Live length
epicsEnvSet("TLIVELEN", "8000");   # ADC Live length
epicsEnvSet("ALEN",100000);        # ADC DMA Length
epicsEnvSet("TLEN", "100000");      # TbT Length
epicsEnvSet("FLEN", "100000");      # FA Length

## Register all support components
dbLoadDatabase "../../dbd/zbpm.dbd"
zbpm_registerRecordDeviceDriver(pdbbase) 

# BPM IP address
epicsEnvSet("ZBPM_IP", "10.0.142.43");  #4030

## Load record instances
#dbLoadRecords("../../db/zbpm.db","user=diag")
dbLoadRecords("../../db/zubpm.db", "P=$(IOCNAME), NO=2, ADC_LIVE_WFM_LEN=$(ALIVELEN), ADC_WFM_LEN=$(ALEN), TBT_LIVE_WFM_LEN=$(TLIVELEN), TBT_WFM_LEN=$(TLEN), SAVG_N=100, SAVG_NSAM=100")
dbLoadRecords("../../db/Live.db", "P=$(IOCNAME), NO=2, ALIVE=$(ALIVELEN), TLIVE=$(TLIVELEN)")
dbLoadRecords("../../db/FA.db", "P=$(IOCNAME), NO=2, FA_LEN=$(FLEN)")
dbLoadRecords("../../db/TBT.db", "P=$(IOCNAME), NO=2, TBT_LEN=$(TLEN)")
dbLoadRecords("../../db/ADC.db", "P=$(IOCNAME), NO=2, ADC_LEN=$(ALEN)")
dbLoadRecords("../../db/SFP.db", "P=$(IOCNAME), B=2, S=0, TMP=140, VCC=164, TXB=188, TXP=212, RXP=236")
dbLoadRecords("../../db/SFP.db", "P=$(IOCNAME), B=2, S=1, TMP=144, VCC=168, TXB=192, TXP=216, RXP=240")
#dbLoadRecords("../../db/SFP.db", "P=$(IOCNAME), B=2, S=2, TMP=148, VCC=172, TXB=196, TXP=220, RXP=244")
#dbLoadRecords("../../db/SFP.db", "P=$(IOCNAME), B=2, S=3, TMP=152, VCC=176, TXB=200, TXP=224, RXP=248")
#dbLoadRecords("../../db/SFP.db", "P=$(IOCNAME), B=2, S=4, TMP=156, VCC=180, TXB=204, TXP=228, RXP=252")
#dbLoadRecords("../../db/SFP.db", "P=$(IOCNAME), B=2, S=5, TMP=160, VCC=184, TXB=208, TXP=232, RXP=256")
dbLoadRecords("../../db/SA.db", "P=$(IOCNAME), NO=2")
dbLoadRecords("../../db/Power.db", "P=$(IOCNAME), NO=2")
dbLoadRecords("../../db/Temp.db", "P=$(IOCNAME), NO=2")
dbLoadRecords("../../db/FFT.db", "P=$(IOCNAME), NO=2")
#####################################################
var(PSCDebug, 1)  #5 full debug

#bpm1 Create the PSC
createPSC("aie_tx_2", $(ZBPM_IP), 7, 0)
createPSC("aie_wfm_rx_2", $(ZBPM_IP), 20, 1)
createPSC("Rx2", $(ZBPM_IP), 600, 1)

iocInit()

## Start any sequence programs
#seq snczbpm,"user=diag"

dbpf $(PX)Gain:RfAtte-SP 4
dbpf $(PX)DMA:FALen-SP 5000
dbpf $(PX)FA:WfmLen-SP 4200
dbpf $(PX)DMA:TBTLen-SP 9000
dbpf $(PX)TBT:WfmLen-SP 8200
dbpf $(PX)DMA:ADCLen-SP 1000
dbpf $(PX)ADC:WfmLen-SP 300


