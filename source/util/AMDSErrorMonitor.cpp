/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "AMDSErrorMonitor.h"

#include <QMutableMapIterator>
#include <QDebug>
#include <QMutexLocker>
#include <QReadLocker>
#include <QWriteLocker>
#include <QThread>
#include <QApplication>

//#include "ui/util/AMMessageBoxWTimeout.h"

AMDSErrorMon* AMDSErrorMon::instance_ = 0;
QMutex AMDSErrorMon::instanceMutex_(QMutex::Recursive);

 AMDSErrorMon::~AMDSErrorMon(){}
AMDSErrorMon::AMDSErrorMon() : QObject(), subsMutex_(QReadWriteLock::Recursive) {
	qRegisterMetaType<AMDSErrorReport>("AMErrorReport");

	// don't display debug notifications by default:
	debugEnabled_ = false;

	// Set up our signal-based forwarding system to be thread-safe:
	connect(this, SIGNAL(reportFF(AMDSErrorReport)), this, SLOT(reportI(AMDSErrorReport)));
}


// Subscribe to all errors from object 'originator'
void AMDSErrorMon::subscribeToObjectI(const QObject* originator, QObject* notifyMe, const char* errorSlot) {
	/// \test Unit test insertions
	QWriteLocker l(&subsMutex_);
	objectSubs_.insertMulti(originator, QPair<QObject*,QString>(notifyMe, errorSlot));

}

// Subscribe to all errors from this class:
void AMDSErrorMon::subscribeToClassI(const QString& className, QObject* notifyMe, const char* errorSlot) {
	/// \test Unit test insertions
	QWriteLocker l(&subsMutex_);
	classSubs_.insertMulti(className, QPair<QObject*,QString>(notifyMe,errorSlot));
}

// Subscribe to all errors that have code 'errorCode'
void AMDSErrorMon::subscribeToCodeI(int errorCode, QObject* notifyMe, const char* errorSlot) {
	/// \test Unit test insertions
	QWriteLocker l(&subsMutex_);
	codeSubs_.insertMulti(errorCode, QPair<QObject*,QString>(notifyMe,errorSlot));
}

// Unsubscribe <notifyMe, errorSlot> from everything.  If errorSlot==0, unsubscribes all of notifyMe's subscriptions.
void AMDSErrorMon::unsubscribeI(QObject* notifyMe, const char* errorSlot) {

	QWriteLocker l(&subsMutex_);
	/// \test unit test removals (all cases)

	// Iterate through all object subscriptions
	QMutableMapIterator<const QObject*, QPair<QObject*,QString> > i(objectSubs_);
	while(i.hasNext()) {
		i.next();
		if( (notifyMe==i.value().first) && (errorSlot==0 || errorSlot==i.value().second) )
			i.remove();
	}

	// Iterate through all className subscriptions:
	QMutableMapIterator<QString, QPair<QObject*,QString> > i2(classSubs_);
	while(i2.hasNext()) {
		if( (notifyMe==i2.value().first) && (errorSlot==0 || errorSlot==i2.value().second) )
			i2.remove();
	}

	// Iterate through all errorCode subscriptions:
	QMutableMapIterator<int, QPair<QObject*,QString> > i3(codeSubs_);
	while(i3.hasNext()) {
		if( (notifyMe==i3.value().first) && (errorSlot==0 || errorSlot==i3.value().second) )
			i3.remove();
	}

}


void AMDSErrorMon::reportF(AMDSErrorReport e, bool showTimeoutMsgBox, QString msgBoxTitle) {
	Q_UNUSED(showTimeoutMsgBox)
	Q_UNUSED(msgBoxTitle)
//	if (showTimeoutMsgBox)
//		AMMessageBoxWTimeout::showMessageWTimeout(msgBoxTitle, e.description);

	QString className;
	if(e.source && e.source->metaObject()) {
		className = e.source->metaObject()->className();

		e.description.prepend(QString("in [%1]: ").arg(className));
	}

	// If this is being called from out-of-thread, we need to unset the source object so it isn't looked at in another thread. This means subscriptions (currently unused) only work within the main thread.
	if(QThread::currentThread() != this->thread()) {
		e.description.append(" [Message received from out of main thread.]");
		e.source = 0;
	}

	lastErrorCode_ = e.errorCode;
	emit reportFF(e);
}

// Handle error reports.
void AMDSErrorMon::reportI(AMDSErrorReport e) {

	QString reportMessage = QString("%1 (code %2)").arg(e.description).arg(e.errorCode);

	// Chapter 0: Throw a qDebug() for everything:
	switch(e.level) {
	case AMDSErrorReport::Debug:
		qDebug() << "Debug:" << reportMessage;
		break;
	case AMDSErrorReport::Information:
		qDebug() << "Information:" << reportMessage;
		break;
	case AMDSErrorReport::Alert:
		qDebug() << "Alert:" << reportMessage;
		break;
	case AMDSErrorReport::Serious:
		qDebug() << "Serious:" << reportMessage;
		break;
	}



	// Chapter 1: Emit signals, as long as it's not a debug message and debug is disabled.

	if( e.level == AMDSErrorReport::Debug && !debugEnabled_)
		; // do nothing
	else
		emit error(e);

	switch(e.level) {
	case AMDSErrorReport::Information:
		emit information(e);
		break;
	case AMDSErrorReport::Alert:
		emit alert(e);
		break;
	case AMDSErrorReport::Serious:
		emit serious(e);
		break;
	case AMDSErrorReport::Debug:
		if(debugEnabled_) {
			emit debug(e);
		}
		break;
	}

	// Chapter 2: go through various subscriptions:
	QReadLocker l(&subsMutex_);


	// This looks complicated.  It's a list of object pointers and corresponding slotNames that need to be called.  We fill it using the subscription records.
	QList<QPair<QObject*, QString> > targets;

	// Object subscriptions:
	targets = objectSubs_.values(e.source);
	// Classname subscriptions:
	if(e.source)
		targets.append( classSubs_.values(e.source->metaObject()->className()) );
	// Errorcode subscriptions:
	targets.append( codeSubs_.values(e.errorCode) );


	// Invoke the slot in each object (We use this to be thread-safe... The notification is delivered just like a slot call.
	// If the object is in the same thread, it's delivered right away.  If in different threads, it's queued into the event loop.
	QPair<QObject*, QString> target;
	foreach(target, targets)
		target.first->metaObject()->invokeMethod( target.first, target.second.toAscii().data(), Q_ARG(AMDSErrorReport, e));
}

AMDSErrorMon * AMDSErrorMon::mon() {
	QMutexLocker ml(&instanceMutex_);

	if(!instance_) {
		instance_ = new AMDSErrorMon();
		if(QThread::currentThread() != QApplication::instance()->thread())
			instance_->moveToThread(QApplication::instance()->thread());
	}
	return instance_;
}
 AMDSErrorReport::~AMDSErrorReport(){}
