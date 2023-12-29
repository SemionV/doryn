source "./build/settings.sh"
printInstallDependency "gl3w"

GL3W_DIR=${LIB_DIR}gl3w/
mkdir -p $GL3W_DIR
git clone https://github.com/skaslev/gl3w.git $GL3W_DIR
cd $GL3W_DIR
python gl3w_gen.py
cd -
mkdir -p ${VENDOR_DIR}GL
cp ${GL3W_DIR}include/GL/gl3w.h ${VENDOR_DIR}GL/gl3w.h
cp ${GL3W_DIR}include/GL/glcorearb.h ${VENDOR_DIR}GL/glcorearb.h
cp ${GL3W_DIR}src/gl3w.c ${VENDOR_DIR}GL/gl3w.c