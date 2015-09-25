#ifndef AMPTEKSDD123SERVERGROUP_H
#define AMPTEKSDD123SERVERGROUP_H

#include <QObject>

class QTimer;
class QSignalMapper;

class AmptekSDD123ConfigurationMap;
class AmptekSDD123Server;

/// AmptekSDD123ServerGroup is used to manager the AmptekServers of one BL
class AmptekSDD123ServerGroup : public QObject
{
Q_OBJECT
public:
//	enum ServerState{
//		ConfigurationState = 0,
//		DwellState = 1
//	};

	AmptekSDD123ServerGroup(QList<AmptekSDD123ConfigurationMap*> configurationMaps, QObject *parent = 0);
	virtual ~AmptekSDD123ServerGroup();

	/// returns the list of AmptekServers
	QList<AmptekSDD123Server*> servers();

public slots:
//	/// slot to set the state of one server
//	void setServerState(int index, AmptekSDD123ServerGroup::ServerState serverState);

	/// slot to set the configuration value
	void setConfigurationValueFrom(const QString &configurationValueCommand, int index);

signals:
	/// signal to indicate that a server (index) changed to configuration state
	void serverChangedToConfigurationState(int index);
	/// signal to indicate that a server (index) changed to dwell state
	void serverChangedToDwellState(int index);

protected slots:
	void initiateAllRequestSpectrum();
	void requestAllSpectra();

	void onServerAboutToChangeToConfigurationState(int index);
	void onServerChangedToConfigurationState(int index);
	void onServerChangedToDwellState(int index);

protected:
	int masterRequestInterval_;
//	AmptekSDD123ServerGroup::ServerState serverState_;

	QTimer *requestSpectraTimer_;

	QList<AmptekSDD123Server*> servers_;
	QList<AmptekSDD123Server*> serversInDwellMode_;

	QSignalMapper *serverAboutToConfigurationStateMapper_;
	QSignalMapper *serverConfigurationStateMapper_;
	QSignalMapper *serverDwellStateMapper_;
};

#endif // AMPTEKSDD123SERVERGROUP_H
