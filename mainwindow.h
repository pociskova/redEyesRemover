#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QImageWriter>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <string>
#include <vector>
#include <iostream>
using namespace cv;
using namespace std;


namespace Ui {
class MainWindow;
}


enum State
{
    AUTO,
    HAND
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void changePixel(Mat eye);
    void detectEyes();
    QImage convertToQimage(const Mat &mat);
    void display(Mat i);
    void autoCorrect();
    void handCorrect();

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_Slider_valueChanged(int value);

private:
    Ui::MainWindow *ui;

    Mat image;
    QImage qimage;
    QPixmap pixmap;
    QString fileName;
    string face_cascade_name = "haarcascade_frontalface_alt.xml";
    string eye_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
    CascadeClassifier face_cascade;
    CascadeClassifier eyes_cascade;
    int r = 0;
    bool drawing = false;
    QPointF pqf1, pqf2;
    QPoint pq1, pq2;
    Mat roi;
    int state = AUTO;
};

#endif // MAINWINDOW_H
