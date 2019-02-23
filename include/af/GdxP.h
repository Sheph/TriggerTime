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

#ifndef _AF_GDXP_H_
#define _AF_GDXP_H_

#include "af/Types.h"
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <vector>
#include <exception>
#include <iostream>

namespace af
{
    class GdxP;
    typedef boost::shared_ptr<GdxP> GdxPPtr;

    enum GdxPSpawnShape
    {
        GdxPSpawnShapePoint = 0,
        GdxPSpawnShapeLine,
        GdxPSpawnShapeSquare,
        GdxPSpawnShapeEllipse
    };

    enum GdxPSpawnEllipseSide
    {
        GdxPSpawnEllipseBoth = 0,
        GdxPSpawnEllipseTop,
        GdxPSpawnEllipseBottom,
    };

    class GdxPReaderException : public std::exception
    {
    public:
        explicit GdxPReaderException(const std::string& msg)
        : msg_(msg)
        {
        }

        virtual ~GdxPReaderException() throw()
        {
        }

        virtual const char* what() const throw ()
        {
            return msg_.c_str();
        }

    private:
        std::string msg_;
    };

    class GdxPReader : boost::noncopyable
    {
    public:
        explicit GdxPReader(const std::string& str);
        ~GdxPReader();

        std::string readLine();

        std::string readString(const std::string& name);

        bool readBool(const std::string& name);

        UInt32 readUInt32(const std::string& name);

        float readFloat(const std::string& name);

        bool eof() const;

        inline int lineno() const { return lineno_; }

    private:
        std::string str_;
        std::string::size_type i_;
        int lineno_;
    };

    class GdxPValue
    {
    public:
        GdxPValue();
        virtual ~GdxPValue();

        inline bool alwaysActive() const { return alwaysActive_; }
        inline void setAlwaysActive(bool v) { alwaysActive_ = v; }

        inline bool active() const { return alwaysActive_ || active_; }
        inline void setActive(bool v) { active_ = v; }

        virtual void load(GdxPReader& reader);

    private:
        bool active_;
        bool alwaysActive_;
    };

    class GdxPNumericValue : public GdxPValue
    {
    public:
        GdxPNumericValue();
        ~GdxPNumericValue();

        inline float value() const { return value_; }
        inline void setValue(float v) { value_ = v; }

        virtual void load(GdxPReader& reader);

    private:
        float value_;
    };

    class GdxPRangedNumericValue : public GdxPValue
    {
    public:
        GdxPRangedNumericValue();
        virtual ~GdxPRangedNumericValue();

        float newLowValue() const;

        void setLow(float v);
        void setLow(float minVal, float maxVal);

        inline float lowMin() const { return lowMin_; }
        inline void setLowMin(float v) { lowMin_ = v; }

        inline float lowMax() const { return lowMax_; }
        inline void setLowMax(float v) { lowMax_ = v; }

        virtual void load(GdxPReader& reader);

    private:
        float lowMin_;
        float lowMax_;
    };

    class GdxPScaledNumericValue : public GdxPRangedNumericValue
    {
    public:
        GdxPScaledNumericValue();
        ~GdxPScaledNumericValue();

        float newHighValue() const;

        void setHigh(float v);
        void setHigh(float minVal, float maxVal);

        inline float highMin() const { return highMin_; }
        inline void setHighMin(float v) { highMin_ = v; }

        inline float highMax() const { return highMax_; }
        inline void setHighMax(float v) { highMax_ = v; }

        inline const std::vector<float>& scaling() const { return scaling_; };
        inline void setScaling(const std::vector<float>& v) { scaling_ = v; };

        inline const std::vector<float>& timeline() const { return timeline_; };
        inline void setTimeline(const std::vector<float>& v) { timeline_ = v; };

        inline bool relative() const { return relative_; }
        inline void setRelative(bool v) { relative_ = v; }

        float getScale(float percent) const;

        virtual void load(GdxPReader& reader);

    private:
        float highMin_;
        float highMax_;
        std::vector<float> scaling_;
        std::vector<float> timeline_;
        bool relative_;
    };

    class GdxPGradientColorValue : public GdxPValue
    {
    public:
        GdxPGradientColorValue();
        ~GdxPGradientColorValue();

        inline const std::vector<float>& colors() const { return colors_; };
        inline void setColors(const std::vector<float>& v) { colors_ = v; };

        inline const std::vector<float>& timeline() const { return timeline_; };
        inline void setTimeline(const std::vector<float>& v) { timeline_ = v; };

        Color getColor(float percent) const;

        virtual void load(GdxPReader& reader);

    private:
        std::vector<float> colors_;
        std::vector<float> timeline_;
    };

    class GdxPSpawnShapeValue : public GdxPValue
    {
    public:
        GdxPSpawnShapeValue();
        ~GdxPSpawnShapeValue();

        inline GdxPSpawnShape shape() const { return shape_; }
        inline void setShape(GdxPSpawnShape v) { shape_ = v; }

        inline GdxPSpawnEllipseSide side() const { return side_; }
        inline void setSide(GdxPSpawnEllipseSide v) { side_ = v; }

        inline bool edges() const { return edges_; }
        inline void setEdges(bool v) { edges_ = v; }

        virtual void load(GdxPReader& reader);

    private:
        GdxPSpawnShape shape_;
        GdxPSpawnEllipseSide side_;
        bool edges_;
    };

    struct GdxPEntry
    {
        GdxPEntry()
        : minCount(0),
          maxCount(4),
          attached(false),
          continuous(false),
          aligned(false),
          additive(true),
          behind(false)
        {
            duration.setAlwaysActive(true);
            emission.setAlwaysActive(true);
            life.setAlwaysActive(true);
            spawnShape.setAlwaysActive(true);
            spawnWidth.setAlwaysActive(true);
            spawnHeight.setAlwaysActive(true);
            scale.setAlwaysActive(true);
            transparency.setAlwaysActive(true);
        }

        std::string name;
        GdxPRangedNumericValue delay;
        GdxPRangedNumericValue duration;
        int minCount;
        int maxCount;
        GdxPScaledNumericValue emission;
        GdxPScaledNumericValue life;
        GdxPScaledNumericValue lifeOffset;
        GdxPRangedNumericValue xOffset;
        GdxPRangedNumericValue yOffset;
        GdxPSpawnShapeValue spawnShape;
        GdxPScaledNumericValue spawnWidth;
        GdxPScaledNumericValue spawnHeight;
        GdxPScaledNumericValue scale;
        GdxPScaledNumericValue velocity;
        GdxPScaledNumericValue angle;
        GdxPScaledNumericValue rotation;
        GdxPScaledNumericValue wind;
        GdxPScaledNumericValue gravity;
        GdxPGradientColorValue tint;
        GdxPScaledNumericValue transparency;
        bool attached;
        bool continuous;
        bool aligned;
        bool additive;
        bool behind;
        std::string imageFileName;
    };

    class GdxP : boost::noncopyable
    {
    public:
        GdxP();
        ~GdxP();

        static GdxPPtr fromStream(const std::string& fileName, std::istream& is);

        static GdxPPtr fromString(const std::string& fileName, const std::string& str);

        void addEntry(const GdxPEntry& entry);

        inline int numEntries() const { return entries_.size(); }
        inline const GdxPEntry& entry(int i) const { return entries_[i]; }

    private:
        typedef std::vector<GdxPEntry> Entries;

        Entries entries_;
    };
}

#endif
