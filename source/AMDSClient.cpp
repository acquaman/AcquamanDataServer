#include "AMDSClient.h"

#include <QtGui>
#include <QtNetwork>
#include <QDateTimeEdit>
#include <QDialog>

#include "source/AMDSDataStream.h"
#include "source/ClientRequest/AMDSClientIntrospectionRequest.h"
#include "source/ClientRequest/AMDSClientRequestSupport.h"
#include "source/ClientRequest/AMDSClientRequest.h"
#include "source/ClientRequest/AMDSClientIntrospectionRequest.h"
#include "source/ClientRequest/AMDSClientStatisticsRequest.h"
#include "source/ClientRequest/AMDSClientStartTimePlusCountDataRequest.h"
#include "source/ClientRequest/AMDSClientRelativeCountPlusCountDataRequest.h"
#include "source/ClientRequest/AMDSClientStartTimeToEndTimeDataRequest.h"
#include "source/ClientRequest/AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest.h"
#include "source/ClientRequest/AMDSClientContinuousDataRequest.h"
#include "source/Connection/AMDSClientTCPSocket.h"
#include "source/Connection/AMDSClientTCPSocketManager.h"
#include "source/util/AMDSErrorMonitor.h"

#include <QDebug>

AMDSClient::AMDSClient(QWidget *parent)
:   QDialog(parent), networkSession(0)
{
	hostLabel = new QLabel(tr("&Server name:"));
	portLabel = new QLabel(tr("S&erver port:"));

//	hostLineEdit = new QLineEdit("10.52.48.40");
	hostLineEdit = new QLineEdit("10.51.3.5");
	portLineEdit = new QLineEdit("28044");
	portLineEdit->setValidator(new QIntValidator(1, 65535, this));

	hostLabel->setBuddy(hostLineEdit);
	portLabel->setBuddy(portLineEdit);

	statusLabel = new QLabel(tr("This examples requires that you run the "
								"Fortune Server example as well."));

	getFortuneButton = new QPushButton(tr("Get Fortune"));
	getFortuneButton->setDefault(true);

	quitButton = new QPushButton(tr("Quit"));

	buttonBox = new QDialogButtonBox;
	buttonBox->addButton(getFortuneButton, QDialogButtonBox::ActionRole);
	buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

	time1Edit = new QDateTimeEdit(QDateTime::currentDateTime());
	time1Edit->setDisplayFormat("HH:mm:ss.zzz");
	time2Edit = new QDateTimeEdit(QDateTime::currentDateTime());
	time2Edit->setDisplayFormat("HH:mm:ss.zzz");
	count1Edit = new QLineEdit();
	count2Edit = new QLineEdit();
	activeContinuousConnection = new QComboBox;
	activeContinuousConnection->setFixedWidth(200);
	activeContinuousConnection->addItem("");

	results = new QTextEdit();
	requestType = new QComboBox;
	requestType->addItem("Introspect");
	requestType->addItem("Statistics");
	requestType->addItem("Start Time + Count");
	requestType->addItem("Relative Count + Count");
	requestType->addItem("Start Time to End Time");
	requestType->addItem("Middle Time + Count Before to Count After");
	requestType->addItem("Continuous");
	requestType->addItem("ContinuousWithBatchStreams");

	const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(requestType->model());
	for(int x = 1; x < AMDSClientRequestDefinitions::InvalidRequest; x++){
		QStandardItem* item = model->item(x);
		item->setFlags(item->flags() & ~(Qt::ItemIsSelectable|Qt::ItemIsEnabled));
		// visually disable by greying out - works only if combobox has been painted already and palette returns the wanted color
		item->setData(requestType->palette().color(QPalette::Disabled, QPalette::Text), Qt::TextColorRole);
	}
	requestType->setCurrentIndex(0);
	connect(requestType, SIGNAL(currentIndexChanged(QString)), this, SLOT(onRequestTypeChanged(QString)));

	bufferNameListView_ = new QListView();
	QStringList bufferNames;
	bufferNames.append("All");
	QStringListModel *bufferNamesModel = new QStringListModel(this);
	bufferNamesModel->setStringList(bufferNames);
	bufferNameListView_->setModel(bufferNamesModel);

	amptekIndex = new QSpinBox();
	includeStatusDataCheckbox = new QCheckBox();

	connect(hostLineEdit, SIGNAL(textChanged(QString)),
			this, SLOT(enableGetFortuneButton()));
	connect(portLineEdit, SIGNAL(textChanged(QString)),
			this, SLOT(enableGetFortuneButton()));
	connect(getFortuneButton, SIGNAL(clicked()),
			this, SLOT(requestNewFortune()));
	connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));

	QGridLayout *mainLayout = new QGridLayout;
	mainLayout->addWidget(hostLabel, 0, 0);
	mainLayout->addWidget(hostLineEdit, 0, 1);
	mainLayout->addWidget(portLabel, 1, 0);
	mainLayout->addWidget(portLineEdit, 1, 1);
	mainLayout->addWidget(statusLabel, 2, 0, 1, 2);
	mainLayout->addWidget(buttonBox, 3, 0, 1, 2);

	QFormLayout *formLayout = new QFormLayout();
	formLayout->addRow("Request Type", requestType);
	formLayout->addRow("Buffer", bufferNameListView_);
	formLayout->addRow("Time1", time1Edit);
	formLayout->addRow("Time2", time2Edit);
	formLayout->addRow("Count1", count1Edit);
	formLayout->addRow("Count2", count2Edit);
	formLayout->addRow("Active continuous connection", activeContinuousConnection);
	formLayout->addRow("Include Status", includeStatusDataCheckbox);
	mainLayout->addLayout(formLayout, 4, 0, 1, 2);
	mainLayout->addWidget(results, 5, 0, 1, 2);
	setLayout(mainLayout);

	setWindowTitle(tr("Fortune Client"));
	portLineEdit->setFocus();

	QNetworkConfigurationManager manager;
	if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
		// Get saved network configuration
		QSettings settings(QSettings::UserScope, QLatin1String("Trolltech"));
		settings.beginGroup(QLatin1String("QtNetwork"));
		const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
		settings.endGroup();

		// If the saved network configuration is not currently discovered use the system default
		QNetworkConfiguration config = manager.configurationFromIdentifier(id);
		if ((config.state() & QNetworkConfiguration::Discovered) !=
			QNetworkConfiguration::Discovered) {
			config = manager.defaultConfiguration();
		}

		networkSession = new QNetworkSession(config, this);
		connect(networkSession, SIGNAL(opened()), this, SLOT(sessionOpened()));

		getFortuneButton->setEnabled(false);
		statusLabel->setText(tr("Opening network session."));
		networkSession->open();
	}
}

