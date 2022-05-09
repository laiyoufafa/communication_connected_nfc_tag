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
#ifndef I_NFC_SERVICE_H
#define I_NFC_SERVICE_H

#include "infcc_host.h"
#include "tag_dispatcher.h"

namespace OHOS {
namespace NFC {
class INfcService {
public:
    virtual ~INfcService() {}

    virtual bool IsNfcEnabled() = 0;
    virtual std::weak_ptr<NFC::NCI::INfccHost> GetNfccHost() = 0;
    virtual std::weak_ptr<TAG::TagDispatcher> GetTagDispatcher() = 0;

private:
};
}  // namespace NFC
}  // namespace OHOS
#endif  // I_NFC_SERVICE_H
