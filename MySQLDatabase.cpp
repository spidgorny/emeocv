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
        std::cout << "MySQL Initialization Failed";
    }
    _connect = mysql_real_connect(_connect, config.getMysqlHost().c_str(), config.getMysqlUser().c_str(),
            config.getMysqlPassword().c_str(), config.getMysqlDatabase().c_str(), 0, NULL, 0);
    if (_connect) {
        std::cout << "Connection Succeeded\n";
    } else {
        std::cout << "Connection Failed\n";
    }
}

MySQLDatabase::~MySQLDatabase() {
    mysql_close(_connect);
    delete _connect;
}

int MySQLDatabase::insert(const char* table, time_t time, double counter) {
    char ts[sizeof "2014-07-19 22:20:00"];
    strftime(ts, sizeof ts, "%F %T", localtime(&time));

    std::string query;
    query = sprintf("INSERT INTO `%s` (timestamp, conskwh, consws) VALUES (%s,%.1f,%.0f);",
            table, ts, counter/*kWh*/, counter * 3600000 /*Ws*/);

    int res = mysql_query(_connect, query.c_str());
    if (res) {
        log4cpp::Category::getRoot() << log4cpp::Priority::ERROR << mysql_error(_connect);
    }

    return res;
}
