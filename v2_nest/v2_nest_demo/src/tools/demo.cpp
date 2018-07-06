#include "mainwindow.hpp"

#include <QApplication>

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  MainWindow *x = new MainWindow();
  x->show();

  return app.exec();
}

