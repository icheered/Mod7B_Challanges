
CXXFLAGS	= -std=gnu++11
LDFLAGS		= -lpthread

SRCS	=	$(wildcard framework/*.cpp) \
			$(wildcard my_protocol/*.cpp)
OBJS	=	$(SRCS:.cpp=.o)

drdtchallenge:	$(OBJS)
	g++ $(LDFLAGS) $(OBJS) -o drdtchallenge

debug:	$(OBJS)
	g++ -g3 $(LDFLAGS) $(OBJS) -o drdtchallenge
clean:
	rm $(OBJS)
	rm drdtchallenge
