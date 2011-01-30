#include <active_record/types.h>
// include <active_record/table_data.h>

namespace ActiveRecord {

TypeNamePair type_pairs[] =
{
TypeNamePair( ActiveRecord::integer, "INTEGER" ),
TypeNamePair( ActiveRecord::text, "TEXT" ),
TypeNamePair( ActiveRecord::floating_point, "REAL" )
};

TypeNameMap type_name( type_pairs, type_pairs + sizeof( type_pairs ) / sizeof( type_pairs[ 0 ] ) );

Type index_to_type[] = { integer, text, floating_point };

Type get_type( const Attribute & attribute )
{
  int type_index = attribute.which();
  return index_to_type[ type_index ];
}

Type to_type( const string &type_name )
{
  if( type_name == "INTEGER" )
    return ActiveRecord::integer;
  else if( type_name == "TEXT" )
    return ActiveRecord::text;
  else if( type_name == "REAL" )
    return ActiveRecord::floating_point;
  else
    return ActiveRecord::unknown;
}

//std::basic_ostream< char, std::char_traits< char > >&
//operator<<( std::basic_ostream< char, std::char_traits< char > > &ostream, NullType nt ) { return ostream; }

} // namespace ActiveRecord
