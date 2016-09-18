# Directories
PREFIX = /usr/local

# Deploy source to other directories via this variable.
# See target titled 'depd' for more information.
DIR =

# You probably don't need to touch this
ARCHIVEDIR=..
MANPREFIX = ${PREFIX}/share/man
LDDIRS = -L$(PREFIX)/lib
LDFLAGS = -lSDL -lm
DSOFLAGS = -fPIC -shared 
DSOEXT = so 
SHARED_LDFLAGS =
CC = cc
#CC = clang

# What follows is a list of defines for compiling in certain features:
# -DCV_PRIMBOOLS      - Let drawing primitives return bools to combine calls
#	-DCV_VERBOSITY      - Be verbose (levels can be 1 - 4?)
#	-DCV_FASTLINES      - Use fast line optimizations
#	-DCV_BOUND_CHECK    - Do bounds checking at plot
#	-DCV_INTERRUPT_PLOT - Use an event to control speed of plotting each point.
#	-DCV_SLOW_PLOT      - Use a timer to slow speed of plotting each point.
# -DCV_SPEED=n        - Define how quickly the timer should fire
# -DCV_GIANT_SCREEN   - Use 32 bit integers for large surfaces
DFLAGS = -DCV_DEBUG \
	-DCV_BOUND_CHECK
	#-DCV_INTERRUPT_PLOT
	#-DTESTS_H #-DFASTLINES #-DSLOW -DSPEED=4000 #-DVERBOSE -DBOUNDS_CHECK

# Performance profiling, comment this if you want none...
# -g allows for source annotations
# -pg allows for regular profiling
# PFLAGS = -g -pg 
# CFLAGS = $(PFLAGS) -fPIC -Wall -Werror -Wno-unused -std=c99 -Wno-deprecated-declarations -O2 -pedantic $(LDDIRS) $(LDFLAGS) $(DFLAGS)
CFLAGS = -g -Wall -Werror -Wno-unused -Wstrict-overflow -ansi -std=c99 -Wno-deprecated-declarations -O2 -pedantic-errors $(LDDIRS) $(LDFLAGS) $(DFLAGS)

# Cygwin 
# CFLAGS = -Wall -Werror -Wno-unused -std=c99 -Wno-deprecated-declarations -O2 -pedantic ${LDFLAGS}
# DSOFLAGS = -fpic
# DSOEXT = dll

# OSX 
# CFLAGS = -Wall -Werror -Wno-unused -std=c99 -Wno-deprecated-declarations -O2 -pedantic ${LDFLAGS}
# DSOFLAGS = -fpic
# DSOEXT = dylib
