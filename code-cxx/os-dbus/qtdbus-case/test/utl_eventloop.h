//
//  utl_eventloop.h
//  BleRcuDaemon
//
//  Copyright © 2017 Sky UK. All rights reserved.
//

#ifndef UTL_EVENTLOOP_H
#define UTL_EVENTLOOP_H

// #include "future.h"

#include <QObject>
#include <QAtomicInteger>

#include <functional>



class ThreadSafeSignaller : public QObject
{
public:
	ThreadSafeSignaller(QObject *parent = nullptr);
	~ThreadSafeSignaller();

	bool event(QEvent *event) override;

public:
	void trigger();

	bool isTriggered() const;
	int count() const;

	bool waitFor(int timeout = 5000);

private:
	QAtomicInteger<int> m_count;
};


bool processEventsUtil(const std::function<bool()> &condition, int timeout = -1);

void processEventsFor(int minTime);



// template<class T>
// static inline bool waitForFuture(const Future<T> &future, int timeout = 5000);

// static inline bool waitForFuture(const Future<void> &future, int timeout = 5000);

// #include "utl_eventloop.tpp"


#endif // !defined(UTL_EVENTLOOP_H)
