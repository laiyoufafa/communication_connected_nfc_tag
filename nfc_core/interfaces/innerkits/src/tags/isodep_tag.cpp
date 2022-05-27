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
#include "isodep_tag.h"

#include "loghelper.h"

namespace OHOS {
namespace NFC {
namespace KITS {
IsoDepTag::IsoDepTag(std::weak_ptr<TagInfo> tag) : BasicTagSession(tag, KITS::TagTechnology::NFC_ISODEP_TECH)
{
    if (tag.expired()) {
        DebugLog("IsoDepTag::IsoDepTag tag invalid ");
        return;
    }
    AppExecFwk::PacMap extraData = tag.lock()->GetTechExtrasData(KITS::TagTechnology::NFC_ISODEP_TECH);
    if (extraData.IsEmpty()) {
        DebugLog("IsoDepTag::IsoDepTag extra data invalid");
        return;
    }
    historicalBytes_ = tag.lock()->GetStringExtrasData(extraData, TagInfo::HISTORICAL_BYTES);
    hiLayerResponse_ = tag.lock()->GetStringExtrasData(extraData, TagInfo::HILAYER_RESPONSE);
    DebugLog("IsoDepTag::IsoDepTag historicalBytes_(%s) hiLayerResponse_(%s)",
        historicalBytes_.c_str(), hiLayerResponse_.c_str());
}

std::shared_ptr<IsoDepTag> IsoDepTag::GetTag(std::weak_ptr<TagInfo> tag)
{
    DebugLog("IsoDepTag::GetTag in");
    if (tag.expired() || !tag.lock()->IsTechSupported(KITS::TagTechnology::NFC_ISODEP_TECH)) {
        DebugLog("IsoDepTag::GetTag err");
        return nullptr;
    }

    return std::make_shared<IsoDepTag>(tag);
}

std::string IsoDepTag::GetHistoricalBytes() const
{
    return historicalBytes_;
}

std::string IsoDepTag::GetHiLayerResponse() const
{
    return hiLayerResponse_;
}

bool IsoDepTag::IsExtendedApduSupported() const
{
    OHOS::sptr<TAG::ITagSession> tagSession = GetRemoteTagSession();
    if (!tagSession) {
        return false;
    }
    return tagSession->IsSupportedApdusExtended();
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
