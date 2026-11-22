/*
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
 * Copyright (C) 2022-2023 The LineageOS Project
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Changes from Qualcomm Innovation Center are provided under the following license:
 * Copyright (c) 2022-2023 Qualcomm Innovation Center, Inc. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */

/*
 * [ofox-haptics] Port untuk OrangeFox fox_12.1 (Android 12) dari
 * LineageOS hardware/xiaomi/vibrator/effect/effect.cpp (libqtivibratoreffect.xiaomi):
 * waveform efek dibaca dari /vendor/etc/vibrator/effect_<id>.bin (sampel int8, 24 kHz)
 * dan dikirim ke driver qcom-hv-haptics sebagai FIFO stream (USE_EFFECT_STREAM).
 * Tanpa std::filesystem supaya aman dengan libc++ Android 12.
 *
 * - efek tanpa file    -> pakai Click (seperti versi Xiaomi)
 * - Double click (1) tanpa file -> Click diputar dua kali
 * - Click pun tidak ada -> NULL, driver memakai efek bawaannya sendiri
 */

#include <stdio.h>

#include <algorithm>
#include <map>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include "effect.h"

namespace {

const uint32_t kDefaultPlayRateHz = 24000;
const uint32_t kPrimitiveMask = (1 << 15);
const uint32_t kEffectClick = 0;        // aidl Effect::CLICK
const uint32_t kEffectDoubleClick = 1;  // aidl Effect::DOUBLE_CLICK
const long kMaxFileSize = 64 * 1024;

std::mutex sLock;
std::map<uint32_t, effect_stream> sEffectStreams;            // node map: pointer tetap valid
std::map<uint32_t, std::vector<int8_t>> sEffectFifoData;

bool readFile(const std::string& path, std::vector<int8_t>& out) {
    FILE* f = fopen(path.c_str(), "rbe");
    if (f == nullptr) return false;
    bool ok = false;
    if (fseek(f, 0, SEEK_END) == 0) {
        long size = ftell(f);
        if (size > 0 && size <= kMaxFileSize && fseek(f, 0, SEEK_SET) == 0) {
            out.resize(static_cast<size_t>(size));
            ok = fread(out.data(), 1, out.size(), f) == out.size();
        }
    }
    fclose(f);
    return ok;
}

const effect_stream* store(uint32_t uniqueEffectId, std::vector<int8_t>&& fifoData) {
    std::vector<int8_t>& data = sEffectFifoData[uniqueEffectId];
    data = std::move(fifoData);
    effect_stream& s = sEffectStreams[uniqueEffectId];
    s.effect_id = uniqueEffectId & ~kPrimitiveMask;
    s.length = static_cast<uint32_t>(data.size());
    s.play_rate_hz = kDefaultPlayRateHz;
    s.data = data.data();
    return &s;
}

const effect_stream* readEffectStreamFromFile(uint32_t uniqueEffectId) {
    const uint32_t effectId = uniqueEffectId & ~kPrimitiveMask;
    const std::string path = ((uniqueEffectId & kPrimitiveMask) != 0)
                                     ? "/vendor/etc/vibrator/primitive_effect_" + std::to_string(effectId) + ".bin"
                                     : "/vendor/etc/vibrator/effect_" + std::to_string(effectId) + ".bin";
    std::vector<int8_t> fifoData;
    if (!readFile(path, fifoData)) return nullptr;
    return store(uniqueEffectId, std::move(fifoData));
}

const effect_stream* duplicateEffect(const effect_stream* effectStream, uint32_t newEffectId) {
    const uint32_t newLength = effectStream->length * 4;
    std::vector<int8_t> fifoData(newLength, 0);
    std::copy(effectStream->data, effectStream->data + effectStream->length, fifoData.begin());
    std::copy(effectStream->data, effectStream->data + effectStream->length,
              fifoData.begin() + (newLength - effectStream->length));
    return store(newEffectId, std::move(fifoData));
}

const effect_stream* getLocked(uint32_t effectId) {
    auto it = sEffectStreams.find(effectId);
    if (it != sEffectStreams.end()) return &it->second;

    const effect_stream* stream = readEffectStreamFromFile(effectId);
    if (stream != nullptr) return stream;

    if (effectId == kEffectDoubleClick) {
        const effect_stream* click = getLocked(kEffectClick);
        return click != nullptr ? duplicateEffect(click, kEffectDoubleClick) : nullptr;
    }
    if (effectId != kEffectClick) return getLocked(kEffectClick);
    return nullptr;
}

}  // namespace

const struct effect_stream* get_effect_stream(uint32_t effect_id) {
    std::lock_guard<std::mutex> lock(sLock);
    return getLocked(effect_id);
}