void AMDSClient::requestNewFortune()
{
	QStringList selectedBufferNames;
	foreach (const QModelIndex &index, bufferNameListView_->selectionModel()->selectedIndexes())
	{
		selectedBufferNames.append(index.data(Qt::DisplayRole).toString());
	}

	if (selectedBufferNames.count() == 0) {
		QMessageBox::information( this, "Fortune Client", "Didn't select any buffer(s)!");
		return;
	}

	quint8 requestTypeId = (quint8)requestType->currentIndex();
	QString bufferName = selectedBufferNames.value(0);
	QDateTime time1 = time1Edit->dateTime();
	QDateTime time2 = time2Edit->dateTime();
	QString value1 = count1Edit->text();
	QString value2 = count2Edit->text();
	QString continuousSocket = activeContinuousConnection->currentText();

	AMDSClientTCPSocket * clientTCPSocket = new AMDSClientTCPSocket(hostLineEdit->text(), portLineEdit->text().toInt());
	connect(clientTCPSocket, SIGNAL(newRequestDataReady(AMDSClientTCPSocket*, AMDSClientRequest*)), this, SLOT(onSocketDataReady(AMDSClientTCPSocket*, AMDSClientRequest*)));
	connect(clientTCPSocket, SIGNAL(socketError(AMDSClientTCPSocket*, QAbstractSocket::SocketError)), this, SLOT(onSocketError(AMDSClientTCPSocket*, QAbstractSocket::SocketError)));

	AMDSClientRequestDefinitions::RequestType clientRequestType = (AMDSClientRequestDefinitions::RequestType)requestTypeId;
	switch(clientRequestType) {
	case AMDSClientRequestDefinitions::Introspection:
		clientTCPSocket->requestData(bufferName);
		break;
	case AMDSClientRequestDefinitions::Statistics:
		clientTCPSocket->requestData();
		break;
	case AMDSClientRequestDefinitions::StartTimePlusCount:
		clientTCPSocket->requestData(bufferName, time1, value1.toInt());
		break;
	case AMDSClientRequestDefinitions::RelativeCountPlusCount:
		clientTCPSocket->requestData(bufferName, value1.toInt(), value1.toInt());
		break;
	case AMDSClientRequestDefinitions::StartTimeToEndTime:
		clientTCPSocket->requestData(bufferName, time1, time2);
		break;
	case AMDSClientRequestDefinitions::MiddleTimePlusCountBeforeAndAfter:
		clientTCPSocket->requestData(bufferName, time1, value1.toInt(), value2.toInt());
		break;
	case AMDSClientRequestDefinitions::Continuous:
		clientTCPSocket->requestData(bufferName, value1.toInt(), continuousSocket);
		if (continuousSocket.length() > 0) {
			AMDSErrorMon::information(this, 0, QString("Hand shake message: %1").arg(continuousSocket));
		}
		break;
	case AMDSClientRequestDefinitions::ContinuousWithBatchStreams:
		clientTCPSocket->requestData(selectedBufferNames, value1.toInt(), continuousSocket);
		if (continuousSocket.length() > 0) {
			AMDSErrorMon::information(this, 0, QString("Hand shake message: %1").arg(continuousSocket));
		}
		break;
	default:
		AMDSErrorMon::alert(this, 0, QString("Invalide client request type: %1").arg(clientRequestType));
		removeTCPSocket(clientTCPSocket);
	}
}

