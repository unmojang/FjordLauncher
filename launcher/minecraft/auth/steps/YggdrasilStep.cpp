#include "YggdrasilStep.h"
#include <QInputDialog>
#include "Application.h"

#include "net/RawHeaderProxy.h"

YggdrasilStep::YggdrasilStep(AccountData* data, std::optional<QString> password) : AuthStep(data), m_password(password) {}

QString YggdrasilStep::describe()
{
    return tr("Logging in with Yggdrasil.");
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
    m_task->setAutoRetryLimit(0);
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
     *  "clientToken": "client identifier",
     *  "accessToken": "current access token to be refreshed",
     *  "selectedProfile": {
     *      "id": "profile ID",
     *      "name": "profile name"
     *  },
     *  "requestUser": true/false               // request the user structure
     * }
     */
    QJsonObject selectedProfile;
    selectedProfile.insert("id", m_data->profileId());
    selectedProfile.insert("name", m_data->profileName());

    QJsonObject req;
    req.insert("clientToken", m_data->clientToken());
    req.insert("accessToken", m_data->accessToken());
    req.insert("selectedProfile", selectedProfile);
    req.insert("requestUser", false);

    QJsonDocument doc(req);
    QByteArray requestData = doc.toJson();

    m_response.reset(new QByteArray());
    m_request = Net::Upload::makeByteArray(url, m_response, requestData);
    m_request->addHeaderProxy(new Net::RawHeaderProxy(headers));

    m_task.reset(new NetJob("YggdrasilStep", APPLICATION->network()));
    m_task->setAskRetry(false);
    m_task->setAutoRetryLimit(0);
    m_task->addNetAction(m_request);

    connect(m_task.get(), &Task::finished, this, &YggdrasilStep::onRequestDone);

    m_task->start();
}

void YggdrasilStep::onRequestDone()
{
    qDebug() << "Yggdrasil request done";
    switch (m_request->error()) {
        case QNetworkReply::NoError:
            break;
        case QNetworkReply::AuthenticationRequiredError:
            // These cases will be handled as usual
            break;
        case QNetworkReply::TimeoutError:
            emit finished(AccountTaskState::STATE_FAILED_SOFT, tr("Authentication operation timed out."));
            return;
        case QNetworkReply::OperationCanceledError:
            emit finished(AccountTaskState::STATE_FAILED_SOFT, tr("Authentication operation cancelled."));
            return;
        case QNetworkReply::SslHandshakeFailedError:
            emit finished(AccountTaskState::STATE_FAILED_SOFT,
                          tr("<b>SSL Handshake failed.</b><br/>There might be a few causes for it:<br/>"
                             "<ul>"
                             "<li>You use Windows and need to update your root certificates, please install any outstanding updates.</li>"
                             "<li>Some device on your network is interfering with SSL traffic. In that case, "
                             "you have bigger worries than Minecraft not starting.</li>"
                             "<li>Possibly something else. Check the log file for details</li>"
                             "</ul>"));
            return;
        // used for invalid credentials and similar errors. Fall through.
        case QNetworkReply::ContentAccessDenied:
        case QNetworkReply::ContentOperationNotPermittedError:
            break;
        case QNetworkReply::ContentGoneError: {
            emit finished(AccountTaskState::STATE_FAILED_GONE,
                          tr("The Mojang account no longer exists. It may have been migrated to a Microsoft account."));
            return;
        }
        default:
            emit finished(AccountTaskState::STATE_FAILED_SOFT, tr("Authentication operation failed due to a network error: %1 (%2)")
                                                                   .arg(m_request->errorString())
                                                                   .arg(m_request->error()));
            return;
    }

    // Try to parse the response regardless of the response code.
    // Sometimes the auth server will give more information and an error code.
    // Check the response code.
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(*m_response, &jsonError);
    // Check the response code.
    int responseCode = m_request->replyStatusCode();

    if (responseCode == 200) {
        // If the response code was 200, then there shouldn't be an error. Make sure
        // anyways.
        // Also, sometimes an empty reply indicates success. If there was no data received,
        // pass an empty json object to the processResponse function.
        if (jsonError.error == QJsonParseError::NoError || m_response->size() == 0) {
            processResponse(m_response->size() > 0 ? doc.object() : QJsonObject());
            return;
        } else {
            emit finished(AccountTaskState::STATE_FAILED_SOFT,
                          tr("Failed to parse authentication server response JSON response: %1 at offset %2.")
                              .arg(jsonError.errorString())
                              .arg(jsonError.offset));
            qCritical() << *m_response;
        }
        return;
    }

    // If the response code was not 200, then Yggdrasil may have given us information
    // about the error.
    // If we can parse the response, then get information from it. Otherwise just say
    // there was an unknown error.
    if (jsonError.error == QJsonParseError::NoError) {
        // We were able to parse the server's response. Woo!
        // Call processError. If a subclass has overridden it then they'll handle their
        // stuff there.
        qDebug() << "The request failed, but the server gave us an error message. Processing error.";
        processError(doc.object());
    } else {
        // The server didn't say anything regarding the error. Give the user an unknown
        // error.
        qDebug() << "The request failed and the server gave no error message. Unknown error.";
        emit finished(
            AccountTaskState::STATE_FAILED_SOFT,
            tr("An unknown error occurred when trying to communicate with the authentication server: %1").arg(m_request->errorString()));
    }

    YggdrasilStep::processResponse(doc.object());
}

