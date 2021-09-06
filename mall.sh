rm -rf build

mkdir build

./mlib.sh
./mplat.sh
./mc.sh

pushd build

mkdir -p blue.app/Contents/Resources
mkdir -p blue.app/Contents/MacOS

cp blue 			blue.app/Contents/MacOS/blue
cp shaders.metallib blue.app/Contents/MacOS/shaders.metallib
cp platform.dylib   blue.app/Contents/MacOS/platform.dylib

# cp -r blue.dSYM bluedit.app/Contents/MacOS/blue.dSYM
# cp -r platform.dylib.dSYM bluedit.app/Contents/MacOS/platform.dylib.dSYM

cp ../resources/Info.plist blue.app/Contents/Info.plist
cp ../resources/PkgInfo blue.app/Contents/PkgInfo
cp ../resources/blue.icns blue.app/Contents/Resources/blue.icns

popd

open build/blue.app