void AMDSClient::onSocketDataReady(AMDSClientTCPSocket* clientTCPSocket, AMDSClientRequest *clientRequest)
{
	if (   clientRequest->requestType() == AMDSClientRequestDefinitions::Continuous
		|| clientRequest->requestType() == AMDSClientRequestDefinitions::ContinuousWithBatchStreams ) {
		AMDSClientTCPSocketManager::socketManager()->appendSocket(clientTCPSocket);
		if (activeContinuousConnection->findText(clientTCPSocket->socketKey()) == -1) {
			activeContinuousConnection->addItem(clientTCPSocket->socketKey());
		}
	} else {
		if (clientRequest->requestType() == AMDSClientRequestDefinitions::Introspection) {
			AMDSClientIntrospectionRequest *introspectionRequest = qobject_cast<AMDSClientIntrospectionRequest*>(clientRequest);
			if (introspectionRequest) {
				if (introspectionRequest->readReady()) {
					const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(requestType->model());
					for(int x = 1; x < AMDSClientRequestDefinitions::InvalidRequest; x++){
						QStandardItem* item = model->item(x);
						item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
						// visually disable by greying out - works only if combobox has been painted already and palette returns the wanted color
						item->setData(QVariant(), Qt::TextColorRole);
					}
				}

				if(introspectionRequest->checkAllBuffer() ){
					QStringList bufferNames = introspectionRequest->getAllBufferNames();
					bufferNames.insert(0, "All");
					QStringListModel *bufferNamesModel = new QStringListModel(this);
					bufferNamesModel->setStringList(bufferNames);
					bufferNameListView_->setModel(bufferNamesModel);
				}

			}
		}

		removeTCPSocket(clientTCPSocket);
	}
}

void AMDSClient::onSocketError(AMDSClientTCPSocket *clientTCPSocket, QAbstractSocket::SocketError socketError)
{
	switch (socketError) {
	case QAbstractSocket::RemoteHostClosedError:
		if (clientTCPSocket->socketKey().length() > 0) {
			activeContinuousConnection->removeItem(activeContinuousConnection->findText(clientTCPSocket->socketKey()));
			AMDSClientTCPSocketManager::socketManager()->removeSocket(clientTCPSocket->socketKey());
		}
		break;
	case QAbstractSocket::HostNotFoundError:
		QMessageBox::information(this, tr("Fortune Client"),
								 tr("The host was not found. Please check the "
									"host name and port settings."));
		break;
	case QAbstractSocket::ConnectionRefusedError:
		QMessageBox::information(this, tr("Fortune Client"),
								 tr("The connection was refused by the peer. "
									"Make sure the fortune server is running, "
									"and check that the host name and port "
									"settings are correct."));
		break;
	default:
		QMessageBox::information(this, tr("Fortune Client"),
								 tr("The following error occurred: %1.")
								 .arg(clientTCPSocket->errorString()));
	}

	removeTCPSocket(clientTCPSocket);
}

void AMDSClient::onRequestTypeChanged(QString requestType)
{
	if (requestType == "ContinuousWithBatchStreams") {
		bufferNameListView_->setSelectionMode(QAbstractItemView::MultiSelection);
	} else {
		bufferNameListView_->setSelectionMode(QAbstractItemView::SingleSelection);

		if (bufferNameListView_->selectionModel()->selectedIndexes().count() > 1)
			bufferNameListView_->clearSelection();
	}
}

void AMDSClient::enableGetFortuneButton()
{
	getFortuneButton->setEnabled((!networkSession || networkSession->isOpen()) &&
								 !hostLineEdit->text().isEmpty() &&
								 !portLineEdit->text().isEmpty());

}

void AMDSClient::sessionOpened()
{
	qDebug() << "Network session has been opened";

	// Save the used configuration
	QNetworkConfiguration config = networkSession->configuration();
	QString id;
	if (config.type() == QNetworkConfiguration::UserChoice)
		id = networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
	else
		id = config.identifier();

	QSettings settings(QSettings::UserScope, QLatin1String("Trolltech"));
	settings.beginGroup(QLatin1String("QtNetwork"));
	settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
	settings.endGroup();

	statusLabel->setText(tr("This examples requires that you run the "
							"Fortune Server example as well."));

	enableGetFortuneButton();
}

void AMDSClient::removeTCPSocket(AMDSClientTCPSocket *clientTCPSocket)
{
	disconnect(clientTCPSocket, SIGNAL(newRequestDataReady(AMDSClientTCPSocket*, AMDSClientRequest*)), this, SLOT(onSocketDataReady(AMDSClientTCPSocket*, AMDSClientRequest*)));
	disconnect(clientTCPSocket, SIGNAL(socketError(AMDSClientTCPSocket*, QAbstractSocket::SocketError)), this, SLOT(onSocketError(AMDSClientTCPSocket*, QAbstractSocket::SocketError)));
	clientTCPSocket->deleteLater();
}
