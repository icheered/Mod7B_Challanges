
# CXXFLAGS	= -std=gnu++11 -Wall
CXXFLAGS	= -std=gnu++11

OBJS	= client.o tcphack.o

tcphack:	$(OBJS)
	g++ $(OBJS) -o tcphack

clean:
	rm $(OBJS)
	rm tcphack
