
xcrun -sdk macosx metal -c shaders.metal -o build/shaders.air
xcrun -sdk macosx metallib build/shaders.air   -o build/shaders.metallib