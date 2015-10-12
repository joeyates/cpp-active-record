bin_PROGRAMS = active_record_x86_64_test

active_record_x86_64_test_SOURCES = test/attribute_test.cpp test/base_associations_test.cpp test/base_test.cpp test/date_test.cpp test/exception_test.cpp test/field_test.cpp test/main.cpp test/postgresql_attribute_test.cpp test/postgresql_connection_test.cpp test/postgresql_table_set_test.cpp test/query_test.cpp test/row_test.cpp test/sqlite3_connection_test.cpp test/table_set_test.cpp test/table_test.cpp test/test_helper.cpp
active_record_x86_64_test_CPPFLAGS = -Iinclude -Iinclude/postgresql -Iinclude/postgresql/server -Itest -DPG_USER=
active_record_x86_64_test_LDFLAGS  = -L.
active_record_x86_64_test_LDADD  = -lgtest -lactive_record_x86_64 -lpq -lsqlite3 -lpthread
lib_LIBRARIES = libactive_record_x86_64.a

libactive_record_x86_64_a_SOURCES = src/active_record.cpp src/attribute.cpp src/attribute/postgresql_attribute.cpp src/attribute/sqlite3_attribute.cpp src/connection.cpp src/connection/postgresql_connection.cpp src/connection/sqlite3_connection.cpp src/exception.cpp src/field.cpp src/row.cpp src/row/postgresql_row.cpp src/row/sqlite3_row.cpp src/table.cpp src/table/sqlite3_table.cpp src/table_set.cpp src/type.cpp
libactive_record_x86_64_a_CPPFLAGS = -Iinclude -Iinclude/postgresql -Iinclude/postgresql/server
