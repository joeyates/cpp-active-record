#include <active_record/connection/sqlite3.h>

#include <ctime>

namespace ActiveRecord {

extern TypeNameMap type_name;

Sqlite3Connection::Sqlite3Connection(): db_(NULL ) {}

Sqlite3Connection::~Sqlite3Connection() {
  disconnect();
}

Sqlite3Connection::Sqlite3Connection(const Sqlite3Connection& other) {}

Sqlite3Connection Sqlite3Connection::operator=(const Sqlite3Connection& other) {
  return *this;
}

/////////////////////////////////////////////////////
// Connection

void Sqlite3Connection::connect(OptionsHash options) {
  log(options["database"] );
  sqlite_initialize(options["database"] );
}

void Sqlite3Connection::disconnect() {
  if(db_ != NULL) {
    sqlite3_close(db_);
    db_ = NULL;
  }
}

bool Sqlite3Connection::connected() {
  return db_ != NULL ? true : false;
}

/////////////////////////////////////////////////////
// Database Structure

bool Sqlite3Connection::table_exists(const string& table_name) {
  AttributeList parameters;
  parameters.push_back(table_name);

  RowSet rows = select_all(
    "SELECT name FROM sqlite_master WHERE type='table' AND name = ?;",
    parameters
  );

  return (rows.size() ? true : false);
}

bool Sqlite3Connection::execute(
  const string& query,
  const AttributeList& parameters
) {
  log("Sqlite3Connection::execute");
  log(query);

  sqlite3_stmt* ppStmt = prepare(query, parameters);
  sqlite3_step(ppStmt);
  sqlite3_finalize(ppStmt);

  return true;
}

long Sqlite3Connection::insert(
  const string& query,
  const AttributeList& parameters
) {
  log("Sqlite3Connection::insert");
  log(query);

  sqlite3_stmt* ppStmt = prepare(query, parameters);
  sqlite3_step(ppStmt);
  sqlite3_finalize(ppStmt);

  return sqlite3_last_insert_rowid(db_);
}

Row Sqlite3Connection::select_one(
  const string& query,
  const AttributeList& parameters
) {
  sqlite3_stmt* ppStmt = prepare(query, parameters);
  int step_result = sqlite3_step(ppStmt);
  if(step_result != SQLITE_ROW ) {
    return Row();
  }

  Row row(ppStmt);
  sqlite3_finalize(ppStmt);

  return row;
}

RowSet Sqlite3Connection::select_all(
  const string& query,
  const AttributeList& parameters
) {
  sqlite3_stmt* ppStmt = prepare(query, parameters);

  RowSet results;
  while(sqlite3_step(ppStmt) == SQLITE_ROW ) {
    results.push_back(Row(ppStmt));
  }

  sqlite3_finalize(ppStmt);

  return results;
}

AttributeList Sqlite3Connection::select_values(
  const string& query,
  const AttributeList& parameters
) {
  sqlite3_stmt* ppStmt = prepare(query, parameters);

  AttributeList results;
  while(sqlite3_step(ppStmt) == SQLITE_ROW ) {
    results.push_back(Attribute::from_field(ppStmt, 0));
  }

  sqlite3_finalize(ppStmt);

  return results;
}

Attribute Sqlite3Connection::select_value(
  const string& query,
  const AttributeList& parameters
) {
  sqlite3_stmt* ppStmt = prepare(query, parameters);

  Attribute result;
  int step_result = sqlite3_step(ppStmt);
  if(step_result != SQLITE_ROW )
    return result;

  result = Attribute::from_field(ppStmt, 0);
  sqlite3_finalize(ppStmt);

  return result;
}

TableSet Sqlite3Connection::schema() {
  TableSet s;

  RowSet rows = select_all(
    "SELECT name FROM sqlite_master WHERE type='table' ORDER BY name;"
  );
  for(RowSet::iterator it = rows.begin(); it != rows.end(); ++it) {
    string table_name = it->get_text("name");
    s[table_name] = table_data(table_name);
  }

  return s;
}

Table Sqlite3Connection::table_data(const string& table_name) {
  Table td(this, table_name);

  string pk = primary_key(table_name);
  td.primary_key(pk);

  stringstream row_query;
  row_query << "PRAGMA table_info(\"" <<table_name << "\");";

  RowSet rows = select_all(row_query.str());
  for(RowSet::iterator it = rows.begin(); it != rows.end(); ++it) {
    // cid | name | type    | notnull | dflt_value | pk
    //   0 |  bar | INTEGER |       0 |            |  0
    string name      = it->get_text("name");
    string type_name = it->get_text("type");

    if(name == pk)
      continue;

    ActiveRecord::Type::Type type = ActiveRecord::to_type(type_name);
    if(type == Type::unknown) {
      stringstream error;
      error << "Unknown type: " <<type_name;
      throw ActiveRecordException(error.str(), __FILE__, __LINE__);
    }
    td.fields().push_back(Field(name, type));
  }

  return td;
}

string Sqlite3Connection::primary_key(const string& table_name) {
  string row_query = "PRAGMA table_info(\"" + table_name + "\");";

  RowSet rows = select_all(row_query);

  for(RowSet::iterator it = rows.begin(); it != rows.end(); ++it) {
    // cid | name | type    | notnull | dflt_value | pk
    //   0 |  bar | INTEGER |       0 |            |  0
    Type::Type t = it->get_type("pk");
    string tn = type_name[t];
    int is_pk = atoi(it->get_text("pk").c_str());
    if(is_pk != 0) {
      string name = it->get_text("name");
      return name;
    }
  }

  return "";
}

string temp_table_name() {
  const char* base_name = "ar_rename";
  time_t t_now = time(0);
  struct tm* now = gmtime(&t_now);

  char* buffer;
  const char* format = "%s_%u%02u%02u_%02u%02u%02u";
  int len = snprintf(
    buffer, 0, format,
    base_name,
    now->tm_year + 1900,
    now->tm_mon + 1,
    now->tm_mday,
    now->tm_hour,
    now->tm_min,
    now->tm_sec
  );

  buffer = new char[len + 1];
  int used = snprintf(
    buffer, len + 1, format,
    base_name,
    now->tm_year + 1900,
    now->tm_mon + 1,
    now->tm_mday,
    now->tm_hour,
    now->tm_min,
    now->tm_sec
  );

  string name = buffer;
  delete[] buffer;

  return name;
}

void Sqlite3Connection::remove_field(
  const string& table_name,
  const string& field_name
) {
  string temp_table = temp_table_name();

  Table td = table_data(table_name);
  string pk = td.primary_key();
  Fields fields = td.fields();

  stringstream copy_fields;
  bool first = true;
  if(pk != "") {
    copy_fields << pk;
    first = false;
  }

  Table temp(this, temp_table);
  temp.primary_key(pk);

  for(Fields::const_iterator it = fields.begin(); it != fields.end(); ++it) {
    string name = it->name();
    if(name == field_name) {
      continue;
    }
    if(first)
      first = false;
    else
      copy_fields << ", ";
    copy_fields <<name;
    temp.fields().push_back(*it);
  }

  TableSet::create_table(temp);

  stringstream copy;
  copy << "INSERT INTO " << temp_table ;
  copy << " SELECT " << copy_fields.str();
  copy << " FROM " << table_name;
  execute(copy.str());

  stringstream drop;
  drop << "DROP TABLE " << table_name;
  execute(drop.str());

  stringstream rename;
  rename << "ALTER TABLE " << temp_table << " RENAME TO " << table_name;
  execute(rename.str());
}

////////////////////////////////////////
// Private

sqlite3_stmt* Sqlite3Connection::prepare(
  const string& query,
  const AttributeList& parameters
) {
  if(db_ == NULL )
    throw ActiveRecordException("Database not connected", __FILE__, __LINE__);

  sqlite3_stmt *ppStmt = 0;
  int prepare_result = sqlite3_prepare_v2(
    db_, query.c_str(), query.size(), &ppStmt, 0
  );

  if(prepare_result != SQLITE_OK) {
    stringstream error;
    error << "SQL error: \"" << sqlite_error(prepare_result) << "\" ";
    error << "in \"" << query << "\"";

    bool added = false;
    for(
      AttributeList::const_iterator it = parameters.begin();
      it != parameters.end();
      ++it
    ) {
      error << ", ";
      if(!added)
        error << "[";
      error << *it;
      added = true;
    }

    if(added)
      error << "]";

    log(error.str());

    throw ActiveRecordException(error.str(), __FILE__, __LINE__);
  }

  bind_parameters(ppStmt, parameters);
  return ppStmt;
}

void Sqlite3Connection::bind_parameters(
  sqlite3_stmt* ppStmt,
  const AttributeList& parameters
) {
  int i = 0;
  for(
    AttributeList::const_iterator it = parameters.begin();
    it != parameters.end();
    ++it
  ) {
    switch(it->which()) {
    case Type::integer: {
      int value = boost::get<int>(*it);
      sqlite3_bind_int(ppStmt, i + 1, value);
      break;
    }
    case Type::text: {
      string value = boost::get<std::string>(*it);
      sqlite3_bind_text(ppStmt, i + 1, value.c_str(), value.size(), 0);
      break;
    }
    case Type::floating_point: {
      double value = boost::get<double>(*it);
      sqlite3_bind_double(ppStmt, i + 1, value);
      break;
    }
    case Type::date: {
      Date value = boost::get<Date>(*it);
      string s   = value.to_string();
      sqlite3_bind_text(ppStmt, i + 1, s.c_str(), s.size(), 0);
      break;
    }
    default: {
      throw ActiveRecordException("Type not implemented", __FILE__, __LINE__);
    }
    }
    ++i;
  }
}

//////////////////////
// SQLite-specific

bool Sqlite3Connection::sqlite_initialize(string database_path_name) {
  int nResult = sqlite3_open(database_path_name.c_str(), &db_);
  if(nResult) {
    stringstream error;
    error << "Can't open database '" <<database_path_name << "'";
    error <<sqlite3_errmsg(db_);
    sqlite3_close(db_);
    throw ActiveRecordException(error.str(), __FILE__, __LINE__);
  }
  return true;
}

string Sqlite3Connection::sqlite_error(int error_code) {
  string error;
  switch(error_code) {
  case SQLITE_ERROR:      error = "SQL error or missing database";               break;
  case SQLITE_INTERNAL:   error = "Internal logic error in SQLite";              break;
  case SQLITE_PERM:       error = "Access permission denied";                    break;
  case SQLITE_ABORT:      error = "Callback routine requested an abort";         break;
  case SQLITE_BUSY:       error = "The database file is locked";                 break;
  case SQLITE_LOCKED:     error = "A table in the database is locked";           break;
  case SQLITE_NOMEM:      error = "A malloc() failed";                           break;
  case SQLITE_READONLY:   error = "Attempt to write a readonly database";        break;
  case SQLITE_INTERRUPT:  error = "Operation terminated by sqlite3_interrupt()"; break;
  case SQLITE_IOERR:      error = "Some kind of disk I/O error occurred";        break;
  case SQLITE_CORRUPT:    error = "The database disk image is malformed";        break;
  case SQLITE_NOTFOUND:   error = "Unknown opcode in sqlite3_file_control()";    break;
  case SQLITE_FULL:       error = "Insertion failed because database is full";   break;
  case SQLITE_CANTOPEN:   error = "Unable to open the database file";            break;
  case SQLITE_PROTOCOL:   error = "Database lock protocol error";                break;
  case SQLITE_EMPTY:      error = "Database is empty";                           break;
  case SQLITE_SCHEMA:     error = "The database schema changed";                 break;
  case SQLITE_TOOBIG:     error = "String or BLOB exceeds size limit";           break;
  case SQLITE_CONSTRAINT: error = "Abort due to constraint violation";           break;
  case SQLITE_MISMATCH:   error = "Data type mismatch";                          break;
  case SQLITE_MISUSE:     error = "Library used incorrectly";                    break;
  case SQLITE_NOLFS:      error = "Uses OS features not supported on host";      break;
  case SQLITE_AUTH:       error = "Authorization denied";                        break;
  case SQLITE_FORMAT:     error = "Auxiliary database format error";             break;
  case SQLITE_RANGE:      error = "2nd parameter to sqlite3_bind out of range";  break;
  case SQLITE_NOTADB:     error = "File opened that is not a database file";     break;
  case SQLITE_ROW:        error = "sqlite3_step() has another row ready";        break;
  case SQLITE_DONE:       error = "sqlite3_step() has finished executing";       break;
  default:                error = "Unknown error";                               break;
  }
  return error;
}

} // namespace ActiveRecord

