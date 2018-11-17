#include <active_record/connection/postgresql.h>
#include <climits>
#include <c.h>
#include <catalog/pg_type.h>
#include <math.h>

#include <active_record/type.h>

namespace ActiveRecord {

PostgresqlConnection::PostgresqlConnection() : pgconn_(NULL) {}

PostgresqlConnection::~PostgresqlConnection() {
  disconnect();
}

PostgresqlConnection::PostgresqlConnection(const PostgresqlConnection& other) {}

PostgresqlConnection PostgresqlConnection::operator=(
    const PostgresqlConnection& other) {
  return *this;
}

bool PostgresqlConnection::create_database(PostgresqlConnection & connection,
    OptionsHash options) {
  stringstream create_stream;
  create_stream << "CREATE DATABASE " <<options["database"] << " ";
  if(options.find("owner") != options.end())
    create_stream << "WITH OWNER " <<options["owner"] << " ";
  if(options.find("template") != options.end())
    create_stream << "TEMPLATE " <<options["template"] << " ";
  if(options.find("encoding") != options.end())
    create_stream << "ENCODING '" <<options["encoding"] << "' ";
  create_stream << ";";
  return connection.execute(create_stream.str());
}

void PostgresqlConnection::drop_database(PostgresqlConnection & connection,
    const string &database_name) {
  stringstream query;
  query << "DROP DATABASE " <<database_name << ";";
  connection.execute(query.str());
}

bool PostgresqlConnection::database_exists(PostgresqlConnection & connection,
    const string &database_name) {
  stringstream query;
  query << "SELECT datname FROM pg_database WHERE datname = '" <<database_name << "';";
  Row row = connection.select_one(query.str());
  return row.has_data();
}

/////////////////////////////////////////////////////
// Connection

void PostgresqlConnection::connect(OptionsHash options) {
  stringstream connection_stream;
  connection_stream << "dbname=" <<options["database"] << " ";
  if(options.find("host") != options.end())
    connection_stream << "host=" <<options["host"] << " ";
  if(options.find("username") != options.end())
    connection_stream << "user=" <<options["username"] << " ";
  if(options.find("port") != options.end())
    connection_stream << "port=" <<options["port"] << " ";
  string conninfo = connection_stream.str();

  pgconn_ = PQconnectdb(conninfo.c_str());

  ConnStatusType status = PQstatus(pgconn_);
  if(status == CONNECTION_BAD) {
    throw ActiveRecordException("Connection to database failed", __FILE__, __LINE__);
  }
}

void PostgresqlConnection::disconnect() {
  if(pgconn_ != NULL) {
    PQfinish(pgconn_);
    pgconn_ = NULL;
  }
}

bool PostgresqlConnection::connected() {
  return pgconn_ != NULL ? true : false;
}

bool PostgresqlConnection::table_exists(const string &table_name) {
  string query = \
    "SELECT c.relname as table       \
     FROM pg_catalog.pg_class c      \
     WHERE c.relname = '" + table_name + "'";
  Row row = select_one(query);
  return row.has_data();
}

bool PostgresqlConnection::execute(const string &query,
    const AttributeList &parameters) {
  log("PostgresqlConnection::execute");
  log(query);
  PGresult * exec_result = PQexec(pgconn_, query.c_str());
  bool success = is_error(exec_result);
  if(!success)
    log_error(exec_result);
  PQclear(exec_result);
  return success;
}

long PostgresqlConnection::insert(const string &query,
    const AttributeList &parameters) {
  log("PostgresqlConnection::insert");
  log(query);
  Attribute id = select_value(query, parameters);
  if(!id.has_data())
    return 0;
  return boost::get<int>(id);
}

Row PostgresqlConnection::select_one(const string &query,
    const AttributeList &parameters) {
  log("PostgresqlConnection::select_one");
  log(query);
  PGresult * exec_result = execute_params(query, parameters);
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

RowSet PostgresqlConnection::select_all(const string &query,
    const AttributeList &parameters) {
  RowSet results;
  PGresult * exec_result = execute_params(query, parameters);
  ExecStatusType status = PQresultStatus(exec_result);
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
    const string &query,
    const AttributeList &parameters) {
  PGresult * exec_result = execute_params(query, parameters);
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

AttributeList PostgresqlConnection::select_values(const string &query,
    const AttributeList &parameters) {
  AttributeList results;
  PGresult * exec_result = execute_params(query, parameters);
  ExecStatusType status = PQresultStatus(exec_result);
  if(status != PGRES_TUPLES_OK) {
    log_error(exec_result);
    PQclear(exec_result);
    return results;
  }
  int row_count = PQntuples(exec_result);
  for(int row = 0; row <row_count; ++row) {
    Attribute value = Attribute::from_field(exec_result, row, 0);
    results.push_back(value);
  }
  PQclear(exec_result);
  return results;
}

Table PostgresqlConnection::table_data(const string &table_name) {
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
  for(RowSet::iterator it = rows.begin(); it != rows.end(); ++it) {
    string name = it->get_text("attname");
    Oid pg_type = it->get_integer("atttypid");
    Type::Type type = Attribute::pg_type_to_ar_type(pg_type);

    if(name == pk)
      continue;

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
  for(RowSet::iterator it = rows.begin(); it != rows.end(); ++it) {
    string table_name = it->get_text("relname");
    s[table_name] = table_data(table_name);
  }
  return s;
}

string PostgresqlConnection::primary_key(const string &table_name) {
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
  if(! pk.has_data())
    return "";
  return boost::get<string>(pk);
}

void PostgresqlConnection::remove_field(const string &table_name,
    const string &field_name) {
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

PGresult * PostgresqlConnection::execute_params(const string &query,
    const AttributeList &parameters) {
  int param_count = parameters.size();
  if(param_count == 0)
    return PQexec(pgconn_, query.c_str());

  char ** paramValues = new char *[param_count];
  Oid * paramTypes = new Oid[param_count];
  int * paramLengths = new int[param_count];
  int * paramFormats = new int[param_count];

  int i = 0;
  for(AttributeList::const_iterator it = parameters.begin();
    it != parameters.end();
    ++it) {
    switch(it->which()) {
      case Type::integer: {
        int value = boost::get<int>(*it);
        // From http://stackoverflow.com/questions/190229/where-is-the-itoa-function-in-linux
        int len = (value == 0) ? 1 : floor(log10l(labs(value))) + 1;
        if(value < 0)
          ++len; // room for negative sign '-'

        paramValues[i] = new char[len + 1];
        int used = snprintf(paramValues[i], len + 1, "%ld", value);
        paramTypes[i] = INT4OID;
        paramLengths[i] = len;
        break;
      }
      case Type::text: {
        string value = boost::get<string>(*it);
        int len = value.size();

        paramValues[i] = new char[len + 1];
        strncpy(paramValues[i], value.c_str(), len + 1);
        paramTypes[i] = TEXTOID;
        paramLengths[i] = len;
        break;
      }
      case Type::floating_point: {
        double value = boost::get<double>(*it);
        int len = snprintf(paramValues[i], 0, "%f", value);

        paramValues[i] = new char[len + 1];
        int used = snprintf(paramValues[i], len + 1, "%f", value);
        paramTypes[i] = NUMERICOID;
        paramLengths[i] = len;
        break;
      }
      case Type::date: {
      }
      default: {
        cout << "Unrecognized type" <<endl;
        throw ActiveRecordException("Type not implemented", __FILE__, __LINE__);
      }
    }
    paramFormats[i] = 0;
    ++i;
  }

  PGresult * result = PQexecParams(
    pgconn_,
    query.c_str(),
    param_count,
    paramTypes,
    paramValues,
    paramLengths,
    paramFormats,
    0
  );

  for(int i = 0; i <param_count; ++i)
    delete[] paramValues[i];

  delete[] paramValues;
  delete[] paramTypes;
  delete[] paramFormats;

  return result;
}

bool PostgresqlConnection::is_error(PGresult *exec_result) {
  ExecStatusType status = PQresultStatus(exec_result);
  return (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK) ? false : true;
 }

void PostgresqlConnection::log_error(PGresult *exec_result) {
  stringstream error;
  error << "PQexec returned an error. ";
  ExecStatusType status = PQresultStatus(exec_result);
  error << "Code: " <<status << " ";
  error << "Message: " << PQresultErrorMessage(exec_result) << " ";
  error << "" << PQresultErrorField(exec_result, PG_DIAG_MESSAGE_PRIMARY) << " ";
  error << "" << PQresultErrorField(exec_result, PG_DIAG_MESSAGE_DETAIL) << " ";
  error << "" << PQresultErrorField(exec_result, PG_DIAG_MESSAGE_HINT) << " ";
  log(error.str());
}

} // namespace ActiveRecord
