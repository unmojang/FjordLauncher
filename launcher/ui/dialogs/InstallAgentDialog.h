#pragma once

#include <QDialog>
#include "ui/pages/BasePageProvider.h"

class PackProfile;
class PageContainer;
class QDialogButtonBox;

class InstallAgentDialog final : public QDialog, protected BasePageProvider {
    Q_OBJECT

   public:
    explicit InstallAgentDialog(PackProfile* profile, QWidget* parent = nullptr);

    QList<BasePage*> getPages() override;
    QString dialogTitle() override;

    void validate(BasePage* page);
    void done(int result) override;

   private:
    PackProfile* profile;
    PageContainer* container;
    QDialogButtonBox* buttons;
};
