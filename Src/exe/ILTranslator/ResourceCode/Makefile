# ILTranslator ResourceCode - MinGW (g++) build
# Usage: make [target]
#   make          -> build outDebug (TestCaseConverter)
#   make clean    -> remove build artifacts

CXX     = g++
CC      = gcc
CXXFLAGS = -std=c++11 -I. -O2 -DNDEBUG
CFLAGS   = -I. -O2
LDFLAGS  =
TARGET   = outDebug

# TestCaseConverter (main in TestCaseConverterMain.cpp)
SRCS_CPP = TestCaseConverterMain.cpp TestCaseConverter.cpp BasicExcel.cpp Utility.cpp
SRCS_C   = LibFuzzerUtility.c
OBJS     = $(SRCS_CPP:.cpp=.o) $(SRCS_C:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	del /Q *.o $(TARGET).exe 2>nul || true
