# Dependencies

See INSTALL.md

# Configuration

See MAINTENANCE.md

# Tests

The tests are run against SQLite and PostgreSQL.
In order to run the PostgreSQL tests you must supply the name of a PostgreSQL
user that can access the database without a password.
(You can also supply PG_PORT if your PostgreSQL is not running on port 5432)

```shell
$ PG_USER=postgres rake test
```
