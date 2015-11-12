#ifndef AMDSSERVERAPPCONTROLLER_H
#define AMDSSERVERAPPCONTROLLER_H

#include "appController/AMDSAppController.h"

class AMDSServerAppController : public AMDSAppController
{
    Q_OBJECT
public:
	static AMDSServerAppController *serverAppController();
	~AMDSServerAppController();

protected:
    explicit AMDSServerAppController(QObject *parent = 0);


};

#endif // AMDSSERVERAPPCONTROLLER_H
