#include "InstallAgentDialog.h"

#include <QDate>
#include <QDateTime>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTime>
#include <QVBoxLayout>
#include "Application.h"
#include "meta/Index.h"
#include "meta/VersionList.h"
#include "minecraft/PackProfile.h"
#include "ui/widgets/PageContainer.h"
#include "ui/widgets/VersionSelectWidget.h"

class InstallAgentPage : public VersionSelectWidget, public BasePage {
    Q_OBJECT
   public:
    InstallAgentPage(const QString& id, const QString& name, bool supported, PackProfile* profile)
        : VersionSelectWidget(nullptr), uid(id), name(name)
    {
        const QString minecraftVersion = profile->getComponentVersion("net.minecraft");
        if (!supported) {  // Agent unsupported
            setEmptyString(tr("This Yggdrasil agent is not supported for Minecraft %1").arg(minecraftVersion));
            setExactFilter(BaseVersionList::ParentVersionRole, "AAA");  // clear list
        } else {
            setEmptyString(tr("No versions are currently available"));
        }

        if (const QString currentVersion = profile->getComponentVersion(id); !currentVersion.isNull())
            setCurrentVersion(currentVersion);
    }

    QString id() const override { return uid; }
    QString displayName() const override { return name; }
    QIcon icon() const override { return QIcon::fromTheme("java"); }

    void openedImpl() override
    {
        if (loaded)
            return;

        const auto versions = APPLICATION->metadataIndex()->get(uid);
        if (!versions)
            return;

        initialize(versions.get());
        loaded = true;
    }

    void setParentContainer(BasePageContainer* container) override
    {
        auto dialog = dynamic_cast<QDialog*>(dynamic_cast<PageContainer*>(container)->parent());
        connect(view(), &QAbstractItemView::doubleClicked, dialog, &QDialog::accept);
    }

   private:
    const QString uid;
    const QString name;
    bool loaded = false;
};

static InstallAgentPage* pageCast(BasePage* page)
{
    auto result = dynamic_cast<InstallAgentPage*>(page);
    Q_ASSERT(result != nullptr);
    return result;
}

InstallAgentDialog::InstallAgentDialog(PackProfile* profile, QWidget* parent)
    : QDialog(parent), profile(profile), container(new PageContainer(this, QString(), this)), buttons(new QDialogButtonBox(this))
{
    auto layout = new QVBoxLayout(this);
    // small margins look ugly on macOS on modal windows
#ifndef Q_OS_MACOS
    layout->setContentsMargins(0, 0, 0, 0);
#endif
    container->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    layout->addWidget(container);

    auto buttonLayout = new QHBoxLayout(this);
    // small margins look ugly on macOS on modal windows
#ifndef Q_OS_MACOS
    buttonLayout->setContentsMargins(0, 0, 6, 6);
#endif
    auto refreshButton = new QPushButton(tr("&Refresh"), this);
    connect(refreshButton, &QPushButton::clicked, this, [this] { pageCast(container->selectedPage())->loadList(); });
    buttonLayout->addWidget(refreshButton);

    buttons->setOrientation(Qt::Horizontal);
    buttons->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
    buttons->button(QDialogButtonBox::Ok)->setText(tr("Ok"));
    buttons->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    buttonLayout->addWidget(buttons);

    container->addButtons(buttonLayout);

    setWindowTitle(dialogTitle());
    setWindowModality(Qt::WindowModal);
    resize(520, 347);

    for (BasePage* page : container->getPages()) {
        connect(pageCast(page), &VersionSelectWidget::selectedVersionChanged, this, [this, page] {
            if (page->id() == container->selectedPage()->id())
                validate(container->selectedPage());
        });
    }
    connect(container, &PageContainer::selectedPageChanged, this, [this](BasePage*, BasePage* current) { validate(current); });
    pageCast(container->selectedPage())->selectSearch();
    validate(container->selectedPage());
}

QList<BasePage*> InstallAgentDialog::getPages()
{
    const QString minecraftVersion = profile->getComponentVersion("net.minecraft");

    bool authlibSupported = false;
    if (!minecraftVersion.isEmpty()) {
        auto meta = APPLICATION->metadataIndex()->get("net.minecraft", minecraftVersion);
        // authlib-injector requires 1.7.10+ (released 2014-06-26)
        if (meta && meta->rawTime() != 0)
            authlibSupported = meta->time() >= QDateTime(QDate(2014, 5, 1), QTime(), Qt::UTC);
    }

    return {
        new InstallAgentPage("org.unmojang.loki", tr("Loki"), true, profile),
        new InstallAgentPage("moe.yushi.authlibinjector", tr("authlib-injector"), authlibSupported, profile),
    };
}

QString InstallAgentDialog::dialogTitle()
{
    return tr("Install Yggdrasil Agent");
}

void InstallAgentDialog::validate(BasePage* page)
{
    buttons->button(QDialogButtonBox::Ok)->setEnabled(pageCast(page)->selectedVersion() != nullptr);
}

void InstallAgentDialog::done(int result)
{
    if (result == Accepted) {
        auto* page = pageCast(container->selectedPage());
        if (page->selectedVersion()) {
            // Remove other agent for mutual exclusivity
            if (page->id() == "org.unmojang.loki")
                profile->remove("moe.yushi.authlibinjector");
            else
                profile->remove("org.unmojang.loki");

            profile->setComponentVersion(page->id(), page->selectedVersion()->descriptor());
            profile->resolve(Net::Mode::Online);
        }
    }

    QDialog::done(result);
}

#include "InstallAgentDialog.moc"
