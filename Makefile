all:
	ndk-build -B
	cp -a ./../../build/sofile/*.so* ./libs/armeabi/
	/home/servo/sdk/android-sdk-linux/tools/android update project --name ServoAndroid --target 1 --path .
	ant debug
