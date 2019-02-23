/*
 * Copyright (c) 2014, Stanislav Vorobiov
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "af/SequentialAppConfig.h"

namespace af
{
    SequentialAppConfig::SequentialAppConfig()
    {
    }

    SequentialAppConfig::~SequentialAppConfig()
    {
    }

    void SequentialAppConfig::add(const AppConfigPtr& config)
    {
        configs_.push_back(config);
    }

    std::string SequentialAppConfig::getLoggerConfig() const
    {
        for (size_t i = configs_.size() - 1; i > 0; --i) {
            std::string res = configs_[i]->getLoggerConfig();
            if (!res.empty()) {
                return res;
            }
        }
        return configs_[0]->getLoggerConfig();
    }

    bool SequentialAppConfig::haveKey(const std::string& key) const
    {
        for (size_t i = configs_.size() - 1; i > 0; --i) {
            if (configs_[i]->haveKey(key)) {
                return true;
            }
        }
        return configs_[0]->haveKey(key);
    }

    std::vector<std::string> SequentialAppConfig::getSubKeys(const std::string& key) const
    {
        for (size_t i = configs_.size() - 1; i > 0; --i) {
            std::vector<std::string> res = configs_[i]->getSubKeys(key);
            if (!res.empty()) {
                return res;
            }
        }
        return configs_[0]->getSubKeys(key);
    }

    std::string SequentialAppConfig::getString(const std::string& key) const
    {
        for (size_t i = configs_.size() - 1; i > 0; --i) {
            if (configs_[i]->haveKey(key)) {
                return configs_[i]->getString(key);
            }
        }
        return configs_[0]->getString(key);
    }

    int SequentialAppConfig::getStringIndex(const std::string& key,
        const std::vector<std::string>& allowed) const
    {
        for (size_t i = configs_.size() - 1; i > 0; --i) {
            if (configs_[i]->haveKey(key)) {
                return configs_[i]->getStringIndex(key, allowed);
            }
        }
        return configs_[0]->getStringIndex(key, allowed);
    }

    int SequentialAppConfig::getInt(const std::string& key) const
    {
        for (size_t i = configs_.size() - 1; i > 0; --i) {
            if (configs_[i]->haveKey(key)) {
                return configs_[i]->getInt(key);
            }
        }
        return configs_[0]->getInt(key);
    }

    float SequentialAppConfig::getFloat(const std::string& key) const
    {
        for (size_t i = configs_.size() - 1; i > 0; --i) {
            if (configs_[i]->haveKey(key)) {
                return configs_[i]->getFloat(key);
            }
        }
        return configs_[0]->getFloat(key);
    }

    bool SequentialAppConfig::getBool(const std::string& key) const
    {
        for (size_t i = configs_.size() - 1; i > 0; --i) {
            if (configs_[i]->haveKey(key)) {
                return configs_[i]->getBool(key);
            }
        }
        return configs_[0]->getBool(key);
    }

    Color SequentialAppConfig::getColor(const std::string& key) const
    {
        for (size_t i = configs_.size() - 1; i > 0; --i) {
            if (configs_[i]->haveKey(key)) {
                return configs_[i]->getColor(key);
            }
        }
        return configs_[0]->getColor(key);
    }

    b2Vec2 SequentialAppConfig::getVec2(const std::string& key) const
    {
        for (size_t i = configs_.size() - 1; i > 0; --i) {
            if (configs_[i]->haveKey(key)) {
                return configs_[i]->getVec2(key);
            }
        }
        return configs_[0]->getVec2(key);
    }
}
