#include "SqlConn.h"
#include <iostream>
#include <libpq-fe.h>
#include <sstream>
#include <unistd.h>

using namespace tet;


SqlConn::SqlConn(): connectionStatus(connNOTINITIALIZED), tablename("")
{
}

SqlConn::SqlConn(std::vector<ConfEntry>& config)
{
    tablename = "";
    std::stringstream connInfo;
    for (auto i : config)
    {
        if (i.name.compare("table") == 0) tablename = i.value;
        else if (i.value.length() >= 1)
        {
            connInfo << i.name << "='" << i.value << "' ";
        }
    }
    if (connInfo.str().length() < 1) conn = PQconnectdb("");
    std::cout << "Connecting with parameters ";
    std::cout<< connInfo.str().substr(0, connInfo.str().length()-1).c_str()<<"!"<<std::endl;
    //Eww
    conn = PQconnectdb(connInfo.str().substr(0, connInfo.str().length()-1).c_str());

    if (PQstatus(conn) != CONNECTION_OK)
    {
        std::cout << "Connection failed" << std::endl;
        exit(conn);
        connectionStatus = connFAIL;
    } else connectionStatus = connSUCCESS;
}


SqlConn::~SqlConn()
{
    if (connectionStatus == connSUCCESS) exit(conn);
}

void SqlConn::exit(PGconn *conn)
{
    PQfinish(conn);
    connectionStatus = connDISCONNECTED;
}

void SqlConn::topList(std::string names[10], int scores[10])
{
    if (PQstatus(conn) != CONNECTION_OK)
    {
        connectionStatus = connFAIL;
        return;
    }
    std::stringstream query;
    query << "SELECT name, score FROM "<< tablename;
    query << " ORDER BY score DESC LIMIT 10";
    result = PQexec(conn, query.str().c_str());
    if (PQresultStatus(result) != PGRES_TUPLES_OK)
    {
        std::cout << "Receiving data failed" << std::endl;
        return;
    }
    if (PQnfields(result) != 2) throw "Invalid columns";
    for (int i = 0; i < std::min(10,PQntuples(result)); i++)
    {
       names[i] = PQgetvalue(result, i, PQfnumber(result, "name"));
       char* scoreChar = PQgetvalue(result, i, PQfnumber(result, "score"));
       int res = 0;
       int offset= -1;
       while (scoreChar[++offset] != 0)
       {
            std::cout << scoreChar[offset] << "," << std::endl;
            res += (offset+1)*10*(scoreChar[offset]-'0');
       }
       scores[i] = res;
    }
    PQclear(result);
}

