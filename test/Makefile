NAME = active_record_test
TEST_SOURCES   = $(shell ls ./*.cpp)
LIB_SOURCES    = $(shell ls ../src/*.cpp)

TEST_OBJECTS   := $(TEST_SOURCES:%.cpp=test_objects/%.o)
LIB_OBJECTS    := $(LIB_SOURCES:../src/%.cpp=lib_objects/%.o)

C++-FLAGS = -c -I../include
C++-LIBS  = -lstdc++ -lsqlite3 -lgtest_main

debug: all
run-debug:
	./${NAME}

all: $(NAME)

$(NAME): $(LIB_OBJECTS) $(TEST_OBJECTS)
	c++ -lstdc++ $(C++-LIBS) -o $(NAME) $(LIB_OBJECTS) $(TEST_OBJECTS)

$(TEST_SOURCES): $(shell ls ./*.h)

clean:
	rm -rf **/*.o
	rm -f $(NAME)

test_objects/%.o: ./%.cpp
	g++ $(C++-FLAGS) $< -o $@

lib_objects/%.o: ../src/%.cpp
	g++ $(C++-FLAGS) $< -o $@
