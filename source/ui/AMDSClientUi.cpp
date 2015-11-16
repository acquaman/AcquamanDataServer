#include "AMDSClientUi.h"

#include <QtGui>
#include <QtNetwork>
#include <QDateTimeEdit>
#include <QDialog>

#include "appController/AMDSClientAppController.h"
#include "Connection/AMDSServer.h"
#include "ClientRequest/AMDSClientRequest.h"
#include "ClientRequest/AMDSClientIntrospectionRequest.h"
#include "ClientRequest/AMDSClientConfigurationRequest.h"
#include "DataElement/AMDSCommandManager.h"
#include "util/AMErrorMonitor.h"
#include "util/AMDSRunTimeSupport.h"

AMDSClientUi::AMDSClientUi(QWidget *parent) :
    QDialog(parent)
{
	/// ==== server section ====
	hostLabel_ = new QLabel(tr("&Server name:"));
	portLabel_ = new QLabel(tr("&erver port:"));

	hostLineEdit_ = new QLineEdit("10.45.0.180");
	portLineEdit_ = new QLineEdit("28044");
	portLineEdit_->setValidator(new QIntValidator(1, 65535, this));

	hostLabel_->setBuddy(hostLineEdit_);
	portLabel_->setBuddy(portLineEdit_);

	activeServerComboBox_ = new QComboBox;
	activeServerComboBox_->setFixedWidth(200);
	activeServerComboBox_->addItem("");
	connect(activeServerComboBox_, SIGNAL(currentIndexChanged(QString)), this, SLOT(onActiveServerChanged(QString)));

	statusLabel_ = new QLabel(tr("This examples requires that you run the Acquaman Data Server example as well."));

	/// ==== buttons section ====
	connectServerButton_ = new QPushButton(tr("Connect to Server"));
	connectServerButton_->setDefault(true);

	disconnectServerButton_ = new QPushButton(tr("Disconnect"));
	disconnectServerButton_->setDefault(true);

	requestDataButton_ = new QPushButton(tr("Request Data"));
	quitButton_ = new QPushButton(tr("Quit"));

	buttonBox_ = new QDialogButtonBox;
	buttonBox_->addButton(connectServerButton_, QDialogButtonBox::ActionRole);
	buttonBox_->addButton(disconnectServerButton_, QDialogButtonBox::ActionRole);
	buttonBox_->addButton(requestDataButton_, QDialogButtonBox::ActionRole);
	buttonBox_->addButton(quitButton_, QDialogButtonBox::RejectRole);

	connect(connectServerButton_, SIGNAL(clicked()), this, SLOT(connectToServer()));
	connect(disconnectServerButton_, SIGNAL(clicked()), this, SLOT(disconnectWithServer()));
	connect(requestDataButton_, SIGNAL(clicked()), this, SLOT(sendClientRequest()));
	connect(quitButton_, SIGNAL(clicked()), this, SLOT(close()));

	/// ==== message compose section ====
	requestTypeComboBox_ = new QComboBox;
	requestTypeComboBox_->addItem("Introspect");
	requestTypeComboBox_->addItem("Statistics");
	requestTypeComboBox_->addItem("Start Time + Count");
	requestTypeComboBox_->addItem("Relative Count + Count");
	requestTypeComboBox_->addItem("Start Time to End Time");
	requestTypeComboBox_->addItem("Middle Time + Count Before to Count After");
	requestTypeComboBox_->addItem("Continuous");
	requestTypeComboBox_->addItem("Configuration");
	requestTypeComboBox_->setCurrentIndex(0);

	const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(requestTypeComboBox_->model());
	for(int x = 1; x < AMDSClientRequestDefinitions::InvalidRequest; x++){
		QStandardItem* item = model->item(x);
		item->setFlags(item->flags() & ~(Qt::ItemIsSelectable|Qt::ItemIsEnabled));
		// visually disable by greying out - works only if combobox has been painted already and palette returns the wanted color
		item->setData(requestTypeComboBox_->palette().color(QPalette::Disabled, QPalette::Text), Qt::TextColorRole);
	}

	bufferNameListView_ = new QListView();
	bufferNameListView_->setEditTriggers(QAbstractItemView::NoEditTriggers);
	QStringList bufferNames("All");
	resetBufferListView(bufferNames);

	time1Edit_ = new QDateTimeEdit(QDateTime::currentDateTimeUtc());
	time1Edit_->setDisplayFormat("HH:mm:ss.zzz");
	time2Edit_ = new QDateTimeEdit(QDateTime::currentDateTimeUtc());
	time2Edit_->setDisplayFormat("HH:mm:ss.zzz");
	count1Edit_ = new QLineEdit();
	count2Edit_ = new QLineEdit();
	includeStatusDataCheckbox_ = new QCheckBox();
	enableFlattenDataCheckbox_ = new QCheckBox();

	activeContinuousConnectionComboBox_ = new QComboBox;
	activeContinuousConnectionComboBox_->setFixedWidth(200);
	activeContinuousConnectionComboBox_->addItem("");

	resultsEdit_ = new QTextEdit();

	connect(requestTypeComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onRequestTypeChanged(int)));


