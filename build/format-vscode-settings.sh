source ./build/settings.sh
VSCODE_DIR=".vscode/"

sed 's,{{SYSTEM_BIN_DIR}},'"${SYSTEM_BIN_DIR}"',g' "${VSCODE_DIR}c_cpp_properties.template.json" > "${VSCODE_DIR}c_cpp_properties.json"
sed -i -e 's,{{VENDOR_DIR}},'"${VENDOR_DIR}"',g' "${VSCODE_DIR}c_cpp_properties.json"

sed 's,{{DEBUGGER_PATH}},'"${DEBUGGER_PATH}"',g' "${VSCODE_DIR}launch.template.json" > "${VSCODE_DIR}launch.json"