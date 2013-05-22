all:
	ndk-build -B
	/home/servo/sdk/android-sdk-linux/tools/android update project --name ServoAndroid --target 1 --path .
	ant debug

with-libs: 
	ndk-build -B
	cp -a $(CFG_BUILD_HOME)sofile/*.so* ./libs/armeabi/
	$(CFG_ANDROID_SDK)/tools/android update project --name ServoAndroid --target 1 --path .
	ant debug
