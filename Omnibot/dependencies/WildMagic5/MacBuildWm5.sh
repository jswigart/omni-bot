#!/bin/tcsh

if ("$1" == "") then
    echo "Usage: $0 cfg opr"
    echo "cfg in {DebugStatic,ReleaseStatic,DebugDynamic,ReleaseDynamic}"
    echo "opr in {build,clean}"
    exit
endif

set libTarget = ''
set appTarget = ''

if ("$1" == "DebugStatic") then
    set libTarget =  'Debug Static'
    set appTarget = 'Glut Debug Static'
else if ("$1" == "ReleaseStatic") then
    set libTarget = 'Release Static'
    set appTarget = 'Glut Release Static'
else if ("$1" == "DebugDynamic") then
    set libTarget = 'Debug Dynamic'
    set appTarget = 'Glut Debug Dynamic'
else if ("$1" == "ReleaseDynamic") then
    set libTarget = 'Release Dynamic'
    set appTarget = 'Glut Release Dynamic'
else
    echo "Usage: $0 cfg opr"
    echo "cfg in {DebugStatic,ReleaseStatic,DebugDynamic,ReleaseDynamic}"
    echo "opr in {build,clean}"
    exit
endif

set opr = ''
if ("$2" == "build") then
    set opr = build
else if ("$2" == "clean") then
    set opr = clean
else
    echo "Usage: $0 cfg opr"
    echo "cfg in {DebugStatic,ReleaseStatic,DebugDynamic,ReleaseDynamic}"
    echo "opr in {build,clean}"
    exit
endif

cd LibCore
xcodebuild -project LibCore.xcodeproj -configuration Default -target "${libTarget}" $opr
cd ..

cd LibMathematics
xcodebuild -project LibMathematics.xcodeproj -configuration Default -target "${libTarget}" $opr
cd ..

cd LibImagics
xcodebuild -project LibImagics.xcodeproj -configuration Default -target "${libTarget}" $opr
cd ..

cd LibPhysics
xcodebuild -project LibPhysics.xcodeproj -configuration Default -target "${libTarget}" $opr
cd ..

cd LibGraphics
xcodebuild -project LibGraphics.xcodeproj -configuration Default -target "${libTarget}" $opr
cd ..

cd LibApplications/GlutApplication
xcodebuild -project GlutApplication.xcodeproj -configuration Default -target "${libTarget}" $opr
cd ../..

cd SampleGraphics
set DIRS = `ls`
foreach dir (${DIRS})
    if (-d $dir) then
        echo $dir
        cd $dir
        xcodebuild -project $dir.xcodeproj -configuration Default -target "$appTarget" $opr
        cd ..
    endif
end
cd ..

cd SampleImagics
set DIRS = `ls`
foreach dir (${DIRS})
    if (-d $dir) then
        echo $dir
        cd $dir
        xcodebuild -project $dir.xcodeproj -configuration Default -target "$appTarget" $opr
        cd ..
    endif
end
cd ..

cd SampleMathematics
set DIRS = `ls`
foreach dir (${DIRS})
    if (-d $dir) then
        echo $dir
        cd $dir
        xcodebuild -project $dir.xcodeproj -configuration Default -target "$appTarget" $opr
        cd ..
    endif
end
cd ..

cd SamplePhysics
set DIRS = `ls`
foreach dir (${DIRS})
    if (-d $dir) then
        echo $dir
        cd $dir
        xcodebuild -project $dir.xcodeproj -configuration Default -target "$appTarget" $opr
        cd ..
    endif
end
cd ..

