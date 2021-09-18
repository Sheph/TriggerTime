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

#ifndef _SCRIPTCONVERTERS_H_
#define _SCRIPTCONVERTERS_H_

#include "af/Types.h"
#include "af/EnumSet.h"
#include "Animation.h"
#include "AssetManager.h"
#include "Platform.h"
#include "luainc.h"
#include <luabind/luabind.hpp>

namespace
{
    static inline void call0Wrapper(const luabind::object& obj)
    {
        try
        {
            luabind::call_function<void>(obj);
        }
        catch (const luabind::error& e)
        {
            ::lua_pop(e.state(), 1);
        }
        catch (const std::exception& e)
        {
            LOG4CPLUS_ERROR(af::logger(), e.what());
        }
    }

    template <class T>
    struct basic_converter
    {
        static luabind::object to_object(lua_State* L, const T& value)
        {
            return luabind::object(L, value);
        }
    };

    template <>
    struct basic_converter<b2Vec2>
    {
        static luabind::object to_object(lua_State* L, const b2Vec2& value)
        {
            luabind::object list = luabind::newtable(L);

            list["x"] = value.x;
            list["y"] = value.y;

            luabind::setmetatable(list, luabind::globals(L)["vec2"]);

            return list;
        }
    };

    template <>
    struct basic_converter<b2Rot>
    {
        static luabind::object to_object(lua_State* L, const b2Rot& value)
        {
            luabind::object list = luabind::newtable(L);

            list["s"] = value.s;
            list["c"] = value.c;

            luabind::setmetatable(list, luabind::globals(L)["rot"]);

            return list;
        }
    };

    template <>
    struct basic_converter<b2Transform>
    {
        static luabind::object to_object(lua_State* L, const b2Transform& value)
        {
            luabind::object list = luabind::newtable(L);

            list["p"] = basic_converter<b2Vec2>::to_object(L, value.p);
            list["q"] = basic_converter<b2Rot>::to_object(L, value.q);

            luabind::setmetatable(list, luabind::globals(L)["transform"]);

            return list;
        }
    };

    template <>
    struct basic_converter<af::Color>
    {
        static luabind::object to_object(lua_State* L, const af::Color& value)
        {
            luabind::object list = luabind::newtable(L);

            list[1] = value.rgba[0];
            list[2] = value.rgba[1];
            list[3] = value.rgba[2];
            list[4] = value.rgba[3];

            return list;
        }
    };

    template <>
    struct basic_converter<af::VideoMode>
    {
        static luabind::object to_object(lua_State* L, const af::VideoMode& value)
        {
            luabind::object list = luabind::newtable(L);

            list[1] = value.width;
            list[2] = value.height;

            return list;
        }
    };

    template <class T>
    struct basic_converter< std::vector<T> >
    {
        static luabind::object to_object(lua_State* L, const std::vector<T>& value)
        {
            luabind::object list = luabind::newtable(L);

            for (int i = 0; i < value.size(); ++i) {
                list[i + 1] = basic_converter<T>::to_object(L, value[i]);
            }

            return list;
        }
    };
}

namespace luabind
{
    template <>
    struct default_converter<b2Vec2> : native_converter_base<b2Vec2>
    {
        static int compute_score(lua_State* L, int index)
        {
            return ::lua_type(L, index) == LUA_TTABLE ? 0 : -1;
        }

        b2Vec2 from(lua_State* L, int index)
        {
            luabind::object tmp(luabind::from_stack(L, index));
            return b2Vec2(luabind::object_cast<float>(tmp["x"]),
                          luabind::object_cast<float>(tmp["y"]));
        }

        void to(lua_State* L, const b2Vec2& value)
        {
            basic_converter<b2Vec2>::to_object(L, value).push(L);
        }
    };

    template <>
    struct default_converter<const b2Vec2&> : default_converter<b2Vec2>
    {};

    template <>
    struct default_converter<b2Rot> : native_converter_base<b2Rot>
    {
        static int compute_score(lua_State* L, int index)
        {
            return ::lua_type(L, index) == LUA_TTABLE ? 0 : -1;
        }

        b2Rot from(lua_State* L, int index)
        {
            luabind::object tmp(luabind::from_stack(L, index));

            b2Rot res;

            res.s = luabind::object_cast<float>(tmp["s"]);
            res.c = luabind::object_cast<float>(tmp["c"]);

            return res;
        }

        void to(lua_State* L, const b2Rot& value)
        {
            basic_converter<b2Rot>::to_object(L, value).push(L);
        }
    };

    template <>
    struct default_converter<const b2Rot&> : default_converter<b2Rot>
    {};

