#include "AMDSClient.h"

#include <QtGui>
#include <QtNetwork>
#include <QDateTimeEdit>

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
	results = new QTextEdit();
	tcpSocket = new QTcpSocket(this);
	requestType = new QComboBox;
	requestType->addItem("Introspect");
	requestType->addItem("Statistics");
	requestType->addItem("Start Time + Count");
	requestType->addItem("Relative Count + Count");
	requestType->addItem("Start Time to End Time");
	requestType->addItem("Middle Time + Count Before to Count After");
	requestType->addItem("Continuous");

	const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(requestType->model());
	for(int x = 1; x < 7; x++){
		QStandardItem* item = model->item(x);
		item->setFlags(item->flags() & ~(Qt::ItemIsSelectable|Qt::ItemIsEnabled));
		// visually disable by greying out - works only if combobox has been painted already and palette returns the wanted color
		item->setData(requestType->palette().color(QPalette::Disabled, QPalette::Text), Qt::TextColorRole);
	}
	requestType->setCurrentIndex(0);

	bufferNameComboBox_ = new QComboBox();
	bufferNameComboBox_->addItem("All");
	bufferNameComboBox_->setMinimumWidth(250);

	amptekIndex = new QSpinBox();
	includeStatusDataCheckbox = new QCheckBox();

	connect(hostLineEdit, SIGNAL(textChanged(QString)),
			this, SLOT(enableGetFortuneButton()));
	connect(portLineEdit, SIGNAL(textChanged(QString)),
			this, SLOT(enableGetFortuneButton()));
	connect(getFortuneButton, SIGNAL(clicked()),
			this, SLOT(requestNewFortune()));
	connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readFortune()));
	connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
			this, SLOT(displayError(QAbstractSocket::SocketError)));

	QGridLayout *mainLayout = new QGridLayout;
	mainLayout->addWidget(hostLabel, 0, 0);
	mainLayout->addWidget(hostLineEdit, 0, 1);
	mainLayout->addWidget(portLabel, 1, 0);
	mainLayout->addWidget(portLineEdit, 1, 1);
	mainLayout->addWidget(statusLabel, 2, 0, 1, 2);
	mainLayout->addWidget(buttonBox, 3, 0, 1, 2);

	QFormLayout *formLayout = new QFormLayout();
	formLayout->addRow("Request Type", requestType);
	formLayout->addRow("Buffer", bufferNameComboBox_);
	formLayout->addRow("Time1", time1Edit);
	formLayout->addRow("Time2", time2Edit);
	formLayout->addRow("Count1", count1Edit);
	formLayout->addRow("Count2", count2Edit);
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
	getFortuneButton->setEnabled(false);
	blockSize = 0;
	tcpSocket->abort();
	tcpSocket->connectToHost(hostLineEdit->text(),
							 portLineEdit->text().toInt());

	QTimer::singleShot(1000, this, SLOT(requestData()));
}

