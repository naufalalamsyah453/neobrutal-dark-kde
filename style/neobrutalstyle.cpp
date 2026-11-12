// Neobrutal Dark - Qt widget style (application style)
// SPDX-License-Identifier: MIT
#include "neobrutalstyle.h"

#include <QAbstractButton>
#include <QAbstractSpinBox>
#include <QApplication>
#include <QComboBox>
#include <QPainter>
#include <QPainterPath>
#include <QScrollBar>
#include <QSettings>
#include <QSlider>
#include <QStandardPaths>
#include <QStyleOption>
#include <QTabBar>

namespace
{
constexpr int BW = 2;        // tebal outline
constexpr int SHADOW = 3;    // offset hard shadow tombol
constexpr int SHADOW_TB = 2; // offset hard shadow tool button
constexpr int ARROW_W = 22;  // lebar area panah combobox
constexpr int SPIN_W = 20;   // lebar tombol spinbox

const QColor INK(18, 18, 18);       // teks di atas warna aksen terang
const QColor CREAM(255, 251, 235);  // teks di atas warna aksen gelap

// luminance 0..1
qreal luma(const QColor &c)
{
    return 0.299 * c.redF() + 0.587 * c.greenF() + 0.114 * c.blueF();
}

// warna teks yang terbaca di atas background c
QColor onColor(const QColor &c)
{
    return luma(c) > 0.55 ? INK : CREAM;
}

void box(QPainter *p, const QRect &r, const QColor &fill, const QColor &line, int bw = BW)
{
    if (r.width() <= 0 || r.height() <= 0) {
        return;
    }
    p->fillRect(r, line);
    if (r.width() > 2 * bw && r.height() > 2 * bw) {
        p->fillRect(r.adjusted(bw, bw, -bw, -bw), fill);
    }
}

void frameRect(QPainter *p, const QRect &r, const QColor &line, int bw = BW)
{
    p->fillRect(QRect(r.left(), r.top(), r.width(), bw), line);
    p->fillRect(QRect(r.left(), r.bottom() - bw + 1, r.width(), bw), line);
    p->fillRect(QRect(r.left(), r.top(), bw, r.height()), line);
    p->fillRect(QRect(r.right() - bw + 1, r.top(), bw, r.height()), line);
}

bool parseRgb(const QVariant &v, QColor *out)
{
    QStringList parts = v.toStringList(); // QSettings memecah nilai "r,g,b"
    if (parts.size() == 1) {
        parts = parts.first().split(QLatin1Char(','));
    }
    if (parts.size() < 3) {
        return false;
    }
    bool ok1, ok2, ok3;
    const int r = parts[0].trimmed().toInt(&ok1);
    const int g = parts[1].trimmed().toInt(&ok2);
    const int b = parts[2].trimmed().toInt(&ok3);
    if (!(ok1 && ok2 && ok3)) {
        return false;
    }
    *out = QColor(r, g, b);
    return true;
}

bool isEnabled(const QStyleOption *o)
{
    return o->state & QStyle::State_Enabled;
}
} // namespace

NeobrutalStyle::NeobrutalStyle()
    : QProxyStyle(QStringLiteral("Fusion"))
    , m_focus(255, 217, 61)
    , m_hover(255, 107, 157)
    , m_link(125, 180, 255)
{
    loadColors();
}

// Ambil warna aksen dari kdeglobals supaya style ikut color scheme yang aktif.
void NeobrutalStyle::loadColors()
{
    const QString path = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + QStringLiteral("/kdeglobals");
    QSettings s(path, QSettings::IniFormat);
    QColor c;
    if (parseRgb(s.value(QStringLiteral("Colors:Window/DecorationFocus")), &c)) {
        m_focus = c;
    }
    if (parseRgb(s.value(QStringLiteral("Colors:Window/DecorationHover")), &c)) {
        m_hover = c;
    }
    if (parseRgb(s.value(QStringLiteral("Colors:Window/ForegroundLink")), &c)) {
        m_link = c;
    }
}

void NeobrutalStyle::polish(QPalette &palette)
{
    QProxyStyle::polish(palette);
    loadColors();
}

void NeobrutalStyle::polish(QWidget *w)
{
    QProxyStyle::polish(w);
    if (qobject_cast<QAbstractButton *>(w) || qobject_cast<QComboBox *>(w) || qobject_cast<QAbstractSpinBox *>(w) || qobject_cast<QScrollBar *>(w)
        || qobject_cast<QSlider *>(w) || qobject_cast<QTabBar *>(w)) {
        w->setAttribute(Qt::WA_Hover, true);
    }
}

