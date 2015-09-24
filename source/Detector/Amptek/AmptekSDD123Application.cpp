#include "AmptekSDD123Application.h"

AmptekSDD123Application* AmptekSDD123Application::instance_ = 0;

AmptekSDD123Application::AmptekSDD123Application()
{
	debuggingEnabled_ = false;

	coreApplication_ = QCoreApplication::instance();
	arguments_ = coreApplication_->arguments();

	if(arguments_.contains("--debug"))
		debuggingEnabled_ = true;
}

bool AmptekSDD123Application::debuggingEnabled() const{
	return debuggingEnabled_;
}

AmptekSDD123Application* AmptekSDD123Application::amptekApp(){
	if(instance_ == 0){
		instance_ = new AmptekSDD123Application();
	}

	return instance_;
}

void AmptekSDD123Application::releaseAmptekApp(){
	if(instance_){
		delete instance_;
		instance_ = 0;
	}
}
