#include <active_record/attribute.h>

namespace ActiveRecord {

Type get_type( const Attribute & attribute ) {
  int type_index = attribute.which();
  return index_to_type( type_index );
}

} // namespace ActiveRecord