//	amptekIndex = new QSpinBox();

	requestCommandComboBox_ = new QComboBox;

	/// ==== layout the components ====
	mainLayout_ = new QGridLayout;
	mainLayout_->addWidget(hostLabel_, 0, 0);
	mainLayout_->addWidget(hostLineEdit_, 0, 1);
	mainLayout_->addWidget(portLabel_, 1, 0);
	mainLayout_->addWidget(portLineEdit_, 1, 1);
	mainLayout_->addWidget(new QLabel("Active Servers"), 2, 0);
	mainLayout_->addWidget(activeServerComboBox_, 2, 1);
	mainLayout_->addWidget(statusLabel_, 3, 0, 1, 2);
	mainLayout_->addWidget(buttonBox_, 4, 0, 1, 2);

	formLayout_ = new QFormLayout();
	formLayout_->addRow("Request Type", requestTypeComboBox_);
	formLayout_->addRow("Buffer", bufferNameListView_);
	formLayout_->addRow("Request Command", requestCommandComboBox_);
	formLayout_->addRow("Time1", time1Edit_);
	formLayout_->addRow("Time2", time2Edit_);
	formLayout_->addRow("Count1", count1Edit_);
	formLayout_->addRow("Count2", count2Edit_);
	formLayout_->addRow("Include Status", includeStatusDataCheckbox_);
	formLayout_->addRow("Enable Flattening", enableFlattenDataCheckbox_);
	formLayout_->addRow("Active continuous connection", activeContinuousConnectionComboBox_);

	mainLayout_->addLayout(formLayout_, 5, 0, 1, 2);
	mainLayout_->addWidget(resultsEdit_, 6, 0, 1, 2);
	setLayout(mainLayout_);

	setWindowTitle(tr("AMDS Client Example"));
	portLineEdit_->setFocus();

	connect(bufferNameListView_->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onBufferNameSelectChanged()));

	/// ==== initialize the app controller ==============
	AMDSClientAppController *clientAppController = AMDSClientAppController::clientAppController();
	connect(clientAppController, SIGNAL(networkSessionOpening()), this, SLOT(onNetworkSessionOpening()));
	connect(clientAppController, SIGNAL(networkSessionOpened()), this, SLOT(onNetworkSessionOpened()));
	connect(clientAppController, SIGNAL(newServerConnected(QString)), this, SLOT(onNewServerConnected(QString)));
	connect(clientAppController, SIGNAL(serverError(int,bool,QString,QString)), this, SLOT(onServerError(int,bool,QString,QString)));
	connect(clientAppController, SIGNAL(requestDataReady(AMDSClientRequest*)), this, SLOT(onRequestDataReady(AMDSClientRequest*)));

	onRequestTypeChanged(0);
}

AMDSClientUi::~AMDSClientUi()
{
	AMDSClientAppController *clientAppController = AMDSClientAppController::clientAppController();
	disconnect(clientAppController, SIGNAL(networkSessionOpening()), this, SLOT(onNetworkSessionOpening()));
	disconnect(clientAppController, SIGNAL(networkSessionOpened()), this, SLOT(onNetworkSessionOpened()));
	disconnect(clientAppController, SIGNAL(newServerConnected(QString)), this, SLOT(onNewServerConnected(QString)));
	disconnect(clientAppController, SIGNAL(serverError(int,bool,QString,QString)), this, SLOT(onServerError(int,bool,QString,QString)));
	disconnect(clientAppController, SIGNAL(requestDataReady(AMDSClientRequest*)), this, SLOT(onRequestDataReady(AMDSClientRequest*)));

	AMDSClientAppController::releaseAppController();

	QAbstractItemModel *currentListModel = bufferNameListView_->model();
	if (currentListModel)
		currentListModel->deleteLater();
}

