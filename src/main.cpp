#include <active_record/connection.h>
#include <active_record/query.h>
#include <active_record/base.h>

//////////////////////////////////////////////
// User model(s)

class Person: public ActiveRecord::Base<Person> {
//  static void has_many(string x);
};

//Person::has_many("books");

// Define the name of the table
template <>
string ActiveRecord::Base<Person>::table_name = "people";

// Optional: Define a name for result sets
typedef ActiveRecord::Query<Person> People;

//////////////////////////////////////////////

#include <iostream>
using namespace std;
// Keep these until we replace the define on list_of:
#include <boost/assign.hpp>
using namespace boost::assign;

namespace ActiveRecord {

extern Connection current_connection;

}

int main(int argc, const char* argv[]) {
  ActiveRecord::current_connection.connect(to_options ("adapter", "sqlite") ("database", "/home/joe/foobar.sqlite3"));
  Person person;
  People people = person.where(to_options ("name = ?", "Joe") ("age = ?", "45")).limit(3).order("surname");
  cout << people.sql() << endl;
}
