/*=====================================================================

QGroundControl Open Source Ground Control Station

(c) 2009, 2010 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>

This file is part of the QGROUNDCONTROL project

    QGROUNDCONTROL is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QGROUNDCONTROL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QGROUNDCONTROL. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/

/**
 * @file
 *   @brief Definition of class MainWindow
 *   @author Lorenz Meier <mavteam@student.ethz.ch>
 *
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "AutoUpdateCheck.h"
#include "AutoUpdateDialog.h"

#include "ui_MainWindow.h"
#include "LinkInterface.h"
#include "UASInterface.h"
#if (defined ENABLE_CAMRAVIW)
#include "CameraView.h"
#endif // ENABLE_CAMRAVIW
#include "MAVLinkSimulationLink.h"
#include "submainwindow.h"
#include "JoystickWidget.h"
#if (defined MOUSE_ENABLED_WIN) | (defined MOUSE_ENABLED_LINUX)
#include "Mouse6dofInput.h"
#endif // MOUSE_ENABLED_WIN
#include "opmapcontrol.h"
#if (defined GOGGLEEARTH) && ((defined Q_OS_MAC) | (defined _MSC_VER))
#include "QGCGoogleEarthView.h"
#endif
#include "LogCompressor.h"
#include "QGCMAVLinkLogPlayer.h"
#include "MAVLinkDecoder.h"
#include "ApmToolBar.h"
#include "DebugOutput.h"
#include "QGCFlightGearLink.h"

#include <QMainWindow>
#include <QStatusBar>
#include <QStackedWidget>
#include <QSettings>
#include <QList>
#include <QNetworkProxy>
#include <QActionGroup>

class QGCMapTool;
class QGCFirmwareUpdate;
class QSplashScreen;
class QGCStatusBar;

/**
 * @brief The LogWindowSingleton class is a helper class providing
 *        an entry point to the debug console widget used by the
 *        loggingMessageHandler. Due to the fact that the debug widget
 *        is created in MainWindow::buildCommonWidgets() it is not
 *        available when the messagehandler is created and installed.
 *        This class provides a write method which is available directly
 *        after the start of APM-Planner and a message buffering for
 *        all logmessages wich are printet before the debug console widget
 *        becomes available.
 *
 * @attention This class is NOT thread safe nor is it reentrant.
 *            Should only be used by the loggingMessageHandler
 */
class LogWindowSingleton
{
public:
    /**
     * @brief instance method providing the static entry
     *        for the loggingMessageHandler.
     *
     * @return A reference to the LogWindowSingleton.
     */
    static LogWindowSingleton &instance();

    /**
     * @brief The write method writes the "message" to the debug
     *        widget, buffering all messages until the debug widget
     *        is created.
     * @param message - QString containing the message to print.
     */
    void write(const QString &message);

    /**
     * @brief setDebugOutput must be called after creating the
     *        DebugOutput widget.
     * @param outputPtr - SmartPointer to the DebugOutput widget object.
     */
    void setDebugOutput(DebugOutput::Ptr outputPtr);

    /**
     * @brief removeDebugOutput must be called when the program terminates
     *        in order to release the DebugOutput widget object. Should be
     *        done before the MainWindow terminates eg. in DTOR
     */
    void removeDebugOutput();

private:
    /**
     * @brief LogWindowSingleton CTOR must be private.
     */
    LogWindowSingleton() : m_startupBuffering(true) {}

    DebugOutput::Ptr m_debugPtr;    /// SmartPointer to the DebugOutput widget
    QStringList m_outPutBuffer;     /// Buffer for startup buffering
    bool m_startupBuffering;        /// Used to avoid buffering after a removeDebugOutput call
};




/**
 * @brief Main Application Window
 *
 **/
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static MainWindow* instance();

    ~MainWindow() override;

    enum QGC_MAINWINDOW_STYLE
    {
        QGC_MAINWINDOW_STYLE_NATIVE = 0,
        QGC_MAINWINDOW_STYLE_INDOOR = 1,
        QGC_MAINWINDOW_STYLE_OUTDOOR = 2
    };

    /** @brief Get current visual style */
    int getStyle();
    /** @brief Get auto link reconnect setting */
    bool autoReconnectEnabled();
    /** @brief Get title bar mode setting */
    bool dockWidgetTitleBarsEnabled();
    /** @brief Get low power mode setting */
    bool lowPowerModeEnabled();
    /** @brief Get Auto Prox mode setting */
    bool autoProxyModeEnabled();

    QList<QAction*> listLinkMenuActions(void);