void NeobrutalStyle::unpolish(QWidget *w)
{
    if (qobject_cast<QAbstractButton *>(w) || qobject_cast<QComboBox *>(w) || qobject_cast<QAbstractSpinBox *>(w) || qobject_cast<QScrollBar *>(w)
        || qobject_cast<QSlider *>(w) || qobject_cast<QTabBar *>(w)) {
        w->setAttribute(Qt::WA_Hover, false);
    }
    QProxyStyle::unpolish(w);
}

// ---------------------------------------------------------------------------
// Tombol timbul: shadow keras di kanan-bawah, ditekan = bergeser + warna fokus
// ---------------------------------------------------------------------------
void NeobrutalStyle::drawButtonFace(QPainter *p, const QStyleOption *o, int shadow, bool hideWhenIdle) const
{
    const bool en = isEnabled(o);
    const bool down = en && (o->state & State_Sunken);
    const bool on = en && (o->state & State_On);
    const bool hov = en && (o->state & State_MouseOver);
    const bool focus = en && (o->state & State_HasFocus);

    if (hideWhenIdle && !(down || on || hov)) {
        return;
    }

    const QColor ink = o->palette.color(QPalette::ButtonText);
    const QColor fill = o->palette.color(QPalette::Button);
    QRect face = o->rect.adjusted(0, 0, -shadow, -shadow);

    p->save();
    if (!en) {
        box(p, face, fill, ink);
    } else if (down || on) {
        face.translate(shadow, shadow);
        box(p, face, m_focus, ink);
    } else {
        p->fillRect(face.translated(shadow, shadow), hov ? m_hover : m_focus);
        box(p, face, hov ? fill.lighter(140) : fill, ink);
        if (focus) {
            p->fillRect(QRect(face.left() + BW + 1, face.top() + BW + 1, face.width() - 2 * (BW + 1), 1), m_focus);
            p->fillRect(QRect(face.left() + BW + 1, face.bottom() - BW - 1, face.width() - 2 * (BW + 1), 1), m_focus);
            p->fillRect(QRect(face.left() + BW + 1, face.top() + BW + 1, 1, face.height() - 2 * (BW + 1)), m_focus);
            p->fillRect(QRect(face.right() - BW - 1, face.top() + BW + 1, 1, face.height() - 2 * (BW + 1)), m_focus);
        }
    }
    p->restore();
}

void NeobrutalStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    switch (element) {
    case PE_FrameFocusRect:
        return; // fokus ditandai lewat warna, bukan garis putus-putus

    case PE_PanelButtonCommand: {
        const auto *b = qstyleoption_cast<const QStyleOptionButton *>(opt);
        const bool flat = b && (b->features & QStyleOptionButton::Flat);
        drawButtonFace(p, opt, SHADOW, flat);
        return;
    }
    case PE_PanelButtonTool:
        drawButtonFace(p, opt, SHADOW_TB, opt->state & State_AutoRaise);
        return;

    case PE_PanelLineEdit:
    case PE_FrameLineEdit: {
        const auto *f = qstyleoption_cast<const QStyleOptionFrame *>(opt);
        const QWidget *parent = w ? w->parentWidget() : nullptr;
        if (parent && (qobject_cast<const QAbstractSpinBox *>(parent) || qobject_cast<const QComboBox *>(parent))) {
            p->fillRect(opt->rect, opt->palette.brush(QPalette::Base));
            return;
        }
        if (f && f->lineWidth == 0) {
            p->fillRect(opt->rect, opt->palette.brush(QPalette::Base));
            return;
        }
        QColor line = opt->palette.color(QPalette::Text);
        if (isEnabled(opt)) {
            if (opt->state & State_HasFocus) {
                line = m_focus;
            } else if (opt->state & State_MouseOver) {
                line = m_hover;
            }
        }
        box(p, opt->rect, opt->palette.color(QPalette::Base), line);
        return;
    }
    case PE_Frame:
    case PE_FrameGroupBox:
    case PE_FrameTabWidget:
    case PE_FrameMenu:
        frameRect(p, opt->rect, opt->palette.color(QPalette::WindowText));
        return;

    case PE_PanelMenu:
        box(p, opt->rect, opt->palette.color(QPalette::Window), opt->palette.color(QPalette::WindowText));
        return;

    case PE_PanelTipLabel:
        box(p, opt->rect, opt->palette.color(QPalette::ToolTipBase), opt->palette.color(QPalette::ToolTipText));
        return;

    case PE_IndicatorCheckBox:
    case PE_IndicatorItemViewItemCheck: {
        const bool en = isEnabled(opt);
        const bool checked = en && (opt->state & (State_On | State_NoChange));
        QRect r(0, 0, 16, 16);
        r.moveCenter(opt->rect.center());
        QColor line = opt->palette.color(QPalette::WindowText);
        if (en && (opt->state & State_MouseOver)) {
            line = m_hover;
        }
        const QColor fill = checked ? m_focus : opt->palette.color(QPalette::Base);
        box(p, r, fill, line);
        if (checked) {
            const QColor mark = onColor(m_focus);
            if (opt->state & State_NoChange) {
                p->fillRect(QRect(r.center().x() - 3, r.center().y(), 7, 2), mark);
            } else {
                p->save();
                p->setRenderHint(QPainter::Antialiasing);
                QPen pen(mark, 2.2, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
                p->setPen(pen);
                p->setBrush(Qt::NoBrush);
                const QPointF o = r.topLeft();
                QPolygonF poly;
                poly << o + QPointF(4, 8.5) << o + QPointF(6.8, 11.3) << o + QPointF(12, 4.8);
                p->drawPolyline(poly);
                p->restore();
            }
        }
        return;
    }
    case PE_IndicatorRadioButton: {
        const bool en = isEnabled(opt);
        const bool checked = en && (opt->state & State_On);
        QRect r(0, 0, 16, 16);
        r.moveCenter(opt->rect.center());
        QColor line = opt->palette.color(QPalette::WindowText);
        if (en && (opt->state & State_MouseOver)) {
            line = m_hover;
        }
        p->save();
        p->setRenderHint(QPainter::Antialiasing);
        p->setPen(Qt::NoPen);
        p->setBrush(line);
        p->drawEllipse(r);
        p->setBrush(checked ? m_focus : opt->palette.color(QPalette::Base));
        p->drawEllipse(r.adjusted(BW, BW, -BW, -BW));
        if (checked) {
            p->setBrush(onColor(m_focus));
            p->drawEllipse(r.adjusted(5, 5, -5, -5));
        }
        p->restore();
        return;
    }
    case PE_PanelItemViewItem: {
        const auto *vi = qstyleoption_cast<const QStyleOptionViewItem *>(opt);
        if (vi && (vi->features & QStyleOptionViewItem::Alternate)) {
            p->fillRect(opt->rect, opt->palette.brush(QPalette::AlternateBase));
        }
        if (vi && vi->backgroundBrush.style() != Qt::NoBrush) {
            p->fillRect(opt->rect, vi->backgroundBrush);
        }
        if (isEnabled(opt) && (opt->state & State_Selected)) {
            p->fillRect(opt->rect, opt->palette.color(QPalette::Highlight));
        } else if (isEnabled(opt) && (opt->state & State_MouseOver)) {
            QColor c = m_hover;
            c.setAlpha(80);
            p->fillRect(opt->rect, c);
        }
        return;
    }
    default:
        break;
    }
    QProxyStyle::drawPrimitive(element, opt, p, w);
}

// ---------------------------------------------------------------------------
QRect NeobrutalStyle::progressFill(const QStyleOptionProgressBar *pb, const QRect &contents) const
{
    const qint64 range = qint64(pb->maximum) - pb->minimum;
    if (range <= 0) {
        return QRect();
    }
    const qreal v = qBound<qreal>(0, qreal(qint64(pb->progress) - pb->minimum) / range, 1);
    const bool horizontal = pb->state & State_Horizontal;
    QRect r = contents;
    if (horizontal) {
        r.setWidth(int(contents.width() * v));
        if ((pb->direction == Qt::RightToLeft) != pb->invertedAppearance) {
            r.moveRight(contents.right());
        }
    } else {
        r.setHeight(int(contents.height() * v));
        if (!pb->invertedAppearance) {
            r.moveBottom(contents.bottom());
        }
    }
    return r;
}

