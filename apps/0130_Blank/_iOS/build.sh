#!/bin/bash

xcodebuild -configuration Release -scheme HelloWorldApp -archivePath ./HelloWorldApp.xcarchive clean
xcodebuild -configuration Release -scheme HelloWorldApp -archivePath ./HelloWorldApp.xcarchive archive
xcodebuild -exportArchive -archivePath ./HelloWorldApp.xcarchive -exportPath ./build -exportOptionsPlist ./build.plist -allowProvisioningUpdates

echo ""
echo 'Press any key to deploy.'
read -s 
ios-deploy -b build/HelloWorldApp.ipa