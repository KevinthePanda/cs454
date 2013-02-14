CXX = g++										# compiler
CXXFLAGS = -g -Wall -Wno-unused-label -MMD -lpthread

OBJECTS1 = stringServer.o
EXEC1 = stringServer

OBJECTS2 = stringClient.o
EXEC2 = stringClient

OBJECTS = ${OBJECTS1} ${OBJECTS2}				# all object files
DEPENDS = ${OBJECTS:.o=.d}						# substitute ".o" with ".d"
EXECS = ${EXEC1} ${EXEC2}						# all executables

.PHONY : all clean

all : ${EXECS}									# build all executables

#############################################################

${EXEC1} : ${OBJECTS1}							# link step 1st executable
	${CXX} ${CXXFLAGS} $^ -o $@

${EXEC2} : ${OBJECTS2}
	${CXX} ${CXXFLAGS} $^ -o $@

#############################################################

-include ${DEPENDS}

clean :											# remove files that can be regenerated
	rm -f *.d *.o ${EXECS} ImplType
