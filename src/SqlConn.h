#pragma once

#include <iostream>
#include <libpq-fe.h>
#include <vector>
#include "WebConf.h"

typedef enum {
    connNOTINITIALIZED,
    connFAIL,
    connSUCCESS,
    connDISCONNECTED
} connStatus;
namespace tet {
class SqlConn {
    public:
        SqlConn();
        SqlConn(std::vector<ConfEntry> &config);
        virtual ~SqlConn();
        void topList(std::string names[10], int scores[10]);
        void pushList(std::string names[10], int scores[10]);
        connStatus connectionStatus;
    private:
        std::string tablename;
        PGconn *conn;
        PGresult *result;
        void exit(PGconn *conn);
        bool getLowest(int& score);
        bool checkConnection();

};
}
