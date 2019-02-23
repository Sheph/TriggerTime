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

#include "af/StreamAppConfig.h"
#include "Logger.h"
#include <boost/property_tree/ini_parser.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/tokenizer.hpp>
#include <set>

namespace af
{
    static const char* yesStrs[] = {"yes", "true", "1", "y", 0};
    static const char* noStrs[] = {"no", "false", "0", "n", 0};

    static int strcmpCI(const char* s1, const char* s2)
    {
#ifdef _WIN32
        return ::_stricmp(s1, s2);
#else
        return ::strcasecmp(s1, s2);
#endif
    }

    static boost::optional<bool> convertToBool(const std::string& value)
    {
        for (const char** i = yesStrs; *i != 0; ++i) {
            if (strcmpCI(value.c_str(), *i) == 0) {
                return true;
            }
        }

        for (const char** i = noStrs; *i != 0; ++i) {
            if (strcmpCI(value.c_str(), *i) == 0) {
                return false;
            }
        }

        return boost::optional<bool>();
    }

    StreamAppConfig::StreamAppConfig()
    {
    }

    StreamAppConfig::~StreamAppConfig()
    {
    }

    bool StreamAppConfig::load(std::istream& is)
    {
        try
        {
            boost::property_tree::ini_parser::read_ini(is, tree_);
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    std::string StreamAppConfig::getLoggerConfig() const
    {
        boost::optional<const boost::property_tree::ptree&> res =
            tree_.get_child_optional("log4cplus");

        if (!res) {
            return "";
        }

        std::ostringstream os;

        for (boost::property_tree::ptree::const_iterator it = res->begin();
             it != res->end();) {
            os << "log4cplus." << it->first << "=" << it->second.data();

            ++it;

            if (it != res->end()) {
                os << "\n";
            }
        }

        return os.str();
    }

    bool StreamAppConfig::haveKey(const std::string& key) const
    {
        boost::optional<const boost::property_tree::ptree&> tmp = findKey(key);

        return !!tmp;
    }

    std::vector<std::string> StreamAppConfig::getSubKeys(const std::string& key) const
    {
        std::vector<std::string> res;

        if (key.empty()) {
            /*
             * Return all section names.
             */

            for (boost::property_tree::ptree::const_iterator it = tree_.begin();
                 it != tree_.end();
                 ++it ) {
                if (it->second.empty()) {
                    continue;
                }

                if (it->first.find('.') == std::string::npos) {
                    /*
                     * Section names with '.' in name are ignored.
                     */

                    res.push_back(it->first);
                }
            }

            return res;
        }

        std::string::size_type pos = key.find('.');

        std::string sectionName;
        std::string path;

        if (pos == std::string::npos) {
            sectionName = key;
        } else {
            sectionName = key.substr(0, pos);
            path = key.substr(pos + 1);
        }

        const boost::property_tree::ptree* section;

        if (sectionName.empty()) {
            /*
             * Walk top level keys and match against 'path'.
             */

            section = &tree_;
        } else {
            /*
             * Walk the specific section and match against 'path'.
             */

            boost::optional<const boost::property_tree::ptree&> tmp =
                tree_.get_child_optional(sectionName);

            if (!tmp || tmp->empty()) {
                /*
                 * No such section or it's a top level key.
                 */

                return res;
            }

            section = &*tmp;
        }

        std::set<std::string> keySet;

        if (!path.empty()) {
            path += '.';
        }

        for (boost::property_tree::ptree::const_iterator it = section->begin();
             it != section->end();
             ++it ) {
            if (!it->second.empty()) {
                continue;
            }

            if (it->first.compare(0, path.length(), path) != 0) {
                continue;
            }

            pos = it->first.find('.', path.length());

            std::string tmp;

            if (pos == std::string::npos) {
                tmp = it->first.substr(path.length());
            } else {
                tmp = it->first.substr(path.length(), pos - path.length());
            }

            if (tmp.empty()) {
                continue;
            }

            if (keySet.insert(tmp).second) {
                res.push_back(tmp);
            }
        }

        return res;
    }

    std::string StreamAppConfig::getString(const std::string& key) const
    {
        boost::optional<const boost::property_tree::ptree&> tmp = findKey(key);

        if (!tmp) {
            LOG4CPLUS_ERROR(afutil::logger(),
                            "Config value for \"" << key << "\" not found");

            return "";
        }

        return tmp->data();
    }

    int StreamAppConfig::getStringIndex(const std::string& key,
                                        const std::vector<std::string>& allowed) const
    {
        boost::optional<const boost::property_tree::ptree&> tmp = findKey(key);

        if (!tmp) {
            LOG4CPLUS_ERROR(afutil::logger(),
                            "Config value for \"" << key << "\" not found");

            return 0;
        }

        for (int i = 0; i < static_cast<int>(allowed.size()); ++i) {
            if (allowed[i] == tmp->data()) {
                return i;
            }
        }

        if (afutil::logger().isEnabledFor(log4cplus::ERROR_LOG_LEVEL)) {
            std::ostringstream os;

            for (std::vector<std::string>::const_iterator it = allowed.begin();
                 it != allowed.end();) {
                os << "\"" << *it << "\"";

                ++it;

                if (it != allowed.end()) {
                    os << ", ";
                }
            }

            LOG4CPLUS_ERROR(afutil::logger(),
                            "Config value for \"" << key << "\" = \""
                            << tmp->data() << "\" must be one of these: " << os.str());
        }

        return 0;
    }

    int StreamAppConfig::getInt(const std::string& key) const
    {
        boost::optional<const boost::property_tree::ptree&> tmp = findKey(key);

        if (!tmp) {
            LOG4CPLUS_ERROR(afutil::logger(),
                            "Config value for \"" << key << "\" not found");

            return 0;
        }

        boost::optional<int> val = tmp->get_value_optional<int>();

        if (!val) {
            LOG4CPLUS_ERROR(afutil::logger(),
                            "Config value for \"" << key << "\" = \""
                            << tmp->data() << "\" cannot be converted to int");

            return 0;
        }

        return *val;
    }

    float StreamAppConfig::getFloat(const std::string& key) const
    {
        boost::optional<const boost::property_tree::ptree&> tmp = findKey(key);

        if (!tmp) {
            LOG4CPLUS_ERROR(afutil::logger(),
                            "Config value for \"" << key << "\" not found");

            return 0.0f;
        }

        boost::optional<float> val = tmp->get_value_optional<float>();

        if (!val) {
            LOG4CPLUS_ERROR(afutil::logger(),
                            "Config value for \"" << key << "\" = \""
                            << tmp->data() << "\" cannot be converted to float");

            return 0.0f;
        }

        return *val;
    }

    bool StreamAppConfig::getBool(const std::string& key) const
    {
        boost::optional<const boost::property_tree::ptree&> tmp = findKey(key);

        if (!tmp) {
            LOG4CPLUS_ERROR(afutil::logger(),
                            "Config value for \"" << key << "\" not found");

            return false;
        }

        boost::optional<bool> val = convertToBool(tmp->data());

        if (!val) {
            LOG4CPLUS_ERROR(afutil::logger(),
                            "Config value for \"" << key << "\" = \""
                            << tmp->data() << "\" is not a correct boolean");
            return false;
        }

        return *val;
    }

    Color StreamAppConfig::getColor(const std::string& key) const
    {
        boost::optional<const boost::property_tree::ptree&> tmp = findKey(key);

        if (!tmp) {
            LOG4CPLUS_ERROR(afutil::logger(),
                            "Config value for \"" << key << "\" not found");

            return Color();
        }

        std::vector<float> arr;

        boost::tokenizer<boost::char_separator<char> > tokens(tmp->data(),
            boost::char_separator<char>(","));

        for (boost::tokenizer<boost::char_separator<char> >::iterator it = tokens.begin();
             it != tokens.end();
             ++it) {
            std::string item = *it;

            boost::algorithm::trim(item);

            std::istringstream is(item);

            float v;

            if (!(is >> v) || !is.eof()) {
                LOG4CPLUS_ERROR(afutil::logger(),
                                "Config value for \"" << key << "\" = \""
                                << tmp->data() << "\" is not a correct color");

                return Color();
            }

            arr.push_back(v);
        }

        if ((arr.size() != 3) && (arr.size() != 4)) {
            LOG4CPLUS_ERROR(afutil::logger(),
                            "Config value for \"" << key << "\" = \""
                            << tmp->data() << "\" is not a correct color");

            return Color();
        }

        float a = (arr.size() == 4) ? arr[3] : 1.0f;

        return Color(arr[0], arr[1], arr[2], a);
    }

    b2Vec2 StreamAppConfig::getVec2(const std::string& key) const
    {
        boost::optional<const boost::property_tree::ptree&> tmp = findKey(key);

        if (!tmp) {
            LOG4CPLUS_ERROR(afutil::logger(),
                            "Config value for \"" << key << "\" not found");

            return b2Vec2_zero;
        }

        std::vector<float> arr;

        boost::tokenizer<boost::char_separator<char> > tokens(tmp->data(),
            boost::char_separator<char>(","));

        for (boost::tokenizer<boost::char_separator<char> >::iterator it = tokens.begin();
             it != tokens.end();
             ++it) {
            std::string item = *it;

            boost::algorithm::trim(item);

            std::istringstream is(item);

            float v;

            if (!(is >> v) || !is.eof()) {
                LOG4CPLUS_ERROR(afutil::logger(),
                                "Config value for \"" << key << "\" = \""
                                << tmp->data() << "\" is not a correct 2D vector");

                return b2Vec2_zero;
            }

            arr.push_back(v);
        }

        if (arr.size() != 2) {
            LOG4CPLUS_ERROR(afutil::logger(),
                            "Config value for \"" << key << "\" = \""
                            << tmp->data() << "\" is not a correct 2D vector");

            return b2Vec2_zero;
        }

        return b2Vec2(arr[0], arr[1]);
    }

    boost::optional<const boost::property_tree::ptree&> StreamAppConfig::findKey(const std::string& key) const
    {
        if (key.empty()) {
            /*
             * Bad key.
             */

            return boost::optional<const boost::property_tree::ptree&>();
        }

        std::string::size_type pos = key.find('.');

        std::string sectionName;
        std::string path;

        if (pos == std::string::npos) {
            sectionName = key;
        } else {
            sectionName = key.substr(0, pos);
            path = key.substr(pos + 1);
        }

        const boost::property_tree::ptree* section;

        if (sectionName.empty()) {
            /*
             * Find in top level keys.
             */

            section = &tree_;
        } else {
            /*
             * Find in specific section.
             */

            boost::optional<const boost::property_tree::ptree&> tmp =
                tree_.get_child_optional(sectionName);

            if (!tmp || tmp->empty()) {
                return boost::optional<const boost::property_tree::ptree&>();
            }

            section = &*tmp;
        }

        boost::property_tree::ptree::const_assoc_iterator it =
            section->find(path);

        if ((it == section->not_found()) || !it->second.empty()) {
            return boost::optional<const boost::property_tree::ptree&>();
        }

        return it->second;
    }
}
