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

#ifndef _AF_RUBEFIXTURE_H_
#define _AF_RUBEFIXTURE_H_

#include "af/RUBEBase.h"
#include <boost/shared_ptr.hpp>

namespace af
{
    class RUBEFixture;
    typedef boost::shared_ptr<RUBEFixture> RUBEFixturePtr;

    class RUBEFixture : public RUBEBase
    {
    public:
        explicit RUBEFixture(const std::string& name);
        virtual ~RUBEFixture();

        static RUBEFixturePtr fromJsonValue(const Json::Value& jsonValue);

        inline b2FixtureDef& fixtureDef() { return fixtureDef_; }

        virtual b2Shape& shape() = 0;

    protected:
        b2FixtureDef fixtureDef_;
    };

    class RUBECircleFixture : public RUBEFixture
    {
    public:
        explicit RUBECircleFixture(const std::string& name);
        ~RUBECircleFixture();

        virtual b2CircleShape& shape() { return shape_; }

    private:
        b2CircleShape shape_;
    };

    typedef boost::shared_ptr<RUBECircleFixture> RUBECircleFixturePtr;

    class RUBEEdgeFixture : public RUBEFixture
    {
    public:
        explicit RUBEEdgeFixture(const std::string& name);
        ~RUBEEdgeFixture();

        virtual b2EdgeShape& shape() { return shape_; }

    private:
        b2EdgeShape shape_;
    };

    typedef boost::shared_ptr<RUBEEdgeFixture> RUBEEdgeFixturePtr;

    class RUBEChainFixture : public RUBEFixture
    {
    public:
        explicit RUBEChainFixture(const std::string& name);
        ~RUBEChainFixture();

        virtual b2ChainShape& shape() { return shape_; }

    private:
        b2ChainShape shape_;
    };

    typedef boost::shared_ptr<RUBEChainFixture> RUBEChainFixturePtr;

    class RUBEPolygonFixture : public RUBEFixture
    {
    public:
        explicit RUBEPolygonFixture(const std::string& name);
        ~RUBEPolygonFixture();

        virtual b2PolygonShape& shape() { return shape_; }

    private:
        b2PolygonShape shape_;
    };

    typedef boost::shared_ptr<RUBEPolygonFixture> RUBEPolygonFixturePtr;
}

#endif
