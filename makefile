CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-g $(shell root-config --cflags)
LDFLAGS=-g $(shell root-config --ldflags)
LDLIBS=$(shell root-config --libs)

BUILDDIR = build/
SOURCEDIR = src/
SRCS = $(wildcard $(SOURCEDIR)*.cpp)
tmp = $(subst $(SOURCEDIR),$(BUILDDIR),$(SRCS))
OBJS = $(tmp:.cpp=.o)

#$(info Building the following:)
#$(info > SRCS: $(SRCS))

# Default target
spacewire: $(OBJS)
	$(CXX) $^ -o $@

$(BUILDDIR)%.o: $(SOURCEDIR)%.cpp
	$(CXX) -MMD -MP -c $< -o $@

.PHONY: clean

# $(RM) is rm -f by default
clean:
	$(RM) $(OBJS) spacewire
