#include <active_record/row.h>

#include <sstream>

#include <active_record/active_record.h>
#include <active_record/attribute.h>
#include <active_record/exception.h>
#include <active_record/type.h>

namespace ActiveRecord {

Type::Type Row::get_type(const std::string &name) {
  return attributes_[name].type();
}

bool Row::is_type(const std::string& name, Type::Type type) {
  Type::Type actual = attributes_[name].type();
  return actual == type;
}

// TYPE_LIST
int64 Row::get_integer(const std::string& name) {
  return boost::get<int64>(attributes_[name]);
}

std::string Row::get_text(const std::string& name) {
  return boost::get<std::string>(attributes_[name]);
}

double Row::get_floating_point(const std::string& name) {
  return boost::get<double>(attributes_[name]);
}

Date Row::get_date(const std::string& name) {
  return boost::get<Date>(attributes_[name]);
}

std::string Row::to_string() const {
  std::stringstream row;
  row << "{" << std::endl;

  for(auto& attribute: attributes_) {
    row << attribute.first << ": ";
    row << attribute.second << std::endl;
  }

  row << "}" << std::endl;

  return row.str();
}

std::ostream& operator<<(std::ostream& cout, const ActiveRecord::Row& row) {
  cout << row.to_string();

  return cout;
}

} // namespace ActiveRecord