public slots:
    void loadTlogMenuClicked();
    void disableTLogReplayBar();
    void enableTLogReplayBar();
    /** @brief Shows a status message on the bottom status bar */
    void showStatusMessage(const QString& status, int timeout);
    /** @brief Shows a status message on the bottom status bar */
    void showStatusMessage(const QString& status);
    /** @brief Shows a critical message as popup or as widget */
    void showCriticalMessage(const QString& title, const QString& message);
    /** @brief Shows an info message as popup or as widget */
    void showInfoMessage(const QString& title, const QString& message);

    /** @brief Show the application settings */
    void showSettings();
    /** @brief Show the application About box */
    void showAbout();
    /** @brief Add a communication link */
    void addLink();
    void addLink(int linkid);
    bool configLink(int linkid);
    void linkError(int linkid,QString errorstring);
    void configure();
    /** @brief Simulate a link */
    void simulateLink(bool simulate);
    /** @brief Set the currently controlled UAS */
    void setActiveUAS(UASInterface* uas);

    /** @brief Add a new UAS */
    void UASCreated(UASInterface* uas);
    /** Delete an UAS */
    void UASDeleted(UASInterface* uas);
    /** @brief Update system specs of a UAS */
    void UASSpecsChanged(int uas);
    void startVideoCapture();
    void stopVideoCapture();
    void saveScreen();
    void enableHeartbeat(bool enabled);

    /** @brief Sets advanced mode, allowing for editing of tool widget locations */
    void setAdvancedMode(bool mode);
    /** @brief Load configuration views */
    void loadHardwareConfigView();
    void loadSoftwareConfigView();
    /** @brief Load default view when no MAV is connected */
    void loadUnconnectedView();
    /** @brief Load view for pilot */
    void loadPilotView();
    /** @brief Load view for simulation */
    void loadSimulationView();
    /** @brief Load view for engineer */
    void loadEngineerView();
    /** @brief Load view for operator */
    void loadOperatorView();
    /** @brief Load MAVLink XML generator view */
    void loadMAVLinkView();
    /** @brief Load firmware update view */
    void loadFirmwareUpdateView();
    /** @brief Load Terminal Console views */
    void loadTerminalView();

    /** @brief Show the online help for users */
    void showHelp();
    /** @brief Show the authors / credits */
    void showCredits();
    /** @brief Show the project roadmap */
    void showRoadMap();

    /** @brief Reload the CSS style sheet */
    void reloadStylesheet();
    /** @brief Let the user select the CSS style sheet */
    void selectStylesheet();
    void selectStylesheetDialogAccepted();
    /** @breif Enable title bars on dock widgets when no in advanced mode */
    void enableDockWidgetTitleBars(bool enabled);
    /** @brief Automatically reconnect last link */
    void enableAutoReconnect(bool enabled);
    /** @brief Save power by reducing update rates */
    void enableLowPowerMode(bool enabled) { lowPowerMode = enabled; }
    /** @brief Use the system proxy for network connections automatically */
    void enableAutoProxyMode(bool enabled);
    /** @brief Switch to native application style */
    void loadNativeStyle();
    /** @brief Switch to indoor mission style */
    void loadIndoorStyle();
    /** @brief Switch to outdoor mission style */
    void loadOutdoorStyle();
    /** @brief Load a specific style */
    void loadStyle(QGC_MAINWINDOW_STYLE style);

    /** @brief Add a custom tool widget */
    void createCustomWidget();

    /** @brief Load a custom tool widget from a file chosen by user (QFileDialog) */
    void loadCustomWidget();

    /** @brief Load a custom tool widget from a file */
    void loadCustomWidget(const QString& fileName, bool singleinstance=false);
    void loadCustomWidget(const QString& fileName, int view);

    /** @brief Load custom widgets from default file */
    void loadCustomWidgetsFromDefaults(const QString& systemType, const QString& autopilotType);

    /** @brief Loads and shows the HIL Configuration Widget for the given UAS*/
    void showHILConfigurationWidget(UASInterface *uas);

    void closeEvent(QCloseEvent* event) override;

    /** @brief Load data view, allowing to plot flight data */
//    void loadDataView(QString fileName);

    /**
     * @brief Shows a Docked Widget based on the action sender
     *
     * This slot is written to be used in conjunction with the addTool() function
     * It shows the QDockedWidget based on the action sender
     *
     */
    void showTool(bool visible);


    /**
     * @brief Shows a Widget from the center stack based on the action sender
     *
     * This slot is written to be used in conjunction with the addCentralWidget() function
     * It shows the Widget based on the action sender
     *
     */
    void showCentralWidget();

    /** @brief Update the window name */
    void configureWindowName();

    void commsWidgetDestroyed(QObject *obj);
#ifndef QGC_TOOLBAR_ENABLED
    APMToolBar &toolBar();
