#ifndef _ACTIVE_RECORD_CONNECTION_H_
#define _ACTIVE_RECORD_CONNECTION_H_

#include <active_record/types.h>
#include <active_record/row.h>
#include <active_record/query.h>
#include <cstdio>
#include <sqlite3.h>

namespace ActiveRecord {

class Connection {
 public:
  Connection();
  void    connect( OptionsHash options );
  void    add_class( const string &klass ) { klasses_.push_back( klass ); }
  void    update_database();
  void    update_table( const string &klass );
  void    begin_transaction();
  void    commit();
  bool    execute( const string &query, const AttributeList &parameters = AttributeList() );
  Row     select_value( const string &query, const AttributeList &parameters = AttributeList() );
  Row     select_one( const string &query, const AttributeList &parameters = AttributeList() );
  RowSet  select_values( const string &query, const AttributeList &parameters = AttributeList() );
  RowSet  select_all( const string &query, const AttributeList &parameters = AttributeList() );
 private:
  Connection( const Connection& other );
  Connection operator=( const Connection& other );
 private:
  bool      sqlite_initialize( string database_path_name );
  void      bind_parameters( sqlite3_stmt *ppStmt, const AttributeList &parameters );
 private:
  vector< string > klasses_;
  sqlite3 *        db_;
};

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_CONNECTION_H_
