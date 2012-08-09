lib_LIBRARIES = libactive_record_sqlite_i386.a libactive_record_sqlite_x86_64.a libactive_record_sqlite_x86_64_profiled.a

libactive_record_sqlite_i386_a_SOURCES = src/active_record.cpp src/attribute.cpp src/connection.cpp src/exception.cpp src/field.cpp src/row.cpp src/table.cpp src/table_set.cpp src/type.cpp
libactive_record_sqlite_i386_a_CPPFLAGS = -Iinclude

libactive_record_sqlite_x86_64_a_SOURCES = src/active_record.cpp src/attribute.cpp src/connection.cpp src/exception.cpp src/field.cpp src/row.cpp src/table.cpp src/table_set.cpp src/type.cpp
libactive_record_sqlite_x86_64_a_CPPFLAGS = -Iinclude

libactive_record_sqlite_x86_64_profiled_a_SOURCES = src/active_record.cpp src/attribute.cpp src/connection.cpp src/exception.cpp src/field.cpp src/row.cpp src/table.cpp src/table_set.cpp src/type.cpp
libactive_record_sqlite_x86_64_profiled_a_CPPFLAGS = -Iinclude -pg

bin_PROGRAMS = active_record_test single_test

active_record_test_SOURCES = test/attribute_test.cpp test/base_associations_test.cpp test/base_test.cpp test/connection_test.cpp test/date_test.cpp test/exception_test.cpp test/field_test.cpp test/main.cpp test/query_test.cpp test/row_test.cpp test/table_set_test.cpp test/table_test.cpp test/test_helper.cpp
active_record_test_CPPFLAGS = -Iinclude -Itest
active_record_test_LDFLAGS = -L.
active_record_test_LDADD = -lgtest -lactive_record_sqlite_i386 -lsqlite3

single_test_SOURCES = test/base_test.cpp test/main.cpp test/test_helper.cpp
single_test_CPPFLAGS = -Iinclude -Itest
single_test_LDFLAGS = -L.
single_test_LDADD = -lgtest -lactive_record_sqlite_i386 -lsqlite3

