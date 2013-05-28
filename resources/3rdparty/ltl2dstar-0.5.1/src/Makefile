GPP = g++

#DEFINES  =
#DEBUGFLAGS = -g -fno-default-inline -fkeep-inline-functions
OPTFLAGS = -O3 -fkeep-inline-functions

LIBRARY = libltl2dstar.a
EXECUTABLE = ltl2dstar

CFLAGS = -I. -Wall -DINLINE_BITSET $(DEFINES) $(DEBUGFLAGS) $(OPTFLAGS)

# uncomment the following line to compile the sample plugins
#PLUGINS = plugins/SamplePlugin.o plugins/DRAReachabilityPlugin.o plugins/WriteDRAPlugin.o

LIB_OBJS = $(PLUGINS) common/BitSet.o common/RunProgram.win32.o common/RunProgram.posix.o StutterSensitivenessInformation.o LTL2DRA.o LTL2DSTAR_Scheduler.o APMonom.o parsers/nba-parser-lbtt.lex.o  parsers/nba-parser-lbtt.tab.o parsers/nba-parser-promela.lex.o  parsers/nba-parser-promela.tab.o plugins/PluginManager.o

MAIN_OBJS = ltl2dstar_main.o

OBJS = $(LIB_OBJS) $(MAIN_OBJ)

.PHONY : all lib clean

all : $(EXECUTABLE)

lib : $(LIBRARY)

$(LIBRARY) : $(LIB_OBJS)
	$(AR) rcs $@ $+

$(EXECUTABLE) : $(MAIN_OBJS) $(LIBRARY)
	$(GPP) -g $(LINK_FLAGS) -o $@ $+

%.o : %.cpp
	$(GPP) -c $(CFLAGS) -o $@ $<

clean :
	rm -f ltl2dstar *.o *.a common/*.o parsers/*.o plugins/*.o 
