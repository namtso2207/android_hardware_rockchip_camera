/*
 * Copyright (C) 2014-2017 Intel Corporation
 * Copyright (c) 2017, Fuzhou Rockchip Electronics Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "RKISP2CtrlLoop"

#include <utils/Errors.h>
#include <math.h>
#include <sys/stat.h>

#include "PlatformData.h"
#include "CameraMetadataHelper.h"
#include "LogHelper.h"
#include "Utils.h"
#include "RKISP2CtrlLoop.h"
#include "PerformanceTraces.h"
#include "RKISP2CameraCapInfo.h"
#include <rk_aiq_user_api_sysctl.h>

NAMESPACE_DECLARATION {
#if defined(ANDROID_VERSION_ABOVE_8_X)
#define RK_3A_TUNING_FILE_PATH  "/vendor/etc/camera/rkisp1/"
#else
#define RK_3A_TUNING_FILE_PATH  "/etc/camera/rkisp1/"
#endif
using namespace rkisp2;

RKISP2CtrlLoop::RKISP2CtrlLoop(int camId):
        mCameraId(camId),
        mIsStarted(false)
{
    LOGI("@%s", __FUNCTION__);
}


static const cl_result_callback_ops_t *s_callbacks_ops;
static rk_aiq_sys_ctx_t* aiq_ctx = NULL;
status_t RKISP2CtrlLoop::init(const char* sensorName,
                          const cl_result_callback_ops_t *cb)
{
    HAL_TRACE_CALL(CAM_GLBL_DBG_INFO);
    PERFORMANCE_ATRACE_NAME("RKISP2CtrlLoop::init");
    status_t status = OK;
    std::string entityName;
    const char *sensorEntityName = nullptr;
    s_callbacks_ops = cb;
    /* get AIQ xml path */
    const android::camera2::rkisp2::RKISP2CameraCapInfo *cap = getRKISP2CameraCapInfo(mCameraId);
    //const RKISP2CameraCapInfo *capInfo = getRKISP2CameraCapInfo(mCameraId);
    PlatformData::getCameraHWInfo()->getSensorEntityName(mCameraId, entityName);
    //const CameraHWInfo* camHwInfo = PlatformData::getCameraHWInfo();
    //camHwInfo->getSensorEntityName(mCameraId, entityName);
    sensorEntityName = entityName.c_str();
    LOGD("@%s : sensorEntityName:%s", __FUNCTION__, sensorEntityName);
    std::string iq_file = cap->getIqTuningFile();
    std::string iq_file_path(RK_3A_TUNING_FILE_PATH);
    std::string iq_file_full_path = iq_file_path + iq_file;
    ALOGD("@%s : iq_file_full_path:%s", __FUNCTION__, iq_file_full_path.c_str());
    aiq_ctx = rk_aiq_uapi_sysctl_init(sensorEntityName, RK_3A_TUNING_FILE_PATH, NULL, NULL);

    //CamHwIsp20::selectIqFile(aiq_ctx->_sensor_entity_name, iq_file_full_name);
    //property_set(CAM_IQ_PROPERTY_KEY,iq_file_full_name);
    
#if 0
    struct stat fileInfo;

    CLEAR(fileInfo);
    if (stat(iq_file_full_path .c_str(), &fileInfo) < 0) {
        if (errno == ENOENT) {
            LOGI("sensor tuning file missing: \"%s\"!", sensorName);
            return NAME_NOT_FOUND;
        } else {
            LOGE("ERROR querying sensor tuning filestat for \"%s\": %s!",
                 iq_file_full_path.c_str(), strerror(errno));
            return UNKNOWN_ERROR;
        }
    }
#endif
    // bool ret = (rkisp_cl_rkaiq_init(&mControlLoopCtx , iq_file_full_path.c_str(), cb, sensorEntityName) == 0 ? true : false);
    // CheckError(ret == false, UNKNOWN_ERROR, "@%s, Error in isp control loop init", __FUNCTION__);
    // struct rkisp_cl_prepare_params_s prepareParams;

    // memset(&prepareParams, 0, sizeof(struct rkisp_cl_prepare_params_s));
    // prepareParams.staticMeta = PlatformData::getStaticMetadata(mCameraId);

    // prepareParams.work_mode = "NORMAL";

    // int result = rkisp_cl_prepare(mControlLoopCtx, &prepareParams);
    // if (result < 0) {
    //     LOGE("%s: rkisp control loop prepare failed !", __FUNCTION__);
    //     return UNKNOWN_ERROR;
    // }

    // result = rkisp_cl_start(mControlLoopCtx);
    // if (result < 0) {
    //     LOGE("%s: rkisp control loop start failed !", __FUNCTION__);
    //     return UNKNOWN_ERROR;
    // }
    return status;
}

