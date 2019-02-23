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

#ifndef _SENSORLISTENER_H_
#define _SENSORLISTENER_H_

#include "af/Types.h"
#include "SceneObject.h"
#include "Logger.h"
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace af
{
    class SensorListener : boost::noncopyable
    {
    public:
        SensorListener() : useCount_(0), ended_(false) {}
        virtual ~SensorListener() {}

        virtual void sensorEnter(const SceneObjectPtr& other) = 0;

        virtual void sensorExit(const SceneObjectPtr& other) = 0;

        /*
         * We need this incUseCount, decUseCount, endUse crap because of
         * laubind C++ shared_ptr class reference problems. Currently we store 'self_'
         * member in lua derived classes (see ScriptSensorListener.h for explanation),
         * however, this is not enough, because this causes reference loop and
         * lua derived objects that are instantiated during the level are never
         * freed (well, they get freed after level is unloaded). To solve this we
         * now set 'self_' to empty whenever C++ no longer needs to call into lua.
         * Here, we use local ref count to find out if it's safe to set 'self_' to
         * empty in ScriptSensorListener class. The similar stuff is done for other
         * lua derived classes, but since they're all component classes it's
         * trivial there, just reset 'self_' after 'onUnregister' call had been
         * processed by the script.
         */

        inline void incUseCount()
        {
            assert(!ended_);
            if (ended_) {
                LOG4CPLUS_ERROR(logger(), "reuse of sensor listeners is not supported!");
            }
            ++useCount_;
        }

        inline void decUseCount()
        {
            --useCount_;
            if (useCount_ <= 0) {
                assert(useCount_ == 0);
                ended_ = true;
                endUse();
            }
        }

    protected:
        virtual void endUse() {}

    private:
        int useCount_;
        bool ended_;
    };

    typedef boost::shared_ptr<SensorListener> SensorListenerPtr;
}

#endif
