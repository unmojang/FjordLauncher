#pragma once
#include <QObject>

#include "minecraft/auth/AuthStep.h"
#include "net/Download.h"
#include "net/NetJob.h"

class YggdrasilMinecraftProfileStep : public AuthStep {
    Q_OBJECT

   public:
    explicit YggdrasilMinecraftProfileStep(AccountData* data);
    virtual ~YggdrasilMinecraftProfileStep() noexcept = default;

    void perform() override;

    QString describe() override;

   private slots:
    void onRequestDone();

   private:
    std::shared_ptr<QByteArray> m_response;
    Net::Download::Ptr m_request;
    NetJob::Ptr m_task;
};
