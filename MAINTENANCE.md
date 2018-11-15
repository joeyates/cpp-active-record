# autoconf, configure and Makefile

This project contains copies of the various make-related build files.

As with any Makefile project there is the usual eternal regression of
build files and templates:

```
Make
  <- configure
  <- configure.ac, Makefile.am
  <- aclocal.m4, Makefile.in
```

# Rebuilding autoconf files

This project uses Rake::Builder to generate autoconf files.

```shell
$ rake configure:rebuild
```

Install the `rake-builder` Ruby gem:

```
$ gem install bundler
$ bundle install
```

# Project maintenance

## automake requirements

If a file, such as `missing` needs updating, remove it and run

```shell
$ automake --add-missing
```

A symlink will be created to the correct file. Simply replace the symlink
with a copy of the file.
