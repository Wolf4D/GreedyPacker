///////////////////////////////////////////////////////////////////////////////
//
// main.cpp. Здесь реализована основная логика взаимодействия.
//
///////////////////////////////////////////////////////////////////////////////

#include <QDir>
#include <QTextStream>
#include <iostream>
#include <cstdint>
#include <fstream>

#include "SOIL2.h"
#include "greedypacker.h"

///////////////////////////////////////

int main(int argc, char *argv[])
{
    // Инициализируем упаковщик
    GreedyPacker packer;

    // Проверим, что ввод правильный
    if (argc<4)
        packer.endWithError("Launch parameters: <Base Dir Path> <Atlas Name> <file1.ext> <file2.ext> ...", 1);

    // Инициализируем базовую папку
    QDir home(argv[1]);
    if (!home.exists()) packer.endWithError("Wrong base path!", 2);

    // Теперь читаем имена текстур
    for(int i=3; i<argc; i++)
    {
        // Ищем заданный файл
        if (!home.exists(argv[i]))
            packer.endWithError("Can't find file: " + QString(argv[i]), 3);

        // Ну-с, грузим файл
        int width = 0;  int height = 0;  int chans = 0;
        unsigned char * imgData =  SOIL_load_image(home.absoluteFilePath(argv[i]).toStdString().c_str(), &width, &height, &chans, SOIL_LOAD_RGBA );

        if (imgData==NULL)
            packer.endWithError("Can't load file: " + QString(argv[i]) + " cause of error " +  SOIL_last_result(), 4);

        // Создадим QImage, дальше будем действовать над ним
        QImage * newImg = new QImage(width, height, QImage::Format_RGBA8888); // TODO: Освобождать память явно, а не при завершении работы

        // Заполним изображение попиксельно. Уродски, но другие простые способы не работают
        for(uint y=0; y<height; y++)
            for(uint x=0; x<width; x++)
            {
                int s = y*width*4 + x*4;
                QColor color(imgData[s], imgData[s+1], imgData[s+2], imgData[s+3]);
                newImg->setPixel(x, y, color.rgba());
            }

        // Добавим изображение в упаковщик
        packer.addImage(newImg, home.relativeFilePath(argv[i]));
    }

    ///////////////////////////////////////
    // Текстуры прочитаны, начинаем формировать атлас

    QStringList atlasText; // текстовое наполнение файла координатной разметки

    // Проведём процесс упаковки
    packer.packIt();

    // Отрисуем текстуру атласа, попутно заполнив текст для файла разметки
    QImage resultImage = packer.renderResult(atlasText);

    // Переформатируем итоговую текстуру
    QImage imgProperlyFormatted = resultImage.convertToFormat(QImage::Format_RGBA8888);

    // Запишем текстуру в файл
    if(0 == SOIL_save_image_quality((home.absoluteFilePath(argv[2]) + ".dds").toStdString().c_str(),
                                    SOIL_SAVE_TYPE_DDS,
                                    imgProperlyFormatted.width(),
                                    imgProperlyFormatted.height(),
                                    4,
                                    imgProperlyFormatted.bits(),
                                    0))
    {
        packer.endWithError("Error saving atlas DDS cause of error " +  QString(SOIL_last_result()), 5);
    }

    // Пишем файл координатной разметки
    QFile altalOutFile(home.absoluteFilePath(argv[2]) + ".atl");
    if (altalOutFile.exists()) altalOutFile.remove();

    if (!altalOutFile.open(QIODevice::WriteOnly | QIODevice::Text))
        packer.endWithError("Error saving atlas ATL", 6);

    {
        QTextStream out(&altalOutFile);
        for(int i=0; i<atlasText.size(); i++)
            out << atlasText.at(i) << "\n";
    }

    // Всё, конец
    return 0;
}
///////////////////////////////////////////////////////////////////////////////
