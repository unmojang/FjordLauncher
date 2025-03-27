#include "AuthlibInjectorMetadataStep.h"

#include <QJsonDocument>
#include <QNetworkRequest>
#include "Application.h"

AuthlibInjectorMetadataStep::AuthlibInjectorMetadataStep(AccountData* data) : AuthStep(data) {}

AuthlibInjectorMetadataStep::~AuthlibInjectorMetadataStep() noexcept = default;

QString AuthlibInjectorMetadataStep::describe()
{
    return tr("Prefetching authlib-injector metadata.");
}

void AuthlibInjectorMetadataStep::perform()
{
    if (m_data->customAuthlibInjectorUrl == "") {
        emit finished(AccountTaskState::STATE_WORKING, tr("Account has no authlib-injector URL."));
        return;
    }

    QUrl url{m_data->customAuthlibInjectorUrl};

    m_response.reset(new QByteArray());
    m_request = Net::Download::makeByteArray(url, m_response);

    m_task.reset(new NetJob("AuthlibInjectorMetadataStep", APPLICATION->network()));
    m_task->setAskRetry(false);
    m_task->setAutoRetryLimit(0);
    m_task->addNetAction(m_request);

    connect(m_task.get(), &Task::finished, this, &AuthlibInjectorMetadataStep::onRequestDone);

    m_task->start();
}

void AuthlibInjectorMetadataStep::onRequestDone()
{
    if (m_request->error() == QNetworkReply::NoError && m_response->size() > 0) {
        QJsonParseError jsonError;
        QJsonDocument doc = QJsonDocument::fromJson(*m_response, &jsonError);
        if (jsonError.error == QJsonParseError::NoError) {
            m_data->authlibInjectorMetadata = m_response->toBase64();
            emit finished(AccountTaskState::STATE_WORKING, tr("Got authlib-injector metadata."));
            return;
        }
    }
    emit finished(AccountTaskState::STATE_WORKING, tr("Couldn't get authlib-injector metadata, continuing anyway."));
}
