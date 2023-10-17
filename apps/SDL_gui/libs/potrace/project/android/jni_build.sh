mkdir -p jni
cp -f Android.mk jni
cp -f Application.mk jni
cd jni

rm src
ln -s ../../../src src

ndk-build clean
ndk-build

cd ..
