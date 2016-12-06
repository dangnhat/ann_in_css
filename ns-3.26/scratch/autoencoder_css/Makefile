# Makefile for hd_project ns-3 simulation

# program name
PROGRAM=autoencoder_css

# waff function
export NS3DIR="/home/dangnhat/ns/ns-allinone-3.26/ns-3.26/"

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

