CXX = g++										# compiler
CXXFLAGS = -g -L. -Wall -lrpc -lpthread

OBJECTS1 = stringServer.o server.o
EXEC1 = server

OBJECTS2 = stringClient.o
EXEC2 = client

OBJECTS3 = stringClient.o
EXEC3 = binder

OBJECTS = ${OBJECTS1} ${OBJECTS2} ${OBJECTS3}				# all object files
DEPENDS = ${OBJECTS:.o=.d}						# substitute ".o" with ".d"
EXECS = ${EXEC1} ${EXEC2}	${EXEC3}					# all executables

.PHONY : all clean

all : ${EXECS}									# build all executables

#############################################################

${EXEC1} : ${OBJECTS1}							# link step 1st executable
	${CXX} ${CXXFLAGS} $^ -o $@

${EXEC2} : ${OBJECTS2}
	${CXX} ${CXXFLAGS} $^ -o $@

${EXEC3} : ${OBJECTS3}
	${CXX} ${CXXFLAGS} $^ -o $@

#############################################################

-include ${DEPENDS}

clean :											# remove files that can be regenerated
	rm -f *.d *.o ${EXECS} ImplType
