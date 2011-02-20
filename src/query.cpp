#include <active_record/query.h>

namespace ActiveRecord {

extern Connection connection;

template < class T >
vector< T > Query< T>::all() {
  QueryParametersPair query = query_and_parameters();
  RowSet rows               = connection.select_all( query.first, query.second );
  Table t                   = connection.get_table( T::class_name );
  string primary_key        = t.primary_key();
  vector< T > results;
  for( RowSet::iterator it = rows.begin(); it != rows.end(); ++it ) {
    T record( it->get_integer( primary_key ) );
    results.push_back( record );
  }
  return results;
}

template < class T >
QueryParametersPair Query< T >::query_and_parameters() {
  Table t = ActiveRecord::connection.get_table( T::class_name );
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

} // namespace ActiveRecord
