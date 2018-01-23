.PHONY : all
all:
		g++ -o client client.cc  -lnsl -lresolv
		g++ -o aws aws.cc  -lnsl -lresolv
		g++ -o serverA serverA.cc  -lnsl -lresolv
		g++ -o serverB serverB.cc  -lnsl -lresolv
		g++ -o serverC serverC.cc  -lnsl -lresolv

.PHONY : serverA
serverA: 
		./serverA

.PHONY : serverB
serverB:
		./serverB

.PHONY : serverC
serverC:
		./serverC

.PHONY : aws
aws:
	./aws