    template <>
    struct default_converter<b2Transform> : native_converter_base<b2Transform>
    {
        static int compute_score(lua_State* L, int index)
        {
            return ::lua_type(L, index) == LUA_TTABLE ? 0 : -1;
        }

        b2Transform from(lua_State* L, int index)
        {
            luabind::object tmp(luabind::from_stack(L, index));
            return b2Transform(luabind::object_cast<b2Vec2>(tmp["p"]),
                               luabind::object_cast<b2Rot>(tmp["q"]));
        }

        void to(lua_State* L, const b2Transform& value)
        {
            basic_converter<b2Transform>::to_object(L, value).push(L);
        }
    };

    template <>
    struct default_converter<const b2Transform&> : default_converter<b2Transform>
    {};

    template <class T>
    struct default_converter< std::vector<T> > : native_converter_base< std::vector<T> >
    {
        static int compute_score(lua_State* L, int index)
        {
            return ::lua_type(L, index) == LUA_TTABLE ? 0 : -1;
        }

        std::vector<T> from(lua_State* L, int index)
        {
            std::vector<T> list;

            for (luabind::iterator it(luabind::object(luabind::from_stack(L, index))), end;
                 it != end; ++it) {
                list.push_back(luabind::object_cast<T>(*it));
            }

            return list;
        }

        void to(lua_State* L, const std::vector<T>& value)
        {
            basic_converter< std::vector<T> >::to_object(L, value).push(L);
        }
    };

    template <class T>
    struct default_converter< const std::vector<T>& > : default_converter< std::vector<T> >
    {};

    template <>
    struct default_converter<b2AABB> : native_converter_base<b2AABB>
    {
        static int compute_score(lua_State* L, int index)
        {
            return -1;
        }

        b2AABB from(lua_State* L, int index)
        {
            throw luabind::cast_failed(L, typeid(value));
        }

        void to(lua_State* L, const b2AABB& value)
        {
            basic_converter<b2Vec2>::to_object(L, value.lowerBound).push(L);
            basic_converter<b2Vec2>::to_object(L, value.upperBound).push(L);
        }
    };

    template <>
    struct default_converter<const b2AABB&> : default_converter<b2AABB>
    {};

    template <>
    struct default_converter<af::Color> : native_converter_base<af::Color>
    {
        static int compute_score(lua_State* L, int index)
        {
            return ::lua_type(L, index) == LUA_TTABLE ? 0 : -1;
        }

        af::Color from(lua_State* L, int index)
        {
            luabind::object tmp(luabind::from_stack(L, index));
            return af::Color(luabind::object_cast<float>(tmp[1]),
                luabind::object_cast<float>(tmp[2]),
                luabind::object_cast<float>(tmp[3]),
                luabind::object_cast<float>(tmp[4]));
        }

        void to(lua_State* L, const af::Color& value)
        {
            basic_converter<af::Color>::to_object(L, value).push(L);
        }
    };

    template <>
    struct default_converter<const af::Color&> : default_converter<af::Color>
    {};

    template <>
    struct default_converter<af::VideoMode> : native_converter_base<af::VideoMode>
    {
        static int compute_score(lua_State* L, int index)
        {
            return ::lua_type(L, index) == LUA_TTABLE ? 0 : -1;
        }

        af::VideoMode from(lua_State* L, int index)
        {
            luabind::object tmp(luabind::from_stack(L, index));
            return af::VideoMode(luabind::object_cast<af::UInt32>(tmp[1]),
                luabind::object_cast<af::UInt32>(tmp[2]));
        }

        void to(lua_State* L, const af::VideoMode& value)
        {
            basic_converter<af::VideoMode>::to_object(L, value).push(L);
        }
    };

    template <>
    struct default_converter<const af::VideoMode&> : default_converter<af::VideoMode>
    {};

    template <class T>
    struct default_converter<boost::shared_ptr<T> >
      : detail::default_converter_generator<boost::shared_ptr<T> >::type
    {
        template <class U>
        int match(lua_State* L, U tmp, int index)
        {
            if (lua_isnil(L, index)) {
                return 0;
            }
            return detail::default_converter_generator<boost::shared_ptr<T> >::type::match(L, tmp, index);
        }

        template <class U>
        boost::shared_ptr<T> apply(lua_State* L, U tmp, int index)
        {
            if (lua_isnil(L, index)) {
                return boost::shared_ptr<T>();
            }

            return detail::default_converter_generator<boost::shared_ptr<T> >::type::apply(L, tmp, index);
        }

        void apply(lua_State* L, const boost::shared_ptr<T>& p)
        {
            detail::default_converter_generator<boost::shared_ptr<T> >::type::apply(L, p);
        }
    };

