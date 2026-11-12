#!/usr/bin/env bash
# Menghapus file Neobrutal Dark saja. Tema lain tidak disentuh.
# Sebelum menjalankan ini, pindahkan dulu tema aktif ke tema lain
# (Breeze) di System Settings: Colors, Plasma Style, Application Style, Window Decorations.
set -e

DIR="$(cd "$(dirname "$0")" && pwd)"
DATA="${XDG_DATA_HOME:-$HOME/.local/share}"
CONF="${XDG_CONFIG_HOME:-$HOME/.config}"

rm -f  "$DATA/color-schemes/NeobrutalDark.colors"
rm -rf "$DATA/plasma/desktoptheme/neobrutal-dark"
rm -rf "$CONF/Kvantum/NeobrutalDark"
rm -f  "$DATA/konsole/NeobrutalDark.colorscheme"
rm -f  "$CONF/plasma-workspace/env/neobrutal.sh"

# plugin style + dekorasi: pakai manifest dari build, atau cari di lokasi umum
SUDO=""; [ "$(id -u)" = 0 ] || SUDO="sudo"
if [ -f "$DIR/build/install_manifest.txt" ]; then
  while IFS= read -r f; do
    if [ -w "$(dirname "$f")" ]; then rm -f "$f"; else $SUDO rm -f "$f"; fi
  done < "$DIR/build/install_manifest.txt"
else
  for base in /usr "$HOME/.local"; do
    for f in "$base/lib/qt6/plugins/styles/neobrutal.so" \
             "$base/lib/qt6/plugins/org.kde.kdecoration3/org.kde.neobrutal.so" \
             "$base/share/kstyle/themes/neobrutal.themerc"; do
      [ -e "$f" ] || continue
      if [ -w "$(dirname "$f")" ]; then rm -f "$f"; else $SUDO rm -f "$f"; fi
    done
  done
fi
rm -rf "$DIR/build"

echo "Neobrutal Dark dihapus."
