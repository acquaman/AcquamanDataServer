#include "AMDSClientUi.h"

#include <QtGui>
#include <QtNetwork>
#include <QDateTimeEdit>
#include <QDialog>

#include "source/appController/AMDSClientAppController.h"
#include "source/Connection/AMDSServer.h"
#include "source/ClientRequest/AMDSClientRequest.h"
#include "source/ClientRequest/AMDSClientIntrospectionRequest.h"
#include "source/util/AMDSErrorMonitor.h"

AMDSClientUi::AMDSClientUi(QWidget *parent) :
    QDialog(parent)
{
	/// ==== server section ====
	hostLabel = new QLabel(tr("&Server name:"));
	portLabel = new QLabel(tr("&erver port:"));

	hostLineEdit = new QLineEdit("10.51.3.5");
	portLineEdit = new QLineEdit("28044");
	portLineEdit->setValidator(new QIntValidator(1, 65535, this));

	hostLabel->setBuddy(hostLineEdit);
	portLabel->setBuddy(portLineEdit);

	activeServerComboBox = new QComboBox;
	activeServerComboBox->setFixedWidth(200);
	activeServerComboBox->addItem("");
	connect(activeServerComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(onActiveServerChanged(QString)));

	statusLabel = new QLabel(tr("This examples requires that you run the Acquaman Data Server example as well."));

	connect(hostLineEdit, SIGNAL(textChanged(QString)), this, SLOT(enableRequestDataButton()));
	connect(portLineEdit, SIGNAL(textChanged(QString)), this, SLOT(enableRequestDataButton()));

	/// ==== buttons section ====
	connectServerButton = new QPushButton(tr("Connect to Server"));
	connectServerButton->setDefault(true);

	requestDataButton = new QPushButton(tr("Request Data"));
	quitButton = new QPushButton(tr("Quit"));

	buttonBox = new QDialogButtonBox;
	buttonBox->addButton(connectServerButton, QDialogButtonBox::ActionRole);
	buttonBox->addButton(requestDataButton, QDialogButtonBox::ActionRole);
	buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

	connect(connectServerButton, SIGNAL(clicked()), this, SLOT(connectToServer()));
	connect(requestDataButton, SIGNAL(clicked()), this, SLOT(sendClientRequest()));
	connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));

	/// ==== message compose section ====
	requestType = new QComboBox;
	requestType->addItem("Introspect");
	requestType->addItem("Statistics");
	requestType->addItem("Start Time + Count");
	requestType->addItem("Relative Count + Count");
	requestType->addItem("Start Time to End Time");
	requestType->addItem("Middle Time + Count Before to Count After");
	requestType->addItem("Continuous");
	requestType->setCurrentIndex(0);

	const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(requestType->model());
	for(int x = 1; x < 7; x++){
		QStandardItem* item = model->item(x);
		item->setFlags(item->flags() & ~(Qt::ItemIsSelectable|Qt::ItemIsEnabled));
		// visually disable by greying out - works only if combobox has been painted already and palette returns the wanted color
		item->setData(requestType->palette().color(QPalette::Disabled, QPalette::Text), Qt::TextColorRole);
	}

	bufferNameListView_ = new QListView();
	QStringList bufferNames("All");
	resetBufferListView(bufferNames);

	time1Edit = new QDateTimeEdit(QDateTime::currentDateTime());
	time1Edit->setDisplayFormat("HH:mm:ss.zzz");
	time2Edit = new QDateTimeEdit(QDateTime::currentDateTime());
	time2Edit->setDisplayFormat("HH:mm:ss.zzz");
	count1Edit = new QLineEdit();
	count2Edit = new QLineEdit();
	includeStatusDataCheckbox = new QCheckBox();
	enableFlattenDataCheckbox = new QCheckBox();

	activeContinuousConnection = new QComboBox;
	activeContinuousConnection->setFixedWidth(200);
	activeContinuousConnection->addItem("");

	results = new QTextEdit();

	connect(requestType, SIGNAL(currentIndexChanged(QString)), this, SLOT(onRequestTypeChanged(QString)));


