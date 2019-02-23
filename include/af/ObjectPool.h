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

#ifndef _AF_OBJECTPOOL_H_
#define _AF_OBJECTPOOL_H_

#include "af/Types.h"
#include "af/Utils.h"
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/thread.hpp>

namespace af
{
    extern boost::mutex objectPoolMutex;
    extern size_t objectPoolMaxBytes;
    extern size_t objectPoolNumBytes;

    void objectPoolDeleted();

    /*
     * This class is thread-safe.
     */
    template <class T>
    class ObjectPool : boost::noncopyable
    {
    public:
        ObjectPool() {}

        ~ObjectPool() {}

        T* allocate()
        {
            boost::mutex::scoped_lock lock(objectPoolMutex);

            if (objects_.empty()) {
                lock.unlock();

                return new T;
            }

            T* res = objects_.pop_back().release();

            objectPoolNumBytes -= res->capacity();

            return res;
        }

        void release(T* obj)
        {
            if (!obj) {
                return;
            }

            obj->reuse();

            size_t bytes = obj->capacity();

            bool pushed = false;

            {
                boost::mutex::scoped_lock lock(objectPoolMutex);

                if ((objectPoolNumBytes + bytes) < objectPoolMaxBytes) {
                    objects_.push_back(obj);
                    pushed = true;
                    objectPoolNumBytes += bytes;
                }
            }

            if (!pushed) {
                delete obj;
                objectPoolDeleted();
            }
        }

    private:
        boost::ptr_vector<T> objects_;
    };
}

#endif
