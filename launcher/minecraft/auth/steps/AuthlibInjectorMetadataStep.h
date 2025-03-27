#pragma once
#include <QObject>

#include "minecraft/auth/AuthStep.h"
#include "net/NetJob.h"
#include "net/Download.h"

class AuthlibInjectorMetadataStep : public AuthStep {
    Q_OBJECT

   public:
    explicit AuthlibInjectorMetadataStep(AccountData* data);
    virtual ~AuthlibInjectorMetadataStep() noexcept;

    void perform() override;

    QString describe() override;

   private slots:
    void onRequestDone();

   private:
    std::shared_ptr<QByteArray> m_response;
    Net::Download::Ptr m_request;
    NetJob::Ptr m_task;
};
