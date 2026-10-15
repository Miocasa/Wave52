```bash
export ANDROID_SDK_ROOT=$HOME/Android/Sdk
export ANDROID_NDK_ROOT=$ANDROID_SDK_ROOT/ndk/27.2.12479018
export JAVA_HOME=/usr/lib/jvm/java-17-openjdk
export QT_ANDROID=$HOME/Qt/6.12.0/android_arm64_v8a
export QT_HOST=$HOME/Qt/6.12.0/gcc_64
export ANDROID_SERIAL=192.168.0.119:39989
export PATH=$ANDROID_SDK_ROOT/platform-tools:$PATH
```

```bash
cd ~/platformio-projects/eink/app
rm -rf build-android
$QT_ANDROID/bin/qt-cmake -S . -B build-android -G Ninja \
  -DQT_HOST_PATH=$QT_HOST \
  -DANDROID_SDK_ROOT=$ANDROID_SDK_ROOT \
  -DANDROID_NDK_ROOT=$ANDROID_NDK_ROOT \
  -DANDROID_ABI=arm64-v8a \
  -DANDROID_PLATFORM=android-31 \
  -DCMAKE_BUILD_TYPE=Debug \
&& cmake --build build-android --target apk \
&& adb install -r build-android/android-build/build/outputs/apk/debug/*.apk
```