void AMDSClientUi::connectToServer()
{
	QString hostName = hostLineEdit_->text();
	quint16 portNumber = portLineEdit_->text().toInt();

	AMDSClientAppController::clientAppController()->connectToServer(hostName, portNumber);
}

void AMDSClientUi::disconnectWithServer()
{
	QString serverIdentifier = activeServerComboBox_->currentText();
	if (serverIdentifier.length() > 0)
		AMDSClientAppController::clientAppController()->disconnectWithServer(serverIdentifier);
}

/// ============= SLOTs to handle AMDSClientAppController signals =========
void AMDSClientUi::onNetworkSessionOpening()
{
	requestDataButton_->setEnabled(false);
	statusLabel_->setText("Opening network session.");
}

void AMDSClientUi::onNetworkSessionOpened()
{
	statusLabel_->setText("This examples requires that you run the Acquaman Data Server example as well.");
//	enableRequestDataButton();
}

void AMDSClientUi::onNewServerConnected(const QString &serverIdentifier)
{
	if (activeServerComboBox_->findText(serverIdentifier) == -1) {
		activeServerComboBox_->addItem(serverIdentifier);
	}

	activeServerComboBox_->setCurrentIndex(activeServerComboBox_->findText(serverIdentifier));

	QStringList bufferNames = AMDSClientAppController::clientAppController()->getBufferNamesByServer(serverIdentifier);
	resetBufferListView(bufferNames);
	resetActiveContinuousConnection(serverIdentifier);
}

void AMDSClientUi::onRequestDataReady(AMDSClientRequest* clientRequest)
{
	switch (clientRequest->requestType()) {
	case AMDSClientRequestDefinitions::Continuous:
		resetActiveContinuousConnection(activeServerComboBox_->currentText());
		break;

	case AMDSClientRequestDefinitions::Introspection:
		{
			AMDSClientIntrospectionRequest *introspectionRequest = qobject_cast<AMDSClientIntrospectionRequest*>(clientRequest);
			if (introspectionRequest){
				if (introspectionRequest->readReady()){
					const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(requestTypeComboBox_->model());
					for(int x = 1; x < AMDSClientRequestDefinitions::InvalidRequest; x++){
						QStandardItem* item = model->item(x);
						item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
						// visually disable by greying out - works only if combobox has been painted already and palette returns the wanted color
						item->setData(QVariant(), Qt::TextColorRole);
					}
				}

				if (introspectionRequest->checkAllBuffer() ){
					QStringList bufferNames = AMDSClientAppController::clientAppController()->getBufferNamesByServer(activeServerComboBox_->currentText());
					resetBufferListView(bufferNames);
				}
			}
		}

		break;

	case AMDSClientRequestDefinitions::Configuration:
		{
			AMDSClientConfigurationRequest *configurationRequest = qobject_cast<AMDSClientConfigurationRequest*>(clientRequest);
			if (configurationRequest){
				resetConfigurationCommands(configurationRequest->configurationCommandDefs());
			}
		}
		break;
	default:
		break;
	}
}

void AMDSClientUi::onServerError(int errorCode, bool removeServer, const QString &serverIdentifier, const QString &errorMessage)
{
	if (serverIdentifier.length() > 0) {
		if (removeServer)
			activeServerComboBox_->removeItem(activeServerComboBox_->findText(serverIdentifier));
		resetActiveContinuousConnection(activeServerComboBox_->currentText());
	}

	if (errorCode != QAbstractSocket::RemoteHostClosedError) {
		QString message = QString("%1 (%2): %3").arg(errorCode).arg(serverIdentifier).arg(errorMessage);
		QMessageBox::information(this, "AMDS Client Example", message);
	}
}

/// ============= SLOTS to handle UI component signals ===============
void AMDSClientUi::onActiveServerChanged(const QString &serverIdentifier)
{
	QStringList bufferNames = AMDSClientAppController::clientAppController()->getBufferNamesByServer(serverIdentifier);
	resetBufferListView(bufferNames);

	resetActiveContinuousConnection(serverIdentifier);
}

