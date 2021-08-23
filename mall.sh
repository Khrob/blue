rm blue 			> /dev/null 2>&1
rm shaders.air 		> /dev/null 2>&1
rm shaders.metallib > /dev/null 2>&1
rm platform.dylib 	> /dev/null 2>&1

./mlib.sh
./mc.sh
./mplat.sh

./blue