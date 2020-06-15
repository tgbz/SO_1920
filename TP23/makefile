CC = gcc

all: rem argusd argus

argusd:
	gcc argusd.c  -o argusd -w

argus:
	gcc argus.c -o argus -w

rem:
	rm -rf argusd argus

clean:
	killall argus;killall argusd;rm -rf log* argusd argus