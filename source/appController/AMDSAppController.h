#ifndef AMDSAPPCONTROLLER_H
#define AMDSAPPCONTROLLER_H

#include <QObject>

class AMDSAppController : public QObject
{
    Q_OBJECT
public:
	enum AMDSAppType {
		Client = 0,
		Server = 1
	};

	static AMDSAppController *appController();
	static void releaseAppController();

	~AMDSAppController() {}

	/// returns the current type of the app controller
	inline AMDSAppController::AMDSAppType appType() const { return appType_; }

protected:
	/// the singalton instance of the appController
	static AMDSAppController *appController_;

	explicit AMDSAppController(AMDSAppController::AMDSAppType appType, QObject *parent = 0);

protected:
	/// the appControler type
	AMDSAppController::AMDSAppType appType_;
};

#endif // AMDSAPPCONTROLLER_H
