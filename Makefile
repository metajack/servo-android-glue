all:
	cd jni && /home/servo/ndk/android-ndk-r8d/ndk-build
	/home/servo/sdk/android-sdk-linux/tools/android update project --name ServoAndroid --target 1 --path .
	ant debug
