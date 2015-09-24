#ifndef AMPTEKSDD123APPLICATION_H
#define AMPTEKSDD123APPLICATION_H

#include <QCoreApplication>
#include <QStringList>

class AmptekSDD123Application
{
public:
	static AmptekSDD123Application* amptekApp();
	static void releaseAmptekApp();

	bool debuggingEnabled() const;

protected:
	AmptekSDD123Application();

protected:
	static AmptekSDD123Application *instance_;

	QCoreApplication *coreApplication_;
	QStringList arguments_;

	bool debuggingEnabled_;
};

#endif // AMPTEKSDD123APPLICATION_H
