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

  for(
    Fields::const_iterator it = td.fields().begin();
    it != td.fields().end();
    ++it
  ) {
    if(is_first) {
      is_first = false;
    } else {
      ss << ", ";
    }
    ss << it->name() << " " << type_name[it->type()];
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
  for(Fields::iterator it = missing.begin(); it != missing.end(); ++it)
    existing.add_field(*it);

  Fields remove  = existing.fields() - required.fields();
  for(Fields::iterator it = remove.begin(); it != remove.end(); ++it) {
    existing.remove_field(*it);
  }
}

////////////////////////////////////
// public methods

// Data Definition / Database Structure

void TableSet::update_database() {
  log("TableSet::update_database");
  for(TableSet::iterator it = begin(); it != end(); ++it) {
    Table td = it->second;
    log(td.table_name());
    if(td.connection()->table_exists(td.table_name()))
      update_table(td);
    else
      create_table(td);
  }
}

} // namespace ActiveRecord