void NeobrutalStyle::drawControl(ControlElement element, const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    switch (element) {
    case CE_PushButtonBevel:
        drawPrimitive(PE_PanelButtonCommand, opt, p, w);
        return;

    case CE_PushButton: {
        const auto *b = qstyleoption_cast<const QStyleOptionButton *>(opt);
        if (!b) {
            break;
        }
        drawControl(CE_PushButtonBevel, opt, p, w);
        QStyleOptionButton sub(*b);
        sub.rect = subElementRect(SE_PushButtonContents, opt, w);
        const bool active = isEnabled(opt) && (b->state & (State_Sunken | State_On));
        const bool flatIdle = (b->features & QStyleOptionButton::Flat) && !active && !(b->state & State_MouseOver);
        if (active) {
            sub.rect.translate(SHADOW, SHADOW);
            sub.palette.setColor(QPalette::ButtonText, onColor(m_focus));
            sub.palette.setColor(QPalette::WindowText, onColor(m_focus));
        }
        Q_UNUSED(flatIdle)
        drawControl(CE_PushButtonLabel, &sub, p, w);
        return;
    }
    case CE_ToolButtonLabel: {
        if (isEnabled(opt) && (opt->state & (State_Sunken | State_On))) {
            QStyleOptionToolButton tb(*qstyleoption_cast<const QStyleOptionToolButton *>(opt));
            tb.palette.setColor(QPalette::ButtonText, onColor(m_focus));
            tb.palette.setColor(QPalette::WindowText, onColor(m_focus));
            QProxyStyle::drawControl(element, &tb, p, w);
            return;
        }
        break;
    }
    case CE_TabBarTabShape: {
        const auto *tab = qstyleoption_cast<const QStyleOptionTab *>(opt);
        if (!tab) {
            break;
        }
        const bool sel = tab->state & State_Selected;
        const bool hov = isEnabled(opt) && (tab->state & State_MouseOver);
        const QColor line = tab->palette.color(QPalette::WindowText);
        QColor fill = tab->palette.color(QPalette::Button);
        if (sel) {
            fill = m_focus;
        } else if (hov) {
            fill = fill.lighter(140);
        }
        const QRect r = tab->rect;
        box(p, r, fill, line);
        if (sel) {
            // buka garis panel di sisi tab terpilih supaya tab menyatu dengan panelnya
            switch (tab->shape) {
            case QTabBar::RoundedNorth:
            case QTabBar::TriangularNorth:
                p->fillRect(QRect(r.left() + BW, r.bottom() - BW + 1, r.width() - 2 * BW, BW), fill);
                break;
            case QTabBar::RoundedSouth:
            case QTabBar::TriangularSouth:
                p->fillRect(QRect(r.left() + BW, r.top(), r.width() - 2 * BW, BW), fill);
                break;
            case QTabBar::RoundedWest:
            case QTabBar::TriangularWest:
                p->fillRect(QRect(r.right() - BW + 1, r.top() + BW, BW, r.height() - 2 * BW), fill);
                break;
            case QTabBar::RoundedEast:
            case QTabBar::TriangularEast:
                p->fillRect(QRect(r.left(), r.top() + BW, BW, r.height() - 2 * BW), fill);
                break;
            }
        }
        return;
    }
    case CE_TabBarTabLabel: {
        const auto *tab = qstyleoption_cast<const QStyleOptionTab *>(opt);
        if (tab && (tab->state & State_Selected) && isEnabled(opt)) {
            QStyleOptionTab t(*tab);
            t.state &= ~State_MouseOver;
            t.palette.setColor(QPalette::WindowText, onColor(m_focus));
            t.palette.setColor(QPalette::ButtonText, onColor(m_focus));
            t.palette.setColor(QPalette::Text, onColor(m_focus));
            QProxyStyle::drawControl(element, &t, p, w);
            return;
        }
        break;
    }
    case CE_HeaderSection: {
        const QColor line = opt->palette.color(QPalette::WindowText);
        QColor fill = opt->palette.color(QPalette::Button);
        if (isEnabled(opt) && (opt->state & State_Sunken)) {
            fill = m_focus;
        } else if (isEnabled(opt) && (opt->state & State_MouseOver)) {
            fill = fill.lighter(140);
        }
        p->fillRect(opt->rect, fill);
        p->fillRect(QRect(opt->rect.left(), opt->rect.bottom() - BW + 1, opt->rect.width(), BW), line);
        p->fillRect(QRect(opt->rect.right() - BW + 1, opt->rect.top(), BW, opt->rect.height()), line);
        return;
    }
    case CE_ProgressBarGroove:
        box(p, opt->rect, opt->palette.color(QPalette::Base), opt->palette.color(QPalette::WindowText));
        return;

    case CE_ProgressBarContents: {
        const auto *pb = qstyleoption_cast<const QStyleOptionProgressBar *>(opt);
        if (!pb || pb->minimum == pb->maximum) {
            break; // mode sibuk: biarkan Fusion
        }
        // Fusion memberi rect penuh (= groove), jadi inset dihitung di sini
        const QRect contents = subElementRect(SE_ProgressBarContents, pb, w);
        p->fillRect(progressFill(pb, contents), isEnabled(opt) ? m_link : opt->palette.color(QPalette::Mid));
        return;
    }
    case CE_ProgressBarLabel: {
        const auto *pb = qstyleoption_cast<const QStyleOptionProgressBar *>(opt);
        if (!pb || !pb->textVisible || pb->text.isEmpty()) {
            return;
        }
        const QRect contents = subElementRect(SE_ProgressBarContents, pb, w);
        const QRect fill = pb->minimum == pb->maximum ? QRect() : progressFill(pb, contents);
        p->save();
        p->setClipRegion(QRegion(pb->rect) - QRegion(fill));
        drawItemText(p, pb->rect, Qt::AlignCenter | Qt::TextSingleLine, pb->palette, isEnabled(pb), pb->text, QPalette::WindowText);
        p->setClipRect(fill);
        QPalette pal = pb->palette;
        pal.setColor(QPalette::WindowText, onColor(m_link));
        drawItemText(p, pb->rect, Qt::AlignCenter | Qt::TextSingleLine, pal, isEnabled(pb), pb->text, QPalette::WindowText);
        p->restore();
        return;
    }
    default:
        break;
    }
    QProxyStyle::drawControl(element, opt, p, w);
}

