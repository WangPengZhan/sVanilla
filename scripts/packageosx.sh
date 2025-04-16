#!/usr/bin/env bash
set -euo pipefail

if [ $# -ne 2 ]; then
  echo "Usage: $0 MyApp.app output_dir"
  exit 1
fi

APP="$1"
OUT="$2"
APP_NAME=$(basename "${APP}" .app)

rm -rf "${OUT}"
mkdir -p "${OUT}"/{bin,lib,plugins,resources}
cp "${APP}/Contents/MacOS/${APP_NAME}" "${OUT}/bin/${APP_NAME}"
cp -R "${APP}/Contents/Frameworks/"* "${OUT}/lib/"
cp -R "${APP}/Contents/PlugIns/"* "${OUT}/plugins/"
if [ -d "${APP}/Contents/Resources" ]; then
  cp -R "${APP}/Contents/Resources/"* "${OUT}/resources/"
fi

cat > "${OUT}/bin/qt.conf" <<EOF
[Paths]
Prefix = .
Plugins = ../plugins
Libraries = ../lib
Resources = ../resources
EOF

install_name_tool -add_rpath "@loader_path/../lib" "${OUT}/bin/${APP_NAME}"

chmod +x "${OUT}/bin/${APP_NAME}"

echo "package success! ${OUT}"
