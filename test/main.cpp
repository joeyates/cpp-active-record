#include <gtest/gtest.h>
#include <active_record/exception.h>

int main( int argc, char **argv ) {
  ::testing::InitGoogleTest( &argc, argv );
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
