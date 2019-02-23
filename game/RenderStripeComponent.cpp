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

#include "RenderStripeComponent.h"
#include "SceneObject.h"
#include "Renderer.h"
#include "Utils.h"
#include "Logger.h"

namespace af
{
    namespace
    {
        struct b2Vec2Compare : public std::binary_function<b2Vec2, b2Vec2, bool>
        {
            inline bool operator()(const b2Vec2& v1, const b2Vec2& v2) const
            {
                return (v1.x < v2.x) || ((v1.x == v2.x) && (v1.y < v2.y));
            }
        };

        bool classifyVec2(const b2Vec2& p1, const b2Vec2& p2,
                          RenderStripeComponent::Side side)
        {
            switch (side) {
            case RenderStripeComponent::SideLeft:
                return p1.x < p2.x;
            case RenderStripeComponent::SideRight:
                return p1.x > p2.x;
            case RenderStripeComponent::SideTop:
                return p1.y > p2.y;
            case RenderStripeComponent::SideBottom:
                return p1.y < p2.y;
            default:
                assert(false);
                return true;
            }
        }
    }

    static const int SideIntValues[] =
    {
        RenderStripeComponent::SideLeft,
        RenderStripeComponent::SideRight,
        RenderStripeComponent::SideTop,
        RenderStripeComponent::SideBottom
    };

    static const char* SideStrValues[] =
    {
        "left",
        "right",
        "top",
        "bottom"
    };

    AF_ENUMTRAITS_IMPL(Side, RenderStripeComponent::Side);

    Points RenderStripeComponent::tmp_;

    RenderStripeComponent::RenderStripeComponent(bool huge,
                                                 Side firstPolygonSide,
                                                 Side baseSide,
                                                 const std::vector<Points>& polygons,
                                                 const DrawablePtr& drawable,
                                                 float imageWidth,
                                                 int zOrder)
    : RenderComponent(zOrder)
    {
        create(huge, firstPolygonSide, baseSide, polygons, drawable, imageWidth);
    }

