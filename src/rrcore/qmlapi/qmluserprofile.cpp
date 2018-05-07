#include "qmluserprofile.h"
#include <QDebug>

#include "database/databasethread.h"
#include "database/databaseexception.h"
#include "singletons/userprofile.h"

QMLUserProfile::QMLUserProfile(QObject *parent) :
    QObject(parent),
    m_busy(false)
{
    connect(this, &QMLUserProfile::executeRequest, &DatabaseThread::instance(), &DatabaseThread::execute);
    connect(&DatabaseThread::instance(), &DatabaseThread::resultsReady, this, &QMLUserProfile::processResult);
}

bool QMLUserProfile::isBusy() const
{
    return m_busy;
}

void QMLUserProfile::setBusy(bool busy)
{
    if (m_busy == busy)
        return;

    m_busy = busy;
    emit busyChanged();
}

void QMLUserProfile::signIn(const QString &userName, const QString &password)
{
    qDebug() << Q_FUNC_INFO << userName << password;
    if (userName.trimmed().isEmpty()) {
        emit error(NoUserNameProvided);
    } else if (password.isEmpty()) {
        emit error(NoPasswordProvided);
    } else {
        setBusy(true);

        QueryRequest request(this);
        request.setCommand("sign_in_user", { { "user_name", userName }, { "password", password } }, QueryRequest::User);
        emit executeRequest(request);
    }
}

void QMLUserProfile::signUp(const QString &userName, const QString &password)
{
    qDebug() << Q_FUNC_INFO << userName << password;
    if (userName.trimmed().isEmpty()) {
        emit error(NoUserNameProvided);
    } else if (password.isEmpty()) {
        emit error(NoPasswordProvided);
    } else {
        setBusy(true);

        QueryRequest request(this);
        request.setCommand("sign_up_user", { { "user_name", userName }, { "password", password } }, QueryRequest::User);
        emit executeRequest(request);
    }
}

void QMLUserProfile::removeUser(const QString &userName)
{
    qDebug() << Q_FUNC_INFO << userName;
    if (userName.trimmed().isEmpty()) {
        emit error(NoUserNameProvided);
    } else {
        setBusy(true);

        QueryRequest request(this);
        request.setCommand("remove_user", { { "user_name", userName } }, QueryRequest::User);
        emit executeRequest(request);
    }
}

void QMLUserProfile::processResult(const QueryResult &result)
{
    if (this != result.request().parent())
        return;

    setBusy(false);

    if (result.isSuccessful()) {
        if (!result.outcome().toMap().isEmpty())
            UserProfile::instance().setUser(result.outcome().toMap().value("id").toInt(),
                                            result.outcome().toMap().value("user_name").toString());

        emit success();
    } else {
        switch (result.errorCode()) {
        case int(DatabaseException::RRErrorCode::SignInFailure):
            emit error(IncorrectCredentials);
            break;
        default:
            emit error(Unknown);
            break;
        }
    }
}
