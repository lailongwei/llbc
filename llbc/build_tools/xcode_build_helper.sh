# xcode_build_helper.sh
# brief: xCode编译辅助脚本（简单脚本，用于合并，迁移库文件）
###############################################################################

# 变量定义
# 项目路径及日志文件定义
LLBC_PROJ_PATH="${SRCROOT}/llbc"
LLBC_BUILD_LOG_FILE="${LLBC_PROJ_PATH}/xcode_build_helper.buildlog"

# 编译架构
LLBC_ARCH_I386=i386
LLBC_ARCH_ARMV7=armv7
LLBC_ARCH_ARMV7S=armv7s
LLBC_BUILD_ARCH=${ARCHS}
# 库文件名 & 扩展名
LLBC_ORIGIN_LIB_NAME=${EXECUTABLE_NAME}
LLBC_LIB_NAME="${EXECUTABLE_NAME%%.*}"
LLBC_LIB_NAME_EXT=${EXECUTABLE_NAME##*.}
# Debug版本架构库文件路径定义
LLBC_DBG_LIB_PATH="${SYMROOT}/Debug-iphoneos"
LLBC_DBG_SIMU_LIB_PATH="${SYMROOT}/Debug-iphonesimulator"
LLBC_DBG_I386_LIB_FILE="${LLBC_DBG_SIMU_LIB_PATH}/${LLBC_LIB_NAME}_debug_iphone-simulator-${LLBC_ARCH_I386}.${LLBC_LIB_NAME_EXT}"
LLBC_DBG_ARMV7_LIB_FILE="${LLBC_DBG_LIB_PATH}/${LLBC_LIB_NAME}_debug_iphone-${LLBC_ARCH_ARMV7}.${LLBC_LIB_NAME_EXT}"
LLBC_DBG_ARMV7S_LIB_FILE="${LLBC_DBG_LIB_PATH}/${LLBC_LIB_NAME}_debug_iphone-${LLBC_ARCH_ARMV7S}.${LLBC_LIB_NAME_EXT}"
# Release版本架构库文件路径定义
LLBC_RELEASE_LIB_PATH="${SYMROOT}/Release-iphoneos"
LLBC_RELEASE_SIMU_LIB_PATH="${SYMROOT}/Release-iphonesimulator"
LLBC_RELEASE_I386_LIB_FILE="${LLBC_RELEASE_SIMU_LIB_PATH}/${LLBC_LIB_NAME}_release_iphone-simulator-${LLBC_ARCH_I386}.${LLBC_LIB_NAME_EXT}"
LLBC_RELEASE_ARMV7_LIB_FILE="${LLBC_RELEASE_LIB_PATH}/${LLBC_LIB_NAME}_release_iphone-${LLBC_ARCH_ARMV7}.${LLBC_LIB_NAME_EXT}"
LLBC_RELEASE_ARMV7S_LIB_FILE="${LLBC_RELEASE_LIB_PATH}/${LLBC_LIB_NAME}_release_iphone-${LLBC_ARCH_ARMV7S}.${LLBC_LIB_NAME_EXT}"

# 清理 Build Log
function LLBC_CleanupBuildLog()
{
    rm -rf "${LLBC_BUILD_LOG_FILE}"
    touch "${LLBC_BUILD_LOG_FILE}"
}

# 写入信息到Build Log
function LLBC_WriteLog()
{
    local outText=""
    for curArg in "$@"
    do
        outText+=${curArg}
    done

    echo "${outText}" >> ${LLBC_BUILD_LOG_FILE}
}

# Dump 想关信息
function LLBC_DumpBuildInfo()
{
    LLBC_CleanupBuildLog
    LLBC_WriteLog "******************* llbc library build helper ***********************"
    LLBC_WriteLog "PROJ PATH: ${LLBC_PROJ_PATH}"
    LLBC_WriteLog "Current build arch: ${LLBC_BUILD_ARCH}"
    LLBC_WriteLog "XCode env executable name: ${LLBC_LIB_NAME}, extension: ${LLBC_LIB_NAME_EXT}"
    LLBC_WriteLog "============================================================================="
    LLBC_WriteLog "Debug ios lib path: ${LLBC_DBG_LIB_PATH}"
    LLBC_WriteLog "Debug ios simulator lib path: ${LLBC_DBG_SIMU_LIB_PATH}"
    LLBC_WriteLog "Release ios lib path: ${LLBC_RELEASE_LIB_PATH}"
    LLBC_WriteLog "Release ios simulator lib path: ${LLBC_RELEASE_SIMU_LIB_PATH}"
    LLBC_WriteLog "============================================================================="
    LLBC_WriteLog "Debug version iphone(i386) lib: ${LLBC_DBG_I386_LIB_FILE}"
    LLBC_WriteLog "Debug version iphone(armv7) lib: ${LLBC_DBG_ARMV7_LIB_FILE}"
    LLBC_WriteLog "Debug version iphone(armv7s) lib: ${LLBC_DBG_ARMV7S_LIB_FILE}"
    LLBC_WriteLog "============================================================================="
    LLBC_WriteLog "Release version iphone(i386) lib: ${LLBC_RELEASE_I386_LIB_FILE}"
    LLBC_WriteLog "Release version iphone(armv7s) lib: ${LLBC_RELEASE_ARMV7_LIB_FILE}"
    LLBC_WriteLog "Release version iphone(armv7s) lib: ${LLBC_RELEASE_ARMV7S_LIB_FILE}"
}

# 创建编译辅助目录
function LLBC_CreateBuildDir()
{
    mkdir -p "${LLBC_PROJ_PATH}/lib"
}

# 执行编译，合并
function LLBC_ExecBuild()
{
    local tempVal=""

    # Get arch lib file(debug & release).
    local dbgLibFile=
    local releaseLibFile=
    local dbgOriginFile=
    local releaseOriginFile=
    if [ "${LLBC_BUILD_ARCH}" = "${LLBC_ARCH_I386}" ]; then
        dbgLibFile="${LLBC_DBG_I386_LIB_FILE}"
        releaseLibFile="${LLBC_RELEASE_I386_LIB_FILE}"
        dbgOriginFile="${LLBC_DBG_SIMU_LIB_PATH}/${LLBC_ORIGIN_LIB_NAME}"
        releaseOriginFile="${LLBC_RELEASE_SIMU_LIB_PATH}/${LLBC_ORIGIN_LIB_NAME}"
    elif [ "${LLBC_BUILD_ARCH}" = "${LLBC_ARCH_ARMV7}" ]; then
        dbgLibFile="${LLBC_DBG_ARMV7_LIB_FILE}"
        releaseLibFile="${LLBC_RELEASE_ARMV7_LIB_FILE}"
        dbgOriginFile="${LLBC_DBG_LIB_PATH}/${LLBC_ORIGIN_LIB_NAME}"
        releaseOriginFile="${LLBC_RELEASE_LIB_PATH}/${LLBC_ORIGIN_LIB_NAME}"
    else
        dbgLibFile="${LLBC_DBG_ARMV7S_LIB_FILE}"
        releaseLibFile="${LLBC_RELEASE_ARMV7S_LIB_FILE}"
        dbgOriginFile="${LLBC_DBG_LIB_PATH}/${LLBC_ORIGIN_LIB_NAME}"
        releaseOriginFile="${LLBC_RELEASE_LIB_PATH}/${LLBC_ORIGIN_LIB_NAME}"
    fi

    # Move library.
    LLBC_WriteLog ""
    LLBC_WriteLog "Move library:"
    if [ "${dbgOriginFile}" -nt "${dbgLibFile}" ]; then
        LLBC_WriteLog "debug iphone library file(${LLBC_BUILD_ARCH}) found(or newer), copy it:"
        LLBC_WriteLog "  `cp -v "${dbgOriginFile}" "${dbgLibFile}"`"
    fi
    if [ "${releaseOriginFile}" -nt "${releaseLibFile}" ]; then
        LLBC_WriteLog "release iphone library file(${LLBC_BUILD_ARCH}) found(or newer), copy it:"
        LLBC_WriteLog "  `cp -v "${releaseOriginFile}" "${releaseLibFile}"`"
    fi
    LLBC_WriteLog "Done"

    # Merge x86 & arm debug librarys.
    LLBC_WriteLog ""
    LLBC_WriteLog "Merge debug libraries:"

    local allDbgLibs=
    local dbgLibsCount=0
    if [ -e "${LLBC_DBG_I386_LIB_FILE}" ]; then
        LLBC_WriteLog "Found dbg lib(i386): `basename "${LLBC_DBG_I386_LIB_FILE}"`"
        allDbgLibs+="${LLBC_DBG_I386_LIB_FILE}"
        dbgLibsCount=$(( dbgLibsCount + 1 ))
    fi
    if [ -e "${LLBC_DBG_ARMV7_LIB_FILE}" ]; then
        LLBC_WriteLog "Found dbg lib(armv7): `basename "${LLBC_DBG_ARMV7_LIB_FILE}"`"
        if [ -n "${allDbgLibs}" ]; then
            allDbgLibs+=" "
        fi
        allDbgLibs+="${LLBC_DBG_ARMV7_LIB_FILE}"
        dbgLibsCount=$(( dbgLibsCount + 1 ))
    fi
    if [ -e "${LLBC_DBG_ARMV7S_LIB_FILE}" ]; then
        LLBC_WriteLog "Found dbg lib(armv7s): `basename "${LLBC_DBG_ARMV7S_LIB_FILE}"`"
        if [ -n "${allDbgLibs}" ]; then
            allDbgLibs+=" "
        fi
        allDbgLibs+="${LLBC_DBG_ARMV7S_LIB_FILE}"
        dbgLibsCount=$(( dbgLibsCount + 1 ))
    fi
    local dbgOutputLibFile="${LLBC_PROJ_PATH}/lib/${LLBC_LIB_NAME}_debug.${LLBC_LIB_NAME_EXT}"
    if [ ${dbgLibsCount} -gt 1 ]; then
        LLBC_WriteLog "Debug libraries count > 1, merge it!"
        LLBC_WriteLog "Origin libs: ${allDbgLibs}"
        LLBC_WriteLog "Output lib: ${dbgOutputLibFile}"
        lipo -create -output "${dbgOutputLibFile}" ${allDbgLibs}
    elif [ ${dbgLibsCount} -eq 1 ]; then
        LLBC_WriteLog "Debug libraries count == 1, just copy it! "
        LLBC_WriteLog "  `cp -v "${allDbgLibs}" "${dbgOutputLibFile}"`"
    else
        LLBC_WriteLog "Not found any debug libraries, merge failed!"
    fi

    LLBC_WriteLog "Done"

    # Merge x86 & arm release librarys.
    LLBC_WriteLog ""
    LLBC_WriteLog "Merge release libraries:"

    local allReleaseLibs=
    local releaseLibsCount=0
    if [ -e "${LLBC_RELEASE_I386_LIB_FILE}" ]; then
        LLBC_WriteLog "Found release lib(i386): `basename "${LLBC_RELEASE_I386_LIB_FILE}"`"
        allReleaseLibs+="${LLBC_RELEASE_I386_LIB_FILE}"
        releaseLibsCount=$(( releaseLibsCount + 1 ))
    fi
    if [ -e "${LLBC_RELEASE_ARMV7_LIB_FILE}" ]; then
        LLBC_WriteLog "Found release lib(armv7): `basename "${LLBC_RELEASE_ARMV7_LIB_FILE}"`"
        if [ -n "${allReleaseLibs}" ]; then
            allReleaseLibs+=" "
        fi
        allReleaseLibs+="${LLBC_RELEASE_ARMV7_LIB_FILE}"
        releaseLibsCount=$(( releaseLibsCount + 1 ))
    fi
    if [ -e "${LLBC_RELEASE_ARMV7S_LIB_FILE}" ]; then
        LLBC_WriteLog "Found release lib(armv7s): `basename "${LLBC_RELEASE_ARMV7S_LIB_FILE}"`"
        if [ -n "${allReleaseLibs}" ]; then
            allReleaseLibs+=" "
        fi
        allReleaseLibs+="${LLBC_RELEASE_ARMV7S_LIB_FILE}"
        releaseLibsCount=$(( releaseLibsCount + 1 ))
    fi
    local releaseOutputLibFile="${LLBC_PROJ_PATH}/lib/${LLBC_LIB_NAME}.${LLBC_LIB_NAME_EXT}"
    if [ ${releaseLibsCount} -gt 1 ]; then
        LLBC_WriteLog "Release libraries count > 1, merge it!"
        LLBC_WriteLog "Origin libs: ${allReleaseLibs}"
        LLBC_WriteLog "Output lib: ${releaseOutputLibFile}"
        lipo -create -output "${releaseOutputLibFile}" ${allReleaseLibs}
    elif [ ${releaseLibsCount} -eq 1 ]; then
        LLBC_WriteLog "Release libraries count == 1, just copy it!"
        LLBC_WriteLog "  `cp -v "${allReleaseLibs}" "${releaseOutputLibFile}"`"
    else
        LLBC_WriteLog "Not found any release libraries, merge failed!"
    fi

    LLBC_WriteLog "Done"
}

# 主函数
function Main()
{
    # Dump build info to log file.
    LLBC_DumpBuildInfo

    # Create build dir.
    LLBC_CreateBuildDir

    # Build lib.
    LLBC_ExecBuild
}

Main $@
