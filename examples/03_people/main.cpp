#include <active_record/base.h>
#include <active_record/type.h>

//////////////////////////////////////////////
// User model(s)

// This would normally go in a .h file

class Person: public ActiveRecord::Base<Person> {
 public:
  AR_CONSTRUCTORS(Person)
  static ActiveRecord::Table table(ActiveRecord::Connection* connection) {
    // Set the table name
    ActiveRecord::Table td(connection, "people");

    // Define the table's structure
    td.fields() =
      ActiveRecord::fields
        ("name", ActiveRecord::Type::text)
        ("surname", ActiveRecord::Type::text)
        ("age", ActiveRecord::Type::integer)
        ("height", ActiveRecord::Type::floating_point)
        ("dob", ActiveRecord::Type::date);

    return td;
  }
};

AR_DECLARE(Person)

//////////////////////////////////////////////

#include <active_record/connection.h>
#include <active_record/connection/sqlite3.h>
#include <active_record/query.h>
#include <iostream>

int main(int argc, const char* argv[]) {
  string database_file = "people.sqlite3";

  // Start clean
  string remove_database = "rm -f " + database_file;
  system(remove_database.c_str());

  // Prepare
  ActiveRecord::Sqlite3Connection connection;
  connection.connect(
    ActiveRecord::options
      ("adapter", "sqlite")
      ("database", database_file)
  );
  Person::setup(&connection); // Must be called for each model
  connection.update_database();

  // Insert data
  Person joe(
    ActiveRecord::attribute_pairs
      ("name", "Joe" )
      ("surname", "Yates")
      ("age", 45)
      ("height", 1.80)
      ("dob", ActiveRecord::Date::parse("1964-01-24")));
  joe.save();

  Person john;
  john.init(
    ActiveRecord::attribute_pairs
      ("name", "John")
      ("surname", "Smith")
      ("age", 43)
      ("height", 1.90)).save();

  Person petra(
    ActiveRecord::attribute_pairs
      ("name", "Petra")
      ("surname", "Smith")
      ("age", 25)
      ("height", 1.70));
  petra.save();
  cout << "Inserted 3 people in db" << endl << endl;

  // Load by id
  Person loaded_joe(1);
  cout << "Joe's surname: " << loaded_joe["surname"] << endl;
  cout << "Joe's date of birth: " << loaded_joe["dob"] << endl << endl;

  // Update
  loaded_joe["surname"] = "Bates";
  loaded_joe.save();

  // Get all people called Smith who ar 45 or under, ordered by name
  vector<Person> people = ActiveRecord::Query<Person>(connection)
    .where("surname = ?", "Smith")
    .where("age <= ?", 45)
    .order("name")
    .limit(5)
    .all();

  int count = people.size();
  cout << "Found " << count << " people with surname 'Smith':" << endl;

  for(auto& person: people) {
    cout << "\t" << person["name"] << " (#" << person["id"] << "), ";
    cout << "age: " << person["age"] << ", ";
    cout << "height: " << person["height"] << endl;
  }

  return 0;
}