    RenderStripeComponent::RenderStripeComponent(bool huge,
                                                 Side firstPoint,
                                                 Side baseSide,
                                                 const PathPtr& path,
                                                 float radius,
                                                 float step,
                                                 const DrawablePtr& drawable,
                                                 int zOrder)
    : RenderComponent(zOrder)
    {
        std::vector<Points> polygons;
        float k = 1.0f;

        switch (baseSide) {
        case SideLeft:
            break;
        case SideRight:
            k = -1.0f;
            break;
        default:
            LOG4CPLUS_ERROR(logger(), "Bad spline stripe base side - " << baseSide);
            assert(false);
            return;
        }

        /*
         * Reserve for first closing polygon.
         */
        polygons.push_back(Points());

        PathIteratorPtr it = path->first();
        PathIteratorPtr lastIt = path->last();

        b2Vec2 first = it->current();
        b2Vec2 first2 = first;

        b2Vec2 prev = first;

        it->step(step);

        int i = 0;

        polygons.push_back(Points());

        while (it->less(lastIt)) {
            if (i > 1) {
                b2Vec2 p1 = polygons.back()[3];
                b2Vec2 p2 = polygons.back()[2];

                polygons.push_back(Points());

                polygons.back().push_back(p1);
                polygons.back().push_back(p2);
            }

            b2Vec2 cur = it->current();

            b2Vec2 other = cur - prev;
            other.Normalize();

            if (i == 0) {
                first2 = cur;
                polygons.back().push_back(prev + radius * k * other.Skew());
                polygons.back().push_back(prev - radius * k * other.Skew());
            } else {
                polygons.back().push_back(prev - radius * k * other.Skew());
                polygons.back().push_back(prev + radius * k * other.Skew());
            }

            it->step(step);
            prev = cur;
            ++i;
        }

        if (i > 1) {
            b2Vec2 p1 = polygons.back()[3];
            b2Vec2 p2 = polygons.back()[2];

            polygons.push_back(Points());

            polygons.back().push_back(p1);
            polygons.back().push_back(p2);
        }

        b2Vec2 cur = lastIt->current();

        b2Vec2 other = cur - prev;
        other.Normalize();

        if (i == 0) {
            first2 = cur;
            polygons.back().push_back(prev + radius * k * other.Skew());
            polygons.back().push_back(prev - radius * k * other.Skew());
        } else {
            polygons.back().push_back(prev - radius * k * other.Skew());
            polygons.back().push_back(prev + radius * k * other.Skew());
        }
        ++i;

        if (i > 1) {
            b2Vec2 p1 = polygons.back()[3];
            b2Vec2 p2 = polygons.back()[2];

            polygons.push_back(Points());

            polygons.back().push_back(p1);
            polygons.back().push_back(p2);
        }

        if (cur == first) {
            /*
             * FIXME: can't use 'cur', need to offset a bit, otherwise
             * 'create' will assert on adjacent edge check.
             */
            cur += 0.01f * (first2 - first);
            other = first2 - first;
            other.Normalize();
        }

        polygons.back().push_back(cur - radius * k * other.Skew());
        polygons.back().push_back(cur + radius * k * other.Skew());

        /*
         * Stripe polygons created, create closing polygons.
         */

        b2Vec2 p1 = polygons.back()[2];
        b2Vec2 p2 = polygons.back()[3];

        polygons.push_back(Points());

        polygons.back().push_back(p2);
        polygons.back().push_back(p1);
        polygons.back().push_back(0.5f * (p1 + p2) + (p2 - p1).Skew());

        p1 = polygons[1][0];
        p2 = polygons[1][1];

        polygons.front().push_back(0.5f * (p1 + p2) - (p2 - p1).Skew());
        polygons.front().push_back(p2);
        polygons.front().push_back(p1);

        if (p1.x < p2.x) {
            baseSide = SideLeft;
        } else if (p1.x == p2.x) {
            if (p1.y < p2.y) {
                baseSide = SideBottom;
            } else {
                baseSide = SideTop;
            }
        } else {
            baseSide = SideRight;
        }

        create(huge, firstPoint, baseSide, polygons, drawable,
            radius * 2.0f * drawable->image().aspect());
    }

