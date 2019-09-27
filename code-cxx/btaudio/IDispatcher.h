/*
 * File:   IDispatcher.h
 * Author: jarek.dziedzic@bskyb.com
 *
 * Created on 26 June 2014
 *
 * Copyright (C) BSKYB 2014
 */
#ifndef IDISPATCHER_H
#define	IDISPATCHER_H

#include "Polymorphic.h"
#include <functional>
#include <thread>
#include <memory>

namespace AICommon
{

/**
 * @brief A dispatcher interface
 */
class IDispatcher : public Polymorphic
{
public:
    /**
     * Post an item of work to be executed.
     */
    virtual void post(std::function<void ()> work) = 0;

    /**
     * @brief Ensures that anything that was in the queue before the call has been
     * executed before returning.
     */
    virtual void sync() = 0;

    /**
     * @brief If it is a threaded dispatcher then the dispatcher thread id, otherwise nullptr.
     */
    virtual bool invokedFromDispatcherThread() = 0;
};

} //AICommon

#endif	/* IDISPATCHER_H */

