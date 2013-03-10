#!/bin/tcsh
mkdir -p ../SDK/Include
mkdir -p ../SDK/Library
mkdir -p ../SDK/Library/Release

set DIRS = `ls`
foreach dir (${DIRS})
    if (-d $dir && $dir != LibGraphics.xcodeproj && $dir != build) then
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

cd Renderers/OpenGLRenderer
set OCURDIR = `pwd`
set OHEADERS = `ls *.h`
foreach oheader (${OHEADERS})
    cp -fp "${OCURDIR}"/$oheader ../../../SDK/Include
end
cd ../..

cd Renderers/GlutRenderer
set ACURDIR = `pwd`
set AHEADERS = `ls *.h`
foreach aheader (${AHEADERS})
    cp -fp "${ACURDIR}"/$aheader ../../../SDK/Include
end
cd ../..

set RHEADERS = `ls *.h *.inl`
foreach rheader (${RHEADERS})
    cp -fp "${SRCROOT}"/$rheader ../SDK/Include
end

if (${XCODE_VERSION_MAJOR} == "0300") then
    cp -fp "${SRCROOT}"/build/Default/libWm5GlutGraphics.a ../SDK/Library/Release/libWm5GlutGraphics.a
endif
if (${XCODE_VERSION_MAJOR} == "0400") then
    cp -fp "${BUILT_PRODUCTS_DIR}"/libWm5GlutGraphics.a ../SDK/Library/Release/libWm5GlutGraphics.a
endif

ranlib ../SDK/Library/Release/libWm5GlutGraphics.a
