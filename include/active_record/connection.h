#ifndef _ACTIVE_RECORD_CONNECTION_H_
#define _ACTIVE_RECORD_CONNECTION_H_

#include <active_record/options.h>
#include <active_record/type.h>
#include <active_record/table.h>
#include <active_record/table_set.h>
#include <active_record/row.h>
#include <cstdio>

namespace ActiveRecord {

class Table;

class Connection {
 public:
  virtual void  connect( OptionsHash options ) = 0;
  virtual void  disconnect()                   = 0;
  virtual bool  connected()                    = 0;

  // Tables/Models
  void          set_table( const string &class_name, const Table &table );
  Table &       get_table( const string &class_name );
  void          update_database();

  // Database Structure
  virtual bool  table_exists( const string &table_name )                                   = 0;
  // Transactions
  void          begin_transaction();
  void          commit();
  // Queries
  virtual bool          execute( const string &query,
                                 const AttributeList &parameters = AttributeList() )       = 0;
  virtual long          insert( const string &query,
                                const AttributeList &parameters = AttributeList() )        = 0;
  //virtual Attribute     select_value( const string &query,
  //                                    const AttributeList &parameters = AttributeList() )  = 0;
  //virtual AttributeList select_values( const string &query,
  //                                     const AttributeList &parameters = AttributeList() ) = 0;
  virtual Row           select_one( const string &query,
                                    const AttributeList &parameters = AttributeList() )    = 0;
  virtual RowSet        select_all( const string &query,
                                    const AttributeList &parameters = AttributeList() )    = 0;
 private:
  TableSet         tables_;
};

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_CONNECTION_H_
