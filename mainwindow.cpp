#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "lldb/API/SBEvent.h"
#include "lldb/API/SBStream.h"
#include "QDebug"
#include <array>
#include <iostream>

void log(const char *msg, void *)
  {
  std::cout << msg << std::endl;
  }

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
  {
  ui->setupUi(this);

  lldb::SBDebugger::Initialize();

  _debugData.reset(new DebugData);
  _debugData->listen = lldb::SBListener("Listener Thing");
  _debugData->debugger = lldb::SBDebugger::Create(true, log, nullptr);

  toolbar = addToolBar("Debug Controls");
  auto load = toolbar->addAction("Load");
  connect(load, &QAction::triggered, [this]()
    {
    _debugData->currentTarget = _debugData->debugger.CreateTarget("../build-test-Desktop_Qt_5_3_clang_64bit-Debug/test");
    });


  auto act = toolbar->addAction("Run");
  connect(act, &QAction::triggered, [this]()
    {
    if (!_debugData->currentTarget.IsValid())
      {
      qWarning() << "Invalid target, cannot start";
      }

    const char *argp[] = { 0 };
    const char *envp[] = { 0 };

    lldb::SBError error;

    assert(_debugData->listen.IsValid());

    _debugData->currentProcess = _debugData->currentTarget.Launch(
      _debugData->listen,
      argp,
      envp,
      nullptr,
      nullptr,
      nullptr,
      nullptr,
      lldb::eLaunchFlagNone,
      false,
      error);

    if (error.Fail())
      {
      std::cerr << error.GetCString() << std::endl;
      }
    });

  auto kill = toolbar->addAction("Kill");
  connect(kill, &QAction::triggered, [this]()
    {
    auto err = _debugData->currentProcess.Kill();
    if (err.Fail())
      {
      std::cout << err.GetCString() << std::endl;
      }
    });

  _stop = toolbar->addAction("");
  syncState(_debugData->currentProcess.GetState());
  _stop->setCheckable(true);
  connect(_stop, SIGNAL(triggered()), this, SLOT(togglePause()));

  connect(&_timer, &QTimer::timeout, [this]
    {
    auto newState = _debugData->currentProcess.GetState();
    if (_debugData->currentState != _debugData->currentProcess.GetState())
      {
      syncState(newState);
      }

    lldb::SBEvent ev;
    while (_debugData->listen.GetNextEvent(ev))
      {
      lldb::SBStream desc;
      ev.GetDescription(desc);

      std::cout << "Event Boy! ";
      if (desc.GetData())
        {
        std::cout << desc.GetData();
        }
      std::cout << std::endl;
      }

    std::array<char, 256> output;

    while (size_t read = _debugData->currentProcess.GetSTDERR(output.data(), output.size()))
      {
      std::cout << output.data();
      }

    while (size_t read = _debugData->currentProcess.GetSTDOUT(output.data(), output.size()))
      {
      std::cout << output.data();
      }
    });
  _timer.start(100);
  }

MainWindow::~MainWindow()
  {
  delete ui;

  lldb::SBDebugger::Terminate();
  }

void MainWindow::syncState(lldb::StateType type)
  {
  ui->statusbar->showMessage(
    QString("Process %1/%2 [%3 %4]")
      .arg(_debugData->currentTarget.GetExecutable().GetDirectory())
      .arg(_debugData->currentTarget.GetExecutable().GetFilename())
      .arg(_debugData->currentProcess.GetProcessID())
      .arg(lldb::SBDebugger::StateAsCString(type))
    );

  _debugData->currentState = type;

  _stop->setText(type == lldb::eStateStopped ? "Continue" : "Break");
  _stop->setChecked(type == lldb::eStateStopped);
  }

void MainWindow::togglePause()
  {
  if (_debugData->currentState == lldb::eStateStopped)
    {
    handleError(_debugData->currentProcess.Continue());
    }
  else
    {
    handleError(_debugData->currentProcess.Stop());
    }
  }

void MainWindow::handleError(const lldb::SBError &err)
  {
  if (err.Fail())
    {
    std::cout << err.GetCString() << std::endl;
    }
  }

int main(int argc, char *argv[])
  {
  QApplication app(argc, argv);

  MainWindow w;

  w.show();

  return app.exec();
  }
