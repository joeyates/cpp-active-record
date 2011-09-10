#ifndef _ACTIVE_RECORD_QUERY_H_
#define _ACTIVE_RECORD_QUERY_H_

#include <sstream>
#include <string>
#include <vector>

#include <active_record/type.h>
#include <active_record/table.h>
#include <active_record/table_set.h>
#include <active_record/row.h>
#include <active_record/connection.h>
using namespace std;

#define INVALID_LIMIT -1

namespace ActiveRecord {

template < class T >
class Query {
 public:
 Query( Connection& connection ) : limit_( INVALID_LIMIT ), connection_( &connection ) {};
  Query( const Query< T > &other );
  Query< T >             operator=( const Query< T > &other );

  Query< T >             where( const string &condition, const Attribute &value );
  Query< T >             order( string order );
  Query< T >             limit( int limit );
  // Results
  T                      first();
  vector< T >            all();

 protected:
  AttributePairList      conditions_;
  int                    limit_;
  vector< string >       orderings_;
  Connection*            connection_;

 private:
  QueryParametersPair    condition_clause();
  string                 order_clause();
  string                 limit_clause();
  QueryParametersPair    query_and_parameters();
};

template < class T >
Query< T >::Query( const Query< T > &other ) {
  conditions_ = other.conditions_;
  limit_      = other.limit_;
  orderings_  = other.orderings_;
  connection_ = other.connection_;
}

template < class T >
Query< T > Query< T >::operator=( const Query< T > &other ) {
  Query< T > result( other );
  return result;
}

// foo.where( "name = ?", "Joe" );
template < class T >
Query< T > Query< T >::where( const string &condition, const Attribute &value ) {
  conditions_.push_back( AttributePair( condition, value ) );
  return *this;
}

// foo.limit( 50 );
template < class T >
Query< T > Query< T >::limit( int limit ) {
  limit_ = limit;
  return *this;
}

// foo.order( "bar DESC" );
template < class T >
Query< T > Query< T >::order( string order ) {
  orderings_.push_back( order );
  return *this;
}
template < class T >
string Query< T >::limit_clause() {
  if( limit_ == INVALID_LIMIT )
    return "";
  stringstream ss;
  ss << " LIMIT " << limit_;
  return ss.str();
}

template < class T >
QueryParametersPair Query< T >::condition_clause() {
  AttributeList parameters;
  if( conditions_.size() == 0 )
    return QueryParametersPair( "", parameters );
  stringstream ss;
  ss << " ";
  for( AttributePairList::const_iterator it = conditions_.begin(); it != conditions_.end(); ++it ) {
    if( it == conditions_.begin() )
      ss << "WHERE ";
    else
      ss << " AND ";
    ss << it->first;
    parameters.push_back( it->second );
  }
  return QueryParametersPair( ss.str(), parameters );
}

template < class T >
QueryParametersPair Query< T >::query_and_parameters() {
  Table t = connection_->get_table( T::class_name );
  stringstream ss;
  ss << "SELECT ";
  ss << t.primary_key() << " ";
  ss << "FROM ";
  ss << t.table_name();
  QueryParametersPair conditions = condition_clause();
  ss << conditions.first;
  ss << order_clause();
  ss << limit_clause();
  ss << ";";
  return QueryParametersPair( ss.str(), conditions.second );
}

template < class T >
vector< T > Query< T >::all() {
  QueryParametersPair query = query_and_parameters();
  RowSet rows               = connection_->select_all( query.first, query.second );
  Table t                   = connection_->get_table( T::class_name );
  string primary_key        = t.primary_key();
  vector< T > results;
  for( RowSet::iterator it = rows.begin(); it != rows.end(); ++it ) {
    T record( it->get_integer( primary_key ) );
    results.push_back( record );
  }
  return results;
}

template < class T >
T Query< T >::first() {
  QueryParametersPair query = query_and_parameters();
  Row row                   = connection_->select_one( query.first, query.second );
  if( ! row.has_data() )
    throw ActiveRecordException( "No data", __FILE__, __LINE__ );

  Table t                   = connection_->get_table( T::class_name );
  string primary_key        = t.primary_key();
  T record( row.get_integer( primary_key ) );

  return record;
}

/////////////////////////////////////////////
// Private

template < class T >
string Query< T >::order_clause() {
  if( orderings_.size() == 0 )
    return "";
  stringstream ss;
  ss << " ";
  for( vector< string >::const_iterator it = orderings_.begin(); it != orderings_.end(); ++it ) {
    if( it == orderings_.begin() )
      ss << "ORDER BY ";
    else
      ss << ", ";
    ss << *it;
  }
  return ss.str();
}

} // namespace ActiveRecord

namespace boost
{
namespace assign
{

template<>
inline assign_detail::generic_list< ActiveRecord::Attribute >
list_of( const ActiveRecord::Attribute &t ) {
  return assign_detail::generic_list< ActiveRecord::Attribute >()( t );
}

} // namespace assign
} // namespace boost

namespace ActiveRecord {

/*
 ( parameters 13 "hello" 15.5 )
*/
inline AttributeList parameters( const Attribute &value ) {
  return boost::assign::list_of( ActiveRecord::Attribute( value ) );
}

} // namespace ActiveRecord

#endif // ndef _ACTIVE_RECORD_QUERY_H_
