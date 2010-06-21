#include <active_record/types.h>
#include <active_record/table_data.h>

namespace ActiveRecord {

TypeNamePair type_pairs[] =
{
TypeNamePair(ActiveRecord::integer, "INTEGER"),
TypeNamePair(ActiveRecord::text, "TEXT"),
TypeNamePair(ActiveRecord::floating_point, "FLOAT")
};

TypeNameMap type_name(type_pairs, type_pairs + sizeof(type_pairs) / sizeof(type_pairs[0]));

//std::basic_ostream< char, std::char_traits< char > >&
//operator<<(std::basic_ostream< char, std::char_traits< char > > &ostream, NullType nt) { return ostream; }

} // namespace ActiveRecord
