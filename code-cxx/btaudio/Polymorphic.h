/*
 * File:   Polymorphic.h
 * Author: jarek.dziedzic@bskyb.com
 *
 * Created on 26 June 2014
 *
 * Copyright (C) BSKYB 2014
 */

#ifndef POLYMORPHIC_H
#define	POLYMORPHIC_H

namespace AICommon
{

/**
 * @brief Inherit from this from all types that have virtual functions.
 *
 * @note Doing so ensures that you have virtual destructor and saves you nasty surprises.
 */
class Polymorphic
{
public:
    virtual ~Polymorphic() {};
};

} //AICommon
#endif	/* POLYMORPHIC_H */