// ---------------------------------------------------------------------------
void NeobrutalStyle::drawComplexControl(ComplexControl cc, const QStyleOptionComplex *opt, QPainter *p, const QWidget *w) const
{
    switch (cc) {
    case CC_ScrollBar: {
        const auto *sb = qstyleoption_cast<const QStyleOptionSlider *>(opt);
        if (!sb) {
            break;
        }
        const bool horiz = sb->orientation == Qt::Horizontal;
        const bool en = isEnabled(sb);
        const QColor line = sb->palette.color(QPalette::WindowText);
        p->fillRect(sb->rect, sb->palette.color(QPalette::Base));
        // garis di sisi dalam (menghadap konten)
        if (horiz) {
            p->fillRect(QRect(sb->rect.left(), sb->rect.top(), sb->rect.width(), BW), line);
        } else {
            p->fillRect(QRect(sb->rect.left(), sb->rect.top(), BW, sb->rect.height()), line);
        }

        QRect slider = subControlRect(CC_ScrollBar, sb, SC_ScrollBarSlider, w);
        slider = horiz ? slider.adjusted(0, BW + 1, 0, -1) : slider.adjusted(BW + 1, 0, -1, 0);
        const bool sliderActive = en && (sb->activeSubControls & SC_ScrollBarSlider);
        const bool sliderDown = sliderActive && (sb->state & State_Sunken);
        if (en && slider.isValid() && sb->maximum > sb->minimum) {
            const QColor fill = sliderDown ? m_focus : (sliderActive ? m_hover : sb->palette.color(QPalette::Button).lighter(130));
            box(p, slider, fill, line);
        }

        struct Btn { SubControl sc; PrimitiveElement arrow; };
        const Btn btns[2] = {{SC_ScrollBarSubLine, horiz ? PE_IndicatorArrowLeft : PE_IndicatorArrowUp},
                             {SC_ScrollBarAddLine, horiz ? PE_IndicatorArrowRight : PE_IndicatorArrowDown}};
        for (const Btn &b : btns) {
            QRect r = subControlRect(CC_ScrollBar, sb, b.sc, w);
            if (!r.isValid()) {
                continue;
            }
            QStyleOption ao;
            ao.QStyleOption::operator=(*sb);
            ao.rect = horiz ? r.adjusted(0, BW, 0, 0) : r.adjusted(BW, 0, 0, 0);
            const bool act = en && (sb->activeSubControls & b.sc);
            if (act && (sb->state & State_Sunken)) {
                p->fillRect(ao.rect, m_focus);
                ao.palette.setColor(QPalette::ButtonText, onColor(m_focus));
            } else if (act) {
                p->fillRect(ao.rect, m_hover);
                ao.palette.setColor(QPalette::ButtonText, onColor(m_hover));
            }
            ao.rect = ao.rect.adjusted(2, 2, -2, -2);
            drawPrimitive(b.arrow, &ao, p, w);
        }
        return;
    }
    case CC_Slider: {
        const auto *sl = qstyleoption_cast<const QStyleOptionSlider *>(opt);
        if (!sl) {
            break;
        }
        const bool horiz = sl->orientation == Qt::Horizontal;
        const bool en = isEnabled(sl);
        const QColor line = sl->palette.color(QPalette::WindowText);
        if (sl->subControls & SC_SliderGroove) {
            const QRect gr = subControlRect(CC_Slider, sl, SC_SliderGroove, w);
            QRect g = horiz ? QRect(gr.left(), gr.center().y() - 3, gr.width(), 7) : QRect(gr.center().x() - 3, gr.top(), 7, gr.height());
            box(p, g, sl->palette.color(QPalette::Base), line);
            // bagian terisi sampai handle
            const QRect hr = subControlRect(CC_Slider, sl, SC_SliderHandle, w);
            QRect gi = g.adjusted(BW, BW, -BW, -BW);
            if (horiz) {
                gi.setRight(hr.center().x());
                if (sl->upsideDown) {
                    gi = g.adjusted(BW, BW, -BW, -BW);
                    gi.setLeft(hr.center().x());
                }
            } else {
                gi.setBottom(hr.center().y());
                if (!sl->upsideDown) {
                    gi = g.adjusted(BW, BW, -BW, -BW);
                    gi.setTop(hr.center().y());
                }
            }
            if (en) {
                p->fillRect(gi, m_link);
            }
        }
        if (sl->subControls & SC_SliderTickmarks) {
            QStyleOptionSlider t(*sl);
            t.subControls = SC_SliderTickmarks;
            QProxyStyle::drawComplexControl(cc, &t, p, w);
        }
        if (sl->subControls & SC_SliderHandle) {
            QRect hr = subControlRect(CC_Slider, sl, SC_SliderHandle, w);
            const bool act = en && (sl->activeSubControls & SC_SliderHandle);
            const bool down = act && (sl->state & State_Sunken);
            QColor fill = en ? m_focus : sl->palette.color(QPalette::Button);
            if (down) {
                fill = m_hover;
            } else if (act) {
                fill = m_focus.lighter(115);
            }
            box(p, hr, fill, line);
        }
        return;
    }
    case CC_SpinBox: {
        const auto *sb = qstyleoption_cast<const QStyleOptionSpinBox *>(opt);
        if (!sb) {
            break;
        }
        const bool en = isEnabled(sb);
        QColor line = sb->palette.color(QPalette::Text);
        if (en && (sb->state & State_HasFocus)) {
            line = m_focus;
        } else if (en && (sb->state & State_MouseOver)) {
            line = m_hover;
        }
        if (sb->frame) {
            box(p, sb->rect, sb->palette.color(QPalette::Base), line);
        } else {
            p->fillRect(sb->rect, sb->palette.color(QPalette::Base));
        }
        if (sb->buttonSymbols == QAbstractSpinBox::NoButtons) {
            return;
        }
        struct Btn { SubControl sc; PrimitiveElement arrow; bool enabledStep; };
        const Btn btns[2] = {{SC_SpinBoxUp, PE_IndicatorSpinUp, bool(sb->stepEnabled & QAbstractSpinBox::StepUpEnabled)},
                             {SC_SpinBoxDown, PE_IndicatorSpinDown, bool(sb->stepEnabled & QAbstractSpinBox::StepDownEnabled)}};
        for (const Btn &b : btns) {
            const QRect r = subControlRect(CC_SpinBox, sb, b.sc, w);
            QStyleOption ao;
            ao.QStyleOption::operator=(*sb);
            QColor fill = sb->palette.color(QPalette::Button);
            const bool act = en && b.enabledStep && (sb->activeSubControls & b.sc);
            if (act && (sb->state & State_Sunken)) {
                fill = m_focus;
                ao.palette.setColor(QPalette::ButtonText, onColor(m_focus));
            } else if (act) {
                fill = m_hover;
                ao.palette.setColor(QPalette::ButtonText, onColor(m_hover));
            }
            p->fillRect(r, fill);
            ao.rect = r.adjusted(0, 0, 0, 0);
            if (!(en && b.enabledStep)) {
                ao.state &= ~State_Enabled;
            }
            drawPrimitive(b.arrow, &ao, p, w);
        }
        const QRect up = subControlRect(CC_SpinBox, sb, SC_SpinBoxUp, w);
        const QRect dn = subControlRect(CC_SpinBox, sb, SC_SpinBoxDown, w);
        p->fillRect(QRect(up.left() - BW, up.top(), BW, up.height() + dn.height()), line);
        p->fillRect(QRect(up.left(), up.bottom() + 1 - BW / 2, up.width(), BW), line);
        return;
    }
    case CC_ComboBox: {
        const auto *cb = qstyleoption_cast<const QStyleOptionComboBox *>(opt);
        if (!cb) {
            break;
        }
        const bool en = isEnabled(cb);
        const bool hov = en && (cb->state & State_MouseOver);
        const bool open = en && (cb->state & State_On);
        const bool focus = en && (cb->state & State_HasFocus);
        const QRect face = subControlRect(CC_ComboBox, cb, SC_ComboBoxFrame, w);
        const QRect arrow = subControlRect(CC_ComboBox, cb, SC_ComboBoxArrow, w);
        const QColor ink = cb->palette.color(QPalette::ButtonText);
        QStyleOption ao;
        ao.QStyleOption::operator=(*cb);
        ao.rect = arrow;
        QColor arrowFill = cb->palette.color(QPalette::Button);
        if (cb->editable) {
            QColor line = cb->palette.color(QPalette::Text);
            if (focus) {
                line = m_focus;
            } else if (hov) {
                line = m_hover;
            }
            box(p, face, cb->palette.color(QPalette::Base), line);
            if (open) {
                arrowFill = m_focus;
            }
            p->fillRect(arrow, arrowFill);
            p->fillRect(QRect(arrow.left() - BW, arrow.top(), BW, arrow.height()), line);
            if (open) {
                ao.palette.setColor(QPalette::ButtonText, onColor(m_focus));
            }
        } else if (!en) {
            box(p, face, cb->palette.color(QPalette::Button), ink);
            p->fillRect(QRect(arrow.left() - BW, arrow.top(), BW, arrow.height()), ink);
        } else if (open || (cb->state & State_Sunken)) {
            const QRect f = face.translated(SHADOW, SHADOW);
            box(p, f, m_focus, ink);
            p->fillRect(QRect(arrow.left() + SHADOW - BW, arrow.top() + SHADOW, BW, arrow.height()), ink);
            ao.rect = arrow.translated(SHADOW, SHADOW);
            ao.palette.setColor(QPalette::ButtonText, onColor(m_focus));
        } else {
            p->fillRect(face.translated(SHADOW, SHADOW), hov ? m_hover : m_focus);
            box(p, face, hov ? cb->palette.color(QPalette::Button).lighter(140) : cb->palette.color(QPalette::Button), ink);
            p->fillRect(QRect(arrow.left() - BW, arrow.top(), BW, arrow.height()), ink);
        }
        ao.rect = ao.rect.adjusted(0, 0, 0, 0);
        drawPrimitive(PE_IndicatorArrowDown, &ao, p, w);
        return;
    }
    default:
        break;
    }
    QProxyStyle::drawComplexControl(cc, opt, p, w);
}

