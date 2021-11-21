CC := gcc
CXX := g++
CFLAGS := $(FLAGS) 
CFLAGS += -Wall -Wno-write-strings -g

#source := server.cc chttp_act.cc
INCLUDE := $(wildcard *.hh)
SRCS := $(wildcard *.cc)

http: $(SRCS) $(INCLUDE)
	$(CXX) $(CFLAGS) -o server $(SRCS)

https: $(SRCS) $(INCLUDE)
	$(CXX) $(CFLAGS) -DHTTPS -o server $(SRCS) -lssl -lcrypto

clean:
	rm -rf sever
