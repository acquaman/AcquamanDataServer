#ifndef AMDSCLIENTUI_H
#define AMDSCLIENTUI_H

#include <QDialog>

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

class AMDSClientAppController;
class AMDSClientRequest;

class AMDSClientUi : public QDialog
{
	Q_OBJECT
public:
	explicit AMDSClientUi(QWidget *parent = 0);
	~AMDSClientUi();

private slots:
	/// connect to a server with the input hostName and portNumber
	void connectToServer();

	/// ============= SLOTs to handle AMDSClientAppController signals =========
	/// slot to handle the signal of networkSessionOpening
	void onNetworkSessionOpening();
	/// slot to handle the signal of networkSessionOpened
	void onNetworkSessionOpened();

	/// slot to handle the signal of newServerConnected (add the serverIdentifier to the combox and update the ui displays -- buffernames and active connections)
	void onNewServerConnected(QString serverIdentifier);
	/// slot to handle the signal of serverError
	void onServerError(int errorCode, QString serverIdentifier, QString errorMessage);
	/// slot to handle the signal of request data ready
	void onRequestDataReady(AMDSClientRequest* clientRequest);
	/// slot to handle the signal of socketEror
	void onSocketError(int errorCode, QString socketKey, QString errorMessage);

	/// ============= SLOTS to handle UI component signals ===============
	/// slot to handle the switch signal among connected servers (to refresh the buffer names and the active connections with that server)
	void onActiveServerChanged(QString serverIdentifier);
	/// slot to handle the switch singal among request types (to change the selection mode of the buffer names: multi selection or signal selection)
	void onRequestTypeChanged(QString);
	/// slot to check whether we should enable the button to send client request
	void enableRequestDataButton();
	/// slot to send client request to the server
	void sendClientRequest();

private:
	/// slot to reset the buffer list view
	void resetBufferListView(QStringList &bufferNames);
	/// slot to reset the acitve continuous connection combox
	void resetActiveContinuousConnection(QString serverIdentifier="");

private:
	/// ==== server section ====
	QLabel *hostLabel;
	QLabel *portLabel;
	QLineEdit *hostLineEdit;
	QLineEdit *portLineEdit;
	/// comboBox to hold the active server connections (server identifier --- hostName:portNumber
	QComboBox* activeServerComboBox;
	QLabel *statusLabel;

	/// ==== server section ====
	QPushButton *connectServerButton;
	QPushButton *requestDataButton;
	QPushButton *quitButton;

	QDialogButtonBox *buttonBox;

	/// ==== message compose section ====
	QComboBox* requestType;
	QListView *bufferNameListView_;

	QDateTimeEdit* time1Edit;
	QDateTimeEdit* time2Edit;
	QLineEdit* count1Edit;
	QLineEdit* count2Edit;
//	QSpinBox* amptekIndex;
	QCheckBox* includeStatusDataCheckbox;
	QCheckBox* enableFlattenDataCheckbox;

	QComboBox* activeContinuousConnection;
	QTextEdit* results;

	/// the handler of the clientAppController
	AMDSClientAppController *clientAppController_;
};

#endif // AMDSCLIENTUI_H
