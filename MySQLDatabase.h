/*
 * MySQLDatabase.h
 *
 */

#ifndef MySQLDatabase_H_
#define MySQLDatabase_H_

#include <mysql/mysql.h>
#include "MySQLDatabase.h"
#include "Config.h"

class MySQLDatabase {
public:
    MySQLDatabase(const Config & config);
    virtual ~MySQLDatabase();
    int insert(const char* table, time_t time, double value);

private:
    MYSQL* _connect;
};

#endif /* MySQLDatabase_H_ */
