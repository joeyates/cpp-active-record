#ifndef _ACTIVE_RECORD_TESTS_MODELS_H_
#define _ACTIVE_RECORD_TESTS_MODELS_H_

#include <active_record/type.h>
#include <active_record/base.h>

using namespace ActiveRecord;

class Person: public ActiveRecord::Base< Person > {
 public:
  AR_CONSTRUCTORS( Person )
  static Table table( Connection * connection ) {
    Table td( connection, "people" );
    td.fields() = fields
                  ( "name",    ActiveRecord::Type::text )
                  ( "surname", ActiveRecord::Type::text )
                  ( "age",     ActiveRecord::Type::integer )
                  ( "height",  ActiveRecord::Type::floating_point )
                  ( "dob",     ActiveRecord::Type::date );
    return td;
  }
};

#endif // ndef _ACTIVE_RECORD_TESTS_MODELS_H_
