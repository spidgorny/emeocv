/*
 * MySQLDatabase.cpp
 *
 */

#include <mysql.h>
#include <iostream>

#include <log4cpp/Category.hh>
#include <log4cpp/Priority.hh>

#include "MySQLDatabase.h"

MySQLDatabase::MySQLDatabase(const Config & config) {
    _connect = mysql_init(NULL);
    if (!connect) {
        cout << "MySQL Initialization Failed";
    }
    _connect = mysql_real_connect(_connect, config.getMysqlHost(), config.getMysqlUser(),
            config.getMysqlPassword(), config.getMysqlDatabase(), 0, NULL, 0);
    if (_connect) {
        cout << "Connection Succeeded\n";
    } else {
        cout << "Connection Failed\n";
    }
}

MySQLDatabase::~MySQLDatabase() {
    mysql_close (_connect);
    delete _connect;
}

int MySQLDatabase::insert(const char* table, time_t time, double counter) {
    char values[256];
    snprintf(values, 255, "%ld,%.1f,%.0f", (long) time, counter/*kWh*/, counter * 3600000. /*Ws*/);

    rrd_clear_error();
    int res = mysql_query(_connect, "INSERT INTO `" + table + "` (timestamp, conskwh, consws) VALUES (" + values + ");");
    if (res) {
        log4cpp::Category::getRoot() << log4cpp::Priority::ERROR << mysql_error(_connect);
    }

    return res;
}
