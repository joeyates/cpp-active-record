#ifndef _ACTIVE_RECORD_SQLITE3_CONNECTION_H_
#define _ACTIVE_RECORD_SQLITE3_CONNECTION_H_

#include <sqlite3.h>
#include <active_record/active_record.h>
#include <active_record/connection.h>

namespace ActiveRecord {

class Table;

class Sqlite3Connection : public Connection {
 public:
  Sqlite3Connection();
  virtual ~Sqlite3Connection();

  virtual void  connect( OptionsHash options );
  virtual void  disconnect();
  virtual bool  connected();

  // Database Structure
  virtual bool  table_exists( const string &table_name );
  // Queries
  virtual bool          execute( const string &query,
                                 const AttributeList &parameters = AttributeList() );
  virtual long          insert( const string &query,
                                const AttributeList &parameters = AttributeList() );
  //virtual Attribute     select_value( const string &query,
  //                                    const AttributeList &parameters = AttributeList() );
  //virtual AttributeList select_values( const string &query,
  //                                     const AttributeList &parameters = AttributeList() );
  virtual Row           select_one( const string &query,
                                    const AttributeList &parameters = AttributeList() );
  virtual RowSet        select_all( const string &query,
                                    const AttributeList &parameters = AttributeList() );
 private:
  Sqlite3Connection( const Sqlite3Connection& other );
  Sqlite3Connection operator=( const Sqlite3Connection& other );

  bool           sqlite_initialize( string database_path_name );
  static string  sqlite_error( int error_code );
  sqlite3_stmt * prepare( const string &query, const AttributeList &parameters );
  void           bind_parameters( sqlite3_stmt *ppStmt, const AttributeList &parameters );

  sqlite3 *      db_;
};

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_SQLITE3_CONNECTION_H_
