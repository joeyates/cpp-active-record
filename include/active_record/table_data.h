#ifndef ACTIVE_RECORD_TABLE_DATA_H
#define ACTIVE_RECORD_TABLE_DATA_H

#include <map>
#include <string>
using namespace std;

namespace ActiveRecord {

class Connection;

struct TableData {
  Connection *        connection;
  string              primary_key;
  string              table_name;
  map<string, string> attributes;
};

} // namespace ActiveRecord

#endif // ndef ACTIVE_RECORD_TABLE_DATA_H
