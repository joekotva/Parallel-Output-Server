##### User configurable options #####

ARCH        = linux
CC          = mpicc
CLINKER     = mpicc
CCC         = mpic++   # For Karst: mpicxx
CCLINKER    = $(CCC)
F77         = mpif77
FLINKER     = mpif77
OPTFLAGS    = -g 
LIB_PATH    = #-L/usr/lib/lam/lib/
LIB_LIST    = #-lmpi -llam 
INCLUDE_DIR =  -I/usr/include/openmpi-x86_64/

### End User configurable options ###

SHELL = /bin/bash

CFLAGS  =  $(OPTFLAGS) -std=c++11 $(INCLUDE_DIR) -DMPI_$(ARCH)
CCFLAGS = $(CFLAGS)
FFLAGS =  $(OPTFLAGS)
FLIBS = $(FLIB_PATH) $(LIB_LIST)
LIBS = $(LIB_PATH) $(PROFLIB) $(LIB_LIST)
LIBSPP = $(MPIPPLIB) $(LIBS)
EXECS = hellomsg

all: default

default: $(EXECS)

#OBJS = hello.o 

hellomsg: hello_hosts.o $(OBJS) 
	$(CCLINKER) $(OPTFLAGS) -o hellomsg hello_hosts.o $(OBJS) $(LIBS)

clean:
	rm -f *.o $(EXECS)

.c.o:
	$(CC) $(CFLAGS) -w -c $*.c
.cc.o:
	$(CCC) $(CFLAGS) -w -c $*.cc
.f.o:
	$(F77) $(FFLAGS) -w -c $*.f

run:
	mpirun  -np 10 ./hellomsg