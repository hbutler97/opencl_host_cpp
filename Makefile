ifeq ($(VERBOSE),1)
ECHO := 
else
ECHO := @
endif

# Where is the Altera SDK for OpenCL software?
ifeq ($(wildcard $(ALTERAOCLSDKROOT)),)
$(error Set ALTERAOCLSDKROOT to the root directory of the Altera SDK for OpenCL software installation)
endif
ifeq ($(wildcard $(ALTERAOCLSDKROOT)/host/include/CL/opencl.h),)
$(error Set ALTERAOCLSDKROOT to the root directory of the Altera SDK for OpenCL software installation.)
endif


# Compilation flags: Note Howard...come back and understand these flags
CXXFLAGS = -g -Wall -fmessage-length=0


# OpenCL compile and link flags.
AOCL_COMPILE_CONFIG := $(shell aocl compile-config )
AOCL_LINK_CONFIG := $(shell aocl link-config )


LIBS =

TARGET =	matrix.exe

INC_DIRS := $(ALTERAOCLSDKROOT)/host/include host/inc \
	     common/inc


SRCS = 	host/src/matrix.cpp \
	host/src/opencl_host.cpp \
	common/src/AOCLUtils/*

INCS =	host/inc/opencl_host.h \
	common/inc/*

OBJS =	matrix.o opencl_host.o opencl.o	


# Host executable target.
$(TARGET) : Makefile $(INCS) $(SRCS)
	$(ECHO)$(CXX) $(CPPFLAGS) $(CXXFLAGS) -fPIC $(foreach D,$(INC_DIRS),-I$D) \
			$(AOCL_COMPILE_CONFIG) $(SRCS) $(AOCL_LINK_CONFIG) \
			$(foreach D,$(LIB_DIRS),-L$D) \
			$(foreach L,$(LIBS),-l$L) \
			-o $(TARGET)

all:	$(TARGET)



clean:
	rm -f $(OBJS) $(TARGET)
 