// ---------------------------------------------------------------------------
int NeobrutalStyle::pixelMetric(PixelMetric m, const QStyleOption *opt, const QWidget *w) const
{
    switch (m) {
    case PM_DefaultFrameWidth:
    case PM_ComboBoxFrameWidth:
    case PM_SpinBoxFrameWidth:
    case PM_MenuPanelWidth:
    case PM_ToolTipLabelFrameWidth:
        return BW;
    case PM_ScrollBarExtent:
        return 16;
    case PM_ScrollBarSliderMin:
        return 28;
    case PM_SliderThickness:
    case PM_SliderControlThickness:
        return 20;
    case PM_SliderLength:
        return 18;
    case PM_IndicatorWidth:
    case PM_IndicatorHeight:
    case PM_ExclusiveIndicatorWidth:
    case PM_ExclusiveIndicatorHeight:
        return 16;
    case PM_ButtonShiftHorizontal:
    case PM_ButtonShiftVertical:
    case PM_TabBarTabShiftHorizontal:
    case PM_TabBarTabShiftVertical:
        return 0;
    case PM_TabBarBaseOverlap:
    case PM_TabBarTabOverlap:
        return BW;
    default:
        break;
    }
    return QProxyStyle::pixelMetric(m, opt, w);
}

QSize NeobrutalStyle::sizeFromContents(ContentsType t, const QStyleOption *opt, const QSize &sz, const QWidget *w) const
{
    QSize s = QProxyStyle::sizeFromContents(t, opt, sz, w);
    switch (t) {
    case CT_PushButton:
        s += QSize(SHADOW + 6, SHADOW + 2);
        s.setHeight(qMax(s.height(), 30 + SHADOW));
        break;
    case CT_ComboBox:
        s += QSize(SHADOW + 4, SHADOW + 2);
        s.setHeight(qMax(s.height(), 30 + SHADOW));
        break;
    case CT_ToolButton:
        s += QSize(SHADOW_TB + 4, SHADOW_TB + 4);
        break;
    case CT_LineEdit:
        s.setHeight(qMax(s.height(), 30));
        break;
    case CT_SpinBox:
        s.setHeight(qMax(s.height(), 30));
        s.rwidth() += SPIN_W;
        break;
    case CT_TabBarTab:
        s += QSize(8, 4);
        break;
    default:
        break;
    }
    return s;
}

