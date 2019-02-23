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

#include "af/RUBEFixture.h"
#include "Logger.h"
#include <boost/make_shared.hpp>
#include <json/json.h>

namespace af
{
    RUBEFixture::RUBEFixture(const std::string& name)
    : RUBEBase(name)
    {
    }

    RUBEFixture::~RUBEFixture()
    {
    }

    RUBEFixturePtr RUBEFixture::fromJsonValue(const Json::Value& jsonValue)
    {
        RUBEFixturePtr fixture;

        std::string name = jsonValue.get("name", "").asString();

        if (!jsonValue["circle"].isNull()) {
            RUBECircleFixturePtr circleFixture = boost::make_shared<RUBECircleFixture>(name);
            fixture = circleFixture;

            circleFixture->shape().m_radius = json2float(jsonValue["circle"]["radius"]);
            circleFixture->shape().m_p = json2vec2(jsonValue["circle"]["center"]);
        } else if (!jsonValue["edge"].isNull()) {
            RUBEEdgeFixturePtr edgeFixture = boost::make_shared<RUBEEdgeFixture>(name);
            fixture = edgeFixture;

            edgeFixture->shape().m_vertex1 = json2vec2(jsonValue["edge"]["vertex1"]);
            edgeFixture->shape().m_vertex2 = json2vec2(jsonValue["edge"]["vertex2"]);
            edgeFixture->shape().m_hasVertex0 = jsonValue["edge"].get("hasVertex0", false).asBool();
            edgeFixture->shape().m_hasVertex3 = jsonValue["edge"].get("hasVertex3", false).asBool();
            if (edgeFixture->shape().m_hasVertex0) {
                edgeFixture->shape().m_vertex0 = json2vec2(jsonValue["edge"]["vertex0"]);
            }
            if (edgeFixture->shape().m_hasVertex3) {
                edgeFixture->shape().m_vertex3 = json2vec2(jsonValue["edge"]["vertex3"]);
            }
        } else if (!jsonValue["chain"].isNull()) {
            RUBEChainFixturePtr chainFixture = boost::make_shared<RUBEChainFixture>(name);
            fixture = chainFixture;

            int numPoints = jsonValue["chain"]["vertices"]["x"].size();

            Points points(numPoints);

            for (int i = 0; i < numPoints; ++i) {
                points[i] = json2vec2(jsonValue["chain"]["vertices"], i);
            }

            chainFixture->shape().CreateChain(&points[0], numPoints);
            chainFixture->shape().m_hasPrevVertex = jsonValue["chain"].get("hasPrevVertex", false).asBool();
            chainFixture->shape().m_hasNextVertex = jsonValue["chain"].get("hasNextVertex", false).asBool();
            if (chainFixture->shape().m_hasPrevVertex) {
                chainFixture->shape().m_prevVertex = json2vec2(jsonValue["chain"]["prevVertex"]);
            }
            if (chainFixture->shape().m_hasNextVertex) {
                chainFixture->shape().m_nextVertex = json2vec2(jsonValue["chain"]["nextVertex"]);
            }
        } else if (!jsonValue["polygon"].isNull()) {
            b2Vec2 points[b2_maxPolygonVertices];

            int numPoints = jsonValue["polygon"]["vertices"]["x"].size();

            if (numPoints > b2_maxPolygonVertices) {
                LOG4CPLUS_WARN(afutil::logger(),
                               "Ignoring polygon fixture with too many points - " << numPoints);
                return RUBEFixturePtr();
            }

            if (numPoints < 2) {
                LOG4CPLUS_WARN(afutil::logger(),
                               "Ignoring polygon fixture with less than two points");
                return RUBEFixturePtr();
            }

            if (numPoints == 2) {
                LOG4CPLUS_DEBUG(afutil::logger(),
                                "Creating edge shape instead of polygon with two points");

                RUBEEdgeFixturePtr edgeFixture = boost::make_shared<RUBEEdgeFixture>(name);
                fixture = edgeFixture;

                edgeFixture->shape().m_vertex1 = json2vec2(jsonValue["polygon"]["vertices"], 0);
                edgeFixture->shape().m_vertex2 = json2vec2(jsonValue["polygon"]["vertices"], 1);
            } else {
                RUBEPolygonFixturePtr polygonFixture = boost::make_shared<RUBEPolygonFixture>(name);
                fixture = polygonFixture;

                for (int i = 0; i < numPoints; ++i) {
                    points[i] = json2vec2(jsonValue["polygon"]["vertices"], i);
                }

                polygonFixture->shape().Set(points, numPoints);
            }
        } else {
            LOG4CPLUS_WARN(afutil::logger(),
                           "Ignoring due to unknown fixture type");
            return RUBEFixturePtr();
        }

        fixture->fixtureDef().friction = json2float(jsonValue["friction"]);
        fixture->fixtureDef().restitution = json2float(jsonValue["restitution"]);
        fixture->fixtureDef().density = json2float(jsonValue["density"]);
        fixture->fixtureDef().isSensor = jsonValue.get("sensor", false).asBool();
        fixture->fixtureDef().filter.categoryBits = jsonValue.get("filter-categoryBits", 0x0001).asInt();
        fixture->fixtureDef().filter.maskBits = jsonValue.get("filter-maskBits", 0xffff).asInt();
        fixture->fixtureDef().filter.groupIndex = jsonValue.get("filter-groupIndex", 0).asInt();

        fixture->setProps(jsonValue);

        return fixture;
    }

    RUBECircleFixture::RUBECircleFixture(const std::string& name)
    : RUBEFixture(name)
    {
        fixtureDef_.shape = &shape_;
    }

    RUBECircleFixture::~RUBECircleFixture()
    {
    }

    RUBEEdgeFixture::RUBEEdgeFixture(const std::string& name)
    : RUBEFixture(name)
    {
        fixtureDef_.shape = &shape_;
    }

    RUBEEdgeFixture::~RUBEEdgeFixture()
    {
    }

    RUBEChainFixture::RUBEChainFixture(const std::string& name)
    : RUBEFixture(name)
    {
        fixtureDef_.shape = &shape_;
    }

    RUBEChainFixture::~RUBEChainFixture()
    {
    }

    RUBEPolygonFixture::RUBEPolygonFixture(const std::string& name)
    : RUBEFixture(name)
    {
        fixtureDef_.shape = &shape_;
    }

    RUBEPolygonFixture::~RUBEPolygonFixture()
    {
    }
}
