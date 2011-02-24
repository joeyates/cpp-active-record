#ifndef _ACTIVE_RECORD_BASE_H_
#define _ACTIVE_RECORD_BASE_H_

#include <algorithm>
#include <sstream>
#include <string>
#include <active_record/exception.h>
#include <active_record/connection.h>
#include <active_record/query.h>
#include <active_record/table.h>

#define ACTIVE_RECORD_UNSAVED -1

#define AR_CONSTRUCTORS( klass ) \
  klass() : ActiveRecord::Base< klass >( ACTIVE_RECORD_UNSAVED ) {} \
  klass( int id ) : ActiveRecord::Base< klass >( id ) {} \
  klass( const GenericAttributePairList &attributes ) : ActiveRecord::Base< klass >( attributes ) {}

#define HAS_MANY( association )
#define HAS_MANY_THROUGH( association, through )
#define HAS_ONE( association )

namespace ActiveRecord {

extern Connection connection;

/*
GCC wants template instantiation in the same file as the declaration,
so all of the code is here.
*/

template < class T >
class Base {
 public:
  // Static members
  static string class_name;
  static void setup( Connection * connection );

  // Constructors
  Base( int id = ACTIVE_RECORD_UNSAVED ) : loaded_( false ) {
    prepare();
    attributes_[ primary_key_ ] = id;
  }
  Base( const GenericAttributePairList &attributes ) : loaded_( true ) {
    prepare();
    for( GenericAttributePairList::const_iterator it = attributes.begin(); it != attributes.end(); ++it )
      attributes_[ it->first ] = it->second;
    attributes_[ primary_key_ ] = ACTIVE_RECORD_UNSAVED;
  }

  // Attributes
  Attribute& operator[]( const string &name ) {
    if( needs_load() )
      load();
    return attributes_[ name ];
  }
  string text( const string &name ) {
    if( needs_load() )
      load();
    return boost::get< string >( attributes_[ name ] );
  }
  int integer( const string &name ) {
    if( needs_load() )
      load();
    return boost::get< int >( attributes_[ name ] );
  }
  double floating_point( const string &name ) {
    if( needs_load() )
      load();
    return boost::get< double >( attributes_[ name ] );
  }

  // Associations
  template< class T1 >
  vector< T1 > many() {
    Query< T1 > query;
    Table t1 = connection.get_table( T1::class_name );
    string primary_key = t1.primary_key();
    stringstream where;
    where << singular_name_ << "_id = ?";
    return query.where( where.str(), id() ).all();
  }

  // Other
  bool save() {
    if( is_new() )
      return create();
    else
      return update();
  }
  inline int id() {
    return boost::get< int >( attributes_[ primary_key_ ] );
  }

 private:
  // Member variables
  bool          loaded_;
  AttributeHash attributes_;
  string        primary_key_;
  string        table_name_;
  string        singular_name_;

  // Load/save
  bool          load();
  bool          create();
  bool          update();
  inline bool is_new() {
    return ( id() == ACTIVE_RECORD_UNSAVED )? true : false;
  }
  inline bool needs_load() {
    return ( id() != ACTIVE_RECORD_UNSAVED && ! loaded_ )? true : false;
  }

  // Setup
  void prepare();
};

template < class T >
void Base< T >::setup( Connection * connection ) {
  if( connection == NULL )
    throw ActiveRecordException( "connection is NULL", __FILE__, __LINE__ );

  Table td = T::table( connection );

  if( td.table_name().empty() ) {
    throw ActiveRecordException( "set the table name when returning Table", __FILE__, __LINE__ );
    }
  connection->set_table( T::class_name, td );
}

/////////////////////////////////////////
// Private

template < class T >
bool Base< T >::load() {
  stringstream ss;
  ss << "SELECT * ";
  ss << "FROM " << table_name_ << " ";
  ss << "WHERE ";
  ss << primary_key_ << " = ";
  ss << attributes_[ primary_key_ ];

  Row row     = connection.select_one( ss.str() );
  attributes_ = row.attributes();
  loaded_     = true;
  return true;
}

// TODO: Re-read the record afterwards to get default values?
template < class T >
bool Base< T >::create() {
  stringstream ss;
  ss << "INSERT INTO " << table_name_ << " ";
  bool          columns_added = false;
  stringstream  columns, placeholders;
  AttributeList parameters;
  for( AttributeHash::iterator it = attributes_.begin(); it != attributes_.end(); ++it ) {
    if( it->first == primary_key_ )
      continue;
    if( columns_added ) {
      columns      << ", ";
      placeholders << ", ";
    }
    columns      << it->first;
    placeholders << "?";
    parameters.push_back( it->second );
    columns_added = true;
  }
  if( columns_added )
    ss << "(" << columns.str() << ") VALUES (" << placeholders.str() << ")";
  else // Handle INSERT with no data
    ss << "(" << primary_key_ << ") VALUES ( NULL )";

  long new_id = connection.insert( ss.str(), parameters );
  attributes_[ primary_key_ ] = ( int ) new_id;

  return true;
}

template < class T >
bool Base< T >::update() {
  if( !loaded_ )
    load();
  stringstream ss;
  ss << "UPDATE " << table_name_ << " ";
  bool          columns_added = false;
  stringstream  columns;
  AttributeList parameters;
  for( AttributeHash::iterator it = attributes_.begin(); it != attributes_.end(); ++it ) {
    if( it->first == primary_key_ )
      continue;
    if( columns_added )
      columns << ", ";
    columns << it->first << " = ?";
    parameters.push_back( it->second );
    columns_added = true;
  }
  ss << "SET "   << columns.str() << " ";
  ss << "WHERE " << primary_key_ << " = ?;";
  parameters.push_back( id() );
  return connection.execute( ss.str(), parameters );
}

template < class T >
void Base< T >::prepare() {
  Table t        = connection.get_table( T::class_name );
  primary_key_   = t.primary_key();
  table_name_    = t.table_name();
  singular_name_ = T::class_name;
  std::transform( singular_name_.begin(), singular_name_.end(), singular_name_.begin(), ::tolower );
}

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_BASE_H_
