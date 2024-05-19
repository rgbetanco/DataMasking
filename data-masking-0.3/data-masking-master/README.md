# **Data Masking**

## Features
* Mask specific field in raw data

## Developing Environment
* OS: Ubuntu 18.04 LTS
* Yacc: bison (GNU Bison) 3.0.4
* Lex: flex 2.6.4

## Prerequisites
### 1. Install RE2 --> Reference: [Install RE2](https://github.com/google/re2/wiki/Install)
```
git clone https://code.googlesource.com/re2
cd re2
make
make test
sudo make install
sudo make testinstall
```
### 2. Install Sodium --> Reference: [Install Sodium](https://download.libsodium.org/doc/installation)
* Leverage Sodium to gain a secure random generator.
* [Download libsodium-stable 1.0.18](https://download.libsodium.org/libsodium/releases/libsodium-1.0.18-stable.tar.gz)
```
cd libsodium-stable
./configure
make && make check
sudo make install
```
### 3. Connect to MySQL
* Install MySQL connector [README_MySQL.md](db_connect/mysql/README_MySQL.md)
* Edit MySQL authentication information in `db_connect/mysql/auth_info.h`
* Prepare a test database [README.md](db_connect/mysql/tools/README.md)
  
## Execution of Testing
```
make
./main.out
```

## Clean Source Files
```
make clean
```

## How to include Data Masking in PHP
```
#include "datamasking.hpp"
```

## Trouble Shooting
When "error while loading shared libraries: libre2.so.0: cannot open shared object file: No such file or directory" occurs.

```
sudo ldconfig
```