    RenderStripeComponent::RenderStripeComponent(bool huge,
                                                 Side firstPoint,
                                                 Side baseSide,
                                                 const Points& points,
                                                 float radius,
                                                 const DrawablePtr& drawable,
                                                 int zOrder)
    : RenderComponent(zOrder)
    {
        std::vector<Points> polygons;
        float k = 1.0f;

        switch (baseSide) {
        case SideLeft:
            break;
        case SideRight:
            k = -1.0f;
            break;
        default:
            LOG4CPLUS_ERROR(logger(), "Bad line stripe base side - " << baseSide);
            assert(false);
            return;
        }

        /*
         * Reserve for first closing polygon.
         */
        polygons.push_back(Points());

        polygons.push_back(Points());

        for (size_t i = 2; i < points.size(); ++i) {
            if (i > 3) {
                b2Vec2 p1 = polygons.back()[3];
                b2Vec2 p2 = polygons.back()[2];

                polygons.push_back(Points());

                polygons.back().push_back(p1);
                polygons.back().push_back(p2);
            }

            b2Vec2 dir1 = (points[i - 1] - points[i - 2]).Skew();
            b2Vec2 dir2 = (points[i] - points[i - 1]).Skew();

            dir1.Normalize();
            dir2.Normalize();

            b2Vec2 other = dir1 + dir2;
            other.Normalize();

            if (i == 2) {
                polygons.back().push_back(points[i - 1] + ((radius * k) / b2Dot(other, dir1)) * other);
                polygons.back().push_back(points[i - 1] + ((-radius * k) / b2Dot(other, dir1)) * other);
            } else {
                b2Vec2 offset = b2Vec2_zero;

                if (points[i] == points[2]) {
                    /*
                     * FIXME: can't use 'points[i - 1]', need to offset a bit, otherwise
                     * 'create' will assert on adjacent edge check.
                     */
                    offset = points[2] - points[1];
                    offset.Normalize();
                    offset *= 0.01f;
                }

                polygons.back().push_back(points[i - 1] + offset + ((-radius * k) / b2Dot(other, dir1)) * other);
                polygons.back().push_back(points[i - 1] + offset + ((radius * k) / b2Dot(other, dir1)) * other);
            }
        }

        /*
         * Stripe polygons created, create closing polygons.
         */

        b2Vec2 p1 = polygons.back()[2];
        b2Vec2 p2 = polygons.back()[3];

        polygons.push_back(Points());

        polygons.back().push_back(p2);
        polygons.back().push_back(p1);
        polygons.back().push_back(0.5f * (p1 + p2) + (p2 - p1).Skew());

        p1 = polygons[1][0];
        p2 = polygons[1][1];

        polygons.front().push_back(0.5f * (p1 + p2) - (p2 - p1).Skew());
        polygons.front().push_back(p2);
        polygons.front().push_back(p1);

        if (p1.x < p2.x) {
            baseSide = SideLeft;
        } else if (p1.x == p2.x) {
            if (p1.y < p2.y) {
                baseSide = SideBottom;
            } else {
                baseSide = SideTop;
            }
        } else {
            baseSide = SideRight;
        }

        create(huge, firstPoint, baseSide, polygons, drawable,
            radius * 2.0f * drawable->image().aspect());
    }

    RenderStripeComponent::~RenderStripeComponent()
    {
    }

    void RenderStripeComponent::accept(ComponentVisitor& visitor)
    {
        visitor.visitRenderComponent(shared_from_this());
    }

    void RenderStripeComponent::update(float dt)
    {
        if ((prevY0_ != drawable_->image().texCoords()[1]) ||
            (prevY1_ != drawable_->image().texCoords()[5])) {
            float ymin = 1.0f;
            float ymax = 0.0f;

            for (size_t j = 1; j < polygons_[0].texCoords.size(); j += 2) {
                if (polygons_[0].texCoords[j] < ymin) {
                    ymin = polygons_[0].texCoords[j];
                }
                if (polygons_[0].texCoords[j] > ymax) {
                    ymax = polygons_[0].texCoords[j];
                }
            }

            for (size_t i = 0; i < polygons_.size(); ++i) {
                for (size_t j = 1; j < polygons_[i].texCoords.size(); j += 2) {
                    if (polygons_[i].texCoords[j] == ymin) {
                        polygons_[i].texCoords[j] = drawable_->image().texCoords()[1];
                    } else if (polygons_[i].texCoords[j] == ymax) {
                        polygons_[i].texCoords[j] = drawable_->image().texCoords()[5];
                    } else {
                        polygons_[i].texCoords[j] = 0.5f * (drawable_->image().texCoords()[1] + drawable_->image().texCoords()[5]);
                    }
                }
            }

            prevY0_ = drawable_->image().texCoords()[1];
            prevY1_ = drawable_->image().texCoords()[5];
        }

        if ((parent()->smoothPos() == prevPos_) &&
            (parent()->smoothAngle() == prevAngle_)) {
            return;
        }

        updateVertices(true);

        prevPos_ = parent()->smoothPos();
        prevAngle_ = parent()->smoothAngle();
    }

