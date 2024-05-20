# Install MySQL server
## Ubuntu 18.04 LTS
* Reference: [https://dev.mysql.com/doc/mysql-apt-repo-quick-guide/en/#apt-repo-fresh-install](https://dev.mysql.com/doc/mysql-apt-repo-quick-guide/en/#apt-repo-fresh-install)
* Download: [https://dev.mysql.com/downloads/repo/apt/](https://dev.mysql.com/downloads/repo/apt/)
```
sudo dpkg -i mysql-apt-config_0.8.13-1_all.deb
```
In the menu, choose the following options:
```
 │          MySQL Server & Cluster (Currently selected: mysql-8.0)           │ 
 │          MySQL Tools & Connectors (Currently selected: Enabled)           │ 
 │          MySQL Preview Packages (Currently selected: Disabled)            |
```
Then,
```
sudo apt-get update
sudo apt-get install mysql-server
```
In the config menu, there are two important points:
1. Setup and remember your MySQL password.
2. Choose "Use Legacy Authentication Method (Retain MySQL 5.x Compatibility)"

# Install MySQL C++ connector
## Ubuntu 18.04 LTS
* Reference: [https://dev.mysql.com/doc/connector-cpp/8.0/en/](https://dev.mysql.com/doc/connector-cpp/8.0/en/)
```
sudo apt-get install libssl-dev
sudo apt install libmysqlclient-dev
sudo apt-get install libmysqlcppconn-dev
```