# Dependencies

See INSTALL.md

# Configuration

See MAINTENANCE.md

# Tests

The tests are run against SQLite and PostgreSQL.

## PostgreSQL Environment

The PostgreSQL tests need to be run via an existing user. The defaut is
`postgres`. You can supply an alternative user name via the `PG_USER`
environment variable.

E.g.

```shell
$ PG_USER=postgres rake test
```

The user must be set up to access the database without a password.

You can supply PG_HOST if your PostgreSQL is not running locally.

You can supply PG_PORT if your PostgreSQL is not running on port 5432.
