// SPDX-License-Identifier: GPL-3.0-only
#include "NitruranWindow.h"

#include <QButtonGroup>
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QDir>
#include <QLabel>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QStackedWidget>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>

#include "Application.h"
#include "NitruranI18n.h"
#include "NitruranStyle.h"
#include "FileSystem.h"
#include "InstanceList.h"
#include "LibraryPage.h"
#include "SyncDialog.h"
#include "QObjectPtr.h"
#include "modplatform/modrinth/ModrinthAPI.h"
#include "settings/SettingsObject.h"
#include "tasks/Task.h"
#include "ui/dialogs/CustomMessageBox.h"
#include "ui/dialogs/NewInstanceDialog.h"
#include "ui/dialogs/ProgressDialog.h"

NitruranWindow::NitruranWindow(QWidget* parent) : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("Nitruran"));
    resize(1100, 700);
    setMinimumSize(800, 500);

    auto* root = new QWidget(this);
    root->setObjectName("NitruranRoot");
    setCentralWidget(root);
    setStyleSheet(Nitruran::windowStyleSheet());

    auto* h = new QHBoxLayout(root);
    h->setContentsMargins(0, 0, 0, 0);
    h->setSpacing(0);

    // ---- left sidebar (Modrinth: icon nav on the left) ----
    auto* sidebar = new QWidget(root);
    sidebar->setObjectName("NitruranSidebar");
    sidebar->setFixedWidth(76);
    auto* sv = new QVBoxLayout(sidebar);
    sv->setContentsMargins(8, 12, 8, 12);
    sv->setSpacing(4);
    h->addWidget(sidebar);

    m_navGroup = new QButtonGroup(this);
    m_navGroup->setExclusive(true);

    auto* home = addNavButton(QStringLiteral("\u2302"), Nitruran::t("Home"), Nitruran::t("Home"), true);
    auto* library = addNavButton(QStringLiteral("\u25A4"), Nitruran::t("Library"), Nitruran::t("Library"), true);
    auto* discover = addNavButton(QStringLiteral("\u2315"), Nitruran::t("Discover"), Nitruran::t("Discover and add instances"), false);
    sv->addWidget(home);
    sv->addWidget(discover);
    sv->addWidget(library);
    sv->addStretch(1);
    auto* accounts = addNavButton(QStringLiteral("\u263A"), Nitruran::t("Accounts"), Nitruran::t("Accounts"), false);
    auto* settings = addNavButton(QStringLiteral("\u2699"), Nitruran::t("Settings"), Nitruran::t("Settings"), false);
    auto* sync = addNavButton(QStringLiteral("\u21BB"), Nitruran::t("Sync"), Nitruran::t("Sync from other launchers"), false);
    sv->addWidget(sync);
    sv->addWidget(accounts);
    sv->addWidget(settings);

    // ---- content ----
    auto* content = new QWidget(root);
    auto* cv = new QVBoxLayout(content);
    cv->setContentsMargins(24, 20, 24, 20);
    cv->setSpacing(12);
    h->addWidget(content, 1);

    m_pages = new QStackedWidget(content);
    cv->addWidget(m_pages, 1);

    // Home page: "Jump back in" row
    auto* homePage = new QWidget(m_pages);
    auto* hp = new QVBoxLayout(homePage);
    hp->setContentsMargins(0, 0, 0, 0);
    auto* hTitle = new QLabel(Nitruran::t("Welcome back!"), homePage);
    hTitle->setObjectName("NitruranTitle");
    auto* hSub = new QLabel(Nitruran::t("Jump back in"), homePage);
    hSub->setObjectName("NitruranSub");
    hp->addWidget(hTitle);
    hp->addWidget(hSub);
    hp->addWidget(new LibraryPage(true, homePage));
    hp->addStretch(1);
    m_pages->addWidget(homePage);  // index 0

    // Library page: full grid
    auto* libPage = new QWidget(m_pages);
    auto* lp = new QVBoxLayout(libPage);
    lp->setContentsMargins(0, 0, 0, 0);
    auto* lTop = new QHBoxLayout();
    auto* lTitle = new QLabel(Nitruran::t("Library"), libPage);
    lTitle->setObjectName("NitruranTitle");
    auto* newBtn = new QPushButton(Nitruran::t("New instance"), libPage);
    newBtn->setObjectName("NitruranPrimary");
    lTop->addWidget(lTitle);
    lTop->addStretch(1);
    lTop->addWidget(newBtn);
    lp->addLayout(lTop);
    lp->addWidget(new LibraryPage(false, libPage), 1);
    m_pages->addWidget(libPage);  // index 1

    connect(newBtn, &QPushButton::clicked, this, [this] { addInstance(); });
    connect(home, &QToolButton::clicked, this, [this] { m_pages->setCurrentIndex(0); });
    connect(library, &QToolButton::clicked, this, [this] { m_pages->setCurrentIndex(1); });
    // Discover / Add: Prism's own new-instance dialog (has Modrinth, CurseForge, FTB, ... pages)
    connect(discover, &QToolButton::clicked, this, [this] { addInstance(); });
    // Reuse Prism's own settings/accounts UI for now
    connect(accounts, &QToolButton::clicked, this, [this] { APPLICATION->ShowGlobalSettings(this, "accounts"); });
    connect(settings, &QToolButton::clicked, this, [this] { APPLICATION->ShowGlobalSettings(this); });

    connect(sync, &QToolButton::clicked, this, [this] { SyncDialog(this).exec(); });

    home->setChecked(true);
    QTimer::singleShot(800, this, &NitruranWindow::offerSyncOnFirstRun);
}

