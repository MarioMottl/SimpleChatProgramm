.PHONY: server client

server:
	echo "making server"
	g++ -pthread -o server server.cpp ./cppsock/out/libcppsock.a
client:
	echo "making client"
	g++ -pthread -o chatwithme chatwithme.cpp cppsock/out/libcppsock.a
run:
	./server