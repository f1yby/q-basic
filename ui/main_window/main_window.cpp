#include "main_window.h"

#include <QFileDialog>
#include <QObject>
#include <fstream>

#include "ui_main_window.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      engine(new engine::MiniBasic) {
  ui->setupUi(this);
}

MainWindow::~MainWindow() {
  delete engine;
  delete ui;
}

void MainWindow::on_cmdLineEdit_editingFinished() {
  QString cmd = ui->cmdLineEdit->text();
  ui->cmdLineEdit->setText("");

  if (redirect_to_engine_input_) {
    redirect_to_engine_input_ = !engine->handle_input(cmd.toStdString());
    if (!redirect_to_engine_input_) {
      continue_run();
    }
  } else {
    switch (engine->handle_command(cmd.toStdString())) {
      case UIBehavior::Run:
        run();
        break;
      case UIBehavior::Load:
        load();
        break;
      case UIBehavior::List:
        list();
        break;
      case UIBehavior::Clear:
        clear();
        break;
      case UIBehavior::Help:
        help();
        break;
      case UIBehavior::Quit:
        quit();
        break;
      case UIBehavior::Input:
        redirect_to_engine_input_ = true;
        break;
      case UIBehavior::Refresh:
        refresh();
        break;
      case UIBehavior::FinishRun:
      case UIBehavior::None:
        break;
    }
  }
}

void MainWindow::on_btnLoadCode_released() { load(); }

void MainWindow::on_btnRunCode_released() { run(); }

void MainWindow::on_btnClearCode_released() { clear(); }
void MainWindow::refresh() {
  ui->CodeDisplay->setText(QString::fromStdString(engine->get_source_copy()));
  ui->treeDisplay->setText(QString::fromStdString(engine->get_ast_copy()));
  update();
}
void MainWindow::run() {
  ui->resultDisplay->append(QString::fromStdString("> RUN"));
  engine->start_run();
  continue_run();
}
void MainWindow::load() {
  ui->resultDisplay->append(QString::fromStdString("> LOAD"));
  QString filename = QFileDialog::getOpenFileName(
      nullptr, QObject::tr("Load Game"), QDir::currentPath(),
      QObject::tr("Basic Script File"));
  if (filename.length() == 0) {
    return;
  }
  std::fstream in(filename.toStdString(), std::ios::in);
  if (!in.good()) {
    return;
  }
  engine->load_source(in);
  in.close();

  refresh();
}
void MainWindow::list() {
  ui->resultDisplay->append(QString::fromStdString("> LIST"));
}
void MainWindow::clear() {
  ui->resultDisplay->append(QString::fromStdString("> CLEAR"));
  engine->clear();
  refresh();
}
void MainWindow::help() {
  ui->resultDisplay->append(QString::fromStdString("> HELP"));
}
void MainWindow::quit() {
  ui->resultDisplay->append(QString::fromStdString("> QUIT"));
}
void MainWindow::continue_run() {
  UIBehavior behavior;
  Str output;
  while (true) {
    behavior = engine->step_run(output);
    switch (behavior) {
      case UIBehavior::Input:
        redirect_to_engine_input_ = true;
        return;
      case UIBehavior::Refresh:
        ui->resultDisplay->append(QString::fromStdString(output));
        output.clear();
        refresh();
        break;
      case UIBehavior::FinishRun:
        return;
      case UIBehavior::Run:
      case UIBehavior::Load:
      case UIBehavior::List:
      case UIBehavior::Clear:
      case UIBehavior::Help:
      case UIBehavior::Quit:
      case UIBehavior::None:
        break;
    }
  }
}
