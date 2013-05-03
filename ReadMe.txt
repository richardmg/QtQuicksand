This code is QuickSand demo on android platform.
To compile this demo on Android platform, you need Qt for Android SDK.
You can load this project by import ./tags/1.0.0/quicksand.pro from qtcreator
This demo need run with landscape mode, so please add: 
"android:screenOrientation="landscape" android:theme="@android:style/Theme.NoTitleBar.Fullscreen" to the "anroid/AndroidManifest.xml" for activity tag.
Please enable Qt5Core, Qt5Gui, Qt5Xml, Qt5Svg, Qt5Widgets, Qt5Sensors and gnutsl_shared libraries in package configurations.