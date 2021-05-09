all: pi-actuator-server test-client
serversocket.o: message.h socket.h log.h
	g++ -c -g -lpthread serversocket.cc
log.o: log.cc log.h
	g++ -c -g log.cc
clientsocket.o: message.h socket.h
	g++ -c -g -lpthread cleintsocket.cc
test-client: test-client.o log.o
	g++ test-client.o -lpthread log.o -o test-client
test-client.o: test-client.cc message.h clientsocket.cc
	g++ -c -g test-client.cc
pi-actuator-server: pi-actuator-server.o log.o serversocket.o
	g++ pi-actuator-server.o -lpthread log.o -o pi-actuator-server
pi-actuator-server.o: pi-actuator-server.cc log.h message.h serversocket.cc
	g++ -c -g pi-actuator-server.cc
clean: 
	rm *.o
	rm -f pi-actuator-server
	rm -f test-client
	rm *.txt
