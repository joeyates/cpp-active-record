#include "test_helper.h"

extern string database_file;

namespace ActiveRecord {
extern Connection connection;
}

class Library: public ActiveRecord::Base< Library > {
 public:
  AR_CONSTRUCTORS( Library )
  static Table table( Connection * connection ) {
    Table td( connection, "libraries" );
    td.fields().push_back( Field( "name", ActiveRecord::text ) );
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

namespace ActiveRecord {
template< class Library > template< class Book >
vector< Book > ActiveRecord::Base< Library >::has_many();
}

class HasManyAssociationTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    delete_database();
    connect_database( connection, database_file );
    Library::setup( &connection );
    Book::setup( &connection );
    connection.update_database();

    british_library.init( attributes
                          ( "name", "The British Library" ) );
    british_library.save();

    lindisfarne.init( attributes
                      ( "title", "The Lindisfarne Gospels" )
                      ( "library_id", british_library.id() ) );
    lindisfarne.save();

    codex_arundel.init( attributes
                        ( "title", "The Codex Arudel" )
                        ( "library_id", british_library.id() ) );
    codex_arundel.save();

    nazionale.init( attributes
                    ( "name", "La biblioteca nazionale" ) );
    nazionale.save();

    galileiana.init( attributes
                     ( "title", "Collezione galileiana" )
                     ( "library_id", nazionale.id() ) );
    galileiana.save();
  }
  virtual void TearDown() {
    delete_database();
  }
  Library british_library;
  Library nazionale;
  Book    lindisfarne;
  Book    codex_arundel;
  Book    galileiana;
};

TEST_F( HasManyAssociationTest, Default ) {

  vector< Book > books = british_library.has_many< Book >();

  ASSERT_EQ( 2, books.size() );
}

// Wrong association
namespace ActiveRecord {
template< class Book > template< class Library >
vector< Library > ActiveRecord::Base< Book >::has_many();
}

TEST_F( HasManyAssociationTest, IncorrectAssociation ) {
  ASSERT_THROW( vector< Library > books = lindisfarne.has_many< Library >(), ActiveRecordException );
}
