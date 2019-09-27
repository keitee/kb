/*
 * File:   Observer.h
 * Author: jarek.dziedzic@bskyb.com
 *
 * Created on 26 June 2014
 *
 * Copyright (C) BSKYB 2014
 */

#ifndef OBSERVER_H
#define	OBSERVER_H

#include <Common/Polymorphic.h>
// #include "Polymorphic.h"

namespace AICommon
{
    
/**
 * @brief A template for observing objects that accept signals defined in T.
 *
 * @note Arguably you could inherit directly from T, however inheriting from
 *       Observer<T> is more intention revealing than inheriting from T.
 *       There is no extra overhead because of Empty Base Class Optimisation.
 */
template <class T>
class Observer : public T, virtual public Polymorphic
{
};

} //AICommon

#endif	/* OBSERVER_H */
