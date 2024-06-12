rm -r build/cmake-artifacts-ninja
SECONDS=0

. build/build-local-dev-cmake.sh

duration=$SECONDS
echo "$((duration / 60)) minutes $((duration % 60)) seconds elapsed."
du -hs build/cmake-artifacts-ninja