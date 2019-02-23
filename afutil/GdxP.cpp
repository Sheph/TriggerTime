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

#include "af/GdxP.h"
#include "af/Utils.h"
#include "Logger.h"
#include <boost/algorithm/string.hpp>
#include <boost/make_shared.hpp>
#include <sstream>
#include <log4cplus/ndc.h>

namespace af
{
    /*
     * GdxPReader.
     */

    GdxPReader::GdxPReader(const std::string& str)
    : str_(str),
      i_(0),
      lineno_(0)
    {
    }

    GdxPReader::~GdxPReader()
    {
    }

    std::string GdxPReader::readLine()
    {
        if (eof()) {
            throw GdxPReaderException("End of file reached");
        }

        std::string::size_type pos = str_.find_first_of("\r\n", i_);

        std::string res;

        if (pos == std::string::npos) {
            res = str_.substr(i_);
            i_ = str_.size();
        } else {
            res = str_.substr(i_, pos - i_);
            pos = str_.find_first_not_of("\r\n", pos);
            if (pos == std::string::npos) {
                i_ = str_.size();
            } else {
                i_ = pos;
            }
        }

        ++lineno_;

        return res;
    }

    std::string GdxPReader::readString(const std::string& name)
    {
        std::string line = readLine();

        std::string::size_type pos = line.find(':');

        if (pos == std::string::npos) {
            throw GdxPReaderException("\":\" expected");
        }

        std::string key = line.substr(0, pos);

        if (key != name) {
            throw GdxPReaderException(std::string("\"") + name + ":\" expected");
        }

        std::string value = line.substr(pos + 1);

        boost::trim(value);

        return value;
    }

    bool GdxPReader::readBool(const std::string& name)
    {
        std::string str = readString(name);

        if (str == "true") {
            return true;
        } else if (str == "false") {
            return false;
        } else {
            throw GdxPReaderException("boolean value expected");
        }
    }

    UInt32 GdxPReader::readUInt32(const std::string& name)
    {
        UInt32 value;

        std::istringstream is(readString(name));

        if ((is >> value) && is.eof()) {
            return value;
        }

        throw GdxPReaderException("UInt32 value expected");
    }

    float GdxPReader::readFloat(const std::string& name)
    {
        float value;

        std::istringstream is(readString(name));

        if ((is >> value) && is.eof()) {
            return value;
        }

        throw GdxPReaderException("float value expected");
    }

    bool GdxPReader::eof() const
    {
        return i_ == str_.size();
    }

    /*
     * GdxPValue.
     */

    GdxPValue::GdxPValue()
    : active_(false),
      alwaysActive_(false)
    {
    }

    GdxPValue::~GdxPValue()
    {
    }

    void GdxPValue::load(GdxPReader& reader)
    {
        if (!alwaysActive_) {
            active_ = reader.readBool("active");
        } else {
            active_ = true;
        }
    }

    /*
     * GdxPNumericValue.
     */

    GdxPNumericValue::GdxPNumericValue()
    : value_(0.0f)
    {
    }

    GdxPNumericValue::~GdxPNumericValue()
    {
    }

    void GdxPNumericValue::load(GdxPReader& reader)
    {
        GdxPValue::load(reader);
        if (!active()) {
            return;
        }
        value_ = reader.readFloat("value");
    }

    /*
     * GdxPRangedNumericValue.
     */

    GdxPRangedNumericValue::GdxPRangedNumericValue()
    : lowMin_(0.0f),
      lowMax_(0.0f)
    {
    }

    GdxPRangedNumericValue::~GdxPRangedNumericValue()
    {
    }

    float GdxPRangedNumericValue::newLowValue() const
    {
        return getRandom(lowMin_, lowMax_);
    }

    void GdxPRangedNumericValue::setLow(float v)
    {
        lowMin_ = v;
        lowMax_ = v;
    }

    void GdxPRangedNumericValue::setLow(float minVal, float maxVal)
    {
        lowMin_ = minVal;
        lowMax_ = maxVal;
    }

    void GdxPRangedNumericValue::load(GdxPReader& reader)
    {
        GdxPValue::load(reader);
        if (!active()) {
            return;
        }
        lowMin_ = reader.readFloat("lowMin");
        lowMax_ = reader.readFloat("lowMax");
    }

    /*
     * GdxPScaledNumericValue.
     */

