#ifndef DB_H
#define DB_H

#include <iostream>
#include <string>
#include "sqlite_modern_cpp.h"

class DB {
public:
    // 생성자: 초기화 리스트에서 db_("nano.db")로 직접 초기화
    DB() : db_("mini.db")
    {
        try {
            db_ <<
                "CREATE TABLE IF NOT EXISTS logs ("
                "   id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "   method TEXT NOT NULL,"
                "   path TEXT NOT NULL,"
                "   status INTEGER NOT NULL,"
                "   ts    TIMESTAMP DEFAULT (datetime('now','localtime'))"
                ");";
        } catch (std::exception& e) {
            std::cerr << "[DB] Initialization error: " << e.what() << "\n";
            throw;
        }
    }

    // 요청이 들어올 때마다 호출: method, path, status 기록
    void insertLog(const std::string& method,
                   const std::string& path,
                   int status) {
        try {
            db_ << "INSERT INTO logs(method, path, status) VALUES(?, ?, ?);"
                << method
                << path
                << status;
        } catch (std::exception& e) {
            std::cerr << "[DB] insertLog error: " << e.what() << "\n";
        }
    }

private:
    sqlite::database db_;
};

#endif // DB_H
