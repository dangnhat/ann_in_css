#Makefile for CS500 Assignment
#Author: Pham Huu Dang Nhat
#Version: 1.0
#Date: 3-April-2016

#------------------------- CONFIG INTERFACE -----------------------------------#

#location of source files and include files
SRCLOC = .
INCLOC =

#location of binary and object files
BINDIR = bin

#------------------------------------------------------------------------------#
VPATH = ${SRCLOC} ${INCLOC}

CCFLAGS = $(addprefix -I, ${INCLOC}) -std=gnu99 -Og -ggdb
CXXFLAGS = $(addprefix -I, ${INCLOC}) -std=c++11 -Og -ggdb
LKFLAGS = 

CC = gcc
CXX = g++
LK = g++

SRC = $(notdir $(wildcard *.c $(addsuffix *.c, ${SRCLOC} )))
OBJC = $(SRC:%.c=${BINDIR}/%.o)

SRCXX = $(notdir $(wildcard *.cpp $(addsuffix *.cpp, ${SRCLOC} )))
OBJCXX = $(SRCXX:%.cpp=${BINDIR}/%.o)

OBJ += ${OBJC}
OBJ += ${OBJCXX}

.PHONY: all clean

all: exfile
	@echo
	@echo "Finished"

exfile: ${OBJ}
	@echo "Linking..."
	${LK} $^ -o ${BINDIR}/$@

${OBJC}: ${BINDIR}/%.o: %.c
	@mkdir -p ${BINDIR}
	@echo "Compling $< ..."
	${CC}${CCFLAGS} -c $< -o $@
	@echo

${OBJCXX}: ${BINDIR}/%.o: %.cpp
	@mkdir -p ${BINDIR}
	@echo "Compling $< ..."
	${CXX}${CXXFLAGS} -c $< -o $@
	@echo

clean:
	rm -f -r ${BINDIR}
	rm -f *~

