#ifndef AMDSCLIENTUI_H
#define AMDSCLIENTUI_H

#include <QDialog>

class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QNetworkSession;
class QDateTimeEdit;
class QComboBox;
class QTextEdit;
class QGridLayout;
class QFormLayout;
class QListView;
class QSpinBox;
class QWidget;
class QCheckBox;
class QItemSelection;

class AMDSCommand;
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
	/// disconnect with a server with the given server identifier
	void disconnectWithServer();

	/// ============= SLOTs to handle AMDSClientAppController signals =========
	/// slot to handle the signal of networkSessionOpening
	void onNetworkSessionOpening();
	/// slot to handle the signal of networkSessionOpened
	void onNetworkSessionOpened();

	/// slot to handle the signal of newServerConnected (add the serverIdentifier to the combox and update the ui displays -- buffernames and active connections)
	void onNewServerConnected(const QString &serverIdentifier);
	/// slot to handle the signal of request data ready
	void onRequestDataReady(AMDSClientRequest* clientRequest);
	/// slot to handle the signal of socketEror
	void onServerError(int errorCode, bool removeServer, const QString &serverIdentifier, const QString &errorMessage);

	/// ============= SLOTS to handle UI component signals ===============
	/// slot to handle the switch signal among connected servers (to refresh the buffer names and the active connections with that server)
	void onActiveServerChanged(const QString &serverIdentifier);
	/// slot to handle the switch singal among request types (to change the selection mode of the buffer names: multi selection or signal selection)
	void onRequestTypeChanged(int);
	/// slot to handle the signal when the selected buffer names is changed
	void onBufferNameSelectChanged();
	/// slot to check whether we should enable the button to send client request
	void enableRequestDataButton();
	/// slot to send client request to the server
	void sendClientRequest();

private:
	/// hide/show a widge in the formlayout
	void displayWidget(QWidget *widget, bool show=true);

	/// slot to reset the buffer list view
	void resetBufferListView(const QStringList &bufferNames);
	/// slot to update the configuration command combobox
	void resetConfigurationCommands(QList<AMDSCommand> commandDefs);
	/// slot to reset the acitve continuous connection combox
	void resetActiveContinuousConnection(const QString &serverIdentifier="");

private:
	/// ==== server section ====
	QLabel *hostLabel_;
	QLabel *portLabel_;
	QLineEdit *hostLineEdit_;
	QLineEdit *portLineEdit_;
	/// comboBox to hold the active server connections (server identifier --- hostName:portNumber
	QComboBox* activeServerComboBox_;
	QLabel *statusLabel_;

	/// ==== server section ====
	QPushButton *connectServerButton_;
	QPushButton *disconnectServerButton_;
	QPushButton *requestDataButton_;
	QPushButton *quitButton_;

	QDialogButtonBox *buttonBox_;

	/// ==== message compose section ====
	QGridLayout *mainLayout_ ;
	QFormLayout *formLayout_;
	QComboBox* requestTypeComboBox_;
	QComboBox* requestCommandComboBox_;
	QListView *bufferNameListView_;

	QDateTimeEdit* time1Edit_;
	QDateTimeEdit* time2Edit_;
	QLineEdit* count1Edit_;
	QLineEdit* count2Edit_;
//	QSpinBox* amptekIndex;
	QCheckBox* includeStatusDataCheckbox_;
	QCheckBox* enableFlattenDataCheckbox_;

	QComboBox* activeContinuousConnectionComboBox_;
	QTextEdit* resultsEdit_;

	QList<AMDSCommand> currentConfigurationCommandDefs_;
};

#endif // AMDSCLIENTUI_H
