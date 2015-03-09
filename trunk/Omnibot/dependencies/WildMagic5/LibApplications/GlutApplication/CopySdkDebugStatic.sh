#!/bin/tcsh
cd ${SRCROOT}
set HEADERS = `ls ../*.h ../*.inl`
foreach header (${HEADERS})
    cp -fp "${SRCROOT}"/$header ../../SDK/Include
end

if (${XCODE_VERSION_MAJOR} == "0300") then
    cp -fp "${SRCROOT}"/build/Default/libWm5GlutApplicationd.a ../../SDK/Library/Debug/libWm5GlutApplicationd.a
endif
if (${XCODE_VERSION_MAJOR} == "0400") then
    cp -fp "${BUILT_PRODUCTS_DIR}"/libWm5GlutApplicationd.a ../../SDK/Library/Debug/libWm5GlutApplicationd.a
endif

ranlib ../../SDK/Library/Debug/libWm5GlutApplicationd.a

