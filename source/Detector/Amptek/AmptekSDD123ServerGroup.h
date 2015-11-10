#ifndef AMPTEKSDD123SERVERGROUP_H
#define AMPTEKSDD123SERVERGROUP_H

#include <QObject>
#include <QThread>

class QTimer;
class QSignalMapper;

class AmptekSDD123ConfigurationMap;
class AmptekSDD123Server;

class AmptekSDD123ServerGroup;

class AmptekSDD123ThreadedDataServerGroup: public QObject
{
	Q_OBJECT
public:
	/// constructor / desctructor
	AmptekSDD123ThreadedDataServerGroup(QList<AmptekSDD123ConfigurationMap*> configurationMaps, QObject *parent = 0);
	~AmptekSDD123ThreadedDataServerGroup();

	/// returns the AmptekServer at index
	AmptekSDD123Server* serverAt(int index) const;

signals:
	/// signal to indicate that a server (index) changed to configuration state, which is originated from amptekServerGroup_
	void serverChangedToConfigurationState(int index);
	/// signal to indicate that a server (index) changed to dwell state, which is originated from amptekServerGroup_
	void serverChangedToDwellState(int index);

protected:
//	/// the RW lock of the thread
//	mutable QReadWriteLock lock_;

	/// the instance of Amptek server group
	AmptekSDD123ServerGroup *amptekServerGroup_;
	/// the instance of thread
	QThread *amptekServerGroupThread_;
};

/// AmptekSDD123ServerGroup is used to manager the AmptekServers of one BL
class AmptekSDD123ServerGroup : public QObject
{
	Q_OBJECT
public:
	enum ServerState{
		ConfigurationState = 0,
		DwellState = 1
	};

	AmptekSDD123ServerGroup(QList<AmptekSDD123ConfigurationMap*> configurationMaps, QObject *parent = 0);
	virtual ~AmptekSDD123ServerGroup();

	/// returns the AmptekServer at given index
	AmptekSDD123Server* serverAt(int index) const;

public slots:
	/// slot to set the state of one server
	void setServerState(int index, AmptekSDD123ServerGroup::ServerState serverState);

	/// slot to set the configuration value
	void setConfigurationValueFrom(const QString &configurationValueCommand, int index);

signals:
	/// signal to indicate that a server (index) changed to configuration state
	void serverChangedToConfigurationState(int index);
	/// signal to indicate that a server (index) changed to dwell state
	void serverChangedToDwellState(int index);

protected slots:
	/// slot to initialize the timer to request all spectral
	void initiateAllRequestSpectrum();

	/// slot to request all the spectrual in the given request interval (masterRequestInterval_)
	void onRequestAllSpectra();

	/// slot to handle the server (at index) is going to change to ConfigurationState
	void onServerAboutToChangeToConfigurationState(int index);
	/// slot to handle the server (at index) changed to ConfigurationState
	void onServerChangedToConfigurationState(int index);
	/// slot to handle the server (at index) changed to DwellState
	void onServerChangedToDwellState(int index);

private:
	/// helper function to release the spectural timer if it is allocated
	void releaseRequestSpectrualTimer();

protected:
	/// the interval to request all the spectrual from the server
	int masterRequestInterval_;
//	/// the current state of the server group, which is NOT used and might should be removed
//	AmptekSDD123ServerGroup::ServerState serverState_;

	/// the instance of the timer to request the spectrum data
	QTimer *requestSpectraTimer_;

	/// the list of all the Amptek servers
	QList<AmptekSDD123Server*> servers_;
	/// the list of the Amptek servers in dwell mode
	QList<AmptekSDD123Server*> serversInDwellMode_;

	/// the signal mapper to handle the state change of the servers
	QSignalMapper *serverAboutToConfigurationStateMapper_;
	QSignalMapper *serverConfigurationStateMapper_;
	QSignalMapper *serverDwellStateMapper_;
};

#endif // AMPTEKSDD123SERVERGROUP_H
