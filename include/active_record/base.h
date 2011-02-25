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
  klass( int id = ACTIVE_RECORD_UNSAVED ) : ActiveRecord::Base< klass >( id ) {} \
  klass( const GenericAttributePairList &attributes ) : ActiveRecord::Base< klass >( attributes ) {}

namespace ActiveRecord {

extern Connection connection;

/*
GCC wants template instantiation in the same file as the declaration,
so all of the code is here.
*/

template < class T >
class Base {
  enum state { blank, prepared, unsaved, loaded };
 public:
  // Static members
  static string class_name;
  static void setup( Connection * connection );

  // Constructors
  Base( int id = ACTIVE_RECORD_UNSAVED ) : state_( blank ) {
    id_ = id;
  }
  Base( const GenericAttributePairList &attributes ) : state_( blank ) {
    id_ = ACTIVE_RECORD_UNSAVED;
    init( attributes );
  }

  // Initialization
  void init( const GenericAttributePairList &attributes ) {
    ensure_prepared();

    for( GenericAttributePairList::const_iterator it = attributes.begin();
         it != attributes.end();
         ++it ) {
      attributes_[ it->first ] = it->second;
    }

    state_ = unsaved;
  }

  // Attributes
  Attribute& operator[]( const string &name ) {
    load_unless_new();
    return attributes_[ name ];
  }
  string text( const string &name ) {
    load_unless_new();
    return boost::get< string >( attributes_[ name ] );
  }
  int integer( const string &name ) {
    load_unless_new();
    return boost::get< int >( attributes_[ name ] );
  }
  double floating_point( const string &name ) {
    load_unless_new();
    return boost::get< double >( attributes_[ name ] );
  }

  // Associations
  template< class T1 >
  vector< T1 > has_many() {
    if( state_ < loaded )
      throw ActiveRecordException( "Instance not loaded", __FILE__, __LINE__ );

    Query< T1 > query;
    Table t1 = connection.get_table( T1::class_name );
    stringstream where;
    where << singular_name_ << "_id = ?";
    return query.where( where.str(), id() ).all();
  }

  template< class T1 >
  T1 belongs_to() {
    if( state_ < loaded )
      throw ActiveRecordException( "Instance not loaded", __FILE__, __LINE__ );

    Query< T1 > query;
    Table t1 = connection.get_table( T1::class_name );
    string primary_key = t1.primary_key();
    stringstream where;
    where << primary_key << " = ?";
    return query.where( where.str(), id() ).first();
  }

  // Other
  bool save() {
    ensure_prepared();
    if( id_ == ACTIVE_RECORD_UNSAVED )
      return create();
    else
      return update();
  }
  inline int id() {
    return id_;
  }
  bool has_data() {
    return ( attributes_.size() > 0 ) ? true : false;
  }

 private:
  // Member variables
  state         state_;
  AttributeHash attributes_;
  int           id_;
  string        primary_key_;
  string        table_name_;
  string        singular_name_;

  // State
  void ensure_prepared() {
    if( state_ < prepared )
      prepare();
  }
  void load_unless_new() {
    ensure_prepared();
    if( id() == ACTIVE_RECORD_UNSAVED )
      return;
    ensure_loaded();
  }
  void ensure_loaded() {
    ensure_prepared();
    if( state_ == loaded )
      return;
    load();
  }

  // Load/save
  bool          load();
  bool          create();
  bool          update();

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
  ss << primary_key_ << " = ?";
  AttributeList parameters;
  parameters.push_back( id() );

  Row row = connection.select_one( ss.str(), parameters );
  if( ! row.has_data() )
    throw ActiveRecordException( "Record not found", __FILE__, __LINE__ );

  attributes_ = row.attributes();
  state_      = loaded;
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

  id_    = ( int ) connection.insert( ss.str(), parameters );
  state_ = loaded;

  return true;
}

template < class T >
bool Base< T >::update() {
  ensure_loaded();
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

// State

template < class T >
void Base< T >::prepare() {
  if( state_ >= prepared )
    return;
  if( T::class_name.empty() )
    return;
  Table t        = connection.get_table( T::class_name );
  primary_key_   = t.primary_key();
  table_name_    = t.table_name();
  singular_name_ = T::class_name;
  std::transform( singular_name_.begin(), singular_name_.end(), singular_name_.begin(), ::tolower );
  state_         = prepared;
}

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_BASE_H_
