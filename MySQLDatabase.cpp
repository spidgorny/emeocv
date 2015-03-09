/*
 * MySQLDatabase.cpp
 *
 */

#include <mysql/mysql.h>
#include <iostream>

#include <log4cpp/Category.hh>
#include <log4cpp/Priority.hh>

#include "MySQLDatabase.h"

MySQLDatabase::MySQLDatabase(const Config & config) {
    _connect = mysql_init(NULL);
    if (!_connect) {
        log4cpp::Category::getRoot() << log4cpp::Priority::ERROR << "MySQL Initialization Failed";
    }
    _connect = mysql_real_connect(_connect, config.getMysqlHost().c_str(), config.getMysqlUser().c_str(),
            config.getMysqlPassword().c_str(), config.getMysqlDatabase().c_str(), 0, NULL, 0);
    if (_connect) {
        log4cpp::Category::getRoot() << log4cpp::Priority::INFO << "Connection Succeeded\n";
    } else {
        log4cpp::Category::getRoot() << log4cpp::Priority::ERROR << "Connection Failed\n";
    }
    my_bool reconnect = 1;
    mysql_options(_connect, MYSQL_OPT_RECONNECT, &reconnect);
    mysql_autocommit(_connect, true);
}

MySQLDatabase::~MySQLDatabase() {
    mysql_close(_connect);
    mysql_library_end();
    delete _connect;
}

int MySQLDatabase::insert(const char* table, time_t time, double counter) {
    char ts[20];
    strftime(ts, sizeof ts, "%F %T", localtime(&time));

    char query[255];
    sprintf(query, "INSERT INTO `%s` (timestamp, conskwh) VALUES ('%s', %.1f);",
            table, ts, counter/*kWh*/);
    log4cpp::Category::getRoot() << log4cpp::Priority::DEBUG << "MySQL-Query: " << query << "\n";

    int res = mysql_query(_connect, query);
    if (res) {
        log4cpp::Category::getRoot() << log4cpp::Priority::ERROR << mysql_error(_connect);
    }
    mysql_close(_connect);

    return res;
}
