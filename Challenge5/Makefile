
# CXXFLAGS	= -std=gnu++11 -Wall
CXXFLAGS	= -std=gnu++11
LDFLAGS		= -lpthread

OBJS	= framework/base64.o framework/DataTable.o framework/LinkLayer.o framework/Packet.o framework/RoutingChallengeClient.o my_protocol/DummyRoute.o my_protocol/DummyRoutingProtocol.o my_protocol/MyRoute.o my_protocol/MyRoutingProtocol.o my_protocol/Program.o

macchallenge:	$(OBJS)
	g++ $(LDFLAGS) $(OBJS) -o dvrchallenge

clean:
	rm $(OBJS)
	rm dvrchallenge
