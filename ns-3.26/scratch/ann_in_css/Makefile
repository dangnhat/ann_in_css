# Makefile for hd_project ns-3 simulation

# program name
PROGRAM=ann_in_css

# waff function
export NS3DIR=${PWD}/../../
export CPUCORES=$(shell nproc)

WAFF=sh -c '\
    CWD="${PWD}";\
    cd ${NS3DIR} >/dev/null;\
    ./waf --cwd="${PWD}"  --run ${PROGRAM} -j${CPUCORES};\
    cd - >/dev/null'

WAFFCLEAN=sh -c '\
    CWD="${PWD}";\
    cd ${NS3DIR} >/dev/null;\
    ./waf clean;\
    cd - >/dev/null'

.PHONY: all clean

all:
	${WAFF}
	
clean:
	${WAFFCLEAN}

