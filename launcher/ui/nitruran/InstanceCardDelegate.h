// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include <QStyledItemDelegate>

class MinecraftInstance;

/** Paints a Modrinth-App-style instance card for an InstanceList index. */
class InstanceCardDelegate : public QStyledItemDelegate {
    Q_OBJECT
   public:
    explicit InstanceCardDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const override { return { 220, 120 }; }
    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override;

   signals:
    void playClicked(MinecraftInstance* instance);

   private:
    static QRect cardRect(const QRect& cell);
    static QRect playRect(const QRect& card);
};
