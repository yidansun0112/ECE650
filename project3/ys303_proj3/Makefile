CFLAGS=-std=gnu++98 -Wall -Werror -pedantic -ggdb3
PROGS=ringmaster player
OBJS=$(patsubst %,%.o,$(PROGS)) potato.o
all:$(PROGS) 
ringmaster: ringmaster.o potato.o
	g++ -o $@ $^
player: player.o potato.o 
	g++ -o $@ $^
%.o: %.cpp
	g++ $(CFLAGS) -c $<
.PHONY: clean
clean:
	rm -f *~ $(PROGS) $(OBJS)

potato.o: potato.hpp
