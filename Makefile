all:
	ndk-build -B
	$(CFG_ANDROID_SDK_PATH)/tools/android update project --name ServoAndroid --target 1 --path .
	ant debug

with-libs: 
	ndk-build -B
	cp -a $(CFG_BUILD_HOME)sofile/*.so* ./libs/armeabi/
	$(CFG_ANDROID_SDK_PATH)/tools/android update project --name ServoAndroid --target 1 --path .
	ant debug