    GdxPScaledNumericValue::GdxPScaledNumericValue()
    : highMin_(0.0f),
      highMax_(0.0f),
      relative_(false)
    {
        scaling_.push_back(1.0f);
        timeline_.push_back(0.0f);
    }

    GdxPScaledNumericValue::~GdxPScaledNumericValue()
    {
    }

    float GdxPScaledNumericValue::newHighValue() const
    {
        return getRandom(highMin_, highMax_);
    }

    void GdxPScaledNumericValue::setHigh(float v)
    {
        highMin_ = v;
        highMax_ = v;
    }

    void GdxPScaledNumericValue::setHigh(float minVal, float maxVal)
    {
        highMin_ = minVal;
        highMax_ = maxVal;
    }

    float GdxPScaledNumericValue::getScale(float percent) const
    {
        int endIndex = -1;

        for (size_t i = 1; i < timeline_.size(); ++i) {
            float t = timeline_[i];
            if (t > percent) {
                endIndex = i;
                break;
            }
        }

        if (endIndex == -1) {
            return scaling_[timeline_.size() - 1];
        }

        int startIndex = endIndex - 1;
        float startValue = scaling_[startIndex];
        float startTime = timeline_[startIndex];

        return startValue +
               (scaling_[endIndex] - startValue) *
               ((percent - startTime) / (timeline_[endIndex] - startTime));
    }

    void GdxPScaledNumericValue::load(GdxPReader& reader)
    {
        GdxPRangedNumericValue::load(reader);
        if (!active()) {
            return;
        }
        highMin_ = reader.readFloat("highMin");
        highMax_ = reader.readFloat("highMax");
        relative_ = reader.readBool("relative");
        scaling_.resize(reader.readUInt32("scalingCount"));
        for (size_t i = 0; i < scaling_.size(); ++i) {
            std::ostringstream os;
            os << "scaling" << i;
            scaling_[i] = reader.readFloat(os.str());
        }
        timeline_.resize(reader.readUInt32("timelineCount"));
        for (size_t i = 0; i < timeline_.size(); ++i) {
            std::ostringstream os;
            os << "timeline" << i;
            timeline_[i] = reader.readFloat(os.str());
        }
    }

    /*
     * GdxPGradientColorValue.
     */

    GdxPGradientColorValue::GdxPGradientColorValue()
    {
        setAlwaysActive(true);
        colors_.push_back(1.0f);
        colors_.push_back(1.0f);
        colors_.push_back(1.0f);
        timeline_.push_back(0.0f);
    }

    GdxPGradientColorValue::~GdxPGradientColorValue()
    {
    }

    Color GdxPGradientColorValue::getColor(float percent) const
    {
        int startIndex = 0, endIndex = -1;

        for (size_t i = 1; i < timeline_.size(); ++i) {
            float t = timeline_[i];
            if (t > percent) {
                endIndex = i;
                break;
            }
            startIndex = i;
        }
        float startTime = timeline_[startIndex];
        startIndex *= 3;
        float r1 = colors_[startIndex];
        float g1 = colors_[startIndex + 1];
        float b1 = colors_[startIndex + 2];

        if (endIndex == -1) {
            return Color(r1, g1, b1);
        }

        float factor = (percent - startTime) / (timeline_[endIndex] - startTime);
        endIndex *= 3;

        return Color(r1 + (colors_[endIndex] - r1) * factor,
                     g1 + (colors_[endIndex + 1] - g1) * factor,
                     b1 + (colors_[endIndex + 2] - b1) * factor);
    }

    void GdxPGradientColorValue::load(GdxPReader& reader)
    {
        GdxPValue::load(reader);
        if (!active()) {
            return;
        }
        colors_.resize(reader.readUInt32("colorsCount"));
        for (size_t i = 0; i < colors_.size(); ++i) {
            std::ostringstream os;
            os << "colors" << i;
            colors_[i] = reader.readFloat(os.str());
        }
        timeline_.resize(reader.readUInt32("timelineCount"));
        for (size_t i = 0; i < timeline_.size(); ++i) {
            std::ostringstream os;
            os << "timeline" << i;
            timeline_[i] = reader.readFloat(os.str());
        }
    }

    /*
     * GdxPSpawnShapeValue.
     */