void AMDSClientUi::onRequestTypeChanged(int requestType)
{
	displayWidget(requestCommandComboBox_, false);
	displayWidget(time1Edit_, false);
	displayWidget(time2Edit_, false);
	displayWidget(count1Edit_, false);
	displayWidget(count2Edit_, false);
	displayWidget(includeStatusDataCheckbox_, false);
	displayWidget(enableFlattenDataCheckbox_, false);
	displayWidget(activeContinuousConnectionComboBox_, false);

	bufferNameListView_->setSelectionMode(QAbstractItemView::SingleSelection);
	if (bufferNameListView_->selectionModel()->selectedIndexes().count() > 1)
		bufferNameListView_->clearSelection();

	switch(requestType) {
	case AMDSClientRequestDefinitions::Introspection:
	case AMDSClientRequestDefinitions::Statistics:
		break;
	case AMDSClientRequestDefinitions::StartTimePlusCount:
		displayWidget(time1Edit_, true);
		displayWidget(count1Edit_, true);
		displayWidget(includeStatusDataCheckbox_, true);
		displayWidget(enableFlattenDataCheckbox_, true);
		break;
	case AMDSClientRequestDefinitions::RelativeCountPlusCount:
		displayWidget(count1Edit_, true);
		displayWidget(count2Edit_, true);
		displayWidget(includeStatusDataCheckbox_, true);
		displayWidget(enableFlattenDataCheckbox_, true);
		break;
	case AMDSClientRequestDefinitions::StartTimeToEndTime:
		displayWidget(time1Edit_, true);
		displayWidget(time2Edit_, true);
		displayWidget(includeStatusDataCheckbox_, true);
		displayWidget(enableFlattenDataCheckbox_, true);
		break;
	case AMDSClientRequestDefinitions::MiddleTimePlusCountBeforeAndAfter:
		displayWidget(time1Edit_, true);
		displayWidget(count1Edit_, true);
		displayWidget(count2Edit_, true);
		displayWidget(includeStatusDataCheckbox_, true);
		displayWidget(enableFlattenDataCheckbox_, true);
		break;
	case AMDSClientRequestDefinitions::Continuous:
		displayWidget(count1Edit_, true);
		displayWidget(includeStatusDataCheckbox_, true);
		displayWidget(enableFlattenDataCheckbox_, true);

		bufferNameListView_->setSelectionMode(QAbstractItemView::MultiSelection);
		break;
	case AMDSClientRequestDefinitions::Configuration :
		displayWidget(requestCommandComboBox_, true);
		displayWidget(count1Edit_, true);
		break;
	}
}

void AMDSClientUi::onBufferNameSelectChanged()
{
	requestCommandComboBox_->clear();
	requestCommandComboBox_->addItem("");

	currentConfigurationCommandDefs_.clear();
}

void AMDSClientUi::enableRequestDataButton()
{
	requestDataButton_->setEnabled(AMDSClientAppController::clientAppController()->isSessionOpen() &&
								 !activeServerComboBox_->currentText().isEmpty());
}

