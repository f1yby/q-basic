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
  ui->resultDisplay->append(QString::fromStdString("> " + cmd.toStdString()));

  if (redirect_to_engine_input_) {
    redirect_to_engine_input_ = !engine->handle_input(cmd.toStdString());
    if (!redirect_to_engine_input_) {
      continue_run();
    }
  } else {
    Str output;
    switch (engine->handle_command(cmd.toStdString(), output)) {
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
      case UIBehavior::FinishRun:
      case UIBehavior::None:
        break;
    }
    if (!output.empty()) {
      ui->resultDisplay->append(QString::fromStdString(output));
      output.clear();
      refresh();
    }
  }
}

void MainWindow::on_btnLoadCode_released() {
  ui->resultDisplay->append(QString::fromStdString("> LOAD"));
  load();
}

void MainWindow::on_btnRunCode_released() {
  ui->resultDisplay->append(QString::fromStdString("> RUN"));
  run();
}

void MainWindow::on_btnClearCode_released() {
  ui->resultDisplay->append(QString::fromStdString("> CLEAR"));
  clear();
}
void MainWindow::refresh() {
  ui->CodeDisplay->setText(QString::fromStdString(engine->get_source_copy()));
  ui->treeDisplay->setText(QString::fromStdString(engine->get_ast_copy()));
  update();
}
void MainWindow::run() {
  engine->reset_pc();
  continue_run();
}
void MainWindow::load() {
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
void MainWindow::list() {}
void MainWindow::clear() {
  engine->clear();
  refresh();
}
void MainWindow::help() {}
void MainWindow::quit() {}
void MainWindow::continue_run() {
  UIBehavior behavior;
  Str output;
  while (true) {
    behavior = engine->step_run(output);
    if (behavior == UIBehavior::Input) {
      redirect_to_engine_input_ = true;
    }
    if (!output.empty()) {
      ui->resultDisplay->append(QString::fromStdString(output));
      output.clear();
      refresh();
    }
    if (behavior == UIBehavior::Input || behavior == UIBehavior::FinishRun) {
      return;
    }
  }
}