#endif
signals:
#ifdef MOUSE_ENABLED_LINUX
    /** @brief Forward X11Event to catch 3DMouse inputs */
    void x11EventOccured(XEvent *event);
#endif //MOUSE_ENABLED_LINUX
    void autoProxyChanged(bool);

public:
    QGCMAVLinkLogPlayer* getLogPlayer()
    {
        return logPlayer;
    }

    //MAVLinkProtocol* getMAVLink()
    //{
    //    return mavlink;
    //}


    bool heartbeatEnabled() { return m_heartbeatEnabled; }
protected:

    MainWindow(QWidget *parent = nullptr);

    typedef enum VIEW_SECTIONSs
    {
        VIEW_ENGINEER,
        VIEW_MISSION,
        VIEW_FLIGHT,
        VIEW_SIMULATION,
        VIEW_MAVLINK,
        VIEW_FIRMWAREUPDATE,
        VIEW_HARDWARE_CONFIG,
        VIEW_SOFTWARE_CONFIG,
        VIEW_TERMINAL,
        VIEW_3DWIDGET,
        VIEW_GOOGLEEARTH,
        VIEW_UNCONNECTED,    ///< View in unconnected mode, when no UAS is available
        VIEW_FULL            ///< All widgets shown at once
    } VIEW_SECTIONS;

    /**
     * @brief Adds an already instantiated QDockedWidget to the Tools Menu
     *
     * This function does all the hosekeeping to have a QDockedWidget added to the
     * tools menu and connects the QMenuAction to a slot that shows the widget and
     * checks/unchecks the tools menu item
     *
     * @param widget    The QDockWidget being added
     * @param title     The entry that will appear in the Menu and in the QDockedWidget title bar
     * @param location  The default location for the QDockedWidget in case there is no previous key in the settings
     */
    void addTool(SubMainWindow *parent,VIEW_SECTIONS view,QDockWidget* widget, const QString& title, Qt::DockWidgetArea area);
    void loadDockWidget(QString name);
    QDockWidget* createDockWidget(QWidget *parent,QWidget *child,QString title,QString objectname,VIEW_SECTIONS view,Qt::DockWidgetArea area,int minwidth=0,int minheight=0);
    /**
     * @brief Adds an already instantiated QWidget to the center stack
     *
     * This function does all the hosekeeping to have a QWidget added to the tools menu
     * tools menu and connects the QMenuAction to a slot that shows the widget and
     * checks/unchecks the tools menu item. This is used for all the central widgets (those in
     * the center stack.
     *
     * @param widget        The QWidget being added
     * @param title         The entry that will appear in the Menu
     */
    void addToCentralStackedWidget(QWidget* widget, VIEW_SECTIONS viewSection, const QString& title);

    /** @brief Catch window resize events */
    void resizeEvent(QResizeEvent * event) override;

    /** @brief Keeps track of the current view */
    VIEW_SECTIONS currentView;
    QGC_MAINWINDOW_STYLE currentStyle;
    bool aboutToCloseFlag;
    bool changingViewsFlag;

    void storeViewState();
    void loadViewState();

    void buildCustomWidget();
    void buildCommonWidgets();
    void connectCommonWidgets();
    void connectCommonActions();
	void connectSenseSoarActions();

    void loadSettings();
    void storeSettings();

    // TODO Should be moved elsewhere, as the protocol does not belong to the UI
    //QPointer<MAVLinkProtocol> mavlink;

    QPointer<MAVLinkSimulationLink> simulationLink;
    QPointer<LinkInterface> udpLink;

    QSettings settings;
    QPointer<QStackedWidget> centerStack;
    QPointer<QActionGroup> centerStackActionGroup;

    // Center widgets
    QPointer<SubMainWindow> plannerView;
    QPointer<SubMainWindow> pilotView;
    QPointer<SubMainWindow> configView;
    QPointer<SubMainWindow> softwareConfigView;
    QPointer<SubMainWindow> mavlinkView;
    QPointer<SubMainWindow> engineeringView;
    QPointer<SubMainWindow> simView;
    QPointer<SubMainWindow> terminalView;
    DebugOutput::Ptr debugOutput;

    // Center widgets
    //QPointer<HUD> hudWidget;
    //QPointer<QGCVehicleConfig> configWidget;
    //QPointer<QGCMapTool> mapWidget;
    //QPointer<XMLCommProtocolWidget> protocolWidget;
    //QPointer<QGCDataPlot2D> dataplotWidget;
#ifdef QGC_OSG_ENABLED
    QPointer<QWidget> q3DWidget;
#endif
#if (defined GOOGLEEARTH) && ((defined _MSC_VER) || (defined Q_OS_MAC))
    QPointer<QGCGoogleEarthView> earthWidget;
