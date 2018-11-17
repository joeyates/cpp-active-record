#include "test_helper.h"

class ExceptionTest: public ::testing::Test {};

TEST_F(ExceptionTest, Full) {
  ActiveRecordException e("foo", "/path/to/file.cpp", 999);

  assert_string("/path/to/file.cpp:999: foo", e.to_string());
}

TEST_F(ExceptionTest, MessageAndFile) {
  ActiveRecordException e("foo", "/path/to/file.cpp");

  assert_string("/path/to/file.cpp: foo", e.to_string());
}

TEST_F(ExceptionTest, MessageOnly) {
  ActiveRecordException e("foo");

  assert_string("foo", e.to_string());
}

TEST_F(ExceptionTest, Ostream) {
  stringstream error_out;
  ActiveRecordException e("foo", "/path/to/file.cpp", 999);

  error_out << e;

  assert_string("/path/to/file.cpp:999: foo", error_out.str());
}
