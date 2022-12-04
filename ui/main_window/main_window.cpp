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
  // FIXME
  ui->CodeDisplay->append(cmd);
}

void MainWindow::on_btnLoadCode_released() {
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

  ui->CodeDisplay->setText(QString::fromStdString(engine->get_source_copy()));
}

void MainWindow::on_btnRunCode_released() {
  engine->generate_ast();
  engine->run_code();
  ui->treeDisplay->setText(QString::fromStdString(engine->get_ast_copy()));
  ui->resultDisplay->setText(QString::fromStdString(engine->get_result_copy()));
}

void MainWindow::on_btnClearCode_released() {
  engine->clear();
  ui->CodeDisplay->setText(QString::fromStdString(engine->get_source_copy()));
  ui->treeDisplay->setText(QString::fromStdString(engine->get_ast_copy()));
  ui->resultDisplay->setText(QString::fromStdString(engine->get_result_copy()));
}
