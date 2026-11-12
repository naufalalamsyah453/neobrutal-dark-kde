// Neobrutal Dark - window decoration (KDecoration3)
// SPDX-License-Identifier: MIT
#pragma once

#include "neobrutaldecoration.h"

#include <KDecoration3/DecorationButton>

namespace Neobrutal
{
class Button : public KDecoration3::DecorationButton
{
    Q_OBJECT

public:
    explicit Button(QObject *parent, const QVariantList &args);
    ~Button() override = default;

    static Button *create(KDecoration3::DecorationButtonType type, KDecoration3::Decoration *decoration, QObject *parent);

    void paint(QPainter *painter, const QRectF &repaintRegion) override;

private:
    explicit Button(KDecoration3::DecorationButtonType type, Decoration *decoration, QObject *parent = nullptr);
    QColor faceColor() const;
    void drawGlyph(QPainter *painter, const QRectF &inner, const QColor &color) const;
};
}
