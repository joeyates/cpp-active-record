#include <active_record/base.h>
#include <active_record/type.h>

//////////////////////////////////////////////

class Greeting: public ActiveRecord::Base<Greeting> {
 public:
  AR_CONSTRUCTORS(Greeting)
  static ActiveRecord::Table table(ActiveRecord::Connection * connection) {
    // Set the table name
    ActiveRecord::Table td(connection, "greetings");

    // Define the table's structure
    td.fields() = ActiveRecord::fields
      ("salute",   ActiveRecord::Type::text)
      ("thing",    ActiveRecord::Type::text)
      ("language", ActiveRecord::Type::text);

    return td;
  }
};

AR_DECLARE(Greeting)

//////////////////////////////////////////////

#include <active_record/connection/sqlite3.h>
#include <active_record/query.h>
#include <iostream>

int main(int argc, const char *argv[]) {
  string database_file   = "greetings.sqlite3";

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
  Greeting::setup(&connection);
  connection.update_database();

  // Insert data
  Greeting hello(
    ActiveRecord::attribute_pairs
      ("salute", "Hello")
      ("thing", "World")
      ("language", "English"));
  hello.save();

  Greeting ciao(
    ActiveRecord::attribute_pairs
      ("salute", "Ciao")
      ("thing", "Mondo")
      ("language", "Italian")
    );
  ciao.save();

  // Get max 5 records, ordered by language
  vector<Greeting> greetings = ActiveRecord::Query<Greeting>(connection)
    .order("language")
    .limit(5)
    .all();

  for(auto& greeting: greetings) {
    cout << "In " << greeting["language"] << " you say: ";
    cout << "'" << greeting["salute"] << " " << greeting["thing"] << "!'";
    cout << endl;
  }
}
