all: pi-actuator-server
serversocket.o: message.h socket.h log.h connection.h
	g++ -c -g -lpthread serversocket.cc
connection.o: connection.cc connection.h log.h
	g++ -c -g -lpthread -lwiringPi connection.cc
log.o: log.cc log.h
	g++ -c -g log.cc
clientsocket.o: message.h socket.h
	g++ -c -g -lpthread cleintsocket.cc
pi-actuator-server: pi-actuator-server.o log.o serversocket.o connection.o
	g++ pi-actuator-server.o -lpthread -lwiringPi log.o -o pi-actuator-server
pi-actuator-server.o: pi-actuator-server.cc log.h message.h connection.h serversocket.cc
	g++ -c -g pi-actuator-server.cc
clean: 
	rm *.o
	rm -f pi-actuator-server
	rm *.txt
