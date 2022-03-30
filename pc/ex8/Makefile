# What program(s) have to be built
PROGRAMS = main

# Libraries needed for the executable file
LIBS = -lwsock32

# Dependencies for the program(s) to build
main: ../common/remregs.o ../common/wperror.o ../common/robot.o ../common/netutil.o ../common/wperror.o ../common/trkcli.o ../common/utils.o main.o

# Includes the common Makefile with the various rules
include ../common/Makefile.inc