    void RenderStripeComponent::render(void* const* parts, size_t numParts)
    {
        renderer.setProgramDef(drawable_->image().texture());

        RenderSimple rop = renderer.renderTriangles();

        if (huge_) {
            for (size_t i = 0; i < numParts; ++i) {
                size_t j = reinterpret_cast<size_t>(parts[i]);

                rop.addVertices(&polygons_[j].vertices[0], polygons_[j].vertices.size() / 2);
                rop.addTexCoords(&polygons_[j].texCoords[0], polygons_[j].texCoords.size() / 2);
            }
        } else {
            for (size_t j = 0; j < polygons_.size(); ++j) {
                rop.addVertices(&polygons_[j].vertices[0], polygons_[j].vertices.size() / 2);
                rop.addTexCoords(&polygons_[j].texCoords[0], polygons_[j].texCoords.size() / 2);
            }
        }

        rop.addColors(color());
    }

    void RenderStripeComponent::onRegister()
    {
        prevPos_ = parent()->smoothPos();
        prevAngle_ = parent()->smoothAngle();
        updateVertices(false);
    }

    void RenderStripeComponent::onUnregister()
    {
        for (size_t i = 0; i < cookies_.size(); ++i) {
            manager()->removeAABB(cookies_[i]);
        }

        cookies_.clear();
    }

