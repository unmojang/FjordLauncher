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

    auto [request, response] = Net::Download::makeByteArray(url);
    m_request = request;

    m_task.reset(new NetJob("AuthlibInjectorMetadataStep", APPLICATION->network()));
    m_task->setAskRetry(false);
    m_task->setAutoRetryLimit(0);
    m_task->addNetAction(m_request);

    connect(m_task.get(), &Task::finished, this, [this, response] { onRequestDone(response); });

    m_task->start();
}

void AuthlibInjectorMetadataStep::onRequestDone(QByteArray* response)
{
    if (m_request->error() == QNetworkReply::NoError && response->size() > 0) {
        QJsonParseError jsonError;
        QJsonDocument doc = QJsonDocument::fromJson(*response, &jsonError);
        if (jsonError.error == QJsonParseError::NoError) {
            m_data->authlibInjectorMetadata = response->toBase64();
            emit finished(AccountTaskState::STATE_WORKING, tr("Got authlib-injector metadata."));
            return;
        }
    }
    emit finished(AccountTaskState::STATE_WORKING, tr("Couldn't get authlib-injector metadata, continuing anyway."));
}
