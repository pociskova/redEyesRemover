#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changePixel(Mat eye)
{
    for(int y=0;y<eye.rows;y++)
        {
            for(int x=0;x<eye.cols;x++)
            {
                Vec3b color = eye.at<Vec3b>(Point(x,y));

                if((color[2] > 2 * color[0])  &&  (color[2] > 2 * color[1]) )
                {
                    color[2] = r;
                    eye.at<Vec3b>(Point(x,y)) = color;
                }
            }
         }
}

void MainWindow::detectEyes()
{
    vector<Rect> faces; //wykryte twarze
    vector<Mat> eyesmats;

    Mat imageCutFace;
    Mat imageEye;

//    Mat hsv;
//    vector<Mat> img_split;
//    cvtColor(image, hsv, COLOR_BGR2HSV );                //Konwersja BGR -> HSV
//    split(hsv, img_split);            //Rozdzielenie HSV na poszczególne kanały

    face_cascade.detectMultiScale( image, faces, 1.1, 1, 0 );

    for(unsigned i = 0; i<faces.size() ; i++)
    {
        Rect rectangleFace( faces[i] );
        //rectangle(image, rectangleFace, Scalar( 120, 5, 86 ), 2, 2, 0 );

        imageCutFace = image( faces[i] );
        vector<Rect> eyes; //wykryte oczy
        eyes_cascade.detectMultiScale( imageCutFace, eyes, 1.1, 2, 0 |CV_HAL_CMP_GE, Size(30, 30) );
        for( unsigned j = 0; j < eyes.size(); j++ )
        {
            Rect rectangleEye( faces[i].x + eyes[j].x, faces[i].y + eyes[j].y , eyes[j].width, eyes[j].height );
            //rectangle(image, rectangleEye, Scalar( 0, 120, 12 ), 2, 2, 0 );
            eyesmats.push_back(imageCutFace( eyes[j] ));
        }

    }

    for( unsigned j = 0; j < eyesmats.size(); j++)
    {
        changePixel(eyesmats[j]);
    }

   // imshow("Remove Red Color", image);
}

QImage MainWindow::convertToQimage(const Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS=1
    if(mat.type()==CV_8UC1)
    {
        // Set the color table (used to translate colour indexes to qRgb values)
        QVector<QRgb> colorTable;
        for (int i=0; i<256; i++)
            colorTable.push_back(qRgb(i,i,i));
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
        img.setColorTable(colorTable);
        return img;
    }
    // 8-bits unsigned, NO. OF CHANNELS=3
    else if(mat.type()==CV_8UC3)
    {
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return img.rgbSwapped();
    }
    else
    {
        //qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}

void MainWindow::display(Mat i)
{
    QImage img = convertToQimage(i);
    pixmap = QPixmap::fromImage(img);

    int pixW = pixmap.width();
    int pixH = pixmap.height();
    ui->label->setFixedWidth(pixW);
    ui->label->setFixedHeight(pixH);
    ui->label->setPixmap(pixmap);
//    ui->label->setScaledContents( true );
//    ui->label->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
}

void MainWindow::autoCorrect()
{
    if(!image.empty())
    {
        if( !face_cascade.load(face_cascade_name) )
        {
            cout<< "No file "<<face_cascade_name<<".";
            //return -1;
        }

        if( !eyes_cascade.load(eye_cascade_name) )
        {
            cout<< "No file "<<eye_cascade_name<<".";
            //return -1;
        }
        detectEyes();
        display(image);
    }
}

void MainWindow::handCorrect()
{
    Point pt1(pqf1.x(), pqf1.y());
    Point pt2(pqf2.x(), pqf2.y());
    Rect selectedArea(pt1.x, pt1.y, pt2.x-pt1.x, pt2.y-pt1.y);
    Mat roi(image, selectedArea);
    changePixel(roi);
    display(image);
    waitKey(0);
}


//open file
void MainWindow::on_pushButton_3_clicked()
{
    fileName = QFileDialog::getOpenFileName(this,
            tr("Open Image"), ".",
            tr("Image Files (*.jpg *.jpeg *.png)"));
    if (!fileName.isEmpty())
    {
        image = imread(fileName.toLatin1().data());
        display(image);
    }
}

//save file
void MainWindow::on_pushButton_4_clicked()
{
    if (!fileName.isEmpty())
    {
            if(image.data )
            {
                QString path = QFileDialog::getSaveFileName(this,tr("Save Image File"), "", tr(" JPG(*.jpg);; PNG(*.png)"));
                QImage qimg = convertToQimage(image);
                //bool save = qimg.save(path);
                QImageWriter imageWriter;
                 imageWriter.setFileName(path);
                 bool save =imageWriter.write(qimg);

                if(save == true)
                {
                    QMessageBox::information(this, tr("Save"), tr("Successfully save"));
                }
                else
                {
                    QMessageBox::critical(this, tr("Save"), tr("Save failed"));
                }
            }
    }
}

//auto correction
void MainWindow::on_pushButton_clicked()
{
    state = AUTO;
    autoCorrect();
}


void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
           drawing = true;
           pqf1 = event->pos();
           pqf1= ui->label->mapFromParent(event->pos());
           pq1=pqf1.toPoint();
       }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    //if ((event->buttons() & Qt::LeftButton) && drawing)

}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && drawing) {
        pqf2 = event->pos();
        pqf2= ui->label->mapFromParent(event->pos());
        pq2=pqf2.toPoint();

        QPainter painter(&pixmap);
        painter.drawRect(pq1.x(),pq1.y(),pq2.x()-pq1.x(),pq2.y()-pq1.y());
        painter.end();
        ui->label->setPixmap(pixmap);
        drawing = false;
    }
}

//zaznaczanie ręczne
void MainWindow::on_pushButton_2_clicked()
{
    state = HAND;
    handCorrect();
}

//suwak
void MainWindow::on_Slider_valueChanged(int value)
{
    r = value;
    image = imread(fileName.toLatin1().data());

    if(state == AUTO)
    {
        autoCorrect();
    }
    if(state == HAND)
    {
        autoCorrect();
        handCorrect();
    }
}