    void RenderStripeComponent::create(bool huge,
                                       Side firstPolygonSide,
                                       Side baseSide,
                                       const std::vector<Points>& polygons,
                                       const DrawablePtr& drawable,
                                       float imageWidth)
    {
        huge_ = huge;
        drawable_ = drawable;
        prevAngle_ = 0.0f;

        std::map< b2Vec2, std::set<size_t>, b2Vec2Compare > polygonMap;

        for (size_t i = 0; i < polygons.size(); ++i) {
            const Points& polygon = polygons[i];
            for (size_t j = 0; j < polygon.size(); ++j) {
                polygonMap[polygon[j]].insert(i);
            }
        }

        /*
         * edgeMap maps each polygon's edge to the neighbor polygon, if no
         * neighbor then value is -1. edge is defined by first polygon
         * point that belongs to that edge in the direction of polygon
         * winding. e.g if we have 2 polygons:
         *
         *     3    0      2
         *     +----+-----+
         *    /      \ 1 /
         *   /   0    \ /
         *  +----------+
         * 2           1
         *
         * edgeMap will be:
         * [0]: [0 -> 1], [1 -> -1], [2 -> -1], [3 -> -1]
         * [1]: [0 -> 0], [1 -> -1], [2 -> -1]
         */

        std::vector< std::vector<int> > edgeMap(polygons.size());

        std::vector<size_t> res;

        for (size_t i = 0; i < polygons.size(); ++i) {
            const Points& polygon = polygons[i];
            edgeMap[i].resize(polygon.size());

            for (size_t j = 0; j < polygon.size(); ++j) {
                const std::set<size_t>& s1 = polygonMap[polygon[j]];
                const std::set<size_t>& s2 = polygonMap[polygon[(j + 1) % polygon.size()]];

                res.clear();

                std::set_intersection(s1.begin(), s1.end(),
                                      s2.begin(), s2.end(),
                                      std::back_inserter(res));

                if (res.size() == 1) {
                    edgeMap[i][j] = -1;
                } else if (res.size() == 2) {
                    edgeMap[i][j] = (res[0] == i) ? res[1] : res[0];
                    if (((j > 0) && (edgeMap[i][j - 1] != -1)) ||
                        ((j == (polygon.size() - 1)) && (edgeMap[i][0] != -1))) {
                        assert(false);
                        LOG4CPLUS_ERROR(logger(), "Bad stripe, adjacent edges of a polygon can't be used to connect it to other polygons");
                        return;
                    }
                } else {
                    assert(false);
                    edgeMap[i][j] = -1;
                }
            }
        }

        /*
         * Closing polygons are the ones that only have 1
         * shared edge with other polygons, i.e. they're "closing"
         * the stripe.
         */

        std::vector<size_t> closingPolygons;

        for (size_t i = 0; i < polygons.size(); ++i) {
            const Points& polygon = polygons[i];
            int numShared = 0;

            for (size_t j = 0; j < polygon.size(); ++j) {
                if (edgeMap[i][j] != -1) {
                    ++numShared;
                }
            }

            if (numShared == 1) {
                closingPolygons.push_back(i);
            }
        }

        if (closingPolygons.size() != 2) {
            LOG4CPLUS_ERROR(logger(), "Bad stripe, " <<
                            closingPolygons.size() <<
                            " closing polygons, expected 2");
            return;
        }

        /*
         * The stripe will run from first to last.
         */

        size_t firstPolygon, lastPolygon;

        if (classifyVec2(polygons[closingPolygons[0]][0],
                         polygons[closingPolygons[1]][0],
                         firstPolygonSide)) {
            firstPolygon = closingPolygons[0];
            lastPolygon = closingPolygons[1];
        } else {
            firstPolygon = closingPolygons[1];
            lastPolygon = closingPolygons[0];
        }

        /*
         * Base line will be mapped to 0.0f, top - to 1.0f.
         */

        b2Vec2 cur;
        bool is_base = false;
        int prevPolygon = firstPolygon;

        for (int i = 0;
             i < static_cast<int>(polygons[firstPolygon].size());
             ++i) {
            int tmp = edgeMap[firstPolygon][i];
            if (tmp != -1) {
                cur = polygons[firstPolygon][i];
                is_base = classifyVec2(cur,
                   polygons[firstPolygon][(i + 1) % static_cast<int>(polygons[firstPolygon].size())],
                   baseSide);
                firstPolygon = tmp;
                break;
            }
        }

        float globalLength = 0.0f;

        while (firstPolygon != lastPolygon) {
            const Points& polygon = polygons[firstPolygon];
            int numPoints = static_cast<int>(polygon.size());
            int a1;

            for (a1 = 0; a1 < numPoints; ++a1) {
                if (polygon[a1] == cur) {
                    break;
                }
            }

            if (edgeMap[firstPolygon][a1] == prevPolygon) {
                ++a1;
                cur = polygon[a1 % numPoints];
                is_base = !is_base;
            }

            b2Vec2 prev;

            /*
             * 'A' is the polygon's point sequence that runs from
             * previous edge connection point to next edge connection point,
             * note that it's not necessarily the stripe's base, it can
             * also be stripe's top. 'is_base' is true only if 'A' is in fact
             * stripe's base.
             */

            int i = 0;
            int a2 = a1;
            float lengthA = 0.0f;

            while (true) {
                ++i;

                int nextPolygon = edgeMap[firstPolygon][a2 % numPoints];

                if (nextPolygon != -1) {
                    prevPolygon = firstPolygon;
                    firstPolygon = nextPolygon;
                    break;
                }

                prev = cur;

                cur = polygon[(++a2) % numPoints];

                lengthA += b2Distance(prev, cur);
            }

            /*
             * 'B' is the polygon point sequence opposite to 'A', i.e.
             * it's either top or base.
             */

            float lengthB = 0.0f;

            for (int j = numPoints - 1; j >= i; --j) {
                b2Vec2 p = polygon[(a2 + j - i + 1) % numPoints];

                if (j != (numPoints - 1)) {
                    lengthB += b2Distance(prev, p);
                }

                prev = p;
            }

            const b2Vec2& va1 = polygon[a1 % numPoints];
            const b2Vec2& va2 = polygon[a2 % numPoints];
            const b2Vec2& vb1 = polygon[(a1 + numPoints - 1) % numPoints];
            const b2Vec2& vb2 = polygon[(a2 + 1) % numPoints];

            /*
             * The following is a bit tricky, in order not to skew the stripe
             * texture and keep it perpendicular to base we need to handle
             * cases like this:
             *
             *     b1
             *     +---
             *    /|   ----
             *   / |       ------+ b2
             *  /  |             |\
             * +-------------------+
             * a1                  a2
             *
             * Suppose a1-a2 is base. Generating texture coordinates for a1-a2
             * is easy, since it's base, but it's much harder for top. First,
             * we need scalar projection of a1-b1 to a1-a2, this is 'ABProj',
             * we'll need to offset top's texture coordinates by that value in
             * order not to skew it, i.e. keep it perpendicular to base.
             * Second, we need scalar projection of b1-b2 to a1-a2, this is
             * 'topProj', texture coordinates for b1-b2 must be produced as if
             * its length is 'topProj', not b1-b2, this is needed in order not
             * to skew things when b1-b2 and a1-a2 are not parallel.
             */

            b2Vec2 vecA = va2 - va1;
            b2Vec2 vecB = vb2 - vb1;
            b2Vec2 vecAB = vb1 - va1;

            float ABProj, topProj;
            float vecALength = vecA.Length();
            float vecBLength = vecB.Length();

            if (is_base) {
                vecA.Normalize();
                topProj = b2Dot(vecB, vecA);
                ABProj = b2Dot(vecAB, vecA);
            } else {
                vecB.Normalize();
                topProj = b2Dot(vecA, vecB);
                ABProj = b2Dot(vecAB, vecB);
            }

            int entryOffset = (numPoints <= 4) ? 0 : 1;

            Polygon entry;

            std::vector<float> texCoords;

            entry.points.resize(numPoints + entryOffset * 2);
            texCoords.resize((numPoints + entryOffset * 2) * 2);

            float tmpLength = 0.0f;
            float localLength;

            if (is_base) {
                localLength = vecALength;
            } else {
                localLength = topProj;
            }

            for (int j = 0; j < i; ++j) {
                b2Vec2 p = polygon[(a1 + j) % numPoints];

                if (j != 0) {
                    tmpLength += b2Distance(prev, p);
                }

                float tmp = globalLength;

                if (!is_base) {
                    tmp -= ABProj;
                }

                tmp += (tmpLength * localLength) / lengthA;

                entry.points[j + entryOffset] = p;
                if (is_base) {
                    texCoords[(j + entryOffset) * 2] = tmp / imageWidth;
                    texCoords[(j + entryOffset) * 2 + 1] = drawable->image().texCoords()[1];
                } else {
                    texCoords[(j + entryOffset) * 2] = tmp / imageWidth;
                    texCoords[(j + entryOffset) * 2 + 1] = drawable->image().texCoords()[5];
                }

                prev = p;
            }

            if (is_base) {
                localLength = topProj;
            } else {
                localLength = vecBLength;
            }

            tmpLength = 0.0f;

            for (int j = numPoints - 1; j >= i; --j) {
                b2Vec2 p = polygon[(a2 + j - i + 1) % numPoints];

                if (j != (numPoints - 1)) {
                    tmpLength += b2Distance(prev, p);
                }

                float tmp = globalLength;

                if (is_base) {
                    tmp += ABProj;
                }

                tmp += (tmpLength * localLength) / lengthB;

                entry.points[(j + entryOffset)] = p;
                if (is_base) {
                    texCoords[(j + entryOffset) * 2] = tmp / imageWidth;
                    texCoords[(j + entryOffset) * 2 + 1] = drawable->image().texCoords()[5];
                } else {
                    texCoords[(j + entryOffset) * 2] = tmp / imageWidth;
                    texCoords[(j + entryOffset) * 2 + 1] = drawable->image().texCoords()[1];
                }

                prev = p;
            }

            if (numPoints > 4) {
                entry.points[numPoints + entryOffset] = entry.points[entryOffset];
                texCoords[(numPoints + entryOffset) * 2] = texCoords[entryOffset * 2 + 0];
                texCoords[(numPoints + entryOffset) * 2 + 1] = texCoords[entryOffset * 2 + 1];
            }

            /*
             * This also requires some explanation. The code here handles
             * polygons with edges > 4. Consider this polygon:
             *
             *       b1
             *        +---
             *       /    ----     b2
             *      /         -----+
             *     /       + d1     \
             * a1 +------------------+ a2
             *     \                 /
             *   c1 +---------------+ c2
             *
             * Now, this is textured as a triangle fan and the starting point
             * is one of these: a1, b1, a2, b2, i.e. edges that connect this
             * polygon to other polygons. The problem is with triangles such as
             * (a1, c2, c1). a1, c2 and c1 all must have Y texture coordinate
             * 0.0f and such triangles will be rendered as bottom texture line
             * repeated throughout all triangle - bad! The solution is to
             * introduce one more point - d1 and put it somewhere inside
             * our polygon and assign Y texture coordinate 0.5f to it. The
             * triangle fan will then come out of this point and will guarantee
             * that there will be no triangles with same Y texture coordinate.
             */

            if (is_base) {
                if (numPoints > 4) {
                    entry.points[0] = va1 + (0.5f * vecAB) + ((vecALength / 2) * vecA);
                    texCoords[0] = (globalLength + ((vecALength + ABProj) / 2)) / imageWidth;
                    texCoords[1] = 0.5f * (drawable->image().texCoords()[1] + drawable->image().texCoords()[5]);
                }

                globalLength += vecALength;
            } else {
                if (numPoints > 4) {
                    entry.points[0] = vb1 - (0.5f * vecAB) + ((vecBLength / 2) * vecB);
                    texCoords[0] = (globalLength + ((vecBLength - ABProj) / 2)) / imageWidth;
                    texCoords[1] = 0.5f * (drawable->image().texCoords()[1] + drawable->image().texCoords()[5]);
                }

                globalLength += vecBLength;
            }

            for (size_t i = 0; i < texCoords.size(); i += 2) {
                appendTriangleFan(entry.texCoords,
                                  texCoords[i], texCoords[i + 1]);
            }

            polygons_.push_back(entry);

            prevY0_ = drawable->image().texCoords()[1];
            prevY1_ = drawable->image().texCoords()[5];
        }
    }

