#include <active_record/table_set.h>

#include <sstream>
#include <active_record/active_record.h>
#include <active_record/exception.h>
#include <active_record/type.h>
#include <active_record/connection.h>

namespace ActiveRecord {

extern TypeNameMap type_name;

//////////////////////////////////////////////////////////
// static methods

void TableSet::create_table(Table& td) {
  log("TableSet::create_table");
  log(td.table_name());

  stringstream ss;
  ss << "CREATE TABLE " << td.table_name();
  ss << " (";

  bool is_first = true;

  string pk = td.primary_key();
  if(pk != "") {
    ss << pk << " INTEGER PRIMARY KEY";
    is_first = false;
  }

  for(auto& field: td.fields()) {
    if(is_first) {
      is_first = false;
    } else {
      ss << ", ";
    }
    ss << field.name() << " " << type_name[field.type()];
  }

  if(td.timestamps()) {
    if(is_first) {
      is_first = false;
    } else {
      ss << ", ";
    }
    ss << "created_at TEXT";
    ss << ", updated_at TEXT";
  }

  ss << ");";

  td.connection()->execute(ss.str());
}

void TableSet::update_table(Table& required) {
  log("TableSet::update_table");
  log(required.table_name());

  Table existing = required.connection()->table_data(required.table_name());

  Fields missing = required.fields() - existing.fields();
  for(auto& field: missing) {
    existing.add_field(field);
  }

  Fields remove = existing.fields() - required.fields();
  for(auto& field: remove) {
    existing.remove_field(field);
  }
}

////////////////////////////////////
// public methods

// Data Definition / Database Structure

void TableSet::update_database() {
  for(auto& pair: *this) {
    Table td = pair.second;
    string name = td.table_name();
    bool exists = td.connection()->table_exists(name);
    if(exists) {
      update_table(td);
    } else {
      create_table(td);
    }
  }
}

} // namespace ActiveRecord
