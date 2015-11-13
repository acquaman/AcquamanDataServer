

# EPICS Dependencies:
EPICS_INCLUDE_DIRS = /home/epics/src/R3.14.12/base/include \
					/home/epics/src/R3.14.12/base/include/os/Linux
EPICS_LIB_DIR = /home/epics/src/R3.14.12/base/lib/linux-x86_64

INCLUDEPATH += $$EPICS_INCLUDE_DIRS

LIBS += -L$$EPICS_LIB_DIR -lca -lCom

QMAKE_LFLAGS_DEBUG *= "-Wl,-rpath,$$EPICS_LIB_DIR"
QMAKE_LFLAGS_RELEASE *= "-Wl,-rpath,$$EPICS_LIB_DIR"

# Acuqaman and MPlot files
HEADERS *= \
	$$PATH_TO_ACQUAMAN/acquaman.h \
	$$PATH_TO_ACQUAMAN/beamline/AMProcessVariable.h \
	$$PATH_TO_ACQUAMAN/beamline/AMProcessVariablePrivate.h \
	$$PATH_TO_ACQUAMAN/beamline/AMControl.h \
	$$PATH_TO_ACQUAMAN/beamline/AMControlSet.h \
	$$PATH_TO_ACQUAMAN/beamline/AMPVControl.h \
	$$PATH_TO_ACQUAMAN/dataman/AMAxisInfo.h \
	$$PATH_TO_ACQUAMAN/dataman/AMnDIndex.h \
	$$PATH_TO_ACQUAMAN/dataman/AMNumber.h \
	$$PATH_TO_ACQUAMAN/dataman/database/AMConstDbObject.h \
	$$PATH_TO_ACQUAMAN/dataman/database/AMDbObject.h \
	$$PATH_TO_ACQUAMAN/dataman/database/AMDbObjectSupport.h \
	$$PATH_TO_ACQUAMAN/dataman/database/AMDatabase.h \
	$$PATH_TO_ACQUAMAN/dataman/datasource/AMDataSource.h \
	$$PATH_TO_ACQUAMAN/dataman/datasource/AMDataSourceSeriesData.h \
	$$PATH_TO_ACQUAMAN/dataman/info/AMControlInfo.h \
	$$PATH_TO_ACQUAMAN/dataman/info/AMControlInfoList.h \
	$$PATH_TO_ACQUAMAN/util/AMBiHash.h \
	$$PATH_TO_ACQUAMAN/util/AMRange.h \
	$$PATH_TO_ACQUAMAN/util/amlikely.h \
	$$PATH_TO_ACQUAMAN/util/AMOrderedList.h \
	$$PATH_TO_ACQUAMAN/util/AMOrderedSet.h \
	$$PATH_TO_ACQUAMAN/util/AMOrderedSetSignalSource.h \
	$$PATH_TO_ACQUAMAN/util/AMSettings.h \
	$$PATH_TO_ACQUAMAN/util/AMDeferredFunctionCall.h \
	$$PATH_TO_ACQUAMAN/util/AMErrorMonitor.h \
	$$PATH_TO_ACQUAMAN/util/AMUtility.h \
	$$PATH_TO_ACQUAMAN/ui/util/AMMessageBoxWTimeout.h \
	$$PATH_TO_MPLOT/MPlot/MPlot.h \
	$$PATH_TO_MPLOT/MPlot/MPlotWidget.h \
	$$PATH_TO_MPLOT/MPlot/MPlotSeries.h \
	$$PATH_TO_MPLOT/MPlot/MPlotPoint.h \
	$$PATH_TO_MPLOT/MPlot/MPlotAbstractTool.h \
	$$PATH_TO_MPLOT/MPlot/MPlotTools.h \
	$$PATH_TO_MPLOT/MPlot/MPlotAxisScale.h \
	$$PATH_TO_MPLOT/MPlot/MPlotLegend.h \
	$$PATH_TO_MPLOT/MPlot/MPlotColorLegend.h \
	$$PATH_TO_MPLOT/MPlot/MPlotMarker.h \
	$$PATH_TO_MPLOT/MPlot/MPlotRectangle.h \
	$$PATH_TO_MPLOT/MPlot/MPlotImageRangeDialog.h \
	$$PATH_TO_MPLOT/MPlot/MPlotImage.h \
	$$PATH_TO_MPLOT/MPlot/MPlotColorMap.h \
	$$PATH_TO_MPLOT/MPlot/MPlotItem.h \
	$$PATH_TO_MPLOT/MPlot/MPlotSeriesData.h \
	$$PATH_TO_MPLOT/MPlot/MPlotAxis.h 

