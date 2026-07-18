#!/usr/bin/env bash
# build-deb.sh — Build goi .deb tu monorepo fcitx5-vnkey.
# dpkg-buildpackage yeu cau thu muc debian/ nam o ROOT cua source tree,
# nen script nay copy packaging/debian -> debian/, build, roi don dep.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "${ROOT}"

# 1. Cai cong cu dong goi neu thieu
if ! command -v dpkg-buildpackage >/dev/null 2>&1; then
    echo "[deb] Cai devscripts + build-essential..."
    sudo apt-get update -qq
    sudo apt-get install -y devscripts build-essential debhelper
fi

# 2. Copy debian/ ra root (khong symlink — dpkg-source khong ua symlink)
rm -rf "${ROOT}/debian"
cp -r "${ROOT}/packaging/debian" "${ROOT}/debian"

# 3. Cai build-deps khai trong debian/control
sudo apt-get build-dep -y "${ROOT}" 2>/dev/null || {
    echo "[deb] 'apt-get build-dep .' can dong 'deb-src' trong sources.list;"
    echo "[deb] fallback: cai truc tiep..."
    sudo apt-get install -y cmake g++ libfcitx5core-dev \
        libfcitx5config-dev libfcitx5utils-dev
}

# 4. Build (khong ky GPG: -us -uc; chi binary: -b)
dpkg-buildpackage -us -uc -b

# 5. Don dep thu muc debian/ tam
rm -rf "${ROOT}/debian"

echo
echo "[deb] Xong! Goi .deb nam o thu muc CHA cua repo:"
ls -1 "${ROOT}/../"fcitx5-vnkey_*.deb 2>/dev/null || true
echo "[deb] Cai bang:  sudo apt install ../fcitx5-vnkey_0.0.1_amd64.deb"