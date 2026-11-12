// SPDX-License-Identifier: MIT
#pragma once

#include <QStylePlugin>

class NeobrutalStylePlugin : public QStylePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QStyleFactoryInterface_iid FILE "neobrutal.json")
public:
    QStyle *create(const QString &key) override;
};