void AMDSClientUi::sendClientRequest()
{
	AMDSServer * server = AMDSClientAppController::clientAppController()->getServerByServerIdentifier(activeServerComboBox_->currentText());
	if (!server) {
		QMessageBox::information( this, "AMDS Client Example", "Didn't select any active server!");
		return;
	}

	QStringList selectedBufferNames;
	foreach (const QModelIndex &index, bufferNameListView_->selectionModel()->selectedIndexes())
	{
		selectedBufferNames.append(index.data(Qt::DisplayRole).toString());
	}

	QString hostName = server->hostName();
	quint16 portNumber = server->portNumber();
	quint8 requestTypeId = (quint8)requestTypeComboBox_->currentIndex();
	QString bufferName = selectedBufferNames.value(0);
	QString configurationCommand = requestCommandComboBox_->currentText();
	QDateTime time1 = time1Edit_->dateTime();
	QDateTime time2 = time2Edit_->dateTime();
	QString value1 = count1Edit_->text();
	QString value2 = count2Edit_->text();
	QString continuousSocket = activeContinuousConnectionComboBox_->currentText();

	bool includeStatus = includeStatusDataCheckbox_->isChecked();
	bool enableFlattening = enableFlattenDataCheckbox_->isChecked();

	AMDSClientRequestDefinitions::RequestType clientRequestType = (AMDSClientRequestDefinitions::RequestType)requestTypeId;
	if (	(selectedBufferNames.count() == 0)
		 && (clientRequestType != AMDSClientRequestDefinitions::Continuous )
		 && (clientRequestType != AMDSClientRequestDefinitions::Statistics)) {
		QMessageBox::information( this, "AMDS Client Example", "Didn't select any buffer(s)!");
		return;
	}

	AMDSClientAppController *clientAppController = AMDSClientAppController::clientAppController();
	switch(clientRequestType) {
	case AMDSClientRequestDefinitions::Introspection:
		clientAppController->requestClientData(hostName, portNumber, bufferName);
		break;
	case AMDSClientRequestDefinitions::Statistics:
		clientAppController->requestClientData(hostName, portNumber);
		break;
	case AMDSClientRequestDefinitions::StartTimePlusCount:
		clientAppController->requestClientData(hostName, portNumber, bufferName, time1, value1.toInt(), includeStatus, enableFlattening);
		break;
	case AMDSClientRequestDefinitions::RelativeCountPlusCount:
		clientAppController->requestClientData(hostName, portNumber, bufferName, value1.toInt(), value2.toInt(), includeStatus, enableFlattening);
		break;
	case AMDSClientRequestDefinitions::StartTimeToEndTime:
		clientAppController->requestClientData(hostName, portNumber, bufferName, time1, time2, includeStatus, enableFlattening);
		break;
	case AMDSClientRequestDefinitions::MiddleTimePlusCountBeforeAndAfter:
		clientAppController->requestClientData(hostName, portNumber, bufferName, time1, value1.toInt(), value2.toInt(), includeStatus, enableFlattening);
		break;
	case AMDSClientRequestDefinitions::Continuous:
		clientAppController->requestClientData(hostName, portNumber, selectedBufferNames, value1.toInt(), includeStatus, enableFlattening, continuousSocket);
		if ((continuousSocket.length() > 0) && AMDSRunTimeSupport::debugAtLevel(2)) {
			AMErrorMon::information(this, AMDS_CLIENT_INFO_HAND_SHAKE_MESSAGE, QString("Hand shake message: %1").arg(continuousSocket));
		}
		break;
	case AMDSClientRequestDefinitions::Configuration:
		clientAppController->requestClientData(hostName, portNumber, bufferName, configurationCommand, value1);
		break;

	default:
		if(AMDSRunTimeSupport::debugAtLevel(1))
			AMErrorMon::alert(this, AMDS_CLIENT_ERR_INVALID_MESSAGE_TYPE, QString("Invalide client request type: %1").arg(clientRequestType));
	}
}

void AMDSClientUi::displayWidget(QWidget *widget, bool show)
{
	if (show) {
		widget->show();
		formLayout_->labelForField(widget)->show();
	} else {
		widget->hide();
		formLayout_->labelForField(widget)->hide();
	}
}

void AMDSClientUi::resetBufferListView(const QStringList &bufferNames)
{
	QAbstractItemModel *currentListModel = bufferNameListView_->model();
	if (currentListModel)
		currentListModel->deleteLater();

	QStringListModel *bufferNamesModel = new QStringListModel();
	bufferNamesModel->setStringList(bufferNames);
	bufferNameListView_->setModel(bufferNamesModel);
}

void AMDSClientUi::resetConfigurationCommands(QList<AMDSCommand> commandDefs)
{
	onBufferNameSelectChanged();

	currentConfigurationCommandDefs_.append(commandDefs);
	foreach(AMDSCommand commandDef, commandDefs) {
		requestCommandComboBox_->addItem(commandDef.command());
	}
}

void AMDSClientUi::resetActiveContinuousConnection(const QString &serverIdentifier)
{
	QStringList activeContinuousClientRequestKeys = AMDSClientAppController::clientAppController()->getActiveSocketKeysByServer(serverIdentifier);

	bool updateActiveContiuousConnectionComboBox = false;
	if (activeContinuousClientRequestKeys.length() == activeContinuousConnectionComboBox_->count()) {
		for (int i = 0; (i < activeContinuousConnectionComboBox_->count()) && (!updateActiveContiuousConnectionComboBox); i++) {
			QString itemText = activeContinuousConnectionComboBox_->itemText(i);
			if (!activeContinuousClientRequestKeys.contains(itemText)) {
				updateActiveContiuousConnectionComboBox = true;
			}
		}
	} else {
		updateActiveContiuousConnectionComboBox = true;
	}

	if (updateActiveContiuousConnectionComboBox) {
		activeContinuousConnectionComboBox_->clear();
		activeContinuousConnectionComboBox_->addItems(activeContinuousClientRequestKeys);
	}
}
