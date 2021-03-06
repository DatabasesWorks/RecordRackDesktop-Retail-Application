#ifndef INCOMESQLMANAGER_H
#define INCOMESQLMANAGER_H

#include "abstractsqlmanager.h"

class IncomeSqlManager : public AbstractSqlManager
{
public:
    explicit IncomeSqlManager(const QString &connectionName);

    QueryResult execute(const QueryRequest &request) override;
private:
    void addNewIncomeTransaction(const QueryRequest &request); // throws DatabaseException
    void viewIncomeTransactions(const QueryRequest &request, QueryResult &result); // throws DatabaseException
    void viewIncomeReport(const QueryRequest &request, QueryResult &result); // throws DatabaseException
};

#endif // INCOMESQLMANAGER_H