void YggdrasilStep::processResponse(QJsonObject responseData)
{
    // Read the response data. We need to get the client token, access token, and the selected
    // profile.

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
    if (profile.isObject()) {
        m_didSelectProfile = false;
    } else {
        if (m_didSelectProfile) {
            emit finished(AccountTaskState::STATE_FAILED_HARD, tr("Authentication server didn't save our selected profile."));
        }
        auto profiles = responseData.value("availableProfiles");
        if (!profiles.isArray()) {
            emit finished(AccountTaskState::STATE_FAILED_HARD, tr("Authentication server didn't send available profiles."));
            return;
        } else {
            const auto& profilesArray = profiles.toArray();
            if (profilesArray.isEmpty()) {
                emit finished(AccountTaskState::STATE_FAILED_HARD, tr("Account has no available profile."));
                return;
            } else if (profilesArray.size() == 1) {
                profile = profiles.toArray().first();
            } else {
                std::map<QString, QJsonValue> profileMap;
                QStringList profileNames;

                const auto& invalidProfileMessage = tr("Authentication server sent an invalid available profile.");
                for (const auto& profileValue : profilesArray) {
                    if (!profileValue.isObject()) {
                        emit finished(AccountTaskState::STATE_FAILED_HARD, invalidProfileMessage);
                    }
                    const auto& profileNameValue = profileValue.toObject().value("name");
                    if (!profileNameValue.isString()) {
                        emit finished(AccountTaskState::STATE_FAILED_HARD, invalidProfileMessage);
                    }
                    const auto& profileName = profileNameValue.toString();
                    profileMap.insert({ profileName, profileValue });
                    profileNames.append(profileName);
                }
                bool ok;
                const auto& profileName =
                    QInputDialog::getItem(nullptr, "Select a player", "Select a player:", profileNames, 0, false, &ok);

                if (!ok || profileName.isEmpty()) {
                    emit finished(AccountTaskState::STATE_FAILED_SOFT, tr("Authentication cancelled."));
                }
                profile = profileMap[profileName];
                m_didSelectProfile = true;
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

    if (m_didSelectProfile) {
        // The authlib-injector specification requires that we refresh immediately after the user has selected a profile:
        // https://github.com/yushijinhun/authlib-injector/wiki/%E5%90%AF%E5%8A%A8%E5%99%A8%E6%8A%80%E6%9C%AF%E8%A7%84%E8%8C%83#%E8%B4%A6%E6%88%B7%E7%9A%84%E6%B7%BB%E5%8A%A0
        return refresh();
    }

    emit finished(AccountTaskState::STATE_WORKING, "Logged in");
}

void YggdrasilStep::processError(QJsonObject responseData)
{
    QJsonValue errorVal = responseData.value("error");
    QJsonValue errorMessageValue = responseData.value("errorMessage");
    QJsonValue causeVal = responseData.value("cause");

    if (errorVal.isString() && errorMessageValue.isString()) {
        emit finished(AccountTaskState::STATE_FAILED_HARD, errorMessageValue.toString(""));
    } else {
        // Error is not in standard format. Don't set m_error and return unknown error.
        emit finished(AccountTaskState::STATE_FAILED_HARD, tr("An unknown Yggdrasil error occurred."));
    }
}
