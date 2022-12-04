#pragma once
#include"engine.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

 private slots:
  void on_cmdLineEdit_editingFinished();
  void on_btnLoadCode_released();
  void on_btnRunCode_released();
  void on_btnClearCode_released();

 private:
  Ui::MainWindow *ui;
  engine::MiniBasic* engine;
};
