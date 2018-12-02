#ifndef _ACTIVE_RECORD_TESTS_MODELS_H_
#define _ACTIVE_RECORD_TESTS_MODELS_H_

#include <active_record/type.h>
#include <active_record/base.h>

namespace ActiveRecord {

class Person: public ActiveRecord::Base<Person> {
 public:
  AR_CONSTRUCTORS( Person)
  static ActiveRecord::Table table(ActiveRecord::Connection* connection) {
    ActiveRecord::Table td(connection, "people");

    td.fields() =
      fields
        ("name",    ActiveRecord::Type::text)
        ("surname", ActiveRecord::Type::text)
        ("age",     ActiveRecord::Type::integer)
        ("height",  ActiveRecord::Type::floating_point)
        ("dob",     ActiveRecord::Type::date);

    return td;
  }
};

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_TESTS_MODELS_H_
