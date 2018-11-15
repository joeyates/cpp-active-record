# Tests

# Dependencies

First, follow INSTALL.md

# Project maintenance

As with any Makefile project there is the usual eternal regression of
build files and templates:

```
Make
  <- configure
  <- configure.ac, Makefile.am
  <- aclocal.m4, Makefile.in
```

If changes need to be made to the build system, edit `Makefile.in` and
regenerate `configure` and `Makefile`.

# Updating automake requirements

If a file, such as `missing` needs updating, remove it and run

```shell
$ automake --add-missing
```

A symlink will be created to the correct file. Simply replace the symlink
with a copy of the file.

# Dependencies

Ubuntu:

```
# apt-get install automake
```

This project uses the `rake-builder` ruby gem to manage part of this process.

Install a recent (>= 2.0) version of Ruby and the `bundler` gem.

Install the `rake-builder` Ruby gem:

```
bundle install
```

# Tests

The tests are run against SQLite and PostgreSQL.
In order to run the PostgreSQL tests you must supply the name of a PostgreSQL
user that can access the database without a password.
(You can also supply PG_PORT if your PostgreSQL is not running on port 5432)

```shell
$ PG_USER=postgres rake test
```

# Rebuilding autoconf files

This project uses Rake::Builder to generate autoconf files.


```shell
$ rake configure:rebuild
```
