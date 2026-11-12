// Render widget gallery ke PNG, untuk mengecek style tanpa login ulang.
// Pakai: QT_QPA_PLATFORM=offscreen ./neobrutal-preview out.png [scheme.colors]
// SPDX-License-Identifier: MIT
#include "../style/neobrutalstyle.h"

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QFile>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QSpinBox>
#include <QSettings>
#include <QTabWidget>
#include <QToolButton>
#include <QTreeWidget>
#include <QHeaderView>
#include <QPalette>

static QColor rgb(QSettings &s, const QString &key)
{
    const QStringList l = s.value(key).toStringList();
    return l.size() >= 3 ? QColor(l[0].toInt(), l[1].toInt(), l[2].toInt()) : QColor();
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    const QString out = argc > 1 ? argv[1] : QStringLiteral("preview-style.png");
    const QString scheme = argc > 2 ? argv[2] : QString();

    QPalette pal;
    if (!scheme.isEmpty()) {
        QSettings s(scheme, QSettings::IniFormat);
        pal.setColor(QPalette::Window, rgb(s, "Colors:Window/BackgroundNormal"));
        pal.setColor(QPalette::WindowText, rgb(s, "Colors:Window/ForegroundNormal"));
        pal.setColor(QPalette::Base, rgb(s, "Colors:View/BackgroundNormal"));
        pal.setColor(QPalette::AlternateBase, rgb(s, "Colors:View/BackgroundAlternate"));
        pal.setColor(QPalette::Text, rgb(s, "Colors:View/ForegroundNormal"));
        pal.setColor(QPalette::Button, rgb(s, "Colors:Button/BackgroundNormal"));
        pal.setColor(QPalette::ButtonText, rgb(s, "Colors:Button/ForegroundNormal"));
        pal.setColor(QPalette::Highlight, rgb(s, "Colors:Selection/BackgroundNormal"));
        pal.setColor(QPalette::HighlightedText, rgb(s, "Colors:Selection/ForegroundNormal"));
        pal.setColor(QPalette::ToolTipBase, rgb(s, "Colors:Tooltip/BackgroundNormal"));
        pal.setColor(QPalette::ToolTipText, rgb(s, "Colors:Tooltip/ForegroundNormal"));
        pal.setColor(QPalette::Disabled, QPalette::WindowText, QColor(120, 116, 100));
        pal.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(120, 116, 100));
        pal.setColor(QPalette::Disabled, QPalette::Text, QColor(120, 116, 100));
    }
    auto *style = new NeobrutalStyle;
    QApplication::setStyle(style);
    if (!scheme.isEmpty()) {
        QApplication::setPalette(pal);
    }

    QWidget win;
    auto *g = new QGridLayout(&win);
    g->setSpacing(12);

    auto *b1 = new QPushButton("Button");
    auto *b2 = new QPushButton("Pressed");
    b2->setDown(true);
    auto *b3 = new QPushButton("Toggled");
    b3->setCheckable(true);
    b3->setChecked(true);
    auto *b4 = new QPushButton("Disabled");
    b4->setEnabled(false);
    auto *b5 = new QPushButton("Flat");
    b5->setFlat(true);
    auto *row = new QHBoxLayout;
    for (auto *b : {b1, b2, b3, b4, b5}) row->addWidget(b);
    g->addLayout(row, 0, 0, 1, 2);

    auto *le = new QLineEdit("Line edit");
    auto *le2 = new QLineEdit;
    le2->setPlaceholderText("Focused");
    auto *cb = new QComboBox;
    cb->addItems({"Combo box", "Item 2"});
    auto *cb2 = new QComboBox;
    cb2->setEditable(true);
    cb2->addItems({"Editable", "Item 2"});
    auto *sp = new QSpinBox;
    sp->setValue(42);
    g->addWidget(le, 1, 0);
    g->addWidget(le2, 1, 1);
    g->addWidget(cb, 2, 0);
    g->addWidget(cb2, 2, 1);
    g->addWidget(sp, 3, 0);

    auto *chk = new QVBoxLayout;
    auto *c1 = new QCheckBox("Unchecked");
    auto *c2 = new QCheckBox("Checked");
    c2->setChecked(true);
    auto *c3 = new QCheckBox("Partial");
    c3->setTristate(true);
    c3->setCheckState(Qt::PartiallyChecked);
    auto *c4 = new QCheckBox("Disabled");
    c4->setEnabled(false);
    auto *r1 = new QRadioButton("Radio");
    auto *r2 = new QRadioButton("Radio on");
    r2->setChecked(true);
    for (QWidget *x : {(QWidget *)c1, (QWidget *)c2, (QWidget *)c3, (QWidget *)c4, (QWidget *)r1, (QWidget *)r2}) chk->addWidget(x);
    g->addLayout(chk, 3, 1, 2, 1);

    auto *sl = new QSlider(Qt::Horizontal);
    sl->setValue(35);
    sl->setTickPosition(QSlider::TicksBelow);
    auto *pb = new QProgressBar;
    pb->setValue(40);
    auto *pb2 = new QProgressBar;
    pb2->setValue(75);
    pb2->setEnabled(true);
    g->addWidget(sl, 4, 0);
    g->addWidget(pb, 5, 0);
    g->addWidget(pb2, 5, 1);

    auto *tabs = new QTabWidget;
    auto *lw = new QListWidget;
    for (int i = 0; i < 12; ++i) lw->addItem(QString("List item %1").arg(i + 1));
    lw->setCurrentRow(1);
    auto *tw = new QTreeWidget;
    tw->setHeaderLabels({"Name", "Size"});
    for (int i = 0; i < 4; ++i) tw->addTopLevelItem(new QTreeWidgetItem({QString("Row %1").arg(i), "1 KB"}));
    auto *gb = new QGroupBox("Group box");
    auto *gl = new QVBoxLayout(gb);
    auto *tb = new QToolButton;
    tb->setText("Tool");
    auto *tb2 = new QToolButton;
    tb2->setText("On");
    tb2->setCheckable(true);
    tb2->setChecked(true);
    auto *tbRow = new QHBoxLayout;
    tbRow->addWidget(tb);
    tbRow->addWidget(tb2);
    tbRow->addStretch();
    gl->addLayout(tbRow);
    gl->addWidget(new QLabel("Label inside group"));
    tabs->addTab(lw, "List");
    tabs->addTab(tw, "Tree");
    tabs->addTab(gb, "Group");
    tabs->addTab(new QWidget, "Four");
    g->addWidget(tabs, 0, 2, 6, 1);

    win.resize(820, 420);
    win.show();
    app.processEvents();
    win.grab().save(out);

    QMenu menu;
    menu.addAction("Open");
    menu.addAction("Save");
    menu.addSeparator();
    auto *chkAct = menu.addAction("Checkable");
    chkAct->setCheckable(true);
    chkAct->setChecked(true);
    menu.addAction("Quit");
    menu.ensurePolished();
    menu.resize(menu.sizeHint());
    menu.show();
    app.processEvents();
    menu.grab().save(out.left(out.lastIndexOf('.')) + "-menu.png");
    return 0;
}
