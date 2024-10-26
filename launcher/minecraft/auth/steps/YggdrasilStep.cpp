#include "YggdrasilStep.h"
#include "Application.h"

#include "net/RawHeaderProxy.h"

YggdrasilStep::YggdrasilStep(AccountData* data, std::optional<QString> password) : AuthStep(data), m_password(password) {}

QString YggdrasilStep::describe()
{
    return tr("Logging in with Mojang account.");
}

void YggdrasilStep::perform()
{
    if (m_password) {
        login(*m_password);
    } else {
        refresh();
    }
}

void YggdrasilStep::login(QString password)
{
    QUrl url(m_data->authServerUrl() + "/authenticate");
    auto headers = QList<Net::HeaderPair>{ { "Content-Type", "application/json" }, { "Accept", "application/json" } };

    /*
     * {
     *   "agent": {                              // optional
     *   "name": "Minecraft",                    // So far this is the only encountered value
     *   "version": 1                            // This number might be increased
     *                                           // by the vanilla client in the future
     *   },
     *   "username": "mojang account name",      // Can be an email address or player name for
     *                                           // unmigrated accounts
     *   "password": "mojang account password",
     *   "clientToken": "client identifier",     // optional
     *   "requestUser": true/false               // request the user structure
     * }
     */
    QJsonObject req;
    {
        QJsonObject agent;
        agent.insert("name", QString("Minecraft"));
        agent.insert("version", 1);
        req.insert("agent", agent);
    }
    req.insert("username", m_data->userName());
    req.insert("password", password);
    req.insert("requestUser", false);
    //
    // If we already have a client token, give it to the server.
    // Otherwise, let the server give us one.

    m_data->generateClientTokenIfMissing();
    req.insert("clientToken", m_data->clientToken());

    QJsonDocument doc(req);
    QByteArray requestData = doc.toJson();

    m_response.reset(new QByteArray());
    m_request = Net::Upload::makeByteArray(url, m_response, requestData);
    m_request->addHeaderProxy(new Net::RawHeaderProxy(headers));

    m_task.reset(new NetJob("YggdrasilStep", APPLICATION->network()));
    m_task->setAskRetry(false);
    m_task->addNetAction(m_request);

    connect(m_task.get(), &Task::finished, this, &YggdrasilStep::onRequestDone);

    m_task->start();
}

void YggdrasilStep::refresh()
{
    QUrl url(m_data->authServerUrl() + "/refresh");
    auto headers = QList<Net::HeaderPair>{ { "Content-Type", "application/json" }, { "Accept", "application/json" } };

    /*
     * {
     *  "clientToken": "client identifier"
     *  "accessToken": "current access token to be refreshed"
     *  "selectedProfile":                      // specifying this causes errors
     *  {
     *   "id": "profile ID"
     *   "name": "profile name"
     *  }
     *  "requestUser": true/false               // request the user structure
     * }
     */
    QJsonObject req;
    req.insert("clientToken", m_data->clientToken());
    req.insert("accessToken", m_data->accessToken());
    req.insert("requestUser", false);

    QJsonDocument doc(req);
    QByteArray requestData = doc.toJson();

    m_response.reset(new QByteArray());
    m_request = Net::Upload::makeByteArray(url, m_response, requestData);
    m_request->addHeaderProxy(new Net::RawHeaderProxy(headers));

    m_task.reset(new NetJob("YggdrasilStep", APPLICATION->network()));
    m_task->setAskRetry(false);
    m_task->addNetAction(m_request);

    connect(m_task.get(), &Task::finished, this, &YggdrasilStep::onRequestDone);

    m_task->start();
}

void YggdrasilStep::onRequestDone()
{
    // TODO handle errors

    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(*m_response, &jsonError);
    YggdrasilStep::processResponse(doc.object());
}

void YggdrasilStep::processResponse(QJsonObject responseData)
{
    // Read the response data. We need to get the client token, access token, and the selected
    // profile.
    qDebug() << "Processing authentication response.";

    // qDebug() << responseData;
    // If we already have a client token, make sure the one the server gave us matches our
    // existing one.
    QString clientToken = responseData.value("clientToken").toString("");
    if (clientToken.isEmpty()) {
        // Fail if the server gave us an empty client token
        emit finished(AccountTaskState::STATE_FAILED_HARD, tr("Authentication server didn't send a client token."));
        return;
    }
    if (m_data->clientToken().isEmpty()) {
        m_data->setClientToken(clientToken);
    } else if (clientToken != m_data->clientToken()) {
        emit finished(AccountTaskState::STATE_FAILED_HARD,
                      tr("Authentication server attempted to change the client token. This isn't supported."));
        return;
    }

    // Now, we set the access token.
    qDebug() << "Getting access token.";
    QString accessToken = responseData.value("accessToken").toString("");
    if (accessToken.isEmpty()) {
        // Fail if the server didn't give us an access token.
        emit finished(AccountTaskState::STATE_FAILED_HARD, tr("Authentication server didn't send an access token."));
        return;
    }

    // Set the access token.
    m_data->yggdrasilToken.token = accessToken;
    m_data->yggdrasilToken.validity = Validity::Certain;
    m_data->yggdrasilToken.issueInstant = QDateTime::currentDateTimeUtc();

    // Get UUID here since we need it for later
    // FIXME: Here is a simple workaround for now,, which uses the first available profile when selectedProfile is not provided
    auto profile = responseData.value("selectedProfile");
    if (!profile.isObject()) {
        auto profiles = responseData.value("availableProfiles");
        if (!profiles.isArray()) {
            emit finished(AccountTaskState::STATE_FAILED_HARD, tr("Authentication server didn't send available profiles."));
            return;
        } else {
            if (profiles.toArray().isEmpty()) {
                emit finished(AccountTaskState::STATE_FAILED_HARD, tr("Account has no available profile."));
                return;
            } else {
                profile = profiles.toArray().first();
            }
        }
    }

    auto profileObj = profile.toObject();
    for (auto i = profileObj.constBegin(); i != profileObj.constEnd(); ++i) {
        if (i.key() == "name" && i.value().isString()) {
            m_data->minecraftProfile.name = i->toString();
        } else if (i.key() == "id" && i.value().isString()) {
            m_data->minecraftProfile.id = i->toString();
        }
    }

    if (m_data->minecraftProfile.id.isEmpty()) {
        emit finished(AccountTaskState::STATE_FAILED_HARD, tr("Authentication server didn't send a UUID in selected profile."));
        return;
    }

    emit finished(AccountTaskState::STATE_WORKING, "Logged in");
}

void YggdrasilStep::processError(QJsonObject responseData)
{
    /*QJsonValue errorVal = responseData.value("error");*/
    /*QJsonValue errorMessageValue = responseData.value("errorMessage");*/
    /*QJsonValue causeVal = responseData.value("cause");*/
    /**/
    /*if (errorVal.isString() && errorMessageValue.isString()) {*/
    /*    m_error = std::shared_ptr<Error>(new Error{ errorVal.toString(""), errorMessageValue.toString(""), causeVal.toString("") });*/
    /*    changeState(AccountTaskState::STATE_FAILED_HARD, m_error->m_errorMessageVerbose);*/
    /*} else {*/
    /*    // Error is not in standard format. Don't set m_error and return unknown error.*/
    /*    changeState(AccountTaskState::STATE_FAILED_HARD, tr("An unknown Yggdrasil error occurred."));*/
    /*}*/
}