void AMDSClient::readFortune()
{

	AMDSDataStream in(tcpSocket);
	in.setVersion(QDataStream::Qt_4_0);

	if (blockSize == 0) {
		if (tcpSocket->bytesAvailable() < (int)sizeof(quint32))
			return;

		in >> blockSize;
	}

//	qDebug() << "Received from: " << tcpSocket->peerAddress();
//	qDebug() << "\n\n\n";
//	qDebug() << "Block size = " << blockSize;
//	qDebug() << "Bytes available = " << tcpSocket->bytesAvailable();

	if (tcpSocket->bytesAvailable() < blockSize)
		return;

	AMDSClientRequest *clientRequest = in.decodeAndInstantiateClientRequestType();
	in.read(*clientRequest);

	switch (clientRequest->requestType()) {
	case AMDSClientRequestDefinitions::Introspection: {
		/// TODO: this need to be reorganized and implemented (printData) in the class of AMDSClientIntrospectionRequest
		AMDSClientIntrospectionRequest *clientIntrospectionRequest = qobject_cast<AMDSClientIntrospectionRequest*>(clientRequest);
		if(clientIntrospectionRequest){
			QList<AMDSBufferGroupInfo> bufferGroupInfos;
			bufferGroupInfos = clientIntrospectionRequest->bufferGroupInfos();

			if(bufferGroupInfos.count() > 0){
				const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(requestType->model());
				for(int x = 1; x < 7; x++){
					QStandardItem* item = model->item(x);
					item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
					// visually disable by greying out - works only if combobox has been painted already and palette returns the wanted color
					item->setData(QVariant(), Qt::TextColorRole);
				}
			}

			if(clientIntrospectionRequest->bufferName() == "All"){
				bufferNameComboBox_->clear();
				bufferNameComboBox_->addItem("All");
			}
			for(int y = 0, ySize = bufferGroupInfos.count(); y < ySize; y++){
				if(clientIntrospectionRequest->bufferName() == "All")
					bufferNameComboBox_->addItem(bufferGroupInfos.at(y).name());
				qDebug() << bufferGroupInfos.at(y).name() << bufferGroupInfos.at(y).description() << bufferGroupInfos.at(y).units() << bufferGroupInfos.at(y).rank() << bufferGroupInfos.at(y).size().toString();
				for(int x = 0, size = bufferGroupInfos.at(y).axes().count(); x < size; x++){
					qDebug() << "\tAxis info at " << x << bufferGroupInfos.at(y).axes().at(x).name() << bufferGroupInfos.at(y).axes().at(x).description() << bufferGroupInfos.at(y).axes().at(x).units() << bufferGroupInfos.at(y).axes().at(x).size() << bufferGroupInfos.at(y).axes().at(x).isUniform() << bufferGroupInfos.at(y).axes().at(x).start() << bufferGroupInfos.at(y).axes().at(x).increment();
				}
			}
		}

		break;
	}

	case AMDSClientRequestDefinitions::Statistics:
	case AMDSClientRequestDefinitions::StartTimePlusCount:
	case AMDSClientRequestDefinitions::RelativeCountPlusCount:
	case AMDSClientRequestDefinitions::StartTimeToEndTime:
	case AMDSClientRequestDefinitions::MiddleTimePlusCountBeforeAndAfter:
	case AMDSClientRequestDefinitions::Continuous:
			clientRequest->validateResponse();
			break;

	default:
		qDebug() << "The printData() function is NOT implemented (or called) for " << clientRequest->requestType();
		break;
	}

	getFortuneButton->setEnabled(true);
	blockSize = 0;
}

void AMDSClient::displayError(QAbstractSocket::SocketError socketError)
{
	switch (socketError) {
	case QAbstractSocket::RemoteHostClosedError:
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
								 .arg(tcpSocket->errorString()));
	}

	getFortuneButton->setEnabled(true);
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

