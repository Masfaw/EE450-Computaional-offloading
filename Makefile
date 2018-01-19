all:
	g++ -o client client.cpp -lsocket -lnsl -lresolv
	g++ -o aws.out aws.cpp -lsocket -lnsl -lresolv
	g++ -o serverA.out serverA.cpp -lsocket -lnsl -lresolv
	g++ -o serverB.out serverB.cpp -lsocket -lnsl -lresolv
	g++ -o serverC.out serverC.cpp -lsocket -lnsl -lresolv

serverA:
	./serverA.out
serverB:
	./serverB.out
serverC:
	./serverC.out
aws:
	./aws.out
