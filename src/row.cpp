#include <active_record/row.h>

#include <sstream>

#include <active_record/active_record.h>
#include <active_record/attribute.h>
#include <active_record/type.h>
#include <active_record/exception.h>

namespace ActiveRecord {

Type::Type Row::get_type( const string &name ) {
  return attributes_[ name ].type();
}

bool Row::is_type( const string &name, Type::Type type ) {
  Type::Type actual = attributes_[ name ].type();
  return ( actual == type )? true : false;
}

string Row::get_text( const string &name ) {
  return boost::get< string >( attributes_[ name ] );
}

int Row::get_integer( const string &name ) {
  return boost::get< int >( attributes_[ name ] );
}

double Row::get_floating_point( const string &name ) {
  return boost::get< double >( attributes_[ name ] );
}

Date Row::get_date( const string &name ) {
  return boost::get< Date >( attributes_[ name ] );
}

string Row::to_string() const {
  stringstream row;
  row << "{" << endl;
  for( AttributeHash::const_iterator it = attributes_.begin();
       it != attributes_.end();
       ++it ) {
    row << it->first << ": ";
    row << it->second << endl;
  }
  row << "}" << endl;
  return row.str();
}

ostream & operator<<( ostream &cout, const ActiveRecord::Row &row ) {
  cout << row.to_string();
  return cout;
}

} // namespace ActiveRecord
