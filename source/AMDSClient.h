#ifndef CLIENT_H
#define CLIENT_H

#include <QDialog>
#include <QTcpSocket>
#include <QVector>
class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTcpSocket;
class QNetworkSession;
class QDateTimeEdit;
class QTextEdit;
class QComboBox;
class QSpinBox;
class QCheckBox;
class Client : public QDialog
{
	Q_OBJECT

public:
	Client(QWidget *parent = 0);

private slots:
	void requestNewFortune();
	void readFortune();
	void displayError(QAbstractSocket::SocketError socketError);
	void enableGetFortuneButton();
	void sessionOpened();
	void requestData();

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
	QTextEdit* results;
	QComboBox* requestType;
	QSpinBox* amptekIndex;
	QComboBox *bufferNameComboBox_;
	QCheckBox* includeStatusDataCheckbox;
	QTcpSocket *tcpSocket;
	QString currentFortune;
	quint32 blockSize;

	QNetworkSession *networkSession;
};

#endif
