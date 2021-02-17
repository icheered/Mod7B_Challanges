
CXXFLAGS	= -std=gnu++11
LDFLAGS		= -lpthread

SRCS	=	$(wildcard framework/*.cpp) \
			$(wildcard my_protocol/*.cpp)
OBJS	=	$(SRCS:.cpp=.o)


macchallenge:	$(OBJS)
	g++ $(LDFLAGS) $(OBJS) -o macchallenge
	
debug:	$(OBJS)
	g++ -g3 $(LDFLAGS) $(OBJS) -o macchallenge

clean:
	rm $(OBJS)
	rm macchallenge
