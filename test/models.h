#ifndef _ACTIVE_RECORD_TESTS_MODELS_H_
#define _ACTIVE_RECORD_TESTS_MODELS_H_

#include <active_record/types.h>
#include <active_record/base.h>

using namespace ActiveRecord;

class Person: public ActiveRecord::Base<Person> {
 public:
  Person() : ActiveRecord::Base<Person>() {} // Necessary
  Person(int id) : ActiveRecord::Base<Person>(id) {} // Necessary
  // Callback to set table_name and override any defaults
  static void set_table_data(TableData& td) {
    td.table_name = "people";
    td.fields.push_back(Field("name", ActiveRecord::text));
    td.fields.push_back(Field("surname", ActiveRecord::text));
    td.fields.push_back(Field("age", ActiveRecord::integer));
    td.fields.push_back(Field("height", ActiveRecord::floating_point));
    td.update_database = true;
    //td.has_many("books");
  }
};

#endif // ndef _ACTIVE_RECORD_TESTS_MODELS_H_