SOURCES *= \
	$$PATH_TO_ACQUAMAN/acquaman.cpp \
	$$PATH_TO_ACQUAMAN/beamline/AMProcessVariable.cpp \
	$$PATH_TO_ACQUAMAN/beamline/AMProcessVariablePrivate.cpp \
	$$PATH_TO_ACQUAMAN/beamline/AMControl.cpp \
	$$PATH_TO_ACQUAMAN/beamline/AMControlSet.cpp \
	$$PATH_TO_ACQUAMAN/beamline/AMPVControl.cpp \
	$$PATH_TO_ACQUAMAN/dataman/AMAxisInfo.cpp \
	$$PATH_TO_ACQUAMAN/dataman/AMNumber.cpp \
	$$PATH_TO_ACQUAMAN/dataman/AMnDIndex.cpp \
	$$PATH_TO_ACQUAMAN/dataman/database/AMConstDbObject.cpp \
	$$PATH_TO_ACQUAMAN/dataman/database/AMDbObject.cpp \
	$$PATH_TO_ACQUAMAN/dataman/database/AMDbObjectSupport.cpp \
	$$PATH_TO_ACQUAMAN/dataman/database/AMDatabase.cpp \
	$$PATH_TO_ACQUAMAN/dataman/datasource/AMDataSource.cpp \
	$$PATH_TO_ACQUAMAN/dataman/datasource/AMDataSourceSeriesData.cpp \
	$$PATH_TO_ACQUAMAN/dataman/info/AMControlInfo.cpp \
	$$PATH_TO_ACQUAMAN/dataman/info/AMControlInfoList.cpp \
	$$PATH_TO_ACQUAMAN/util/AMRange.cpp \
	$$PATH_TO_ACQUAMAN/util/AMOrderedSetSignalSource.cpp \
	$$PATH_TO_ACQUAMAN/util/AMSettings.cpp \
	$$PATH_TO_ACQUAMAN/util/AMDeferredFunctionCall.cpp \
	$$PATH_TO_ACQUAMAN/util/AMErrorMonitor.cpp \
	$$PATH_TO_ACQUAMAN/util/AMUtility.cpp \
	$$PATH_TO_ACQUAMAN/ui/util/AMMessageBoxWTimeout.cpp \
	$$PATH_TO_MPLOT/MPlot/MPlot.cpp \
	$$PATH_TO_MPLOT/MPlot/MPlotWidget.cpp \
	$$PATH_TO_MPLOT/MPlot/MPlotSeries.cpp \
	$$PATH_TO_MPLOT/MPlot/MPlotPoint.cpp \
	$$PATH_TO_MPLOT/MPlot/MPlotAbstractTool.cpp \
	$$PATH_TO_MPLOT/MPlot/MPlotTools.cpp \
	$$PATH_TO_MPLOT/MPlot/MPlotAxisScale.cpp \
	$$PATH_TO_MPLOT/MPlot/MPlotLegend.cpp	\
	$$PATH_TO_MPLOT/MPlot/MPlotColorLegend.cpp \
	$$PATH_TO_MPLOT/MPlot/MPlotMarker.cpp \
	$$PATH_TO_MPLOT/MPlot/MPlotRectangle.cpp \
	$$PATH_TO_MPLOT/MPlot/MPlotImageRangeDialog.cpp \
	$$PATH_TO_MPLOT/MPlot/MPlotImage.cpp \
	$$PATH_TO_MPLOT/MPlot/MPlotColorMap.cpp \
	$$PATH_TO_MPLOT/MPlot/MPlotItem.cpp \
	$$PATH_TO_MPLOT/MPlot/MPlotSeriesData.cpp \
	$$PATH_TO_MPLOT/MPlot/MPlotAxis.cpp 
