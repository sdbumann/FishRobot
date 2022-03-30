# What program(s) have to be built
PROGRAMS = robot

# Libraries needed for the executable file
LIBS = -lwsock32

# Dependencies for the program(s) to build
robot: ../common/remregs.o ../common/wperror.o ../common/robot.o ../common/netutil.o ../common/wperror.o ../common/trkcli.o ../common/utils.o robot.o

# Includes the common Makefile with the various rules
include ../common/Makefile.inc
