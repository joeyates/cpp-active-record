#include <active_record/connection/sqlite3.h>

#include <ctime>

namespace ActiveRecord {

Sqlite3Connection::Sqlite3Connection(): db_(nullptr) {}

Sqlite3Connection::~Sqlite3Connection() {
  disconnect();
}

/////////////////////////////////////////////////////
// Connection

void Sqlite3Connection::connect(OptionsHash options) {
  log(options["database"]);
  sqlite_initialize(options["database"]);
}

void Sqlite3Connection::disconnect() {
  if(db_ != nullptr) {
    sqlite3_close(db_);
    db_ = nullptr;
  }
}

bool Sqlite3Connection::connected() {
  return db_ != nullptr;
}

/////////////////////////////////////////////////////
// Database Structure

bool Sqlite3Connection::table_exists(const std::string& table_name) {
  AttributeList parameters;
  parameters.push_back(table_name);

  RowSet rows = select_all(
    "SELECT name FROM sqlite_master WHERE type='table' AND name = ?;",
    parameters
  );

  return !rows.empty();
}

bool Sqlite3Connection::execute(
  const std::string& query,
  const AttributeList& parameters
) {
  log("Sqlite3Connection::execute");
  log(query);

  ParameterAllocations pa;
  sqlite3_stmt* ppStmt = prepare(query, parameters, pa);
  sqlite3_step(ppStmt);
  sqlite3_finalize(ppStmt);
  cleanup(pa);

  return true;
}

int64 Sqlite3Connection::insert(
  const std::string& query,
  const AttributeList& parameters
) {
  log("Sqlite3Connection::insert");
  log(query);

  ParameterAllocations pa;
  sqlite3_stmt* ppStmt = prepare(query, parameters, pa);
  sqlite3_step(ppStmt);
  sqlite3_finalize(ppStmt);
  cleanup(pa);

  return sqlite3_last_insert_rowid(db_);
}

Row Sqlite3Connection::select_one(
  const std::string& query,
  const AttributeList& parameters
) {
  ParameterAllocations pa;
  sqlite3_stmt* ppStmt = prepare(query, parameters, pa);
  int step_result = sqlite3_step(ppStmt);
  if(step_result != SQLITE_ROW) {
    sqlite3_finalize(ppStmt);
    cleanup(pa);
    return Row();
  }

  Row row(ppStmt);
  sqlite3_finalize(ppStmt);
  cleanup(pa);

  return row;
}

RowSet Sqlite3Connection::select_all(
  const std::string& query,
  const AttributeList& parameters
) {
  ParameterAllocations pa;
  sqlite3_stmt* ppStmt = prepare(query, parameters, pa);

  RowSet results;
  while(sqlite3_step(ppStmt) == SQLITE_ROW) {
    results.push_back(Row(ppStmt));
  }

  sqlite3_finalize(ppStmt);
  cleanup(pa);

  return results;
}

AttributeList Sqlite3Connection::select_values(
  const std::string& query,
  const AttributeList& parameters
) {
  ParameterAllocations pa;
  sqlite3_stmt* ppStmt = prepare(query, parameters, pa);

  AttributeList results;
  while(sqlite3_step(ppStmt) == SQLITE_ROW) {
    results.push_back(Attribute::from_field(ppStmt, 0));
  }

  sqlite3_finalize(ppStmt);
  cleanup(pa);

  return results;
}

Attribute Sqlite3Connection::select_value(
  const std::string& query,
  const AttributeList& parameters
) {
  ParameterAllocations pa;
  sqlite3_stmt* ppStmt = prepare(query, parameters, pa);

  int step_result = sqlite3_step(ppStmt);
  if(step_result != SQLITE_ROW) {
    return Attribute();
  }

  Attribute result = Attribute::from_field(ppStmt, 0);

  sqlite3_finalize(ppStmt);
  cleanup(pa);

  return result;
}

TableSet Sqlite3Connection::schema() {
  TableSet s;

  RowSet rows = select_all(
    "SELECT name FROM sqlite_master WHERE type='table' ORDER BY name;"
  );

  for(auto& row: rows) {
    std::string table_name = row.get_text("name");
    s[table_name] = table_data(table_name);
  }

  return s;
}

Table Sqlite3Connection::table_data(const std::string& table_name) {
  Table td(this, table_name);

  std::string pk = primary_key(table_name);
  td.primary_key(pk);

  std::stringstream row_query;
  row_query << "PRAGMA table_info(\"" << table_name << "\");";

  RowSet rows = select_all(row_query.str());

  for(auto& row: rows) {
    // cid | name | type    | notnull | dflt_value | pk
    //   0 |  bar | INTEGER |       0 |            |  0
    std::string name      = row.get_text("name");
    std::string type_name = row.get_text("type");

    if(name == pk) {
      continue;
    }

    ActiveRecord::Type::Type type = ActiveRecord::to_type(type_name);
    if(type == Type::unknown) {
      std::stringstream error;
      error << "Unknown type: " << type_name;
      throw ActiveRecordException(error.str(), __FILE__, __LINE__);
    }
    td.fields().push_back(Field(name, type));
  }

  return td;
}

std::string Sqlite3Connection::primary_key(const std::string& table_name) {
  std::string row_query = "PRAGMA table_info(\"" + table_name + "\");";

  RowSet rows = select_all(row_query);

  for(auto& row: rows) {
    // cid | name | type    | notnull | dflt_value | pk
    //   0 |  bar | INTEGER |       0 |            |  0
    bool is_pk = row.get_text("pk") != "0";

    if(is_pk) {
      std::string name = row.get_text("name");
      return name;
    }
  }

  return "";
}

std::string temp_table_name() {
  const char* base_name = "ar_rename";
  time_t t_now = time(nullptr);
  struct tm* now = gmtime(&t_now);

  char* buffer = nullptr;
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
  snprintf(
    buffer, len + 1, format,
    base_name,
    now->tm_year + 1900,
    now->tm_mon + 1,
    now->tm_mday,
    now->tm_hour,
    now->tm_min,
    now->tm_sec
  );

  std::string name = buffer;
  delete[] buffer;

  return name;
}

void Sqlite3Connection::remove_field(
  const std::string& table_name,
  const std::string& field_name
) {
  std::string temp_table = temp_table_name();

  Table td = table_data(table_name);
  std::string pk = td.primary_key();
  Fields fields = td.fields();

  std::stringstream copy_fields;
  bool first = true;
  if(!pk.empty()) {
    copy_fields << pk;
    first = false;
  }

  Table temp(this, temp_table);
  temp.primary_key(pk);

  for(auto& field: fields) {
    const std::string& name = field.name();
    if(name == field_name) {
      continue;
    }

    if(first) {
      first = false;
    } else {
      copy_fields << ", ";
    }

    copy_fields << name;
    temp.fields().push_back(field);
  }

  TableSet::create_table(temp);

  std::stringstream copy;
  copy << "INSERT INTO " << temp_table ;
  copy << " SELECT " << copy_fields.str();
  copy << " FROM " << table_name;
  execute(copy.str(), AttributeList());

  std::stringstream drop;
  drop << "DROP TABLE " << table_name;
  execute(drop.str(), AttributeList());

  std::stringstream rename;
  rename << "ALTER TABLE " << temp_table << " RENAME TO " << table_name;
  execute(rename.str(), AttributeList());
}

////////////////////////////////////////
// Private

sqlite3_stmt* Sqlite3Connection::prepare(
  const std::string& query,
  const AttributeList& parameters,
  ParameterAllocations& pa
) {
  if(db_ == nullptr) {
    throw ActiveRecordException("Database not connected", __FILE__, __LINE__);
  }

  sqlite3_stmt* ppStmt = nullptr;
  int prepare_result = sqlite3_prepare_v2(
    db_, query.c_str(), query.size(), &ppStmt, nullptr
  );

  if(prepare_result != SQLITE_OK) {
    std::stringstream error;
    error << "SQL error: \"" << sqlite_error(prepare_result) << "\" ";
    error << "in \"" << query << "\"";
    if(!parameters.empty()) {
      error << ": " << parameters;
    }

    log(error.str());

    throw ActiveRecordException(error.str(), __FILE__, __LINE__);
  }

  bind_parameters(ppStmt, parameters, pa);
  return ppStmt;
}

void Sqlite3Connection::bind_parameters(
  sqlite3_stmt* ppStmt,
  const AttributeList& parameters,
  ParameterAllocations& pa
) {
  pa.param_count = parameters.size();

  if(pa.param_count == 0) {
    pa.param_values = nullptr;
    return;
  }

  pa.param_values = new char* [pa.param_count];

  int i = 0;
  for(auto& parameter: parameters) {
    switch(parameter.which()) {
      case Type::integer: {
        int64 value = boost::get<int64>(parameter);
        pa.param_values[i] = nullptr;
        sqlite3_bind_int(ppStmt, i + 1, value);
        break;
      }

      case Type::text: {
        std::string value = boost::get<std::string>(parameter);
        int len = value.size();

        pa.param_values[i] = new char[len + 1];
        strncpy(pa.param_values[i], value.c_str(), len + 1);
        sqlite3_bind_text(ppStmt, i + 1, pa.param_values[i], len, nullptr);
        break;
      }

      case Type::floating_point: {
        double value = boost::get<double>(parameter);
        pa.param_values[i] = nullptr;
        sqlite3_bind_double(ppStmt, i + 1, value);
        break;
      }

      case Type::date: {
        Date date = boost::get<Date>(parameter);
        std::string value = date.to_string();
        int len = value.size();

        pa.param_values[i] = new char[len + 1];
        strncpy(pa.param_values[i], value.c_str(), len + 1);
        sqlite3_bind_text(ppStmt, i + 1, pa.param_values[i], len, nullptr);
        break;
      }

      default: {
        throw ActiveRecordException("Type not implemented", __FILE__, __LINE__);
      }
    }
    ++i;
  }
}

void Sqlite3Connection::cleanup(ParameterAllocations& pa) {
  if(pa.param_count == 0) {
    return;
  }

  if(pa.param_values == nullptr) {
    return;
  }

  for(int i = 0; i < pa.param_count; ++i) {
    if(pa.param_values[i] != nullptr) {
      delete[] pa.param_values[i];
      pa.param_values[i] = nullptr;
    }
  }

  delete[] pa.param_values;
  pa.param_count = 0;
  pa.param_values = nullptr;
}

//////////////////////
// SQLite-specific

bool Sqlite3Connection::sqlite_initialize(std::string& database_path_name) {
  int nResult = sqlite3_open(database_path_name.c_str(), &db_);
  if(nResult != 0) {
    std::stringstream error;
    error << "Can't open database '" << database_path_name << "'";
    error << sqlite3_errmsg(db_);
    sqlite3_close(db_);
    throw ActiveRecordException(error.str(), __FILE__, __LINE__);
  }
  return true;
}

std::string Sqlite3Connection::sqlite_error(int error_code) {
  std::string error;
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
