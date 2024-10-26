#pragma once
#include <QObject>
#include <optional>

#include "minecraft/auth/AuthStep.h"
#include "net/NetJob.h"
#include "net/Upload.h"

class Yggdrasil;

class YggdrasilStep : public AuthStep {
    Q_OBJECT

   public:
    explicit YggdrasilStep(AccountData* data, std::optional<QString> password);
    virtual ~YggdrasilStep() noexcept = default;

    void perform() override;

    QString describe() override;

   private slots:
    void onRequestDone();

   private:
    void login(QString password);
    void refresh();

    void processResponse(QJsonObject responseData);
    void processError(QJsonObject responseData);

    std::optional<QString> m_password;
    std::shared_ptr<QByteArray> m_response;
    Net::Upload::Ptr m_request;
    NetJob::Ptr m_task;
};
