all: libs/armeabi/iw libs/arm64-v8a/iw

libs/armeabi/iw: Android.mk
	$(NDK_ROOT)/ndk-build NDK_APPLICATION_MK=`pwd`/Application.mk NDK_APP_OUT=. TARGET_PLATFORM=android-21

libs/arm64-v8a/iw: Android.mk
	$(NDK_ROOT)/ndk-build NDK_APPLICATION_MK=`pwd`/Application.mk NDK_APP_OUT=. TARGET_PLATFORM=android-21

install: libs/armeabi/iw
	adb push $< /sdcard/
	adb shell 'su -c "mount -o rw,remount /system/bin"'
	adb shell 'su -c "cp /sdcard/iw /system/bin/iw"'
	adb shell 'su -c "chmod +x /system/bin/iw"'

install: libs/arm64-v8a/iw
	adb push $< /sdcard/
	adb shell 'su -c "mount -o rw,remount /system/bin"'
	adb shell 'su -c "cp /sdcard/iw /system/bin/iw"'
	adb shell 'su -c "chmod +x /system/bin/iw"'

clean:
	rm -Rf libs
	rm -Rf local