QRect NeobrutalStyle::subElementRect(SubElement e, const QStyleOption *opt, const QWidget *w) const
{
    switch (e) {
    case SE_PushButtonContents:
    case SE_PushButtonFocusRect:
        return opt->rect.adjusted(BW, BW, -SHADOW - BW, -SHADOW - BW);
    case SE_ProgressBarGroove:
    case SE_ProgressBarLabel:
        return opt->rect;
    case SE_ProgressBarContents:
        return (w ? w->rect() : opt->rect).adjusted(BW + 2, BW + 2, -(BW + 2), -(BW + 2));
    default:
        break;
    }
    return QProxyStyle::subElementRect(e, opt, w);
}

QRect NeobrutalStyle::subControlRect(ComplexControl cc, const QStyleOptionComplex *opt, SubControl sc, const QWidget *w) const
{
    switch (cc) {
    case CC_SpinBox: {
        const auto *sb = qstyleoption_cast<const QStyleOptionSpinBox *>(opt);
        if (!sb) {
            break;
        }
        const QRect inner = sb->rect.adjusted(BW, BW, -BW, -BW);
        const bool buttons = sb->buttonSymbols != QAbstractSpinBox::NoButtons;
        const int bw = buttons ? SPIN_W : 0;
        const int half = inner.height() / 2;
        switch (sc) {
        case SC_SpinBoxUp:
            return QRect(inner.right() - bw + 1, inner.top(), bw, half);
        case SC_SpinBoxDown:
            return QRect(inner.right() - bw + 1, inner.top() + half, bw, inner.height() - half);
        case SC_SpinBoxEditField:
            return QRect(inner.left(), inner.top(), inner.width() - (buttons ? bw + BW : 0), inner.height());
        case SC_SpinBoxFrame:
            return sb->rect;
        default:
            break;
        }
        break;
    }
    case CC_ComboBox: {
        const auto *cb = qstyleoption_cast<const QStyleOptionComboBox *>(opt);
        if (!cb) {
            break;
        }
        const QRect face = cb->rect.adjusted(0, 0, -SHADOW, -SHADOW);
        const QRect inner = face.adjusted(BW, BW, -BW, -BW);
        switch (sc) {
        case SC_ComboBoxFrame:
            return face;
        case SC_ComboBoxArrow:
            return QRect(inner.right() - ARROW_W + 1, inner.top(), ARROW_W, inner.height());
        case SC_ComboBoxEditField: {
            QRect r(inner.left(), inner.top(), inner.width() - ARROW_W - BW, inner.height());
            return cb->editable ? r : r.adjusted(6, 0, 0, 0);
        }
        case SC_ComboBoxListBoxPopup:
            return QProxyStyle::subControlRect(cc, opt, sc, w);
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
    return QProxyStyle::subControlRect(cc, opt, sc, w);
}

int NeobrutalStyle::styleHint(StyleHint h, const QStyleOption *opt, const QWidget *w, QStyleHintReturn *ret) const
{
    switch (h) {
    case SH_EtchDisabledText:
    case SH_DitherDisabledText:
        return 0;
    default:
        break;
    }
    return QProxyStyle::styleHint(h, opt, w, ret);
}
