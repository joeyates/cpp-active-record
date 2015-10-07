# Tests

# Dependencies

First, follow INSTALL.md

```shell
$ rake test
```

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

# Dependencies

Ubuntu:

```
# apt-get install automake xutils-dev
```

This project uses the `rake-builder` ruby gem to manage part of this process.

Install a recent (>= 2.0) version of Ruby and the `bundler` gem.

Install the `rake-builder` Ruby gem:

```
bundle install
```

# Rebuilding autoconf files

Rebuild configure.ac and Makefile.am:

```shell
$ rm -f configure configure.ac Makefile.am
$ bundle exec rake "autoconf[cpp-active-record]"
```

Create configure and Makefile:

```
$ autoconf
$ ./configure
```
