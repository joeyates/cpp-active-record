#include <cstdlib>
#include <gtest/gtest.h>
#include <active_record/exception.h>

const char * pg_user;
const char * pg_host;
const char * pg_port;

int main( int argc, char **argv ) {
  ::testing::InitGoogleTest( &argc, argv );

  pg_user = std::getenv("PG_USER");
  pg_host = std::getenv("PG_HOST");
  pg_port = std::getenv("PG_PORT");

  int result = -1;
  try {
    result = RUN_ALL_TESTS();
  }
  catch( ActiveRecord::ActiveRecordException e ) {
    cout << "Test raised an unexpected exception:" << endl;
    cout << e.to_string() << endl;
  }
  return result;
}
