#ifndef _ACTIVE_RECORD_TESTS_MODELS_H_
#define _ACTIVE_RECORD_TESTS_MODELS_H_

#include <active_record/type.h>
#include <active_record/base.h>
#include <active_record/sqlite3_connection.h>

using namespace ActiveRecord;

class Person: public ActiveRecord::Base< Person > {
 public:
  AR_CONSTRUCTORS( Person )
  static Table table( Connection * connection ) {
    Table td( connection, "people" );
    td.fields() = fields
                  ( "name",    ActiveRecord::text )
                  ( "surname", ActiveRecord::text )
                  ( "age",     ActiveRecord::integer )
                  ( "height",  ActiveRecord::floating_point )
                  ( "dob",     ActiveRecord::date );
    return td;
  }
};

#endif // ndef _ACTIVE_RECORD_TESTS_MODELS_H_
