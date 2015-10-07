# Project maintenance

# Dependencies

Ubuntu:

```
# apt-get install automake
```

Install a recent (>= 2.0) version of Ruby and the `bundler` gem.

Install the `rake-builder` Ruby gem:

```
bundle install
```

# Rebuilding autoconf files

Rebuild configure.ac and Makefile.am:

```shell
$ rm -f configure configure.ac Makefile.am
$ rake "autoconf[cpp-active-record]"
```

Create configure:

```
$ autoconf
```

