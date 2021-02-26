TARGETS=ringmaster player

all: $(TARGETS)
clean:
	rm -f $(TARGETS)

ringmaster: ringmaster.cpp
	g++ -g -o $@ $<

player: player.cpp
	g++ -g -o $@ $<

