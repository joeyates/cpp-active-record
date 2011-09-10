#ifndef _ACTIVE_RECORD_BASE_H_
#define _ACTIVE_RECORD_BASE_H_

#include <algorithm>
#include <sstream>
#include <string>

#include <active_record/active_record.h>
#include <active_record/exception.h>
#include <active_record/connection.h>
#include <active_record/query.h>
#include <active_record/table.h>

#define ACTIVE_RECORD_UNSAVED -1

#define AR_CONSTRUCTORS( klass )                                                                     \
  klass( int id = ACTIVE_RECORD_UNSAVED ) : ActiveRecord::Base< klass >( id ) {}                     \
  klass( const GenericAttributePairList &attributes ) : ActiveRecord::Base< klass >( attributes ) {}

// The model needs to know it's own name
#define AR_DECLARE( klass )                                             \
  template <>                                                           \
  ActiveRecord::Connection* ActiveRecord::Base< klass >::connection__ = NULL ; \
  template <>                                                           \
  string ActiveRecord::Base< klass >::class_name = #klass;

#define AR_HAS_MANY( owner, item )                           \
  namespace ActiveRecord {                                   \
    template< class owner > template< class other >          \
    vector< other > ActiveRecord::Base< owner >::has_many(); \
  }

#define AR_BELONGS_TO( item, owner )                \
  namespace ActiveRecord {                          \
    template< class item > template< class owner >  \
    owner ActiveRecord::Base< item >::belongs_to(); \
  }

namespace ActiveRecord {

/*
GCC wants template instantiation in the same file as the declaration,
so all of the code is here.
*/

template < class T >
class Base {
  enum state { blank, prepared, unsaved, loaded };
 public:
  // Static members
  static string       class_name;
  static Connection * connection__;
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
  Base& init( const GenericAttributePairList &attributes ) {
    ensure_prepared();

    for( GenericAttributePairList::const_iterator it = attributes.begin();
         it != attributes.end();
         ++it ) {
      attributes_[ it->first ] = it->second;
    }

    state_ = unsaved;

    return *this;
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
  Date date( const string &name ) {
    load_unless_new();
    return boost::get< Date >( attributes_[ name ] );
  }
  // Associations
  template< class T1 >
  vector< T1 > has_many() {
    if( state_ < loaded )
      throw ActiveRecordException( "Instance not loaded", __FILE__, __LINE__ );

    Query< T1 > query( *connection__ );
    Table t1 = connection__->get_table( T1::class_name );
    stringstream where;
    where << singular_name_ << "_id = ?";
    return query.where( where.str(), id() ).all();
  }

  template< class T1 >
  T1 belongs_to() {
    if( state_ < loaded )
      throw ActiveRecordException( "Instance not loaded", __FILE__, __LINE__ );

    Query< T1 > query( *connection__ );
    Table t1 = connection__->get_table( T1::class_name );
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
  inline int id() const {
    return id_;
  }
  bool has_data() const {
    return ( attributes_.size() > 0 ) ? true : false;
  }
  // No need to load either instance:
  // if states and attributes are the same,
  // we accept they're the same
  bool operator==( const Base& other ) const {
    if( id() != other.id() )
      return false;
    if( state_ != other.state_ )
      return false;
    if( attributes_.size() != other.attributes_.size() )
      return false;
    for( AttributeHash::const_iterator it = attributes_.begin();
         it != attributes_.end();
         ++it ) {
    }
    return true;
  }
  string to_string() const;

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

  T::connection__ = connection;
  Table td = T::table( connection__ );

  if( td.table_name().empty() ) {
    throw ActiveRecordException( "set the table name when returning Table", __FILE__, __LINE__ );
  }
  connection__->set_table( T::class_name, td );
}

template< class T >
string Base< T >::to_string() const {
  const_cast<Base< T > *>( this )->load_unless_new();
  stringstream ss;
  ss << T::class_name << ": ";
  for( AttributeHash::const_iterator it = attributes_.begin();
       it != attributes_.end();
       ++it ) {
    if( it != attributes_.begin() )
      ss << ", ";
    ss << it->first << " " << it->second;
  }
  return ss.str();
}

template< class T >
ostream & operator<<( ostream &cout, const ActiveRecord::Base< T >& record ) {
  cout << record.to_string();
  return cout;
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

  Row row = connection__->select_one( ss.str(), parameters );
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

  id_    = ( int ) connection__->insert( ss.str(), parameters );
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
  return connection__->execute( ss.str(), parameters );
}

// State

template < class T >
void Base< T >::prepare() {
  log( "Base::prepare" );
  if( state_ >= prepared )
    return;
  if( T::class_name.empty() )
    return;
  log( T::class_name );
  log( "connection__.get_table" );
  Table t        = connection__->get_table( T::class_name );
  primary_key_   = t.primary_key();
  table_name_    = t.table_name();
  log( "table_name_:" );
  log( table_name_ );
  singular_name_ = T::class_name;
  std::transform( singular_name_.begin(), singular_name_.end(), singular_name_.begin(), ::tolower );
  state_         = prepared;
}

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_BASE_H_
