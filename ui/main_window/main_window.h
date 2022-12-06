#pragma once
#include <QMainWindow>

#include "engine.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
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
  engine::MiniBasic *engine;

  bool redirect_to_engine_input_;
  Str input_for_engine_;


  void run();
  void continue_run();
  void load();
  void list();
  void clear();
  void help();
  void quit();
  void refresh();
};
