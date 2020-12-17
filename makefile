.PHONY: server client

full:
	make server
	make client
init:
	git clone https://github.com/PrugClem/cppsock.git
	make -C cppsock lib
clear:
	make -C cppsock clear
	rm ./chatwithme ./server || cd .
server:
	echo "making server"
	g++ -pthread -o server simple_server.cpp ./cppsock/out/libcppsock.a
client:
	echo "making client"
	g++ -pthread -o chatwithme chatwithme.cpp cppsock/out/libcppsock.a