#endif
    QPointer<QGCFirmwareUpdate> firmwareUpdateWidget;

    // Dock widgets
    QPointer<QDockWidget> controlDockWidget;
    QPointer<QDockWidget> controlParameterWidget;
    QPointer<QDockWidget> infoDockWidget;
    QPointer<QDockWidget> cameraDockWidget;
    QPointer<QDockWidget> listDockWidget;
    QPointer<QDockWidget> waypointsDockWidget;
    QPointer<QDockWidget> detectionDockWidget;
    QPointer<QDockWidget> parametersDockWidget;
    QPointer<QDockWidget> headDown1DockWidget;
    QPointer<QDockWidget> headDown2DockWidget;
    QPointer<QDockWidget> watchdogControlDockWidget;

    QPointer<QDockWidget> headUpDockWidget;
    QPointer<QDockWidget> video1DockWidget;
    QPointer<QDockWidget> video2DockWidget;
    QPointer<QDockWidget> rgbd1DockWidget;
    QPointer<QDockWidget> rgbd2DockWidget;
    QPointer<QDockWidget> logPlayerDockWidget;

    QPointer<QDockWidget> hsiDockWidget;
    QPointer<QDockWidget> rcViewDockWidget;
    QPointer<QDockWidget> hudDockWidget;
    QPointer<QDockWidget> slugsDataWidget;
    QPointer<QDockWidget> slugsHilSimWidget;
    QPointer<QDockWidget> slugsCamControlWidget;

#ifdef QGC_TOOLBAR_ENABLED
    QPointer<QGCToolBar> toolBar;
#else
    QPointer<APMToolBar> m_apmToolBar;
#endif

    QPointer<QGCStatusBar> customStatusBar;


    QPointer<QDockWidget> mavlinkInspectorWidget;
    QPointer<MAVLinkDecoder> mavlinkDecoder;
    QPointer<QDockWidget> mavlinkSenderWidget;
    QGCMAVLinkLogPlayer* logPlayer;
    QMap<int, QDockWidget*> hilDocks;

    // Popup widgets
    QPointer<JoystickWidget> joystickWidget;

    QPointer<JoystickInput> joystick;

#ifdef MOUSE_ENABLED_WIN
    /** @brief 3d Mouse support (WIN only) */
    Mouse3DInput* mouseInput;               ///< 3dConnexion 3dMouse SDK
    Mouse6dofInput* mouse;                  ///< Implementation for 3dMouse input
#endif // MOUSE_ENABLED_WIN

#ifdef MOUSE_ENABLED_LINUX
    /** @brief Reimplementation of X11Event to handle 3dMouse Events (magellan) */
    bool x11Event(XEvent *event);
    Mouse6dofInput* mouse;                  ///< Implementation for 3dMouse input
#endif // MOUSE_ENABLED_LINUX

    /** User interface actions **/
    QPointer<QAction> connectUASAct;
    QPointer<QAction> disconnectUASAct;
    QPointer<QAction> startUASAct;
    QPointer<QAction> returnUASAct;
    QPointer<QAction> stopUASAct;
    QPointer<QAction> killUASAct;
    QPointer<QAction> simulateUASAct;


    QPointer<LogCompressor> comp;
    QString screenFileName;
    QPointer<QTimer> videoTimer;
    QString styleFileName;
    bool autoReconnect;
    Qt::WindowStates windowStateVal;
    bool lowPowerMode; ///< If enabled, QGC reduces the update rates of all widgets
    bool autoProxyMode;
    QPointer<QGCFlightGearLink> fgLink;
    QTimer windowNameUpdateTimer;

private slots:
    void showAutoUpdateDownloadDialog(QString version, QString releaseType, QString url, QString name);
    void autoUpdateCancelled(QString version);
    void showNoUpdateAvailDialog();

    void showTerminalConsole();
    void closeTerminalConsole();

private:
    bool m_heartbeatEnabled;
    QList<QObject*> commsWidgetList;
    QMap<QString,QString> customWidgetNameToFilenameMap;
    QMap<QAction*,QString > menuToDockNameMap;
    QMap<QDockWidget*,QWidget*> dockToTitleBarMap;
    QMap<VIEW_SECTIONS,QMap<QString,QWidget*> > centralWidgetToDockWidgetsMap;
    bool isAdvancedMode;
    bool dockWidgetTitleBarEnabled;
    Ui::MainWindow ui;

    QString getWindowStateKey();
    QString getWindowGeometryKey();

    AutoUpdateCheck m_autoUpdateCheck;
    AutoUpdateDialog* m_dialog;

    QDialog* m_terminalDialog;

};

#endif /* MAINWINDOW_H */
