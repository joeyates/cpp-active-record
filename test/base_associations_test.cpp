#include "test_helper.h"
#include <active_record/connection/sqlite3.h>

extern string database_name;

using namespace ActiveRecord;

class Library: public ActiveRecord::Base<Library> {
  public:

  AR_CONSTRUCTORS(Library)
  static Table table(Connection * connection) {
    Table td(connection, "libraries");
    td.fields() = fields ("name", Type::text);
    return td;
  }
};

class Book: public ActiveRecord::Base<Book> {
  public:

  AR_CONSTRUCTORS( Book)
  static Table table(Connection * connection) {
    Table td(connection, "books");
    td.fields() =
      fields
        ("library_id", Type::integer)
        ("title", Type::text);
    return td;
  }
};

AR_DECLARE(Library)
AR_DECLARE(Book)

class AssociationTest: public ::testing::Test {
  protected:

  virtual void SetUp() {
    sqlite_delete_database();
    sqlite_connect_database(connection, database_name);
    Library::setup(&connection);
    Book::setup(&connection);
    connection.update_database();

    british_library.init(
      attribute_pairs ("name", "The British Library")
    ).save();

    lindisfarne.init(
      attribute_pairs
        ("library_id", british_library.id())
        ("title", "The Lindisfarne Gospels")
    ).save();

    codex_arundel.init(
      attribute_pairs
        ("library_id", british_library.id())
        ("title", "The Codex Arundel")
    ).save();

    nazionale.init(attribute_pairs ("name", "La biblioteca nazionale")).save();

    galileiana.init(
      attribute_pairs
        ("library_id", nazionale.id())
        ("title", "Collezione galileiana")
    ).save();
  }

  virtual void TearDown() {
    sqlite_delete_database();
  }

  protected:

  Sqlite3Connection connection;
  Library british_library;
  Library nazionale;
  Book    lindisfarne;
  Book    codex_arundel;
  Book    galileiana;
};

TEST_F(AssociationTest, HasMany) {
  vector<Book> books = british_library.has_many<Book>();

  ASSERT_EQ(2, books.size());
}

TEST_F(AssociationTest, HasManyIncorrectAssociation) {
  ASSERT_THROW(
    vector<Library> books = lindisfarne.has_many<Library>(),
    ActiveRecordException
  );
}

TEST_F(AssociationTest, BelongsTo) {
  Library owner = lindisfarne.belongs_to<Library>();

  ASSERT_EQ(british_library.id(), owner.id());
}
