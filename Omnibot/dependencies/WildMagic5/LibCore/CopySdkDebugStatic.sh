#!/bin/tcsh
mkdir -p ../SDK/Include
mkdir -p ../SDK/Library
mkdir -p ../SDK/Library/Debug

set DIRS = `ls`
foreach dir (${DIRS})
    if (-d $dir && $dir != LibCore.xcodeproj && $dir != build) then
        echo $dir
        cd $dir
        set CURDIR = `pwd`
        set HEADERS = `ls *.h *.inl`
        foreach header (${HEADERS})
            cp -fp "${CURDIR}"/$header ../../SDK/Include
        end
        cd ..
    endif
end

set RHEADERS = `ls *.h`
foreach rheader (${RHEADERS})
    cp -fp "${SRCROOT}"/$rheader ../SDK/Include
end

if (${XCODE_VERSION_MAJOR} == "0300") then
    cp -fp "${SRCROOT}"/build/Default/libWm5Cored.a ../SDK/Library/Debug/libWm5Cored.a
endif
if (${XCODE_VERSION_MAJOR} == "0400") then
    cp -fp "${BUILT_PRODUCTS_DIR}"/libWm5Cored.a ../SDK/Library/Debug/libWm5Cored.a
endif

ranlib ../SDK/Library/Debug/libWm5Cored.a
