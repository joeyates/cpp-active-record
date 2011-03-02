#include <active_record/exception.h>

ostream & operator<<( ostream &cout, const ActiveRecord::ActiveRecordException &e ) {
  cout << e.to_string();
  return cout;
}