NitruranWindow::~NitruranWindow() = default;

static QPixmap glyphPixmap(const QString& glyph, const QColor& color)
{
    QPixmap pm(28, 28);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(color);
    QFont f = p.font();
    f.setPixelSize(22);
    p.setFont(f);
    p.drawText(pm.rect(), Qt::AlignCenter, glyph);
    return pm;
}

QToolButton* NitruranWindow::addNavButton(const QString& glyph, const QString& label, const QString& tooltip, bool checkable)
{
    auto* b = new QToolButton(this);
    QIcon icon;
    icon.addPixmap(glyphPixmap(glyph, Nitruran::kText), QIcon::Normal, QIcon::Off);
    icon.addPixmap(glyphPixmap(glyph, Nitruran::kBrand), QIcon::Normal, QIcon::On);
    b->setIcon(icon);
    b->setIconSize(QSize(28, 28));
    b->setText(label);
    b->setToolTip(tooltip);
    b->setCheckable(checkable);
    b->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    if (checkable)
        m_navGroup->addButton(b);
    return b;
}

void NitruranWindow::closeEvent(QCloseEvent* event)
{
    APPLICATION->settings()->set("MainWindowState", QString::fromUtf8(saveState().toBase64()));
    APPLICATION->settings()->set("MainWindowGeometry", QString::fromUtf8(saveGeometry().toBase64()));
    event->accept();
    emit isClosing();
}

void NitruranWindow::checkInstancePathForProblems()
{
    const QString instanceFolder = APPLICATION->settings()->get("InstanceDir").toString();
    if (FS::checkProblemticPathJava(QDir(instanceFolder))) {
        QMessageBox warning(this);
        warning.setText(Nitruran::t("Your instance folder path contains characters that can break Java (e.g. '!' or ';'). "
                           "Please move your instances to a different folder in the settings."));
        warning.setIcon(QMessageBox::Warning);
        warning.exec();
    }
}

void NitruranWindow::updatesAllowedChanged(bool) {}

void NitruranWindow::addInstance(const QString& url, const QMap<QString, QString>& extraInfo)
{
    NewInstanceDialog dlg(APPLICATION->settings()->get("LastUsedGroupForNewInstance").toString(), url, extraInfo, this);
    if (!dlg.exec())
        return;
    APPLICATION->settings()->set("LastUsedGroupForNewInstance", dlg.instGroup());
    APPLICATION->settings()->set("LastUsedInstDirForNewInstance", dlg.instDir());
    if (InstanceTask* t = dlg.extractTask())
        runInstanceTask(t);
}

void NitruranWindow::runInstanceTask(InstanceTask* rawTask)
{
    unique_qobject_ptr<Task> task(APPLICATION->instances()->wrapInstanceTask(rawTask));
    runModalTask(task.get());
}

void NitruranWindow::runModalTask(Task* task)
{
    connect(task, &Task::failed, this,
            [this](QString reason) { CustomMessageBox::selectable(this, Nitruran::t("Error"), reason, QMessageBox::Critical)->show(); });
    connect(task, &Task::succeeded, this, [this, task]() {
        const QStringList warnings = task->warnings();
        if (warnings.count())
            CustomMessageBox::selectable(this, Nitruran::t("Warnings"), warnings.join('\n'), QMessageBox::Warning)->show();
    });
    ProgressDialog loadDialog(this);
    loadDialog.showSkipButton();
    loadDialog.execWithTask(task);
}

// Reduced port of MainWindow::processURLs (MainWindow.cpp:931). Modrinth modpack links and plain
// file/URL imports are handled; the CurseForge/"import" URL branches are NOT ported yet.
void NitruranWindow::processURLs(QList<QUrl> urls)
{
    for (auto& url : urls) {
        if (url.isEmpty() || url.toString().trimmed().isEmpty())
            continue;
        if (url.scheme().isEmpty())
            url.setScheme("file");

        QMap<QString, QString> extra;
        if (url.scheme().compare("modrinth", Qt::CaseInsensitive) == 0) {
            const auto packId = ModrinthAPI::getModpackIdFromUrl(url);
            if (!packId.isEmpty()) {
                extra.insert("pack_id", packId);
                addInstance(url.toString(), extra);
            }
            continue;
        }
        addInstance(url.isLocalFile() ? url.toLocalFile() : url.toString(), extra);
        break;  // like the original: only one dropped file
    }
}

// First launch: if Prism Launcher / mlcmLauncher data exists next to ours, offer to import it once.
void NitruranWindow::offerSyncOnFirstRun()
{
    auto* s = APPLICATION->settings();
    if (!s->contains("NitruranSyncOffered"))
        s->registerSetting("NitruranSyncOffered", false);
    if (s->get("NitruranSyncOffered").toBool())
        return;
    const auto sources = SyncDialog::detectSources();
    s->set("NitruranSyncOffered", true);  // ask only once, even if the app closes while the box is open
    if (sources.isEmpty())
        return;

    QStringList names;
    for (const auto& src : sources)
        names << src.displayName;
    QMessageBox box(this);
    box.setIcon(QMessageBox::Question);
    box.setWindowTitle(Nitruran::t("Other launcher data found"));
    box.setText(Nitruran::t("Nitruran found data from %1. Import it now?").arg(names.join(QStringLiteral(" / "))));
    auto* importBtn = box.addButton(Nitruran::t("Import"), QMessageBox::AcceptRole);
    box.addButton(Nitruran::t("Later"), QMessageBox::RejectRole);
    box.exec();
    if (box.clickedButton() == importBtn)
        SyncDialog(this).exec();
}
