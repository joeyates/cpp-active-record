# Installation

# Download the source

```
$ git clone git://github.com/joeyates/cpp-active-record.git
```

# Install Dependencies

* [Boost](https://www.boost.org/)
* [GoogleTest](https://github.com/google/googletest)
* [makedepend](https://en.wikipedia.org/wiki/Makedepend) - used by build system
* SQLite3

## Ubuntu

`xutils-dev` provides `makedepend`.

```shell
$ sudo apt-get install \
  automake \
  cmake \
  libboost-all-dev \
  libgtest-dev \
  libsqlite3-dev \
  xutils-dev
```

Compile gtest:

```shell
# cd /usr/src/gtest && cmake . && make && mv libg* /usr/local/lib/
```

# Install the library

```shell
$ ./configure
$ make
$ make install
```
