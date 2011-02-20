#ifndef _ACTIVE_RECORD_TESTS_MODELS_H_
#define _ACTIVE_RECORD_TESTS_MODELS_H_

#include <active_record/type.h>
#include <active_record/base.h>

using namespace ActiveRecord;

//namespace ActiveRecord {

class Person: public ActiveRecord::Base< Person > {
 public:
  AR_CONSTRUCTORS( Person )
  static Table table( Connection * connection ) {
    Table td( connection, "people" );
    td.fields().push_back( Field( "name",    ActiveRecord::text ) );
    td.fields().push_back( Field( "surname", ActiveRecord::text ) );
    td.fields().push_back( Field( "age",     ActiveRecord::integer ) );
    td.fields().push_back( Field( "height",  ActiveRecord::floating_point ) );
    return td;
  }
};

//}

#endif // ndef _ACTIVE_RECORD_TESTS_MODELS_H_
