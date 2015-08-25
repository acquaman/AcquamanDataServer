#ifndef CLIENT_H
#define CLIENT_H

#include <QDialog>
#include <QTcpSocket>
#include <QVector>

class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QNetworkSession;
class QDateTimeEdit;
class QTextEdit;
class QComboBox;
class QListView;
class QSpinBox;
class QCheckBox;

class AMDSClientTCPSocket;
class AMDSClientRequest;

class AMDSClient : public QDialog
{
	Q_OBJECT

public:
	AMDSClient(QWidget *parent = 0);

private slots:
	void requestNewFortune();
	void onSocketDataReady(AMDSClientTCPSocket* socket, AMDSClientRequest *clientRequest);

	void onSocketError(AMDSClientTCPSocket *clientTCPSocket, QAbstractSocket::SocketError socketError);
	void onRequestTypeChanged(QString);
	void enableGetFortuneButton();
	void sessionOpened();

private:
	void removeTCPSocket(AMDSClientTCPSocket *clientTCPSocket);

private:
	QLabel *hostLabel;
	QLabel *portLabel;
	QLineEdit *hostLineEdit;
	QLineEdit *portLineEdit;
	QLabel *statusLabel;
	QPushButton *getFortuneButton;
	QPushButton *quitButton;
	QDialogButtonBox *buttonBox;
	QLineEdit* count1Edit;
	QLineEdit* count2Edit;
	QDateTimeEdit* time1Edit;
	QDateTimeEdit* time2Edit;
	QComboBox* activeContinuousConnection;
	QTextEdit* results;
	QComboBox* requestType;
	QSpinBox* amptekIndex;
	QListView *bufferNameListView_;
	QCheckBox* includeStatusDataCheckbox;

	QString currentFortune;
	quint32 blockSize;

	QNetworkSession *networkSession;
};

#endif