//	amptekIndex = new QSpinBox();


	/// ==== layout the components ====
	QGridLayout *mainLayout = new QGridLayout;
	mainLayout->addWidget(hostLabel, 0, 0);
	mainLayout->addWidget(hostLineEdit, 0, 1);
	mainLayout->addWidget(portLabel, 1, 0);
	mainLayout->addWidget(portLineEdit, 1, 1);
	mainLayout->addWidget(new QLabel("Active Servers"), 2, 0);
	mainLayout->addWidget(activeServerComboBox, 2, 1);
	mainLayout->addWidget(statusLabel, 3, 0, 1, 2);
	mainLayout->addWidget(buttonBox, 4, 0, 1, 2);

	QFormLayout *formLayout = new QFormLayout();
	formLayout->addRow("Request Type", requestType);
	formLayout->addRow("Buffer", bufferNameListView_);
	formLayout->addRow("Time1", time1Edit);
	formLayout->addRow("Time2", time2Edit);
	formLayout->addRow("Count1", count1Edit);
	formLayout->addRow("Count2", count2Edit);
	formLayout->addRow("Include Status", includeStatusDataCheckbox);
	formLayout->addRow("Enable Flattening", enableFlattenDataCheckbox);
	mainLayout->addLayout(formLayout, 5, 0, 1, 2);
	formLayout->addRow("Active continuous connection", activeContinuousConnection);
	mainLayout->addWidget(results, 6, 0, 1, 2);
	setLayout(mainLayout);

	setWindowTitle(tr("AMDS Client Example"));
	portLineEdit->setFocus();

	/// ==== initialize the app controller ==============
	clientAppController_ = new AMDSClientAppController();
	connect(clientAppController_, SIGNAL(networkSessionOpening()), this, SLOT(onNetworkSessionOpening()));
	connect(clientAppController_, SIGNAL(networkSessionOpened()), this, SLOT(onNetworkSessionOpened()));
	connect(clientAppController_, SIGNAL(newServerConnected(QString)), this, SLOT(onNewServerConnected(QString)));
	connect(clientAppController_, SIGNAL(serverError(int,QString,QString)), this, SLOT(onServerError(int,QString,QString)));
	connect(clientAppController_, SIGNAL(requestDataReady(AMDSClientRequest*)), this, SLOT(onRequestDataReady(AMDSClientRequest*)));
	connect(clientAppController_, SIGNAL(socketError(int, QString, QString)), this, SLOT(onSocketError(int, QString, QString)));

	clientAppController_->openNetworkSession();
}

AMDSClientUi::~AMDSClientUi()
{
	disconnect(clientAppController_, SIGNAL(networkSessionOpening()), this, SLOT(onNetworkSessionOpening()));
	disconnect(clientAppController_, SIGNAL(networkSessionOpened()), this, SLOT(onNetworkSessionOpened()));
	disconnect(clientAppController_, SIGNAL(newServerConnected(QString)), this, SLOT(onNewServerConnected(QString)));
	disconnect(clientAppController_, SIGNAL(serverError(int,QString,QString)), this, SLOT(onServerError(int,QString,QString)));
	disconnect(clientAppController_, SIGNAL(requestDataReady(AMDSClientRequest*)), this, SLOT(onRequestDataReady(AMDSClientRequest*)));
	disconnect(clientAppController_, SIGNAL(socketError(QString, QString)), this, SLOT(onSocketError(QString, QString)));

	clientAppController_->deleteLater();
}

void AMDSClientUi::connectToServer()
{
	QString hostName = hostLineEdit->text();
	quint16 portNumber = portLineEdit->text().toInt();

	clientAppController_->connectToServer(hostName, portNumber);
}

/// ============= SLOTs to handle AMDSClientAppController signals =========
void AMDSClientUi::onNetworkSessionOpening()
{
	requestDataButton->setEnabled(false);
	statusLabel->setText("Opening network session.");
}

void AMDSClientUi::onNetworkSessionOpened()
{
	statusLabel->setText("This examples requires that you run the Acquaman Data Server example as well.");
	enableRequestDataButton();
}

void AMDSClientUi::onNewServerConnected(QString serverIdentifier)
{
	if (activeServerComboBox->findText(serverIdentifier) == -1) {
		activeServerComboBox->addItem(serverIdentifier);
	}

	activeServerComboBox->setCurrentIndex(activeServerComboBox->findText(serverIdentifier));

	QStringList bufferNames = clientAppController_->getBufferNamesByServer(serverIdentifier);
	resetBufferListView(bufferNames);
	resetActiveContinuousConnection(serverIdentifier);
}

void AMDSClientUi::onServerError(int errorCode, QString serverIdentifier, QString errorMessage)
{
	QString message = QString("%1 (%2): %3").arg(errorCode).arg(serverIdentifier).arg(errorMessage);
	QMessageBox::information(this, "AMDS Client Example", message);
}

void AMDSClientUi::onRequestDataReady(AMDSClientRequest* clientRequest)
{
	if (clientRequest->isContinuousMessage()) {
		resetActiveContinuousConnection(activeServerComboBox->currentText());
	} else {
		if (clientRequest->isIntrospectionMessage()) {
			AMDSClientIntrospectionRequest *introspectionRequest = qobject_cast<AMDSClientIntrospectionRequest*>(clientRequest);
			if (introspectionRequest){
				if (introspectionRequest->readReady()){
					const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(requestType->model());
					for(int x = 1; x < AMDSClientRequestDefinitions::InvalidRequest; x++){
						QStandardItem* item = model->item(x);
						item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
						// visually disable by greying out - works only if combobox has been painted already and palette returns the wanted color
						item->setData(QVariant(), Qt::TextColorRole);
					}
				}

				if (introspectionRequest->checkAllBuffer() ){
					QStringList bufferNames = clientAppController_->getBufferNamesByServer(activeServerComboBox->currentText());
					resetBufferListView(bufferNames);
				}
			}
		}
	}
}

