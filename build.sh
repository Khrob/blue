LIBRARIES="-I/usr/local/include
			-lavutil 
			-lavcodec 
			-lswscale 
			-lavformat	
			-lbz2 
			-liconv 
			-lavdevice 
			-lavfilter 
			-lswresample 
			-lm 
			-lz
			-lx264"

# VideoDecodeAcceleration.framework, CoreVideo.framework, CoreFoundation.framework

MACOS_FRAMEWORKS="-framework VideoDecodeAcceleration 
					-framework CoreVideo
              		-framework CoreFoundation"


clang $LIBRARIES $MACOS_FRAMEWORKS betoo.c -o betoo

./betoo -test one