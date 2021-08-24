
BUNDLE_RESOURCES_PATH="blue.app/Contents/Resources"

#RESOURCES_PATH="resources"

xcrun -sdk macosx metal -c shaders.metal -o shaders.air
xcrun -sdk macosx metallib shaders.air -o shaders.metallib
swiftc blue.swift -emit-library -o platform.dylib

rm -rf blue.app
mkdir -p $BUNDLE_RESOURCES_PATH
cp blue blue.app/blue
cp -r blue.dSYM ${BUNDLE_RESOURCES_PATH}/blue.dSYM
#cp -r "${RESOURCES_PATH}/animation.asset" ${BUNDLE_RESOURCES_PATH}
cp ${PLATFORM_RESOURCES_PATH}/Info.plist PixelShaderDemo.app/Contents/Info.plist
cp shaders.metallib ${BUNDLE_RESOURCES_PATH}/shaders.metallib

popd