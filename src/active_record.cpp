#include <active_record/active_record.h>

#include <active_record/connection.h>
#include <active_record/base.h>

namespace ActiveRecord {

bool debug = false;

void set_debug( bool new_debug )
{
  debug = new_debug;
}

void log( const string& message )
{
  if( ! debug )
    return;
  cout << message << endl;
}

} // namespace ActiveRecord