void RKISP2CtrlLoop::deinit()
{
    
    //     rkisp_cl_stop(mControlLoopCtx);
    //     mIsStarted = false;
    
    rk_aiq_uapi_sysctl_deinit(aiq_ctx);
    // HAL_TRACE_CALL(CAM_GLBL_DBG_INFO);
    // PERFORMANCE_ATRACE_NAME("RKISP2CtrlLoop::deinit");

    // rkisp_cl_deinit(mControlLoopCtx);
    mControlLoopCtx = NULL;
}

status_t RKISP2CtrlLoop::start(const struct rkisp_cl_prepare_params_s& params)
{
    if (mIsStarted == true)
        return OK;

    PERFORMANCE_ATRACE_NAME("RKISP2CtrlLoop::start");
    HAL_TRACE_CALL(CAM_GLBL_DBG_INFO);
    int ret = 0;

    // LOGI("@%s %d: isp:%s, param:%s, stat:%s, sensor:%s", __FUNCTION__, __LINE__,
    //      params.isp_sd_node_path, params.isp_vd_params_path, params.isp_vd_stats_path, params.sensor_sd_node_path);
    rk_aiq_uapi_sysctl_prepare(aiq_ctx, 640, 480, RK_AIQ_WORKING_MODE_NORMAL);
    //char iq_file_full_name[128] = {'\0'}; 
rk_aiq_uapi_sysctl_start(aiq_ctx);

    // ret = rkisp_cl_prepare(mControlLoopCtx, &params);
    // if (ret < 0) {
    //     LOGE("%s: rkisp control loop prepare failed !", __FUNCTION__);
    //     return UNKNOWN_ERROR;
    // }

    // ret = rkisp_cl_start(mControlLoopCtx);
    // if (ret < 0) {
    //     LOGE("%s: rkisp control loop start failed !", __FUNCTION__);
    //     return UNKNOWN_ERROR;
    // }
    mIsStarted = true;
    return NO_ERROR;
}

status_t RKISP2CtrlLoop::setFrameParams(rkisp_cl_frame_metadata_s* frame_params)
{
    int ret = 0;

    // ret = rkisp_cl_set_frame_params(mControlLoopCtx, frame_params);
    // if (ret < 0) {
    //     LOGE("%s: rkisp control loop set frame params failed !", __FUNCTION__);
    //     return UNKNOWN_ERROR;
    // }
    rkisp_cl_frame_metadata_s cb_result;
    
    camera_metadata_t *_meta = allocate_camera_metadata(DEFAULT_ENTRY_CAP, DEFAULT_DATA_CAP);
    CameraMetadata *_metadata = new CameraMetadata(_meta);
    cb_result.id = frame_params->id;
    cb_result.metas = _metadata->getAndLock();
    if (s_callbacks_ops)
        s_callbacks_ops->metadata_result_callback(s_callbacks_ops, &cb_result);
    _metadata->unlock(cb_result.metas);
    return NO_ERROR;

}

status_t RKISP2CtrlLoop::stop()
{
    if (mIsStarted == false)
        return OK;

    int ret = 0;
    HAL_TRACE_CALL(CAM_GLBL_DBG_INFO);
    PERFORMANCE_ATRACE_NAME("RKISP2CtrlLoop::stop");
    rk_aiq_uapi_sysctl_stop(aiq_ctx, false);
    // ret = rkisp_cl_stop(mControlLoopCtx);
    // if (ret < 0) {
    //     LOGE("%s: rkisp control loop stop failed !", __FUNCTION__);
    //     return UNKNOWN_ERROR;
    // }

    mIsStarted = false;
    return NO_ERROR;
 }

} NAMESPACE_DECLARATION_END

