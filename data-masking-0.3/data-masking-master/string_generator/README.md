# **Mask Data Generator**

## Features
* Generate masked data based on the regexp rules.

## Platform
* Ubuntu 18.04 LTS

## Prerequisites
### Install Sodium --> Reference: [Install Sodium](https://download.libsodium.org/doc/installation)
* Leverage Sodium to gain a secure random generator.
* [Download libsodium-stable 1.0.18](https://download.libsodium.org/libsodium/releases/libsodium-1.0.18-stable.tar.gz)
```
cd libsodium-stable
./configure
make && make check
sudo make install
```

## Execution
```
make
./main.out
```

## Clean Source Files
```
make clean
```