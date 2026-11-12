#!/usr/bin/env bash
# Neobrutal Dark untuk KDE Plasma 6
# Memasang tema sebagai tema BARU (nama sendiri), tidak menimpa tema yang sudah ada.
#
#   ./install.sh                 -> pasang semuanya (style + dekorasi dikompilasi, pakai sudo)
#   ./install.sh --apply         -> pasang lalu langsung aktifkan semuanya
#   ./install.sh --user          -> pasang plugin ke ~/.local (tanpa sudo)
#   ./install.sh --no-build      -> hanya file tema (warna, plasma style, konsole, kvantum)
set -e

DIR="$(cd "$(dirname "$0")" && pwd)"
DATA="${XDG_DATA_HOME:-$HOME/.local/share}"
CONF="${XDG_CONFIG_HOME:-$HOME/.config}"

APPLY=0; USER_INSTALL=0; BUILD=1
for a in "$@"; do
  case "$a" in
    --apply) APPLY=1 ;;
    --user) USER_INSTALL=1 ;;
    --no-build) BUILD=0 ;;
    *) echo "Opsi tidak dikenal: $a"; exit 1 ;;
  esac
done

# 1. Color scheme -> System Settings > Colors & Themes > Colors
mkdir -p "$DATA/color-schemes"
cp "$DIR/color-schemes/NeobrutalDark.colors" "$DATA/color-schemes/"

# 2. Plasma Style (app menu, panel, tooltip, popup)
mkdir -p "$DATA/plasma/desktoptheme"
rm -rf "$DATA/plasma/desktoptheme/neobrutal-dark"
cp -r "$DIR/plasma/desktoptheme/neobrutal-dark" "$DATA/plasma/desktoptheme/"
cp "$DIR/color-schemes/NeobrutalDark.colors" "$DATA/plasma/desktoptheme/neobrutal-dark/colors"

# 3. Kvantum (opsional, hanya cadangan; style bawaan tema ini tidak butuh Kvantum)
mkdir -p "$CONF/Kvantum/NeobrutalDark"
cp "$DIR/Kvantum/NeobrutalDark/"* "$CONF/Kvantum/NeobrutalDark/"

# 4. Konsole
mkdir -p "$DATA/konsole"
cp "$DIR/konsole/NeobrutalDark.colorscheme" "$DATA/konsole/"

echo "File tema terpasang: 'Neobrutal Dark'."

# 5. Application Style + Window Decoration (C++, dikompilasi)
if [ "$BUILD" = 1 ]; then
  command -v cmake >/dev/null || { echo "cmake tidak ada. Arch: sudo pacman -S --needed base-devel cmake extra-cmake-modules qt6-base kcoreaddons kdecoration"; exit 1; }
  if [ "$USER_INSTALL" = 1 ]; then PREFIX="$HOME/.local"; else PREFIX="/usr"; fi
  BUILD_DIR="$DIR/build"
  cmake -S "$DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$PREFIX"
  cmake --build "$BUILD_DIR" -j"$(nproc)"
  if [ "$USER_INSTALL" = 1 ] || [ "$(id -u)" = 0 ]; then
    cmake --install "$BUILD_DIR"
  else
    sudo cmake --install "$BUILD_DIR"
  fi

  if [ "$USER_INSTALL" = 1 ]; then
    # plugin di ~/.local tidak ada di path default Qt; daftarkan lewat env Plasma
    mkdir -p "$CONF/plasma-workspace/env"
    cat > "$CONF/plasma-workspace/env/neobrutal.sh" <<EOF
export QT_PLUGIN_PATH="\$HOME/.local/lib/qt6/plugins\${QT_PLUGIN_PATH:+:\$QT_PLUGIN_PATH}"
EOF
    echo "Plugin dipasang ke ~/.local. Logout/login diperlukan supaya QT_PLUGIN_PATH terbaca."
  fi
  echo "Application Style 'Neobrutal' dan Window Decoration 'Neobrutal Dark' terpasang."
fi

if [ "$APPLY" = 1 ]; then
  plasma-apply-colorscheme NeobrutalDark
  plasma-apply-desktoptheme neobrutal-dark
  if [ "$BUILD" = 1 ]; then
    kwriteconfig6 --file kdeglobals --group KDE --key widgetStyle neobrutal
    kwriteconfig6 --file kwinrc --group org.kde.kdecoration2 --key library org.kde.neobrutal
    kwriteconfig6 --file kwinrc --group org.kde.kdecoration2 --key theme "Neobrutal Dark"
    qdbus6 org.kde.KWin /KWin reconfigure 2>/dev/null || true
  fi
  echo "Diaktifkan. Logout lalu login lagi supaya semua app memuat style baru."
else
  cat <<'EOF'

Aktifkan lewat System Settings:
  1. Colors & Themes > Colors            -> "Neobrutal Dark"
  2. Colors & Themes > Plasma Style      -> "Neobrutal Dark"
  3. Colors & Themes > Application Style -> "Neobrutal"
  4. Colors & Themes > Window Decorations-> "Neobrutal Dark"
  5. Konsole: Settings > Edit Current Profile > Appearance > "Neobrutal Dark"
Atau jalankan ulang: ./install.sh --apply
EOF
fi
