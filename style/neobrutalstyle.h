// Neobrutal Dark - Qt widget style (application style)
// SPDX-License-Identifier: MIT
#pragma once

#include <QColor>
#include <QProxyStyle>

class QStyleOptionProgressBar;

// Style Qt widget bergaya neobrutalism: outline tebal, tombol timbul dengan
// hard shadow, tanpa gradasi / sudut membulat / blur.
// Dasarnya Fusion (QProxyStyle), semua elemen utama digambar ulang di sini.
// Warna aksen dibaca dari color scheme aktif (kdeglobals), jadi ikut skema warna.
class NeobrutalStyle : public QProxyStyle
{
    Q_OBJECT
public:
    NeobrutalStyle();

    using QProxyStyle::polish;
    using QProxyStyle::unpolish;
    void polish(QPalette &palette) override;
    void polish(QWidget *widget) override;
    void unpolish(QWidget *widget) override;

    void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const override;
    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const override;
    void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const override;
    int pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const override;
    QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const override;
    QRect subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const override;
    QRect subControlRect(ComplexControl control, const QStyleOptionComplex *option, SubControl subControl, const QWidget *widget) const override;
    int styleHint(StyleHint hint, const QStyleOption *option, const QWidget *widget, QStyleHintReturn *returnData) const override;

private:
    void loadColors();
    void drawButtonFace(QPainter *p, const QStyleOption *o, int shadow, bool hideWhenIdle) const;
    QRect progressFill(const QStyleOptionProgressBar *pb, const QRect &contents) const;

    // warna dari color scheme aktif
    QColor m_focus;    // kuning: fokus, tombol ditekan, shadow normal
    QColor m_hover;    // pink: hover, seleksi
    QColor m_link;     // biru: progress bar
};
