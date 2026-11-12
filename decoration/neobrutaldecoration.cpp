// Neobrutal Dark - window decoration (KDecoration3)
// SPDX-License-Identifier: MIT
#include "neobrutaldecoration.h"

#include "neobrutal.h"
#include "neobrutalbutton.h"

#include <KDecoration3/DecorationButtonGroup>
#include <KDecoration3/DecorationShadow>
#include <KDecoration3/ScaleHelpers>

#include <KPluginFactory>

#include <QFontMetricsF>
#include <QImage>
#include <QPainter>

K_PLUGIN_FACTORY_WITH_JSON(NeobrutalDecoFactory, "neobrutal.json", registerPlugin<Neobrutal::Decoration>(); registerPlugin<Neobrutal::Button>();)

namespace Neobrutal
{
using KDecoration3::ColorGroup;
using KDecoration3::ColorRole;

Decoration::Decoration(QObject *parent, const QVariantList &args)
    : KDecoration3::Decoration(parent, args)
{
}

bool Decoration::init()
{
    auto s = settings();
    connect(s.get(), &KDecoration3::DecorationSettings::fontChanged, this, &Decoration::recalculate);
    connect(s.get(), &KDecoration3::DecorationSettings::spacingChanged, this, &Decoration::recalculate);
    connect(s.get(), &KDecoration3::DecorationSettings::reconfigured, this, &Decoration::recalculate);
    connect(s.get(), &KDecoration3::DecorationSettings::decorationButtonsLeftChanged, this, &Decoration::updateButtonsGeometryDelayed);
    connect(s.get(), &KDecoration3::DecorationSettings::decorationButtonsRightChanged, this, &Decoration::updateButtonsGeometryDelayed);

    auto w = window();
    connect(w, &KDecoration3::DecoratedWindow::activeChanged, this, [this]() {
        updateShadow();
        update();
    });
    connect(w, &KDecoration3::DecoratedWindow::maximizedChanged, this, &Decoration::recalculate);
    connect(w, &KDecoration3::DecoratedWindow::maximizedHorizontallyChanged, this, &Decoration::recalculate);
    connect(w, &KDecoration3::DecoratedWindow::maximizedVerticallyChanged, this, &Decoration::recalculate);
    connect(w, &KDecoration3::DecoratedWindow::adjacentScreenEdgesChanged, this, &Decoration::recalculate);
    connect(w, &KDecoration3::DecoratedWindow::shadedChanged, this, &Decoration::recalculate);
    connect(w, &KDecoration3::DecoratedWindow::widthChanged, this, &Decoration::recalculate);
    connect(w, &KDecoration3::DecoratedWindow::nextScaleChanged, this, &Decoration::recalculate);
    connect(w, &KDecoration3::DecoratedWindow::captionChanged, this, [this]() {
        update(titleBar());
    });
    connect(w, &KDecoration3::DecoratedWindow::paletteChanged, this, [this]() {
        updateShadow();
        update();
    });
    connect(this, &KDecoration3::Decoration::bordersChanged, this, &Decoration::updateButtonsGeometry);

    setOpaque(true);
    createButtons();
    recalculate();
    updateShadow();
    return true;
}

// ---------------------------------------------------------------------------
bool Decoration::isLeftEdge() const
{
    return window()->isMaximizedHorizontally() || window()->adjacentScreenEdges().testFlag(Qt::LeftEdge);
}
bool Decoration::isRightEdge() const
{
    return window()->isMaximizedHorizontally() || window()->adjacentScreenEdges().testFlag(Qt::RightEdge);
}
bool Decoration::isTopEdge() const
{
    return window()->isMaximizedVertically() || window()->adjacentScreenEdges().testFlag(Qt::TopEdge);
}
bool Decoration::isBottomEdge() const
{
    return window()->isMaximizedVertically() || window()->adjacentScreenEdges().testFlag(Qt::BottomEdge);
}

int Decoration::buttonBox() const
{
    const int box = qRound(QFontMetricsF(settings()->font()).height()) + 4;
    return qMax(18, box + (box % 2));
}

qreal Decoration::titleHeight() const
{
    return buttonBox() + ButtonShadow + 2 * TitlePad;
}

qreal Decoration::topInset() const
{
    return isTopEdge() ? 0 : KDecoration3::snapToPixelGrid(Border, window()->nextScale());
}

// ---------------------------------------------------------------------------
// warna: ikut color scheme (WM section + palette), jadi aman di skema lain
// ---------------------------------------------------------------------------
QColor Decoration::outlineColor() const
{
    const QPalette pal = window()->palette();
    const QColor text = pal.color(QPalette::WindowText);
    if (window()->isActive()) {
        return text;
    }
    const QColor bg = pal.color(QPalette::Window);
    return QColor::fromRgbF(text.redF() * 0.5 + bg.redF() * 0.5, text.greenF() * 0.5 + bg.greenF() * 0.5, text.blueF() * 0.5 + bg.blueF() * 0.5);
}

QColor Decoration::titleBarColor() const
{
    return window()->color(window()->isActive() ? ColorGroup::Active : ColorGroup::Inactive, ColorRole::TitleBar);
}

QColor Decoration::fontColor() const
{
    return window()->color(window()->isActive() ? ColorGroup::Active : ColorGroup::Inactive, ColorRole::Foreground);
}

// warna garis dan simbol: gelap di atas titlebar terang (aktif), samar saat tidak aktif
QColor Decoration::inkColor() const
{
    return window()->isActive() ? Ink : outlineColor();
}

// ---------------------------------------------------------------------------
void Decoration::recalculate()
{
    const qreal sc = window()->nextScale();
    auto snap = [sc](qreal v) {
        return KDecoration3::snapToPixelGrid(v, sc);
    };

    const qreal left = isLeftEdge() ? 0 : snap(Border);
    const qreal right = isRightEdge() ? 0 : snap(Border);
    const qreal bottom = (isBottomEdge() && !window()->isShaded()) ? 0 : snap(Border);
    const qreal top = topInset() + snap(titleHeight()) + snap(Divider);
    setBorders(QMarginsF(left, top, right, bottom));

    // area tarik tambahan di luar jendela supaya bingkai tipis tetap mudah di-resize
    const qreal ext = snap(4);
    setResizeOnlyBorders(QMarginsF(isLeftEdge() ? 0 : ext, isTopEdge() ? 0 : ext, isRightEdge() ? 0 : ext, isBottomEdge() ? 0 : ext));

    setTitleBar(QRectF(left, topInset(), window()->width() - left - right, snap(titleHeight()) + snap(Divider)));
    updateButtonsGeometry();
    updateShadow();
    update();
}

void Decoration::createButtons()
{
    m_leftButtons = new KDecoration3::DecorationButtonGroup(KDecoration3::DecorationButtonGroup::Position::Left, this, &Button::create);
    m_rightButtons = new KDecoration3::DecorationButtonGroup(KDecoration3::DecorationButtonGroup::Position::Right, this, &Button::create);
}

void Decoration::updateButtonsGeometryDelayed()
{
    QMetaObject::invokeMethod(this, &Decoration::updateButtonsGeometry, Qt::QueuedConnection);
}

void Decoration::updateButtonsGeometry()
{
    if (!m_leftButtons || !m_rightButtons) {
        return;
    }
    const qreal sc = window()->nextScale();
    const int box = buttonBox();
    const qreal geom = box + ButtonShadow;

    const auto all = m_leftButtons->buttons() + m_rightButtons->buttons();
    for (KDecoration3::DecorationButton *b : all) {
        const qreal wdt = b->type() == KDecoration3::DecorationButtonType::Menu ? box : geom;
        b->setGeometry(QRectF(0, 0, wdt, geom));
    }

    const qreal y = topInset() + KDecoration3::snapToPixelGrid((titleHeight() - geom) / 2.0, sc);
    m_leftButtons->setSpacing(ButtonGap);
    m_rightButtons->setSpacing(ButtonGap);
    m_leftButtons->setPos(QPointF(borderLeft() + SideMargin, y));
    m_rightButtons->setPos(QPointF(size().width() - borderRight() - SideMargin - m_rightButtons->geometry().width(), y));
    update();
}

QRectF Decoration::captionRect() const
{
    const qreal x0 = m_leftButtons && !m_leftButtons->buttons().isEmpty() ? m_leftButtons->geometry().right() + SideMargin : borderLeft() + SideMargin;
    const qreal x1 = m_rightButtons && !m_rightButtons->buttons().isEmpty() ? m_rightButtons->geometry().left() - SideMargin : size().width() - borderRight() - SideMargin;
    return QRectF(x0, topInset(), qMax<qreal>(0, x1 - x0), titleHeight());
}

// ---------------------------------------------------------------------------
void Decoration::paint(QPainter *painter, const QRectF &repaintRegion)
{
    Q_UNUSED(repaintRegion)
    const qreal w = size().width();
    const qreal h = size().height();
    const qreal L = borderLeft();
    const qreal R = borderRight();
    const qreal T = borderTop();
    const qreal B = borderBottom();
    const QColor outline = outlineColor();

    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setRenderHint(QPainter::Antialiasing, false);

    // bingkai
    painter->fillRect(QRectF(0, 0, w, T), outline);
    painter->fillRect(QRectF(0, T, L, h - T - B), outline);
    painter->fillRect(QRectF(w - R, T, R, h - T - B), outline);
    painter->fillRect(QRectF(0, h - B, w, B), outline);

    // titlebar + garis pemisah
    const QRectF bar(L, topInset(), w - L - R, titleHeight());
    painter->fillRect(bar, titleBarColor());
    painter->fillRect(QRectF(bar.left(), bar.bottom(), bar.width(), Divider), inkColor());

    // judul
    QFont font = settings()->font();
    font.setBold(true);
    painter->setFont(font);
    painter->setPen(fontColor());
    const QRectF cap = captionRect();
    const QString text = QFontMetricsF(font).elidedText(window()->caption(), Qt::ElideRight, cap.width());
    painter->drawText(cap, Qt::AlignVCenter | Qt::AlignLeft | Qt::TextSingleLine, text);
    painter->restore();

    m_leftButtons->paint(painter, repaintRegion);
    m_rightButtons->paint(painter, repaintRegion);
}

// ---------------------------------------------------------------------------
// hard shadow kanan-bawah: tekstur 9-patch, bagian kiri/atas tersembunyi di bawah jendela
// ---------------------------------------------------------------------------
void Decoration::updateShadow()
{
    if (isLeftEdge() || isRightEdge() || isTopEdge() || isBottomEdge()) {
        setShadow(nullptr);
        return;
    }
    const int E = WindowShadow;
    QImage img(2 * E + 1, 2 * E + 1, QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::transparent);
    const QColor c = window()->isActive() ? titleBarColor() : QColor(78, 78, 72);
    QPainter p(&img);
    p.setCompositionMode(QPainter::CompositionMode_Source);
    p.fillRect(QRect(E + 1, E, E, E + 1), c);
    p.fillRect(QRect(E, E + 1, E + 1, E), c);
    p.end();

    auto shadow = std::make_shared<KDecoration3::DecorationShadow>();
    shadow->setPadding(QMarginsF(0, 0, E, E));
    shadow->setInnerShadowRect(QRectF(E, E, 1, 1));
    shadow->setShadow(img);
    setShadow(shadow);
}
}

#include "neobrutaldecoration.moc"
