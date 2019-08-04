#!/usr/bin/env bash
# ---------------------------------------------------------------------
# @file     post_build.sh
# @author   Longwei Lai<lailongwei@126.com>
# @date     2016/10/04
#
# @brief    iOS post build script, use to merge device library and simulator library.
# ---------------------------------------------------------------------

# ---------------------------------------------------------------------
# Some useful global variables define.
#
# The device and simulator library output directory define.
__DEVICE_DIR=${SYMROOT}/${CONFIGURATION}-iphoneos
__SIMULATOR_DIR=${SYMROOT}/${CONFIGURATION}-iphonesimulator

# Target output directory define.
__OUTPUT_DIR=${SRCROOT}/../output/iOS
[ -d "${__OUTPUT_DIR}" ] || \mkdir -p "${__OUTPUT_DIR}"

# Build log file
__POST_BUILD_LOG_FILE="${__OUTPUT_DIR}/${TARGET_NAME}.post_build_log"


# ---------------------------------------------------------------------
# Methods implementation.
#
function Log() {
    echo "$@" | tee -a "${__POST_BUILD_LOG_FILE}"
}

function AddSuffix() {
    local normalizedName="$1"
    local isDevice=$2
    if [ "${CONFIGURATION}" == "Debug" ]; then
        normalizedName="${normalizedName}_debug"
    fi

    if [ "${isDevice}" == TRUE ]; then
        normalizedName="${normalizedName}-iphoneos"
    else
        normalizedName="${normalizedName}-iphonesimulator"
    fi

    echo -n "${normalizedName}${EXECUTABLE_SUFFIX}"
}

function MergeLibraries() {
    # Copy device library to normalized output path(__OUTPUT_DIR), if exists.
    local srcDeviceLibPath="${__DEVICE_DIR}/${EXECUTABLE_NAME}"
    local destDeviceLibPath="$(AddSuffix "${__OUTPUT_DIR}/${EXECUTABLE_PREFIX}${TARGET_NAME}" TRUE)"
    if [ -e "${srcDeviceLibPath}" ]; then
        Log "Find device library file[${srcDeviceLibPath}], copy to ${destDeviceLibPath}"
        \cp -rf "${srcDeviceLibPath}" "${destDeviceLibPath}"
    fi

    # Copy simulator library to normalized output path(__OUTPUT_DIR), if exists.
    local srcSimulatorLibPath="${__SIMULATOR_DIR}/${EXECUTABLE_NAME}"
    local destSimulatorLibPath="$(AddSuffix "${__OUTPUT_DIR}/${EXECUTABLE_PREFIX}${TARGET_NAME}" FALSE)"
    if [ -e "${srcSimulatorLibPath}" ]; then
        Log "Find simulator library file[${srcSimulatorLibPath}], copy to ${destSimulatorLibPath}"
        \cp -rf "${srcSimulatorLibPath}" "${destSimulatorLibPath}"
    fi

    # If exist device library and simulator library both, do merge.
    local universalLibPath="${__OUTPUT_DIR}/${EXECUTABLE_PREFIX}${TARGET_NAME}"
    if [ "${CONFIGURATION}" == "Debug" ]; then
        universalLibPath="${universalLibPath}_debug"
    fi
    universalLibPath="${universalLibPath}${EXECUTABLE_SUFFIX}"
    if [ -e "${destDeviceLibPath}" -a -e "${destSimulatorLibPath}" ]; then
        Log "Merge `basename "${destDeviceLibPath}"`, `basename "${destSimulatorLibPath}"`..."
        lipo -create -output "${universalLibPath}" "${destDeviceLibPath}" "${destSimulatorLibPath}"
        local mergeRet=$?
        if [ ${mergeRet} -ne 0 ]; then
            Log "Failed to merge libraries, return code(return from lipo tool): ${mergeRet}"
            return $mergeRet
        fi

        Log "Merge libraries success, universal library path: ${universalLibPath}"
    fi
}

function main() {
    echo "Exec ${TARGET_NAME} post build:" > "${__POST_BUILD_LOG_FILE}"
    MergeLibraries || exit 1
}

main $@

