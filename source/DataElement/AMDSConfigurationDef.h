#ifndef AMDSCONFIGURATIONDEF_H
#define AMDSCONFIGURATIONDEF_H

class AMDSServerConfiguration
{
public:
	AMDSServerConfiguration()
	{
		serverName_ = "invalid";
	}

	AMDSServerConfiguration(const QString &serverName, const QString &ipAddress, quint16 port)
	{
		serverName_ = serverName;
		ipAddress_ = ipAddress;
		port_ = port;
	}

	inline QString serverName() const { return serverName_; }
	inline QString ipAddress() const { return ipAddress_; }
	inline quint16 port() const { return port_; }
	inline QString serverIdentifier() const { return QString("%1:%2").arg(ipAddress_).arg(port_); }

protected:
	QString serverName_;
	QString ipAddress_;
	quint16 port_;

};

#endif // AMDSCONFIGURATIONDEF_H
