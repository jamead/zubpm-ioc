TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = zbpm3
# zbpm3.dbd will be created and installed
DBD += zbpm3.dbd

# zbpm3.dbd will be made up from these files:
zbpm3_DBD += base.dbd

# Include dbd files from all support applications:
#zbpm3_DBD += xxx.dbd
zbpm3_DBD += zbpm3Subs.dbd
zbpm3_DBD += pscCore.dbd

# Add all the support libraries needed by this IOC
#zbpm3_LIBS += xxx
zbpm3_LIBS += pscCore

# zbpm3_registerRecordDeviceDriver.cpp derives from zbpm3.dbd
zbpm3_SRCS += zbpm3_registerRecordDeviceDriver.cpp
zbpm3_SRCS += LiveSub.c
zbpm3_SRCS += SAsub.c
zbpm3_SRCS += FAsub.c
zbpm3_SRCS += TBTsub.c
zbpm3_SRCS += ADCsub.c

# Build the main IOC entry point on workstation OSs.
zbpm3_SRCS_DEFAULT += zbpm3Main.cpp
zbpm3_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#zbpm3_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
zbpm3_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