void AMDSClientUi::onSocketError(int errorCode, QString socketKey, QString errorMessage)
{
	if (socketKey.length() > 0) {
		resetActiveContinuousConnection(activeServerComboBox->currentText());
	}

	if (errorCode != QAbstractSocket::RemoteHostClosedError)
		QMessageBox::information(this, "AMDS Client Example", errorMessage + socketKey);
}

/// ============= SLOTS to handle UI component signals ===============
void AMDSClientUi::onActiveServerChanged(QString serverIdentifier)
{
	QStringList bufferNames = clientAppController_->getBufferNamesByServer(serverIdentifier);
	resetBufferListView(bufferNames);

	resetActiveContinuousConnection(serverIdentifier);
}

void AMDSClientUi::onRequestTypeChanged(QString requestType)
{
	if (requestType == "Continuous") {
		bufferNameListView_->setSelectionMode(QAbstractItemView::MultiSelection);
	} else {
		bufferNameListView_->setSelectionMode(QAbstractItemView::SingleSelection);

		if (bufferNameListView_->selectionModel()->selectedIndexes().count() > 1)
			bufferNameListView_->clearSelection();
	}
}

void AMDSClientUi::enableRequestDataButton()
{
	requestDataButton->setEnabled(clientAppController_->isSessionOpen() &&
								 !activeServerComboBox->currentText().isEmpty());
}

void AMDSClientUi::sendClientRequest()
{
	AMDSServer * server = clientAppController_->getServerByServerIdentifier(activeServerComboBox->currentText());
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
	quint8 requestTypeId = (quint8)requestType->currentIndex();
	QString bufferName = selectedBufferNames.value(0);
	QDateTime time1 = time1Edit->dateTime();
	QDateTime time2 = time2Edit->dateTime();
	QString value1 = count1Edit->text();
	QString value2 = count2Edit->text();
	QString continuousSocket = activeContinuousConnection->currentText();

	bool includeStatus = includeStatusDataCheckbox->isChecked();
	bool enableFlattening = enableFlattenDataCheckbox->isChecked();

	AMDSClientRequestDefinitions::RequestType clientRequestType = (AMDSClientRequestDefinitions::RequestType)requestTypeId;
	if (	(selectedBufferNames.count() == 0)
		 && (clientRequestType != AMDSClientRequestDefinitions::Continuous )
		 && (clientRequestType != AMDSClientRequestDefinitions::Statistics)) {
		QMessageBox::information( this, "AMDS Client Example", "Didn't select any buffer(s)!");
		return;
	}

	switch(clientRequestType) {
	case AMDSClientRequestDefinitions::Introspection:
		clientAppController_->requestClientData(hostName, portNumber, bufferName);
		break;
	case AMDSClientRequestDefinitions::Statistics:
		clientAppController_->requestClientData(hostName, portNumber);
		break;
	case AMDSClientRequestDefinitions::StartTimePlusCount:
		clientAppController_->requestClientData(hostName, portNumber, bufferName, time1, value1.toInt(), includeStatus, enableFlattening);
		break;
	case AMDSClientRequestDefinitions::RelativeCountPlusCount:
		clientAppController_->requestClientData(hostName, portNumber, bufferName, value1.toInt(), value2.toInt(), includeStatus, enableFlattening);
		break;
	case AMDSClientRequestDefinitions::StartTimeToEndTime:
		clientAppController_->requestClientData(hostName, portNumber, bufferName, time1, time2, includeStatus, enableFlattening);
		break;
	case AMDSClientRequestDefinitions::MiddleTimePlusCountBeforeAndAfter:
		clientAppController_->requestClientData(hostName, portNumber, bufferName, time1, value1.toInt(), value2.toInt(), includeStatus, enableFlattening);
		break;
	case AMDSClientRequestDefinitions::Continuous:
		clientAppController_->requestClientData(hostName, portNumber, selectedBufferNames, value1.toInt(), includeStatus, enableFlattening, continuousSocket);
		if (continuousSocket.length() > 0) {
			AMDSErrorMon::information(this, 0, QString("Hand shake message: %1").arg(continuousSocket));
		}
		break;
	default:
		AMDSErrorMon::alert(this, 0, QString("Invalide client request type: %1").arg(clientRequestType));
	}
}

void AMDSClientUi::resetBufferListView(QStringList &bufferNames)
{
	QStringListModel *bufferNamesModel = new QStringListModel(this);
	bufferNamesModel->setStringList(bufferNames);
	bufferNameListView_->setModel(bufferNamesModel);
}

void AMDSClientUi::resetActiveContinuousConnection(QString serverIdentifier)
{
	QStringList activeContinuousClientRequestKeys = clientAppController_->getActiveSocketKeysByServer(serverIdentifier);

	activeContinuousConnection->clear();
	activeContinuousConnection->addItems(activeContinuousClientRequestKeys);
}
