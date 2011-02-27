#include <active_record/type.h>

namespace ActiveRecord {

TypeNamePair type_pairs[] = {
  TypeNamePair( ActiveRecord::integer,        "INTEGER" ),
  TypeNamePair( ActiveRecord::text,           "TEXT" ),
  TypeNamePair( ActiveRecord::floating_point, "FLOAT" ),
  TypeNamePair( ActiveRecord::date,           "DATE" )
};

Type type_list[] = {
  integer,
  text,
  floating_point
};

TypeNameMap type_name( type_pairs,
                       type_pairs + sizeof( type_pairs ) / sizeof( type_pairs[ 0 ] ) );

Type index_to_type( int index ) {
  type_list[ index ];
}

Type to_type( const string &type_name ) {
  if( type_name == "INTEGER" )
    return ActiveRecord::integer;
  else if( type_name == "TEXT" )
    return ActiveRecord::text;
  else if( type_name == "FLOAT" )
    return ActiveRecord::floating_point;
  else if( type_name == "DATE" )
    return ActiveRecord::date;
  else
    return ActiveRecord::unknown;
}

} // namespace ActiveRecord
