/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "nfc_napi_utils.h"
#include "securec.h"
#include "log.h"

namespace OHOS {
namespace NFC {
TraceFuncCall::TraceFuncCall(const std::string funcName): m_funcName(funcName)
{
    if (m_isTrace) {
        m_startTime = std::chrono::steady_clock::now();
        HILOGD("Call func: %{public}s (start)", m_funcName.c_str());
    }
}

TraceFuncCall::~TraceFuncCall()
{
    if (m_isTrace) {
        auto us = std::chrono::duration_cast<std::chrono::microseconds>
            (std::chrono::steady_clock::now() - m_startTime).count();
        constexpr int usForPerMs = 1000;
        HILOGD("Call func: %{public}s (end), time cost:%{public}lldus, %{public}lldms",
            m_funcName.c_str(), us, us / usForPerMs);
    }
}

napi_value UndefinedNapiValue(const napi_env& env)
{
    napi_value result;
    napi_get_undefined(env, &result);
    return result;
}

napi_value JsObjectToInt(const napi_env& env, const napi_value& object, const char* fieldStr, int& fieldRef)
{
    bool hasProperty = false;
    NAPI_CALL(env, napi_has_named_property(env, object, fieldStr, &hasProperty));
    if (hasProperty) {
        napi_value field;
        napi_valuetype valueType;

        napi_get_named_property(env, object, fieldStr, &field);
        NAPI_CALL(env, napi_typeof(env, field, &valueType));
        NAPI_ASSERT(env, valueType == napi_number, "Wrong argument type. Number expected.");
        napi_get_value_int32(env, field, &fieldRef);
    } else {
        HILOGW("Js to int no property: %{public}s", fieldStr);
    }
    return UndefinedNapiValue(env);
}

napi_value JsObjectToBool(const napi_env& env, const napi_value& object, const char* fieldStr, bool& fieldRef)
{
    bool hasProperty = false;
    NAPI_CALL(env, napi_has_named_property(env, object, fieldStr, &hasProperty));
    if (hasProperty) {
        napi_value field;
        napi_valuetype valueType;

        napi_get_named_property(env, object, fieldStr, &field);
        NAPI_CALL(env, napi_typeof(env, field, &valueType));
        NAPI_ASSERT(env, valueType == napi_boolean, "Wrong argument type. Bool expected.");
        napi_get_value_bool(env, field, &fieldRef);
    } else {
        HILOGW("Js to bool no property: %{public}s", fieldStr);
    }
    return UndefinedNapiValue(env);
}

napi_status SetValueUtf8String(const napi_env& env, const char* fieldStr, const char* str, napi_value& result)
{
    napi_value value;
    napi_status status = napi_create_string_utf8(env, str, NAPI_AUTO_LENGTH, &value);
    if (status != napi_ok) {
        HILOGE("Set value create utf8 string error! field: %{public}s", fieldStr);
        return status;
    }
    status = napi_set_named_property(env, result, fieldStr, value);
    if (status != napi_ok) {
        HILOGE("Set utf8 string named property error! field: %{public}s", fieldStr);
    }
    return status;
}

napi_status SetValueInt32(const napi_env& env, const char* fieldStr, const int intValue, napi_value& result)
{
    napi_value value;
    napi_status status = napi_create_int32(env, intValue, &value);
    if (status != napi_ok) {
        HILOGE("Set value create int32 error! field: %{public}s", fieldStr);
        return status;
    }
    status = napi_set_named_property(env, result, fieldStr, value);
    if (status != napi_ok) {
        HILOGE("Set int32 named property error! field: %{public}s", fieldStr);
    }
    return status;
}

napi_status SetValueUnsignedInt32(const napi_env& env, const char* fieldStr, const int intValue, napi_value& result)
{
    napi_value value;
    napi_status status = napi_create_uint32(env, intValue, &value);
    if (status != napi_ok) {
        HILOGE("Set value create unsigned int32 error! field: %{public}s", fieldStr);
        return status;
    }
    status = napi_set_named_property(env, result, fieldStr, value);
    if (status != napi_ok) {
        HILOGE("Set unsigned int32 named property error! field: %{public}s", fieldStr);
    }
    return status;
}

napi_status SetValueInt64(const napi_env& env, const char* fieldStr, const int64_t intValue, napi_value& result)
{
    napi_value value;
    napi_status status = napi_create_int64(env, intValue, &value);
    if (status != napi_ok) {
        HILOGE("Set value create int64 error! field: %{public}s", fieldStr);
        return status;
    }
    status = napi_set_named_property(env, result, fieldStr, value);
    if (status != napi_ok) {
        HILOGE("Set int64 named property error! field: %{public}s", fieldStr);
    }
    return status;
}

napi_status SetValueBool(const napi_env& env, const char* fieldStr, const bool boolvalue, napi_value& result)
{
    napi_value value;
    napi_status status = napi_get_boolean(env, boolvalue, &value);
    if (status != napi_ok) {
        HILOGE("Set value create boolean error! field: %{public}s", fieldStr);
        return status;
    }
    status = napi_set_named_property(env, result, fieldStr, value);
    if (status != napi_ok) {
        HILOGE("Set boolean named property error! field: %{public}s", fieldStr);
    }
    return status;
}

static napi_value InitAsyncCallBackEnv(const napi_env& env, AsyncContext *asyncContext,
    const size_t argc, const napi_value *argv, const size_t nonCallbackArgNum)
{
    for (size_t i = nonCallbackArgNum; i != argc; ++i) {
        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, argv[i], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "Wrong argument type. Function expected.");
        napi_create_reference(env, argv[i], 1, &asyncContext->callback[i - nonCallbackArgNum]);
    }
    return nullptr;
}

static napi_value InitAsyncPromiseEnv(const napi_env& env, AsyncContext *asyncContext, napi_value& promise)
{
    napi_deferred deferred;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
    asyncContext->deferred = deferred;
    return nullptr;
}

static napi_value DoCallBackAsyncWork(const napi_env& env, AsyncContext *asyncContext)
{
    napi_create_async_work(
        env,
        nullptr,
        asyncContext->resourceName,
        [](napi_env env, void* data) {
            if (data == nullptr) {
                HILOGE("Async data parameter is null");
                return;
            }
            AsyncContext *context = static_cast<AsyncContext *>(data);
            context->executeFunc(context);
        },
        [](napi_env env, napi_status status, void* data) {
            if (data == nullptr) {
                HILOGE("Async data parameter is null");
                return;
            }
            AsyncContext *context = static_cast<AsyncContext *>(data);
            napi_value undefine;
            napi_get_undefined(env, &undefine);
            napi_value callback;
            context->completeFunc(data);
            constexpr int ARGS_TWO = 2;
            napi_value result[ARGS_TWO] = {nullptr};
            napi_create_uint32(env, context->errorCode, &result[0]);
            result[1] = context->result;
            if (context->errorCode == ERR_CODE_SUCCESS) {
                napi_get_reference_value(env, context->callback[0], &callback);
                napi_call_function(env, nullptr, callback, ARGS_TWO, result, &undefine);
            } else {
                if (context->callback[1]) {
                    napi_get_reference_value(env, context->callback[1], &callback);
                    napi_call_function(env, nullptr, callback, ARGS_TWO, result, &undefine);
                } else {
                    HILOGE("Get callback func[1] is null");
                }
            }
            if (context->callback[0] != nullptr) {
                napi_delete_reference(env, context->callback[0]);
            }
            if (context->callback[1] != nullptr) {
                napi_delete_reference(env, context->callback[1]);
            }
            napi_delete_async_work(env, context->work);
            delete context;
        },
        (void *)asyncContext,
        &asyncContext->work);
    NAPI_CALL(env, napi_queue_async_work(env, asyncContext->work));
    return UndefinedNapiValue(env);
}

static napi_value DoPromiseAsyncWork(const napi_env& env, AsyncContext *asyncContext)
{
    napi_create_async_work(
        env,
        nullptr,
        asyncContext->resourceName,
        [](napi_env env, void *data) {
            if (data == nullptr) {
                HILOGE("Async data parameter is null");
                return;
            }
            AsyncContext *context = static_cast<AsyncContext *>(data);
            context->executeFunc(context);
        },
        [](napi_env env, napi_status status, void *data) {
            if (data == nullptr) {
                HILOGE("Async data parameter is null");
                return;
            }
            AsyncContext *context = static_cast<AsyncContext *>(data);
            context->completeFunc(data);
            if (context->errorCode == ERR_CODE_SUCCESS) {
                napi_resolve_deferred(context->env, context->deferred, context->result);
            } else {
                napi_reject_deferred(context->env, context->deferred, context->result);
            }
            napi_delete_async_work(env, context->work);
            delete context;
        },
        (void *)asyncContext,
        &asyncContext->work);
    napi_queue_async_work(env, asyncContext->work);
    return UndefinedNapiValue(env);
}

napi_value DoAsyncWork(const napi_env& env, AsyncContext *asyncContext,
    const size_t argc, const napi_value *argv, const size_t nonCallbackArgNum)
{
    if (argc > nonCallbackArgNum) {
        InitAsyncCallBackEnv(env, asyncContext, argc, argv, nonCallbackArgNum);
        return DoCallBackAsyncWork(env, asyncContext);
    } else {
        napi_value promise;
        InitAsyncPromiseEnv(env, asyncContext, promise);
        DoPromiseAsyncWork(env, asyncContext);
        return promise;
    }
}

bool ParseString(napi_env env, std::string &param, napi_value args)
{
    napi_valuetype valuetype;
    napi_typeof(env, args, &valuetype);

    HILOGI("param=%{public}d.", valuetype);
    if (valuetype != napi_string) {
        HILOGE("Wrong argument type. String expected.");
        return false;
    }
    size_t size = 0;

    if (napi_get_value_string_utf8(env, args, nullptr, 0, &size) != napi_ok) {
        HILOGE("can not get string size");
        param = "";
        return false;
    }
    param.reserve(size + 1);
    param.resize(size);
    if (napi_get_value_string_utf8(env, args, param.data(), (size + 1), &size) != napi_ok) {
        HILOGE("can not get string value");
        param = "";
        return false;
    }
    return true;
}
}  // namespace NFC
}  // namespace OHOS
