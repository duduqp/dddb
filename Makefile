CC := g++
CXXFLAGS := -g -Wall  -I./include  -L./lib
EXEC = ./bin/program
SRCS = $(wildcard ./src/*.cpp)
OBJS = $(patsubst ./src/%.cpp,./obj/%.o,$(SRCS))

$(EXEC):$(OBJS)
	$(CC) $^  -o $@

./obj/%.o:./src/%.cpp
	@echo "generate obj:"$@
	@echo "using :"$<
	$(CC) -c $< -o $@ -Wall  -I./include -pthread -std=c++1z

.PHONY : clean
clean:
	rm -rvf $(OBJS) $(EXEC)
