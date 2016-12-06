# Makefile for hd_project ns-3 simulation

# program name
PROGRAM=ann_in_css

# waff function
export NS3DIR=${PWD}../../

WAFF=sh -c '\
    CWD="${PWD}";\
    cd ${NS3DIR} >/dev/null;\
    ./waf --cwd="${CWD}"  --run ${PROGRAM} -j2;\
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

