// Neobrutal Dark - window decoration (KDecoration3)
// SPDX-License-Identifier: MIT
#include "neobrutalbutton.h"

#include "neobrutal.h"

#include <KDecoration3/DecoratedWindow>

#include <QPainter>
#include <QPolygonF>

namespace Neobrutal
{
using KDecoration3::DecorationButtonType;

Button::Button(DecorationButtonType type, Decoration *decoration, QObject *parent)
    : KDecoration3::DecorationButton(type, decoration, parent)
{
    connect(decoration->window(), &KDecoration3::DecoratedWindow::iconChanged, this, [this]() {
        update();
    });
}

Button::Button(QObject *parent, const QVariantList &args)
    : Button(args.at(0).value<DecorationButtonType>(), args.at(1).value<Decoration *>(), parent)
{
    setGeometry(QRectF(0, 0, 22, 22));
}

Button *Button::create(DecorationButtonType type, KDecoration3::Decoration *decoration, QObject *parent)
{
    auto d = qobject_cast<Decoration *>(decoration);
    if (!d) {
        return nullptr;
    }
    auto *b = new Button(type, d, parent);
    const auto c = d->window();
    switch (type) {
    case DecorationButtonType::Close:
        b->setVisible(c->isCloseable());
        connect(c, &KDecoration3::DecoratedWindow::closeableChanged, b, &Button::setVisible);
        break;
    case DecorationButtonType::Maximize:
        b->setVisible(c->isMaximizeable());
        connect(c, &KDecoration3::DecoratedWindow::maximizeableChanged, b, &Button::setVisible);
        break;
    case DecorationButtonType::Minimize:
        b->setVisible(c->isMinimizeable());
        connect(c, &KDecoration3::DecoratedWindow::minimizeableChanged, b, &Button::setVisible);
        break;
    case DecorationButtonType::ContextHelp:
        b->setVisible(c->providesContextHelp());
        connect(c, &KDecoration3::DecoratedWindow::providesContextHelpChanged, b, &Button::setVisible);
        break;
    case DecorationButtonType::Shade:
        b->setVisible(c->isShadeable());
        connect(c, &KDecoration3::DecoratedWindow::shadeableChanged, b, &Button::setVisible);
        break;
    default:
        break;
    }
    return b;
}

// warna isi kotak tombol
QColor Button::faceColor() const
{
    switch (type()) {
    case DecorationButtonType::Close:
        return Pink;
    case DecorationButtonType::Maximize:
        return Blue;
    case DecorationButtonType::Minimize:
        return Green;
    default:
        return isChecked() ? Pink : Cream;
    }
}

void Button::paint(QPainter *painter, const QRectF &repaintRegion)
{
    Q_UNUSED(repaintRegion)
    auto d = qobject_cast<Decoration *>(decoration());
    if (!d) {
        return;
    }
    if (type() == DecorationButtonType::Spacer) {
        return;
    }
    if (type() == DecorationButtonType::Menu) {
        d->window()->icon().paint(painter, geometry().toRect());
        return;
    }

    const bool active = d->window()->isActive();
    const bool down = isPressed();
    const bool hover = isHovered();

    const QRect g = geometry().toRect();
    QRect face(g.left(), g.top(), g.width() - ButtonShadow, g.height() - ButtonShadow);
    const QColor ink = d->inkColor();
    QColor fill = active ? faceColor() : Dim;
    if (active && hover && !down) {
        fill = fill.lighter(112);
    }

    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setRenderHint(QPainter::Antialiasing, false);
    if (down) {
        face.translate(ButtonShadow, ButtonShadow);
    } else if (active) {
        painter->fillRect(face.translated(ButtonShadow, ButtonShadow), ink);
    }
    painter->fillRect(face, ink);
    painter->fillRect(face.adjusted(2, 2, -2, -2), fill);

    drawGlyph(painter, QRectF(face.adjusted(2, 2, -2, -2)), active ? Ink : d->outlineColor());
    painter->restore();
}

// simbol digambar dengan garis 2px lurus, siku
void Button::drawGlyph(QPainter *p, const QRectF &inner, const QColor &color) const
{
    const QPointF c = inner.center();
    const qreal s = qFloor(inner.width() / 4.0 + 0.5) + 1; // setengah sisi simbol
    QPen pen(color, 2, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
    p->setPen(pen);
    p->setBrush(Qt::NoBrush);
    p->setRenderHint(QPainter::Antialiasing, true);

    switch (type()) {
    case DecorationButtonType::Close:
        p->drawLine(QPointF(c.x() - s, c.y() - s), QPointF(c.x() + s, c.y() + s));
        p->drawLine(QPointF(c.x() + s, c.y() - s), QPointF(c.x() - s, c.y() + s));
        break;
    case DecorationButtonType::Maximize:
        if (isChecked()) {
            p->drawRect(QRectF(c.x() - s + 2, c.y() - s - 1, 2 * s - 2, 2 * s - 2));
            p->drawRect(QRectF(c.x() - s - 1, c.y() - s + 2, 2 * s - 2, 2 * s - 2));
        } else {
            p->drawRect(QRectF(c.x() - s, c.y() - s, 2 * s, 2 * s));
        }
        break;
    case DecorationButtonType::Minimize:
        p->drawLine(QPointF(c.x() - s, c.y() + s - 1), QPointF(c.x() + s, c.y() + s - 1));
        break;
    case DecorationButtonType::OnAllDesktops:
        if (isChecked()) {
            p->setPen(Qt::NoPen);
            p->setBrush(color);
        }
        p->drawEllipse(c, s - 0.5, s - 0.5);
        break;
    case DecorationButtonType::Shade:
        p->drawLine(QPointF(c.x() - s, c.y() - s), QPointF(c.x() + s, c.y() - s));
        if (isChecked()) {
            p->drawPolyline(QPolygonF({QPointF(c.x() - s, c.y()), QPointF(c.x(), c.y() + s), QPointF(c.x() + s, c.y())}));
        } else {
            p->drawPolyline(QPolygonF({QPointF(c.x() - s, c.y() + s), QPointF(c.x(), c.y()), QPointF(c.x() + s, c.y() + s)}));
        }
        break;
    case DecorationButtonType::KeepAbove:
        p->drawPolyline(QPolygonF({QPointF(c.x() - s, c.y() + s / 2), QPointF(c.x(), c.y() - s / 2), QPointF(c.x() + s, c.y() + s / 2)}));
        if (isChecked()) {
            p->drawLine(QPointF(c.x() - s, c.y() + s), QPointF(c.x() + s, c.y() + s));
        }
        break;
    case DecorationButtonType::KeepBelow:
        p->drawPolyline(QPolygonF({QPointF(c.x() - s, c.y() - s / 2), QPointF(c.x(), c.y() + s / 2), QPointF(c.x() + s, c.y() - s / 2)}));
        if (isChecked()) {
            p->drawLine(QPointF(c.x() - s, c.y() - s), QPointF(c.x() + s, c.y() - s));
        }
        break;
    case DecorationButtonType::ApplicationMenu:
        for (int i = -1; i <= 1; ++i) {
            p->drawLine(QPointF(c.x() - s, c.y() + i * s * 0.8), QPointF(c.x() + s, c.y() + i * s * 0.8));
        }
        break;
    case DecorationButtonType::ContextHelp: {
        QFont f = p->font();
        f.setBold(true);
        f.setPixelSize(qRound(inner.height() * 0.8));
        p->setFont(f);
        p->drawText(inner, Qt::AlignCenter, QStringLiteral("?"));
        break;
    }
    default:
        p->setPen(Qt::NoPen);
        p->setBrush(color);
        p->drawRect(QRectF(c.x() - 2, c.y() - 2, 4, 4));
        break;
    }
}
}
