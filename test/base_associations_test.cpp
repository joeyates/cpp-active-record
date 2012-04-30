#include "test_helper.h"

extern string database_file;

class Library: public ActiveRecord::Base< Library > {
 public:
  AR_CONSTRUCTORS( Library )
  static Table table( Connection * connection ) {
    Table td( connection, "libraries" );
    td.fields() = fields ( "name", ActiveRecord::text );
    return td;
  }
};

class Book: public ActiveRecord::Base< Book > {
 public:
  AR_CONSTRUCTORS( Book )
  static Table table( Connection * connection ) {
    Table td( connection, "books" );
    td.fields() = fields ( "title", ActiveRecord::text )
                         ( "date", ActiveRecord::date )
                         ( "library_id", ActiveRecord::integer );
    return td;
  }
};

AR_DECLARE( Library )
AR_DECLARE( Book )
AR_HAS_MANY( Library, Book )
AR_HAS_MANY( Book, Library ) // Incorrect association
AR_BELONGS_TO( Book, Library )

class AssociationTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    delete_database();
    connect_database( connection, database_file );
    Library::setup( &connection );
    Book::setup( &connection );
    connection.update_database();

    british_library.init( attributes
                          ( "name", "The British Library" ) ).save();
    lindisfarne.init( attributes
                      ( "title", "The Lindisfarne Gospels" )
                      ( "library_id", british_library.id() ) ).save();
    codex_arundel.init( attributes
                        ( "title", "The Codex Arundel" )
                        ( "date", "1518-01-01" )
                        ( "library_id", british_library.id() ) ).save();

    nazionale.init( attributes
                    ( "name", "La biblioteca nazionale" ) ).save();

    galileiana.init( attributes
                     ( "title", "Collezione galileiana" )
                     ( "library_id", nazionale.id() ) ).save();
  }
  virtual void TearDown() {
    delete_database();
  }
  Connection connection;
  Library british_library;
  Library nazionale;
  Book    lindisfarne;
  Book    codex_arundel;
  Book    galileiana;
};

TEST_F( AssociationTest, HasMany ) {
  vector< Book > books = british_library.has_many< Book >();

  ASSERT_EQ( 2, books.size() );
}

TEST_F( AssociationTest, HasManyIncorrectAssociation ) {
  ASSERT_THROW( vector< Library > books = lindisfarne.has_many< Library >(), ActiveRecordException );
}

TEST_F( AssociationTest, BelongsTo ) {
  Library owner = lindisfarne.belongs_to< Library >();

  ASSERT_EQ( british_library.id(), owner.id() );
}
