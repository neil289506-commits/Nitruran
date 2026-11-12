// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include <QWidget>

class QListView;
class QSortFilterProxyModel;
class InstanceCardDelegate;

/** Instance grid (Modrinth "Library") and, with recentOnly, the "Jump back in" row of Home. */
class LibraryPage : public QWidget {
    Q_OBJECT
   public:
    explicit LibraryPage(bool recentOnly, QWidget* parent = nullptr);

   private slots:
    void onPlay(class MinecraftInstance* inst);
    void onOpen(const QModelIndex& index);
    void onContextMenu(const QPoint& pos);

   private:
    QListView* m_view = nullptr;
    QSortFilterProxyModel* m_proxy = nullptr;
    InstanceCardDelegate* m_delegate = nullptr;
};
