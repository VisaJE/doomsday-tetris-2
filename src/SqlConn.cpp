#include "SqlConn.h"
#include <iostream>
#include <libpq-fe.h>
#include <sstream>
#include <unistd.h>
#include "UniqueIdentifier.h"
#include "Log.h"

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
    //std::cout << "Connecting with parameters ";
    //std::cout<< connInfo.str().substr(0, connInfo.str().length()-1).c_str()<<"!"<<std::endl;
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

void trimTrailingWhitespace(std::string *s)
{
   size_t len = s->length();
   while (s->at(len-1) == ' ')
   {
       --len;
   }
   *s = s->substr(0, len);
}

void SqlConn::topList(std::string names[10], int scores[10])
{
    if (!checkConnection()) return;
    std::stringstream query;
    query << "SELECT name, score FROM "<< tablename;
    query << " ORDER BY score DESC LIMIT 10;";
    result = PQexec(conn, query.str().c_str());
    if (PQresultStatus(result) != PGRES_TUPLES_OK)
    {
        PQclear(result);
        std::cout << "Receiving data failed" << std::endl;
        return;
    }
    for (int i = 0; i <PQntuples(result); i++)
    {
       names[i] = PQgetvalue(result, i, PQfnumber(result, "name"));

       trimTrailingWhitespace(&names[i]);
       if (names[i].size() > 10) names[i] = names[i].substr(0, 10);

       if (PQfformat(result, PQfnumber(result, "score")) != 0)
       {
           std::cout << "Binary formatting is not supported for now" << std::endl;
           break;;
       }
       char* scoreChar = PQgetvalue(result, i, PQfnumber(result, "score"));
       scores[i] = atoi(scoreChar);
    }
    PQclear(result);
}

bool SqlConn::getLowest(int& score)
{
    if (!checkConnection()) return false;
    std::stringstream query;
    query << "SELECT score FROM " << tablename;
    query << " ORDER BY score DESC LIMIT 1;";
    result = PQexec(conn, query.str().c_str());
    //std::cout << query.str() << std::endl;
    if (PQresultStatus(result) != PGRES_TUPLES_OK)
    {
        PQclear(result);
        std::cout << "Receiving lowest data failed" << std::endl;
        return false;
    }
    if (PQntuples(result) < 1)
    {
        score = 0;
        PQclear(result);
        return true;
    }
    auto scoreChar = PQgetvalue(result, 0,
           PQfnumber(result, "score"));
    score = atoi(scoreChar);
    PQclear(result);
    return true;
}

bool SqlConn::getIdList(Uid ids[10], int scores[10])
{
    if (!checkConnection()) return false;
    std::stringstream query;
    query << "SELECT uid, score FROM "<< tablename;
    query << " ORDER BY score DESC LIMIT 10;";
    result = PQexec(conn, query.str().c_str());
    if (PQresultStatus(result) != PGRES_TUPLES_OK)
    {
        PQclear(result);
        std::cout << "Receiving id data failed" << std::endl;
        return false;
    }
    for (int i = 0; i <PQntuples(result); i++)
    {
       ids[i] = PQgetvalue(result, i, PQfnumber(result, "uid"));

       if (PQfformat(result, PQfnumber(result, "score")) != 0)
       {
           std::cout << "Binary formatting is not supported for now" << std::endl;
           break;;
       }
       char* scoreChar = PQgetvalue(result, i, PQfnumber(result, "score"));
       scores[i] = atoi(scoreChar);
    }
    PQclear(result);
    return true;
}
bool SqlConn::checkConnection()
{

    if (PQstatus(conn) != CONNECTION_OK)
    {
        connectionStatus = connFAIL;
        return false;
    }
    return true;
}

/*
 * Pushes names and scores if they will appear on the top 10.
 */
void SqlConn::pushList(std::string names[10], int scores[10], Uid ids[10])
{
    int onlScores[10];
    Uid onlIds[10];
    for (int i = 0; i < 10; i++) onlScores[i] = 0;
    getIdList(onlIds, onlScores);
    LOG("Found highscore from server\n%s %i\n \
             %s %i\n  %s %i\n  %s %i\n  %s %i\n  %s %i\n\
             %s %i\n  %s %i\n  %s %i\n  %s %i\n",
            onlIds[0].c_str(), onlScores[0],
            onlIds[1].c_str(), onlScores[1],
            onlIds[2].c_str(), onlScores[2],
            onlIds[3].c_str(), onlScores[3],
            onlIds[4].c_str(), onlScores[4],
            onlIds[5].c_str(), onlScores[5],
            onlIds[6].c_str(), onlScores[6],
            onlIds[7].c_str(), onlScores[7],
            onlIds[8].c_str(), onlScores[8],
            onlIds[9].c_str(), onlScores[9]
            );
    // for (int i = 0; i < 10; i++) std::cout << onlIds[i] << ", " << onlScores[i] << std::endl;
    std::string basicS = "INSERT INTO " + tablename +
        " (name, score, uid) VALUES (";
    if (!checkConnection()) return;
    int idCollisions = 0;
    int inserted = 0;
    int LastLocal = 0;
    while(LastLocal < 10 && scores[LastLocal] >0) LastLocal++;
    LOG("Last local index %d", LastLocal);
    for (int local = 0; local < LastLocal; local++)
    {
        for (int online = local - idCollisions-inserted; online < 10-inserted; online++)
        {
        if (UniqueIdentifier::compare(ids[local], onlIds[online]))
        {
            idCollisions++;
            online = 10;
        }
        else if (scores[local] > onlScores[online])
        {
            ++inserted;
            char* sanitized = PQescapeLiteral(conn, names[local].c_str(), 10);

            char* sanId = PQescapeLiteral(conn, ids[local].c_str(), 10);

            std::stringstream query;
            query << basicS << (const char*)sanitized << ", " << scores[local] << ", ";
            query << (const char*) sanId << ");";
            PGresult *res = PQexec(conn, query.str().c_str());
            if (PQresultStatus(res) != PGRES_COMMAND_OK)
            {
                std::cout << "Insertion failed" << std::endl;
                std::cout << PQresStatus(PQresultStatus(res)) << std::endl;
            }
            PQfreemem(sanitized);
            PQfreemem(sanId);
            PQclear(res);
            online = 10;
        }
        }
    }
}

