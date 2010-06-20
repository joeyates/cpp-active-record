#include <active_record/connection.h>
#include <active_record/query.h>
#include <active_record/base.h>

//////////////////////////////////////////////
// User model(s)

class Person: public ActiveRecord::Base<Person> {
 public:
  Person(int id) : ActiveRecord::Base<Person>(id) {}
};

//Person::has_many("books");

namespace ActiveRecord {
  extern Connection current_connection;
}

// Define the name of the table
template <>
string ActiveRecord::Base<Person>::table_name = "people";
template <>
ActiveRecord::Connection * ActiveRecord::Base<Person>::connection = &ActiveRecord::current_connection;

// Optional: Define a name for result sets
typedef vector<Person> People;

//////////////////////////////////////////////

#include <iostream>
using namespace std;
using namespace ActiveRecord;

int main(int argc, const char* argv[]) {
  connect(options ("adapter", "sqlite") ("database", "/home/joe/foobar.sqlite3"));
  ActiveRecord::Query<Person> query;
  //People people = query.where(options ("name = ?", "Joe") ("age = ?", "45")).limit(3).order("surname").all();
  People people = query.where(conditions ("surname = ?", "Yates")).all();
  int count = people.size();
  cout << "Result count: " << count << endl;
  for (int i = 0; i < count; ++i) {
    cout << people[i]["id"] << endl;
    cout << people[i]["name"] << endl;
  }
}