void AMDSClient::requestData()
{
	QByteArray block;
	AMDSDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_0);
	out << (quint16)0;

	quint8 requestTypeId = (quint8)requestType->currentIndex();

	AMDSClientRequestDefinitions::RequestType clientRequestType = (AMDSClientRequestDefinitions::RequestType)requestTypeId;
	AMDSClientRequest *clientRequest = AMDSClientRequestSupport::instantiateClientRequestFromType(clientRequestType);
	if (!clientRequest) {
		qDebug() << "AMDSClient::Failed to parse clientRequest for type: " << clientRequestType;
		return;
	}

	if(clientRequest->requestType() == AMDSClientRequestDefinitions::Introspection){
		AMDSClientIntrospectionRequest *clientIntrospectionRequest = qobject_cast<AMDSClientIntrospectionRequest*>(clientRequest);
		if(clientIntrospectionRequest)
			clientIntrospectionRequest->setBufferName(bufferNameComboBox_->currentText());
	}
	else if(clientRequest->requestType() == AMDSClientRequestDefinitions::Statistics){

	}
	else if(clientRequest->requestType() == AMDSClientRequestDefinitions::StartTimePlusCount){
		AMDSClientStartTimePlusCountDataRequest *clientStartTimePlusCountDataRequest = qobject_cast<AMDSClientStartTimePlusCountDataRequest*>(clientRequest);
		if(clientStartTimePlusCountDataRequest){
			clientStartTimePlusCountDataRequest->setBufferName(bufferNameComboBox_->currentText());
			clientStartTimePlusCountDataRequest->setStartTime(time1Edit->dateTime());
			quint64 count = count1Edit->text().toUInt();
			clientStartTimePlusCountDataRequest->setCount(count);
		}
	}
	else if(clientRequest->requestType() == AMDSClientRequestDefinitions::RelativeCountPlusCount){
		AMDSClientRelativeCountPlusCountDataRequest *clientRelativeCountPlusCountDataRequest = qobject_cast<AMDSClientRelativeCountPlusCountDataRequest*>(clientRequest);
		if(clientRelativeCountPlusCountDataRequest){
			quint64 relativeCount = count1Edit->text().toUInt();
			quint64 count = count2Edit->text().toUInt();

			clientRelativeCountPlusCountDataRequest->setBufferName(bufferNameComboBox_->currentText());
			clientRelativeCountPlusCountDataRequest->setRelativeCount(relativeCount);
			clientRelativeCountPlusCountDataRequest->setCount(count);
		}
	}
	else if(clientRequest->requestType() == AMDSClientRequestDefinitions::StartTimeToEndTime){
		AMDSClientStartTimeToEndTimeDataRequest *clientStartTimeToEndTimeDataRequest = qobject_cast<AMDSClientStartTimeToEndTimeDataRequest*>(clientRequest);
		if(clientStartTimeToEndTimeDataRequest){
			clientStartTimeToEndTimeDataRequest->setBufferName(bufferNameComboBox_->currentText());
			clientStartTimeToEndTimeDataRequest->setStartTime(time1Edit->dateTime());
			clientStartTimeToEndTimeDataRequest->setEndTime(time2Edit->dateTime());
		}
	}
	else if(clientRequest->requestType() == AMDSClientRequestDefinitions::MiddleTimePlusCountBeforeAndAfter){
		AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest *clientMiddleTimePlusCountBeforeAndAfterDataRequest = qobject_cast<AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest*>(clientRequest);
		if(clientMiddleTimePlusCountBeforeAndAfterDataRequest){
			quint64 countBefore = count1Edit->text().toUInt();
			quint64 countAfter = count2Edit->text().toUInt();

			clientMiddleTimePlusCountBeforeAndAfterDataRequest->setBufferName(bufferNameComboBox_->currentText());
			clientMiddleTimePlusCountBeforeAndAfterDataRequest->setMiddleTime(time1Edit->dateTime());
			clientMiddleTimePlusCountBeforeAndAfterDataRequest->setCountBefore(countBefore);
			clientMiddleTimePlusCountBeforeAndAfterDataRequest->setCountAfter(countAfter);
		}
	}
	else if(clientRequest->requestType() == AMDSClientRequestDefinitions::Continuous){
		AMDSClientContinuousDataRequest *clientContinuousDataRequest = qobject_cast<AMDSClientContinuousDataRequest*>(clientRequest);
		if(clientContinuousDataRequest){
			quint64 updateInterval = 500;
			if (count1Edit->text().length() > 0)
				updateInterval = count1Edit->text().toUInt();

			clientContinuousDataRequest->setBufferName(bufferNameComboBox_->currentText());
			clientContinuousDataRequest->setUpdateInterval(updateInterval);
			if (count2Edit->text().length() > 0) {
				clientContinuousDataRequest->setHandShakeSocketKey(count2Edit->text());
			}
		}
	}

	out.encodeClientRequestType(*clientRequest);
	out.write(*clientRequest);

	out.device()->seek(0);
	out << (quint16)(block.size() - sizeof(quint16));
	tcpSocket->write(block);
	qDebug() << (block.size() - sizeof(quint16));
}
