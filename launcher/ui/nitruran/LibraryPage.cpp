// SPDX-License-Identifier: GPL-3.0-only
#include "LibraryPage.h"

#include <QListView>
#include <QMenu>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>

#include "Application.h"
#include "InstanceCardDelegate.h"
#include "InstanceList.h"
#include "minecraft/MinecraftInstance.h"

namespace {
/** Sorts instances by name, or by most recently launched first. */
class InstanceSortProxy : public QSortFilterProxyModel {
   public:
    InstanceSortProxy(bool byRecent, QObject* parent) : QSortFilterProxyModel(parent), m_byRecent(byRecent) {}

   protected:
    bool lessThan(const QModelIndex& l, const QModelIndex& r) const override
    {
        auto* a = static_cast<MinecraftInstance*>(l.data(InstanceList::InstancePointerRole).value<void*>());
        auto* b = static_cast<MinecraftInstance*>(r.data(InstanceList::InstancePointerRole).value<void*>());
        if (!a || !b)
            return false;
        if (m_byRecent)
            return a->lastLaunch() > b->lastLaunch();  // newest first with AscendingOrder
        return a->name().compare(b->name(), Qt::CaseInsensitive) < 0;
    }

   private:
    bool m_byRecent;
};
}  // namespace

LibraryPage::LibraryPage(bool recentOnly, QWidget* parent) : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_proxy = new InstanceSortProxy(recentOnly, this);
    m_proxy->setSourceModel(APPLICATION->instances());
    m_proxy->setDynamicSortFilter(true);
    m_proxy->sort(0, Qt::AscendingOrder);

    m_delegate = new InstanceCardDelegate(this);
    connect(m_delegate, &InstanceCardDelegate::playClicked, this, &LibraryPage::onPlay);

    m_view = new QListView(this);
    m_view->setObjectName("NitruranGrid");
    m_view->setModel(m_proxy);
    m_view->setItemDelegate(m_delegate);
    m_view->setMouseTracking(true);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);
    m_view->setUniformItemSizes(true);
    if (recentOnly) {
        m_view->setFlow(QListView::LeftToRight);
        m_view->setWrapping(false);
        m_view->setFixedHeight(140);
        m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    } else {
        m_view->setViewMode(QListView::IconMode);
        m_view->setResizeMode(QListView::Adjust);
        m_view->setMovement(QListView::Static);
        m_view->setSpacing(0);
    }
    connect(m_view, &QListView::doubleClicked, this, &LibraryPage::onOpen);
    connect(m_view, &QListView::customContextMenuRequested, this, &LibraryPage::onContextMenu);
    layout->addWidget(m_view);
}

void LibraryPage::onPlay(MinecraftInstance* inst)
{
    if (inst->isRunning())
        APPLICATION->kill(inst);
    else
        APPLICATION->launch(inst);
}

void LibraryPage::onOpen(const QModelIndex& index)
{
    auto* inst = static_cast<MinecraftInstance*>(index.data(InstanceList::InstancePointerRole).value<void*>());
    if (inst)
        APPLICATION->showInstanceWindow(inst);
}

void LibraryPage::onContextMenu(const QPoint& pos)
{
    const QModelIndex idx = m_view->indexAt(pos);
    if (!idx.isValid())
        return;
    auto* inst = static_cast<MinecraftInstance*>(idx.data(InstanceList::InstancePointerRole).value<void*>());
    if (!inst)
        return;
    QMenu menu(this);
    menu.addAction(inst->isRunning() ? tr("Stop") : tr("Play"), this, [this, inst] { onPlay(inst); });
    menu.addAction(tr("Edit instance"), this, [inst] { APPLICATION->showInstanceWindow(inst); });
    menu.exec(m_view->viewport()->mapToGlobal(pos));
}
