SRC_DEPENDENCY_SCRIPT_DIR="build/dependencies/"

set -e  # exit on error
for f in $SRC_DEPENDENCY_SCRIPT_DIR*.sh; do
  bash "$f"
done