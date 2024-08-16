set -e

TARGET_DEFCONFIG=${1:-pineapple_gki_defconfig}

cd "$(dirname "$0")"

LOCALVERSION=-android14-7

if [ "$LTO" == "thin" ]; then
  LOCALVERSION+="-thin"
fi

ARGS="
CC=clang
ARCH=arm64
LLVM=1 LLVM_IAS=1
LOCALVERSION=$LOCALVERSION
"

# build kernel
make -j$(nproc) -C $(pwd) O=$(pwd)/out ${ARGS} $TARGET_DEFCONFIG

./scripts/config --file out/.config \
  -d UH \
  -d RKP \
  -d KDP \
  -d SECURITY_DEFEX \
  -d INTEGRITY \
  -d FIVE \
  -d TRIM_UNUSED_KSYMS

if [ "$LTO" = "thin" ]; then
  ./scripts/config --file out/.config -e LTO_CLANG_THIN -d LTO_CLANG_FULL
fi

make -j$(nproc) -C $(pwd) O=$(pwd)/out ${ARGS}