    GdxPSpawnShapeValue::GdxPSpawnShapeValue()
    : shape_(GdxPSpawnShapePoint),
      side_(GdxPSpawnEllipseBoth),
      edges_(false)
    {
    }

    GdxPSpawnShapeValue::~GdxPSpawnShapeValue()
    {
    }

    void GdxPSpawnShapeValue::load(GdxPReader& reader)
    {
        GdxPValue::load(reader);
        if (!active()) {
            return;
        }
        std::string tmp = reader.readString("shape");
        if (tmp == "point") {
            shape_ = GdxPSpawnShapePoint;
        } else if (tmp == "line") {
            shape_ = GdxPSpawnShapeLine;
        } else if (tmp == "square") {
            shape_ = GdxPSpawnShapeSquare;
        } else if (tmp == "ellipse") {
            shape_ = GdxPSpawnShapeEllipse;
        } else {
            throw GdxPReaderException("shape type expected");
        }
        if (shape_ == GdxPSpawnShapeEllipse) {
            edges_ = reader.readBool("edges");
            tmp = reader.readString("side");
            if (tmp == "both") {
                side_ = GdxPSpawnEllipseBoth;
            } else if (tmp == "top") {
                side_ = GdxPSpawnEllipseTop;
            } else if (tmp == "bottom") {
                side_ = GdxPSpawnEllipseBottom;
            } else {
                throw GdxPReaderException("side type expected");
            }
        }
    }

    /*
     * GdxP.
     */

    GdxP::GdxP()
    {
    }

    GdxP::~GdxP()
    {
    }

    GdxPPtr GdxP::fromStream(const std::string& fileName, std::istream& is)
    {
        std::string str;

        {
            log4cplus::NDCContextCreator ndc(fileName);

            if (!is) {
                LOG4CPLUS_ERROR(afutil::logger(), "Cannot open file");

                return GdxPPtr();
            }

            if (!readStream(is, str)) {
                LOG4CPLUS_ERROR(afutil::logger(), "Error reading file");

                return GdxPPtr();
            }
        }

        return fromString(fileName, str);
    }

    GdxPPtr GdxP::fromString(const std::string& fileName, const std::string& str)
    {
        log4cplus::NDCContextCreator ndc(fileName);

        LOG4CPLUS_DEBUG(afutil::logger(), "Processing...");

        GdxPPtr gdxp = boost::make_shared<GdxP>();

        GdxPReader reader(str);

        try {
            while (!reader.eof()) {
                GdxPEntry entry;

                entry.name = reader.readLine();
                reader.readLine();
                entry.delay.load(reader);
                reader.readLine();
                entry.duration.load(reader);
                reader.readLine();
                entry.minCount = reader.readUInt32("min");
                entry.maxCount = reader.readUInt32("max");
                reader.readLine();
                entry.emission.load(reader);
                reader.readLine();
                entry.life.load(reader);
                reader.readLine();
                entry.lifeOffset.load(reader);
                reader.readLine();
                entry.xOffset.load(reader);
                reader.readLine();
                entry.yOffset.load(reader);
                reader.readLine();
                entry.spawnShape.load(reader);
                reader.readLine();
                entry.spawnWidth.load(reader);
                reader.readLine();
                entry.spawnHeight.load(reader);
                reader.readLine();
                entry.scale.load(reader);
                reader.readLine();
                entry.velocity.load(reader);
                reader.readLine();
                entry.angle.load(reader);
                reader.readLine();
                entry.rotation.load(reader);
                reader.readLine();
                entry.wind.load(reader);
                reader.readLine();
                entry.gravity.load(reader);
                reader.readLine();
                entry.tint.load(reader);
                reader.readLine();
                entry.transparency.load(reader);
                reader.readLine();
                entry.attached = reader.readBool("attached");
                entry.continuous = reader.readBool("continuous");
                entry.aligned = reader.readBool("aligned");
                entry.additive = reader.readBool("additive");
                entry.behind = reader.readBool("behind");
                reader.readLine();
                entry.imageFileName = reader.readLine();

                gdxp->addEntry(entry);
            }
        } catch (const GdxPReaderException& e) {
            LOG4CPLUS_ERROR(afutil::logger(), "line #" << reader.lineno() << " - " << e.what());

            return GdxPPtr();
        }

        return gdxp;
    }

    void GdxP::addEntry(const GdxPEntry& entry)
    {
        entries_.push_back(entry);
    }
}
