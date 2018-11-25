#include <active_record/exception.h>

#include <c.h>
#include <sstream>

namespace ActiveRecord {

int64 string_to_int64(const char* raw, bool& found) {
  errno = 0;
  int64 result = strtol(raw, nullptr, 10);

  if(errno != 0) {
    return Attribute();
  }

  found = true;
  return result;
}

double string_to_double(const char* raw, bool& found) {
  char* end;
  double d = std::strtod(raw, &end);

  if(end == raw) {
    found = false;
    return 0;
  }

  found = true;
  return d;
}

} // namespace ActiveRecord
