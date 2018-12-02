#include <active_record/active_record.h>

#include <active_record/base.h>
#include <active_record/connection.h>

namespace ActiveRecord {

bool debug = false;

void set_debug(bool new_debug) {
  debug = new_debug;
}

void log(const std::string& message) {
  if(!debug) {
    return;
  }
  std::cout << message << std::endl;
}

} // namespace ActiveRecord
