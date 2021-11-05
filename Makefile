CC := gcc
CXX := g++
CFLAGS := -Wall

source := server.cc chttp_act.cc

server: $(source)
	$(CXX) -o server $(source)

clean:
	rm -rf sever
