GPP = g++
GCC = gcc
CMDS_OUTFILE = "samp-commands.so"

COMPILE_FLAGS = -std=c++0x -m32 -fPIC -c -lrt -w -DLINUX -DSAMPGDK_AMALGAMATION -D_GNU_SOURCE -fpermissive -I ../ -D PROJECT_NAME=\"samp-commands\"

samp-commands = -D samp-commands $(COMPILE_FLAGS)

all: samp-commands

clean:
	-rm -f *~ *.o *.so

samp-commands: clean
	$(GPP) $(samp-commands) ../*.cpp
	$(GPP) -m32 -fshort-wchar -lrt  -shared -o $(CMDS_OUTFILE) *.o

