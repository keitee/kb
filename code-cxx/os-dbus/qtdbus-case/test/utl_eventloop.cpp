//
//  utl_eventloop.cpp
//  BleRcuDaemon
//
//  Copyright © 2017 Sky UK. All rights reserved.
//

#include "utl_eventloop.h"

#include <QCoreApplication>
#include <QTimer>

#include <functional>





// -----------------------------------------------------------------------------
/*!
	Runs the event loop until the \a condition function returns \c true or if
	the function has been blocking for more than \a timeout milliseconds.

	Set \a timeout to \c -1 to disable the timeout.

	Returns \c true if the \a condition is triggered, otherwise \c false.

 */
bool processEventsUtil(const std::function<bool()> &condition, int timeout)
{
	if (timeout > 0) {

		QTimer timeOutTimer;
		timeOutTimer.setSingleShot(true);
		timeOutTimer.start(timeout);

		// add a dummy function to wake the event thread
		std::function<void()> lambda = []() {
			;
		};
		QObject::connect(&timeOutTimer, &QTimer::timeout, lambda);

		while (timeOutTimer.isActive() && !condition()) {
			QCoreApplication::processEvents();
		}

	} else {

		while (!condition()) {
			QCoreApplication::processEvents();
		}
	}

	return condition();
}

// -----------------------------------------------------------------------------
/*!
	Runs the event loop at least \a minTime milliseconds.

 */
void processEventsFor(int minTime)
{
	volatile bool done = false;

	std::function<void()> lambda = [&] {
		done = true;
	};

	QTimer::singleShot(minTime, lambda);
	while (!done) {
		QCoreApplication::processEvents();
	}
}



#define SIGNALLER_EVENT_TYPE    QEvent::Type(QEvent::User + 123)


ThreadSafeSignaller::ThreadSafeSignaller(QObject *parent)
	: QObject(parent)
	, m_count(0)
{
}

ThreadSafeSignaller::~ThreadSafeSignaller()
{
}

void ThreadSafeSignaller::trigger()
{
	QCoreApplication::postEvent(this, new QEvent(SIGNALLER_EVENT_TYPE));
}

bool ThreadSafeSignaller::event(QEvent *event)
{
	if (event && (event->type() == SIGNALLER_EVENT_TYPE)) {
		m_count++;
		return true;
	}

	return QObject::event(event);
}

bool ThreadSafeSignaller::isTriggered() const
{
	return (m_count > 0);
}

bool ThreadSafeSignaller::waitFor(int timeout)
{
	return processEventsUtil(std::bind(&ThreadSafeSignaller::isTriggered, this), timeout);
}

int ThreadSafeSignaller::count() const
{
	return m_count;
}




