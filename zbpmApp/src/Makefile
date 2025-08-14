# SPDX-FileCopyrightText: 2003 Argonne National Laboratory
#
# SPDX-License-Identifier: EPICS

TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = zbpm
# zbpm.dbd will be created and installed
DBD += zbpm.dbd

# zbpm.dbd will be made up from these files:
zbpm_DBD += base.dbd

# Include dbd files from all support applications:
#zbpm_DBD += xxx.dbd
zbpm_DBD += zbpmSubs.dbd
zbpm_DBD += pscCore.dbd
#zbpm_DBD += pscSig.dbd

# Add all the support libraries needed by this IOC
#zbpm_LIBS += xxx
zbpm_LIBS += pscCore

# zbpm_registerRecordDeviceDriver.cpp derives from zbpm.dbd
zbpm_SRCS += zbpm_registerRecordDeviceDriver.cpp
zbpm_SRCS += LiveSub.c
zbpm_SRCS += SAsub.c
zbpm_SRCS += FAsub.c
zbpm_SRCS += TBTsub.c
zbpm_SRCS += ADCsub.c
zbpm_SRCS += Tony.c
zbpm_SRCS += FAselect.c
zbpm_SRCS += TBTselect.c
zbpm_SRCS += ADCselect.c
zbpm_SRCS += SAselect.c
zbpm_SRCS += FFTcalc.c

# Build the main IOC entry point on workstation OSs.
zbpm_SRCS_DEFAULT += zbpmMain.cpp
zbpm_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#zbpm_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
zbpm_LIBS += $(EPICS_BASE_IOC_LIBS)
zbpm_SYS_LIBS += fftw3
#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

