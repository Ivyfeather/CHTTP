CC := gcc
CXX := g++
CFLAGS := -Wall -Wno-write-strings -g

#source := server.cc chttp_act.cc
INCLUDE := $(wildcard *.hh)
SRCS := $(wildcard *.cc)

server: $(SRCS) $(INCLUDE)
	$(CXX) $(CFLAGS) -o server $(SRCS)

clean:
	rm -rf sever
