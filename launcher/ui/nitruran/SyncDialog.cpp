// SPDX-License-Identifier: GPL-3.0-only
#include "SyncDialog.h"

#include <QCheckBox>
#include <QDir>
#include <QLabel>
#include <QListWidget>
#include <QPointer>
#include <QPushButton>
#include <QHBoxLayout>
#include <QThread>
#include <QVBoxLayout>
#include <memory>

#include "Application.h"
#include "InstanceList.h"
#include "NitruranI18n.h"
#include "settings/SettingsObject.h"

QList<SyncSource> SyncDialog::detectSources()
{
    const QString own = QDir::cleanPath(QDir(APPLICATION->dataRoot()).absolutePath());
    const QString roaming = QFileInfo(own).absolutePath();  // folder that holds every launcher's data folder
    return SyncService::detect(roaming, own);
}

SyncDialog::SyncDialog(QWidget* parent) : QDialog(parent)
{
    using Nitruran::t;
    setWindowTitle(t("Sync from other launchers"));
    setMinimumWidth(460);
    m_sources = detectSources();

    auto* v = new QVBoxLayout(this);
    if (m_sources.isEmpty()) {
        v->addWidget(new QLabel(t("No data from Prism Launcher or mlcmLauncher was found."), this));
        auto* close = new QPushButton(t("Close"), this);
        connect(close, &QPushButton::clicked, this, &QDialog::accept);
        v->addWidget(close);
        return;
    }

    v->addWidget(new QLabel(t("Detected launchers"), this));
    m_list = new QListWidget(this);
    for (const auto& s : m_sources) {
        auto* item = new QListWidgetItem(QStringLiteral("%1  —  %2").arg(s.displayName, t("%1 instances").arg(s.instances.size())), m_list);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Checked);
    }
    v->addWidget(m_list);

    m_instances = new QCheckBox(t("Instances"), this);
    m_accounts = new QCheckBox(t("Accounts"), this);
    m_icons = new QCheckBox(t("Icons"), this);
    m_settings = new QCheckBox(t("Settings (memory, Java, language)"), this);
    for (auto* c : { m_instances, m_accounts, m_icons, m_settings }) {
        c->setChecked(true);
        v->addWidget(c);
    }
    // Accounts are tied to the application ID that signed them in, so they usually need a fresh sign-in anyway.
    m_accounts->setChecked(false);
    auto* note = new QLabel(t("Existing items are never overwritten.") + QStringLiteral("\n") +
                                t("Accounts are not copied by default: they are tied to the app that signed them in, so sign in again in Nitruran."),
                            this);
    note->setWordWrap(true);
    v->addWidget(note);

    m_status = new QLabel(this);
    m_status->setWordWrap(true);
    v->addWidget(m_status);

    m_syncBtn = new QPushButton(t("Sync now"), this);
    auto* close = new QPushButton(t("Close"), this);
    connect(m_syncBtn, &QPushButton::clicked, this, &SyncDialog::startSync);
    connect(close, &QPushButton::clicked, this, &QDialog::accept);
    auto* row = new QHBoxLayout;
    row->addStretch(1);
    row->addWidget(m_syncBtn);
    row->addWidget(close);
    v->addLayout(row);
}

void SyncDialog::startSync()
{
    using Nitruran::t;
    SyncOptions opt;
    opt.instances = m_instances->isChecked();
    opt.accounts = m_accounts->isChecked();
    opt.icons = m_icons->isChecked();
    opt.settings = m_settings->isChecked();

    QList<SyncSource> chosen;
    for (int i = 0; i < m_list->count(); ++i)
        if (m_list->item(i)->checkState() == Qt::Checked)
            chosen << m_sources.at(i);
    if (chosen.isEmpty())
        return;

    const QString dataDir = APPLICATION->dataRoot();
    const QString instDir = APPLICATION->settings()->get("InstanceDir").toString();
    m_syncBtn->setEnabled(false);
    m_status->setText(t("Copying..."));

    auto report = std::make_shared<SyncReport>();
    QPointer<QLabel> status(m_status);
    QThread* worker = QThread::create([=]() {
        for (const auto& src : chosen) {  // later sources never override earlier ones: existing items are skipped
            auto r = SyncService::sync(src, dataDir, instDir, opt, [status](const QString& msg) {
                if (status)
                    QMetaObject::invokeMethod(status.data(), [status, msg] { if (status) status->setText(msg); }, Qt::QueuedConnection);
            });
            report->instancesCopied += r.instancesCopied;
            report->instancesSkipped += r.instancesSkipped;
            report->accountsCopied = report->accountsCopied || r.accountsCopied;
            report->iconsCopied += r.iconsCopied;
            report->errors += r.errors;
            for (auto it = r.settings.begin(); it != r.settings.end(); ++it)
                if (!report->settings.contains(it.key()))
                    report->settings.insert(it.key(), it.value());
        }
    });
    connect(worker, &QThread::finished, this, [this, worker, report]() {
        worker->deleteLater();
        // apply on the GUI thread
        for (auto it = report->settings.begin(); it != report->settings.end(); ++it)
            APPLICATION->settings()->set(it.key(), it.value());
        if (report->instancesCopied > 0)
            APPLICATION->instances()->loadList();

        QString text = t("Sync finished") + QStringLiteral(": ") +
                       t("Copied %1 instances, skipped %2 already present.").arg(report->instancesCopied).arg(report->instancesSkipped);
        if (report->accountsCopied)
            text += QStringLiteral("\n") + t("Accounts copied. Restart Nitruran to load them.");
        if (!report->errors.isEmpty())
            text += QStringLiteral("\n") + t("Errors:") + QStringLiteral(" ") + report->errors.join(QStringLiteral("; "));
        m_status->setText(text);
        m_syncBtn->setEnabled(true);
    });
    worker->start();
}
