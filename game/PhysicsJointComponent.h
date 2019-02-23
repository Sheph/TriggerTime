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

#ifndef _PHYSICSJOINTCOMPONENT_H_
#define _PHYSICSJOINTCOMPONENT_H_

#include "af/RUBEWorld.h"
#include "PhysicsComponent.h"
#include "JointProxy.h"
#include <boost/enable_shared_from_this.hpp>
#include <vector>

namespace af
{
    class PhysicsJointComponent : public boost::enable_shared_from_this<PhysicsJointComponent>,
                                  public PhysicsComponent
    {
    public:
        typedef std::vector<SceneObject*> Objects;
        typedef std::vector<JointProxyPtr> Joints;

        PhysicsJointComponent(const RUBEWorldPtr& rubeWorld,
                              const Objects& objects);
        ~PhysicsJointComponent();

        virtual ComponentPtr sharedThis() { return shared_from_this(); }

        virtual void accept(ComponentVisitor& visitor);

        template <class T>
        inline boost::shared_ptr<T> joint(const std::string& jointName) const
        {
            for (int i = 0; i < rubeWorld_->numJoints(); ++i) {
                if (rubeWorld_->joint(i)->name() == jointName) {
                    const boost::shared_ptr<T>& tmp =
                        boost::dynamic_pointer_cast<T>(joints_[i]);
                    if (tmp) {
                        return tmp;
                    } else {
                        break;
                    }
                }
            }

            return boost::shared_ptr<T>();
        }

        template <class T>
        std::vector< boost::shared_ptr<T> > joints(const std::string& jointName) const
        {
            std::vector< boost::shared_ptr<T> > res;

            for (int i = 0; i < rubeWorld_->numJoints(); ++i) {
                if (rubeWorld_->joint(i)->name() == jointName) {
                    const boost::shared_ptr<T>& tmp =
                        boost::dynamic_pointer_cast<T>(joints_[i]);
                    if (tmp) {
                        res.push_back(tmp);
                    }
                }
            }

            return res;
        }

        inline const Joints& joints() const { return joints_; }

        inline Joints script_getJoints() const { return joints_; }

    private:
        virtual void onRegister();

        virtual void onUnregister();

        RUBEWorldPtr rubeWorld_;
        Objects objects_;
        Joints joints_;
    };

    typedef boost::shared_ptr<PhysicsJointComponent> PhysicsJointComponentPtr;
}

#endif