    void RenderStripeComponent::updateVertices(bool updateAABBs)
    {
        const b2Transform& xf = parent()->getSmoothTransform();

        if (huge_) {
            for (size_t i = 0; i < polygons_.size(); ++i) {
                tmp_.clear();
                polygons_[i].vertices.clear();

                for (size_t j = 0; j < polygons_[i].points.size(); ++j) {
                    tmp_.push_back(b2Mul(xf, polygons_[i].points[j]));
                    appendTriangleFan(polygons_[i].vertices, tmp_.back().x, tmp_.back().y);
                }

                b2AABB aabb = computeAABB(&tmp_[0], tmp_.size());

                if (updateAABBs) {
                    b2Vec2 displacement = parent()->smoothPos() - prevPos_;

                    manager()->moveAABB(cookies_[i], polygons_[i].prevAABB, aabb, displacement);
                } else {
                    cookies_.push_back(manager()->addAABB(this,
                        aabb,
                        reinterpret_cast<void*>(i)));
                }

                polygons_[i].prevAABB = aabb;
            }
        } else {
            tmp_.clear();

            for (size_t i = 0; i < polygons_.size(); ++i) {
                polygons_[i].vertices.clear();

                for (size_t j = 0; j < polygons_[i].points.size(); ++j) {
                    tmp_.push_back(b2Mul(xf, polygons_[i].points[j]));
                    appendTriangleFan(polygons_[i].vertices, tmp_.back().x, tmp_.back().y);
                }
            }

            b2AABB aabb = computeAABB(&tmp_[0], tmp_.size());

            if (updateAABBs) {
                b2Vec2 displacement = parent()->smoothPos() - prevPos_;

                manager()->moveAABB(cookies_[0], prevAABB_, aabb, displacement);
            } else {
                cookies_.push_back(manager()->addAABB(this, aabb, NULL));
            }

            prevAABB_ = aabb;
        }
    }
}
