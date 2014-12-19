#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "QMainWindow"
#include "QToolBar"
#include "QTimer"
#include "lldb/API/SBDebugger.h"
#include "lldb/API/SBTarget.h"
#include "lldb/API/SBProcess.h"
#include "lldb/API/SBListener.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
  {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private slots:
  void togglePause();

private:
  void syncState(lldb::StateType type);
  void handleError(const lldb::SBError &err);

  Ui::MainWindow *ui;
  QToolBar *toolbar;

  struct DebugData
    {
    lldb::SBDebugger debugger;
    lldb::SBTarget currentTarget;
    lldb::SBProcess currentProcess;
    lldb::StateType currentState;

    lldb::SBListener listen;
    };

  std::unique_ptr<DebugData> _debugData;
  QTimer _timer;
  QAction *_stop;
  };

#endif // MAINWINDOW_H
