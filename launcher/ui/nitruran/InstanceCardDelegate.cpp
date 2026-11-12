// SPDX-License-Identifier: GPL-3.0-only
#include "InstanceCardDelegate.h"

#include <QDateTime>
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>

#include "Application.h"
#include "NitruranStyle.h"
#include "InstanceList.h"
#include "icons/IconList.h"
#include "minecraft/MinecraftInstance.h"

QRect InstanceCardDelegate::cardRect(const QRect& cell)
{
    return cell.adjusted(6, 6, -6, -6);
}

QRect InstanceCardDelegate::playRect(const QRect& card)
{
    return QRect(card.right() - 12 - 36, card.bottom() - 12 - 36, 36, 36);
}

void InstanceCardDelegate::paint(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& idx) const
{
    using namespace Nitruran;
    auto* inst = static_cast<MinecraftInstance*>(idx.data(InstanceList::InstancePointerRole).value<void*>());
    if (!inst)
        return;

    p->save();
    p->setRenderHint(QPainter::Antialiasing);

    const QRect card = cardRect(opt.rect);
    const bool hover = opt.state & QStyle::State_MouseOver;
    const bool selected = opt.state & QStyle::State_Selected;

    p->setPen(QPen(selected ? kBrand : kBorder, 1));
    p->setBrush(hover ? kSurface4 : kSurface3);
    p->drawRoundedRect(card, kRadiusMd, kRadiusMd);

    // icon
    const QIcon icon = APPLICATION->icons()->getIcon(idx.data(Qt::DecorationRole).toString());
    const QRect iconRect(card.left() + 14, card.top() + 14, 56, 56);
    icon.paint(p, iconRect);

    // name
    p->setPen(kTextPrimary);
    QFont nameFont = opt.font;
    nameFont.setBold(true);
    nameFont.setPointSizeF(opt.font.pointSizeF() + 1);
    p->setFont(nameFont);
    const QRect nameRect(iconRect.right() + 12, card.top() + 16, card.width() - iconRect.width() - 14 - 12 - 14, 48);
    p->drawText(nameRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap,
                p->fontMetrics().elidedText(inst->name(), Qt::ElideRight, nameRect.width() * 2 - 24));

    // subtitle: last played
    p->setPen(kTextSecondary);
    p->setFont(opt.font);
    QString sub;
    if (inst->lastLaunch() > 0)
        sub = QObject::tr("Played %1").arg(QDateTime::fromMSecsSinceEpoch(inst->lastLaunch()).date().toString(Qt::ISODate));
    else
        sub = QObject::tr("Never played");
    p->drawText(QRect(card.left() + 14, card.bottom() - 36, card.width() - 14 - 60, 24), Qt::AlignLeft | Qt::AlignVCenter, sub);

    // play / running button
    const bool running = inst->isRunning();
    p->setPen(Qt::NoPen);
    p->setBrush(running ? kSurface5 : kBrand);
    const QRect pr = playRect(card);
    p->drawRoundedRect(pr, kRadiusMd, kRadiusMd);
    p->setPen(running ? kTextPrimary : QColor(0, 0, 0));
    QFont glyph = opt.font;
    glyph.setBold(true);
    p->setFont(glyph);
    p->drawText(pr, Qt::AlignCenter, running ? QStringLiteral("\u25A0") : QStringLiteral("\u25B6"));

    p->restore();
}

bool InstanceCardDelegate::editorEvent(QEvent* event, QAbstractItemModel*, const QStyleOptionViewItem& option, const QModelIndex& index)
{
    if (event->type() != QEvent::MouseButtonRelease)
        return false;
    auto* me = static_cast<QMouseEvent*>(event);
    if (me->button() != Qt::LeftButton)
        return false;
    if (!playRect(cardRect(option.rect)).contains(me->position().toPoint()))
        return false;
    auto* inst = static_cast<MinecraftInstance*>(index.data(InstanceList::InstancePointerRole).value<void*>());
    if (!inst)
        return false;
    emit playClicked(inst);
    return true;
}
