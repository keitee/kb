/*
 * File:   CallerThreadDispatcher.h
 * Author: jarek.dziedzic@bskyb.com
 *
 * Created on 26 June 2014
 *
 * Copyright (C) BSKYB 2014
 */

#ifndef CALLERTHREADDISPATCHER_H
#define	CALLERTHREADDISPATCHER_H

#include "IDispatcher.h"
#include <memory>

namespace AICommon
{
/**
 * @brief A dispatcher that does all the work immediately on the thread that calls post.
 */
class CallerThreadDispatcher : public IDispatcher
{
public:
    virtual void post(std::function<void ()> work) final
    {
        work();
    }

    virtual void sync() final {}

    virtual bool invokedFromDispatcherThread() final
    {
        return false;
    }
};

} //AICommon

#endif	/* CALLERTHREADDISPATCHER_H */

