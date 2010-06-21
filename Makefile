SOURCES   = \
  src/connection.cpp \
  src/row.cpp \
  src/types.cpp \
  src/main.cpp
OBJECTS   := $(SOURCES:src/%.cpp=objects/%.o)
C++-FLAGS = -c -I./include
C++-LIBS  = -lstdc++ -lsqlite3

$(OBJECTS): \
  include/active_record/base.h \
  include/active_record/row.h \
  include/active_record/connection.h \
  include/active_record/query.h \
  include/active_record/types.h \
  include/active_record/table_data.h

debug: active_record
run-debug:
	./active_record

active_record: $(OBJECTS)
	g++ $(C++-LIBS) -o active_record $(OBJECTS)

compile: $(OBJECTS)

clean:
	rm -f ./objects/*.o
	rm -f active_record

$(OBJECTS): objects/%.o: src/%.cpp
	g++ $(C++-FLAGS) $< -o $@
