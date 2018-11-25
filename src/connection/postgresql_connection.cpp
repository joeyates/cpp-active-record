#include <active_record/connection/postgresql.h>

#include <c.h>
#include <catalog/pg_type.h>
#include <climits>
#include <cmath>

#include <active_record/type.h>

namespace ActiveRecord {

PostgresqlConnection::PostgresqlConnection(): pgconn_(nullptr) {}

PostgresqlConnection::~PostgresqlConnection() {
  disconnect();
}

bool PostgresqlConnection::create_database(
  PostgresqlConnection& connection,
  OptionsHash options
) {
  stringstream create_stream;
  create_stream << "CREATE DATABASE " << options["database"] << " ";

  if(options.find("owner") != options.end()) {
    create_stream << "WITH OWNER " << options["owner"] << " ";
  }

  if(options.find("template") != options.end()) {
    create_stream << "TEMPLATE " << options["template"] << " ";
  }

  if(options.find("encoding") != options.end()) {
    create_stream << "ENCODING '" << options["encoding"] << "' ";
  }

  create_stream << ";";

  return connection.execute(create_stream.str());
}

void PostgresqlConnection::drop_database(
  PostgresqlConnection& connection,
  const string& database_name
) {
  stringstream query;
  query << "DROP DATABASE " << database_name << ";";
  connection.execute(query.str());
}

bool PostgresqlConnection::database_exists(
  PostgresqlConnection& connection,
  const string& database_name
) {
  stringstream query;
  query << "SELECT datname FROM pg_database ";
  query << "WHERE datname = '" << database_name << "';";
  Row row = connection.select_one(query.str());
  return row.has_data();
}

/////////////////////////////////////////////////////
// Connection

void PostgresqlConnection::connect(OptionsHash options) {
  stringstream connection_stream;
  connection_stream << "dbname=" << options["database"] << " ";

  if(options.find("host") != options.end()) {
    connection_stream << "host=" << options["host"] << " ";
  }

  if(options.find("username") != options.end()) {
    connection_stream << "user=" << options["username"] << " ";
  }

  if(options.find("port") != options.end()) {
    connection_stream << "port=" << options["port"] << " ";
  }

  string conninfo = connection_stream.str();

  pgconn_ = PQconnectdb(conninfo.c_str());

  ConnStatusType status = PQstatus(pgconn_);
  if(status == CONNECTION_BAD) {
    throw ActiveRecordException(
      "Connection to database failed", __FILE__, __LINE__
    );
  }
}

void PostgresqlConnection::disconnect() {
  if(pgconn_ != nullptr) {
    PQfinish(pgconn_);
    pgconn_ = nullptr;
  }
}

bool PostgresqlConnection::connected() {
  return pgconn_ != nullptr;
}

bool PostgresqlConnection::table_exists(const string& table_name) {
  string query = \
    "SELECT c.relname as table       \
     FROM pg_catalog.pg_class c      \
     WHERE c.relname = '" + table_name + "'";
  Row row = select_one(query);
  return row.has_data();
}

bool PostgresqlConnection::execute(
  const string& query,
  const AttributeList& parameters
) {
  log("PostgresqlConnection::execute");
  log(query);

  ParameterAllocations pa;
  PGresult* exec_result = prepare(query, parameters, pa);

  bool error = is_error(exec_result);
  if(error) {
    log_error(exec_result);
  }

  PQclear(exec_result);

  return !error;
}

int64 PostgresqlConnection::insert(
  const string& query,
  const AttributeList& parameters
) {
  log("PostgresqlConnection::insert");
  log(query);

  Attribute id = select_value(query, parameters);
  if(!id.has_data()) {
    return 0;
  }

  return boost::get<int64>(id);
}

Row PostgresqlConnection::select_one(
  const string& query,
  const AttributeList& parameters
) {
  log("PostgresqlConnection::select_one");
  log(query);

  ParameterAllocations pa;
  PGresult* exec_result = prepare(query, parameters, pa);

  ExecStatusType status = PQresultStatus(exec_result);
  if(status != PGRES_TUPLES_OK) {
    log_error(exec_result);
    PQclear(exec_result);
    return Row();
  }

  int row_count = PQntuples(exec_result);
  if(row_count == 0) {
    log("No data");
    PQclear(exec_result);
    return Row();
  }

  Row row(exec_result, 0);
  PQclear(exec_result);

  return row;
}

RowSet PostgresqlConnection::select_all(
  const string& query,
  const AttributeList& parameters
) {
  ParameterAllocations pa;
  PGresult* exec_result = prepare(query, parameters, pa);

  ExecStatusType status = PQresultStatus(exec_result);
  RowSet results;
  if(status != PGRES_TUPLES_OK) {
    log_error(exec_result);
    PQclear(exec_result);
    return results;
  }

  int row_count = PQntuples(exec_result);
  for(int row = 0; row <row_count; ++row) {
    results.push_back(Row(exec_result, row));
  }

  PQclear(exec_result);

  return results;
}

Attribute PostgresqlConnection::select_value(
    const string& query,
    const AttributeList& parameters
) {
  ParameterAllocations pa;
  PGresult* exec_result = prepare(query, parameters, pa);

  ExecStatusType status = PQresultStatus(exec_result);
  if(status != PGRES_TUPLES_OK) {
    log_error(exec_result);
    PQclear(exec_result);
    return Attribute();
  }

  int row_count = PQntuples(exec_result);
  if(row_count == 0) {
    PQclear(exec_result);
    return Attribute();
  }

  int columns = PQnfields(exec_result);
  if(columns == 0) {
    PQclear(exec_result);
    return Attribute();
  }

  Attribute value = Attribute::from_field(exec_result, 0, 0);
  PQclear(exec_result);

  return value;
}

AttributeList PostgresqlConnection::select_values(
  const string& query,
  const AttributeList& parameters
) {
  ParameterAllocations pa;
  PGresult* exec_result = prepare(query, parameters, pa);

  AttributeList results;
  ExecStatusType status = PQresultStatus(exec_result);
  if(status != PGRES_TUPLES_OK) {
    log_error(exec_result);
    PQclear(exec_result);
    return results;
  }

  int row_count = PQntuples(exec_result);
  for(int row = 0; row < row_count; ++row) {
    Attribute value = Attribute::from_field(exec_result, row, 0);
    results.push_back(value);
  }

  PQclear(exec_result);

  return results;
}

Table PostgresqlConnection::table_data(const string& table_name) {
  Table td(this, table_name);

  string pk = primary_key(table_name);
  td.primary_key(pk);

  string query = "                      \
    SELECT a.attname, a.atttypid        \
    FROM   pg_index i                   \
    JOIN   pg_attribute a               \
      ON   a.attrelid = i.indrelid      \
    WHERE  i.indrelid = '" + table_name + "'::regclass \
      AND  a.attnum> 0                 \
      AND  NOT a.attisdropped           \
  ";
  RowSet rows = select_all(query);

  for(auto& row: rows) {
    string name = row.get_text("attname");
    Oid pg_type = row.get_integer("atttypid");
    Type::Type type = Attribute::pg_type_to_ar_type(pg_type);

    if(name == pk) {
      continue;
    }

    td.fields().push_back(Field(name, type));
  }

  return td;
}

TableSet PostgresqlConnection::schema() {
  TableSet s;
  string query = "                         \
  SELECT c.relname                         \
  FROM   pg_catalog.pg_class c             \
    LEFT JOIN pg_catalog.pg_namespace n    \
    ON   n.oid = c.relnamespace            \
  WHERE c.relkind = 'r'                    \
    AND n.nspname <> 'pg_catalog'          \
    AND n.nspname <> 'information_schema'  \
    AND n.nspname !~ '^pg_toast'           \
    AND pg_catalog.pg_table_is_visible(c.oid) \
  ";

  RowSet rows = select_all(query);

  for(auto& row: rows) {
    string table_name = row.get_text("relname");
    s[table_name] = table_data(table_name);
  }

  return s;
}

string PostgresqlConnection::primary_key(const string& table_name) {
  string query = "                      \
    SELECT a.attname                    \
    FROM   pg_index i                   \
    JOIN   pg_attribute a               \
      ON   a.attrelid = i.indrelid      \
      AND  a.attnum = ANY(i.indkey)     \
    WHERE  i.indrelid = '" + table_name + "'::regclass \
      AND  i.indisprimary               \
  ";

  Attribute pk = select_value(query);

  if(!pk.has_data()) {
    return "";
  }

  return boost::get<string>(pk);
}

void PostgresqlConnection::remove_field(
  const string& table_name,
  const string& field_name
) {
  begin_transaction();
  string query = "                \
  ALTER TABLE " + table_name + "  \
  DROP COLUMN " + field_name + "  \
  ";
  execute(query);
  commit();
}

////////////////////////////////////////
// Private

void PostgresqlConnection::bind_parameters(
  const AttributeList& parameters,
  ParameterAllocations& pa
) {
  pa.param_count = parameters.size();

  if(pa.param_count == 0) {
    pa.param_values = nullptr;
    pa.param_types = nullptr;
    pa.param_lengths = nullptr;
    pa.param_formats = nullptr;
    return;
  }

  pa.param_values = new char*[pa.param_count];
  pa.param_types = new Oid[pa.param_count];
  pa.param_lengths = new int[pa.param_count];
  pa.param_formats = new int[pa.param_count];

  int i = 0;
  for(auto& parameter: parameters) {
    // TYPE_LIST
    switch(parameter.which()) {
      case Type::integer: {
        int64 value = boost::get<int64>(parameter);
        // From https://stackoverflow.com/q/190229
        int len = (value == 0) ? 1 : floor(log10l(labs(value))) + 1;

        if(value < 0) {
          ++len; // room for negative sign '-'
        }

        pa.param_values[i] = new char[len + 1];
        snprintf(pa.param_values[i], len + 1, "%ld", value);
        pa.param_types[i] = INT4OID;
        pa.param_lengths[i] = len;
        break;
      }

      case Type::text: {
        string value = boost::get<string>(parameter);
        int len = value.size();

        pa.param_values[i] = new char[len + 1];
        strncpy(pa.param_values[i], value.c_str(), len + 1);
        pa.param_types[i] = TEXTOID;
        pa.param_lengths[i] = len;
        break;
      }

      case Type::floating_point: {
        double value = boost::get<double>(parameter);
        int len = snprintf(pa.param_values[i], 0, "%f", value);

        pa.param_values[i] = new char[len + 1];
        snprintf(pa.param_values[i], len + 1, "%f", value);
        pa.param_types[i] = NUMERICOID;
        pa.param_lengths[i] = len;
        break;
      }

      case Type::date: {
        Date value = boost::get<Date>(parameter);
        string s = value.to_string();

        size_t len = s.size();
        pa.param_values[i] = new char[len + 1];
        strncpy(pa.param_values[i], s.c_str(), len + 1);
        pa.param_types[i] = DATEOID;
        pa.param_lengths[i] = len;
        break;
      }

      default: {
        cout << "Unrecognized type" << endl;
        throw ActiveRecordException("Type not implemented", __FILE__, __LINE__);
      }
    }
    pa.param_formats[i] = 0;
    ++i;
  }
}

PGresult* PostgresqlConnection::prepare(
  const string& query,
  const AttributeList& parameters,
  ParameterAllocations& pa
) {
  bind_parameters(parameters, pa);

  if(pa.param_count == 0) {
    return PQexec(pgconn_, query.c_str());
  }

  PGresult* result = PQexecParams(
    pgconn_,
    query.c_str(),
    pa.param_count,
    pa.param_types,
    pa.param_values,
    pa.param_lengths,
    pa.param_formats,
    0
  );

  cleanup(pa);

  return result;
}

void PostgresqlConnection::cleanup(ParameterAllocations& pa) {
  for(int i = 0; i < pa.param_count; ++i) {
    delete[] pa.param_values[i];
  }

  delete[] pa.param_values;
  delete[] pa.param_types;
  delete[] pa.param_lengths;
  delete[] pa.param_formats;
}

bool PostgresqlConnection::is_error(PGresult* exec_result) {
  ExecStatusType status = PQresultStatus(exec_result);
  return status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK;
 }

void PostgresqlConnection::log_error(PGresult* exec_result) {
  stringstream error;
  error << "PQexec returned an error. ";
  ExecStatusType status = PQresultStatus(exec_result);
  error << "Code: " <<status << " ";
  error << "Message: " << PQresultErrorMessage(exec_result) << " ";
  error << PQresultErrorField(exec_result, PG_DIAG_MESSAGE_PRIMARY) << " ";
  error << PQresultErrorField(exec_result, PG_DIAG_MESSAGE_DETAIL) << " ";
  error << PQresultErrorField(exec_result, PG_DIAG_MESSAGE_HINT) << " ";
  log(error.str());
}

} // namespace ActiveRecord