    template <class T>
    struct default_converter<const boost::shared_ptr<T>& >
        : detail::default_converter_generator<const boost::shared_ptr<T>& >::type
    {
        template <class U>
        int match(lua_State* L, U tmp, int index)
        {
            if (lua_isnil(L, index)) {
                return 0;
            }
            return detail::default_converter_generator<const boost::shared_ptr<T>& >::type::match(L, tmp, index);
        }

        template <class U>
        boost::shared_ptr<T> apply(lua_State* L, U tmp, int index)
        {
            if (lua_isnil(L, index)) {
                return boost::shared_ptr<T>();
            }

            return detail::default_converter_generator<const boost::shared_ptr<T>& >::type::apply(L, tmp, index);
        }

        void apply(lua_State* L, const boost::shared_ptr<T>& p)
        {
            detail::default_converter_generator<const boost::shared_ptr<T>& >::type::apply(L, p);
        }
    };

    template <>
    struct default_converter<af::Animation> : native_converter_base<af::Animation>
    {
        static int compute_score(lua_State* L, int index)
        {
            return ::lua_type(L, index) == LUA_TTABLE ? 0 : -1;
        }

        af::Animation from(lua_State* L, int index)
        {
            luabind::object tmp(luabind::from_stack(L, index));

            af::Animation animation;

            af::Texture::WrapMode wrapX = af::Texture::WrapModeRepeat;
            af::Texture::WrapMode wrapY = af::Texture::WrapModeRepeat;

            boost::optional<af::Texture::WrapMode> wm =
                luabind::object_cast_nothrow<af::Texture::WrapMode>(tmp["wrapX"]);
            if (wm) {
                wrapX = *wm;
            }

            wm = luabind::object_cast_nothrow<af::Texture::WrapMode>(tmp["wrapY"]);
            if (wm) {
                wrapY = *wm;
            }

            for (luabind::iterator it(tmp["frames"]), end; it != end; ++it) {
                animation.addFrame(
                    af::assetManager.getImage(luabind::object_cast<std::string>((*it)[1]), wrapX, wrapY),
                    luabind::object_cast<float>((*it)[2]));
            }

            boost::optional<bool> loop =
                luabind::object_cast_nothrow<bool>(tmp["loop"]);

            if (loop) {
                animation.setLoop(*loop);
            }

            return animation;
        }

        void to(lua_State* L, const af::Animation& value)
        {
            throw luabind::cast_failed(L, typeid(value));
        }
    };

    template <>
    struct default_converter<const af::Animation&> : default_converter<af::Animation>
    {};

    template <class T1, class T2>
    struct default_converter< std::pair<T1, T2> > : native_converter_base< std::pair<T1, T2> >
    {
        static int compute_score(lua_State* L, int index)
        {
            return -1;
        }

        std::pair<T1, T2> from(lua_State* L, int index)
        {
            throw luabind::cast_failed(L, typeid(value));
        }

        void to(lua_State* L, const std::pair<T1, T2>& value)
        {
            basic_converter<T1>::to_object(L, value.first).push(L);
            basic_converter<T2>::to_object(L, value.second).push(L);
        }
    };

    template <class T1, class T2>
    struct default_converter< const std::pair<T1, T2>& > : default_converter< std::pair<T1, T2> >
    {};

    template <class T, int N>
    struct default_converter< af::EnumSet<T, N> > : native_converter_base< af::EnumSet<T, N> >
    {
        static int compute_score(lua_State* L, int index)
        {
            return ::lua_type(L, index) == LUA_TTABLE ? 0 : -1;
        }

        af::EnumSet<T, N> from(lua_State* L, int index)
        {
            af::EnumSet<T, N> res;

            for (luabind::iterator it(luabind::object(luabind::from_stack(L, index))), end;
                 it != end; ++it) {
                res.set(luabind::object_cast<T>(*it));
            }

            return res;
        }

        void to(lua_State* L, const af::EnumSet<T, N>& value)
        {
            throw luabind::cast_failed(L, typeid(value));
        }
    };

    template <class T, int N>
    struct default_converter< const af::EnumSet<T, N>& > : default_converter< af::EnumSet<T, N> >
    {};

    template <>
    struct default_converter< boost::function<void()> > : native_converter_base< boost::function<void()> >
    {
        static int compute_score(lua_State* L, int index)
        {
            return ::lua_type(L, index) == LUA_TFUNCTION ? 0 : -1;
        }

        boost::function<void()> from(lua_State* L, int index)
        {
            return boost::bind(&call0Wrapper, luabind::object(luabind::from_stack(L, index)));
        }

        void to(lua_State* L, const boost::function<void()>& value)
        {
            throw luabind::cast_failed(L, typeid(value));
        }
    };

    template <>
    struct default_converter<const boost::function<void()>&> : default_converter< boost::function<void()> >
    {};
}

#endif
