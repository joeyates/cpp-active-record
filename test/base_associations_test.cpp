#include "test_helper.h"

extern string database_file;

namespace ActiveRecord {
extern Connection connection;
}

class Library: public ActiveRecord::Base< Library > {
 public:
  AR_CONSTRUCTORS( Library )
  HAS_MANY( books )
  static Table table( Connection * connection ) {
    Table td( connection, "libraries" );
    td.fields().push_back( Field( "name", ActiveRecord::text ) );
    td.has_many( "books" );
    return td;
  }
};

template <>
string ActiveRecord::Base< Library >::class_name = "Library";

class Book: public ActiveRecord::Base< Book > {
 public:
  AR_CONSTRUCTORS( Book )
  static Table table( Connection * connection ) {
    Table td( connection, "books" );
    td.fields().push_back( Field( "title", ActiveRecord::text ) );
    td.fields().push_back( Field( "library_id", ActiveRecord::integer ) );
    return td;
  }
};

template <>
string ActiveRecord::Base<Book>::class_name = "Book";

class HasManyAssociationTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    delete_database();
    connect_database( connection, database_file );
    Library::setup( &connection );
    Book::setup( &connection );
    connection.update_database();
  }
  virtual void TearDown() {
    delete_database();
  }
};

namespace ActiveRecord {
template< class Library > template< class Book >
vector< Book > ActiveRecord::Base< Library >::many();
}

TEST_F( HasManyAssociationTest, Save ) {
  Library british( attributes
                   ( "name", "The British Library" ) );
  british.save();

  Book lindisfarne( attributes
                    ( "title", "The Lindisfarne Gospels" )
                    ( "library_id", british.id() ) );
  lindisfarne.save();

  Book codex_arundel( attributes
                      ( "title", "The Codex Arudel" )
                      ( "library_id", british.id() ) );
  codex_arundel.save();

  Library nazionale( attributes
                     ( "name", "La biblioteca nazionale" ) );
  nazionale.save();

  Book galileiana( attributes
                   ( "title", "Collezione galileiana" )
                   ( "library_id", nazionale.id() ) );
  galileiana.save();

  vector< Book > books = british.many< Book >();

  ASSERT_EQ( 2, books.size() );
}
