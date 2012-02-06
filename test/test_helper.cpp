#include "test_helper.h"

#include <sstream>
#include <stdio.h>

#include <active_record/active_record.h>

AR_DECLARE( Person )

string database_name = "./test.sqlite3";

void connect_database( ActiveRecord::Connection &connection,
                       const string &database_name ) {
  connection.connect( options
                      ( "adapter", "sqlite" )
                      ( "database", database_name ) );
}

void delete_database() {
  string remove_database = "rm -f " + database_name;
  system( remove_database.c_str() );
}

list< string > shell_command( const string &command ) {
  FILE *pipe = popen( command.c_str(), "r" );
  if( !pipe )
    throw "Failed to open pipe";

  list< string > output;
  char buffer[ 100 ];
  while( fgets( buffer, 100, pipe ) != NULL ) {
    string line = buffer;
    output.push_back( line );
  }
  pclose( pipe );
  return output;
}

string table_definition( Connection &connection, const string &table_name ) {
  stringstream query;
  query << "SELECT sql FROM sqlite_master ";
  query << "WHERE type='table'";
  query << "AND name='" << table_name << "'";
  RowSet rows = connection.select_all( query.str() );
  if( rows.size() == 0 )
    return "";

  return rows.front().get_text( "sql" );
}

void pipe_to_sqlite( const string &database_name,
                     const string &command ) {
  stringstream ss;
  ss << "echo '" << command << "' | sqlite3 " << database_name << ";";
  system( ss.str().c_str() );
}

void assert_string( const string &expected, const string &actual ) {
  ASSERT_STREQ( expected.c_str(), actual.c_str() );
}

void assert_attribute( const Attribute &expected, const Attribute &actual ) {
  if( expected == actual )
    return;
  cout << "Actual: " << actual << endl;
  cout << "Expected: " << expected << endl;
  FAIL();
}

void assert_attribute_pair_list( const AttributePairList &expected,
                                 const AttributePairList &actual ) {
  ASSERT_EQ( expected.size(), actual.size() );
  for( AttributePairList::const_iterator itexp = expected.begin(), itact = actual.begin();
       itexp != expected.end();
       ++itexp, ++itact ) {
    assert_string( itexp->first, itact->first );
    assert_attribute( itexp->second, itact->second );
  }
}

// TODO: this is sqlite only
void assert_table_exists( const string &database_name,
                          const string &table_name ) {
  stringstream row_query;
  row_query << "echo '";
  row_query << "SELECT name FROM sqlite_master ";
  row_query << "WHERE type=\"table\" AND name = \"" << table_name << "\";";
  row_query << "' | sqlite3 " << database_name;
  FILE *pipe = popen( row_query.str().c_str(), "r" );
  if( !pipe )
    throw "Failed to open pipe";

  // We assume that any result means the table exists
  bool found = false;
  if( !feof( pipe ) ) {
    found = true;
  }
  pclose( pipe );
  ASSERT_TRUE( found );
}

void assert_file_exists( const string &file_name ) {
  struct stat buffer;
  int status = lstat( file_name.c_str(), &buffer);
  ASSERT_EQ( status, 0 );
}

void assert_file_non_zero_length( const string &file_name ) {
  struct stat buffer;
  int status = lstat( file_name.c_str(), &buffer);
  ASSERT_EQ( status, 0 );
  ASSERT_GT( buffer.st_size, 0 );
}

void assert_field_name( Table &td,
                        int field_index,
                        const string &name ) {
  ASSERT_STREQ( td.fields()[ field_index ].name().c_str(), name.c_str() );
}

void assert_field_type(Table &td, int field_index,
    ActiveRecord::Type::Type type) {
  ASSERT_EQ(type, td.fields()[field_index].type());
}

void assert_field( Table &td,
                   int field_index,
                   const string &name,
                   ActiveRecord::Type::Type type ) {
  assert_field_name( td, field_index, name );
  assert_field_type( td, field_index, type );
}

strings postgresql_shell_command( const string& database_name, const string &database_user, const string &query ) {
  string command = "echo '" + query + "' | psql -p 5432 -U " + database_user + " " + database_name;
  return shell_command(command.c_str());
}

void postgresql_shell_create_database( const string &create_database_name,
                                       const string &access_database_name,
                                       const string &database_user ) {
  postgresql_shell_drop_database(create_database_name, access_database_name, database_user);
  string query = "CREATE DATABASE " + create_database_name;
  postgresql_shell_command( access_database_name, database_user, query );
}

void postgresql_shell_drop_database( const string &drop_database_name,
                                     const string &access_database_name,
                                     const string &database_user ) {
  if( ! postgresql_shell_database_exists( drop_database_name, database_user ) )
    return;
  string query = "DROP DATABASE " + drop_database_name;
  postgresql_shell_command( access_database_name, database_user, query );
}

bool postgresql_shell_database_exists( const string &database_name, const string &database_user ) {
  string command = "psql -p 5432 -U " + database_user + " -l | grep " + database_name;
  list< string > output = shell_command( command );
  return output.size() > 0 ? true : false;
}

void assert_postgresql_table_exists(const string &database_name,
    const string &table_name, const string &database_user) {
  // TODO
}
