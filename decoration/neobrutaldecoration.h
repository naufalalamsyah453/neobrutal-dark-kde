// Neobrutal Dark - window decoration (KDecoration3)
// SPDX-License-Identifier: MIT
#pragma once

#include <KDecoration3/DecoratedWindow>
#include <KDecoration3/Decoration>
#include <KDecoration3/DecorationSettings>

#include <QColor>
#include <QVariant>

namespace KDecoration3
{
class DecorationButtonGroup;
}

namespace Neobrutal
{
class Decoration : public KDecoration3::Decoration
{
    Q_OBJECT

public:
    explicit Decoration(QObject *parent = nullptr, const QVariantList &args = QVariantList());
    ~Decoration() override = default;

    void paint(QPainter *painter, const QRectF &repaintRegion) override;

    // ukuran kotak tombol (tanpa shadow)
    int buttonBox() const;
    QColor outlineColor() const;
    QColor titleBarColor() const;
    QColor fontColor() const;
    QColor inkColor() const;

public Q_SLOTS:
    bool init() override;

private Q_SLOTS:
    void recalculate();
    void updateButtonsGeometry();
    void updateButtonsGeometryDelayed();
    void updateShadow();

private:
    bool isLeftEdge() const;
    bool isRightEdge() const;
    bool isTopEdge() const;
    bool isBottomEdge() const;
    qreal titleHeight() const;
    qreal topInset() const; // bingkai di atas titlebar
    QRectF captionRect() const;
    void createButtons();

    KDecoration3::DecorationButtonGroup *m_leftButtons = nullptr;
    KDecoration3::DecorationButtonGroup *m_rightButtons = nullptr;
};
}
