// SPDX-License-Identifier: MIT
#include "neobrutalstyleplugin.h"
#include "neobrutalstyle.h"

QStyle *NeobrutalStylePlugin::create(const QString &key)
{
    if (key.compare(QLatin1String("neobrutal"), Qt::CaseInsensitive) == 0) {
        return new NeobrutalStyle;
    }
    return nullptr;
}
