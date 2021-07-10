////////////////////////////////////////////////////////////////////////////////

#include "greedypacker.h"
#include <QPainter>
#include <QSet>
#include <iostream>
#include <math.h>

////////////////////////////////////////////////////////////////////////////////

void GreedyPacker::addImage(QImage *img, QString imgName)
{
    ImageData newImg;
    newImg.img = img;
    newImg.imageName = imgName;

    // Не квадратные изображения не принимаются!
    if (img->size().width()!=img->size().height())
        this->endWithError("Non-square image passed!", 69);
    else
        newImg.sizeInPx = img->size().height();

    images.insert(newImg.sizeInPx, newImg);
}

////////////////////////////////////////////////////////////////////////////////

void GreedyPacker::packIt(int expandBy)
{
    atlasSizeInPX = 64; // начнём с самого маленького кванта размера

    // Считаем, сколько пикселей площади должен быть атлас
    long long pixelsRequired = 0;
    for(auto item = images.begin(); item != images.end(); item++ )
        pixelsRequired += powf(item.key(), 2);

    // Увеличиваем сторону атласа двукратно, пока все картинки не поместятся по площади
    while (atlasSizeInPX*atlasSizeInPX < pixelsRequired)
        atlasSizeInPX *= 2;

    // Надо ли увеличить атлас? Если на прошлых итерациях нас постигла неудача, то увеличиваем на этой!
    atlasSizeInPX *= expandBy;

    // Начинаем рассчитывать ряды
    rows.clear();

    int currentY = 0; // последняя занятая горизонтальная позиция

    // Сортируем изображения по размерам в multi-map (одному ключу соответствует множество значений)
    QList<int> imgSizes = images.keys();
    imgSizes = imgSizes.toSet().toList();     // Удалим дубликаты
    qSort(imgSizes.begin(), imgSizes.end(),qGreater<int>());  // отсортируем
    // TODO: Вообще, практика показала, что сортировка по размерам
    // оказалась бессмысленна в концепции "пихаем в ряды всё, что влезает".
    // Надо бы избавиться от этой сущности.

    // Начинаем последовательно, проходя от большего размера к меньшему, распихивать картинки по рядам
    for(int i=0; i<imgSizes.size(); i++)
    {
        int thisImgSize = imgSizes[i];
        QList<ImageData> imagesOfThisSize = images.values(thisImgSize);

        // Добавляем новый ряд (или продолжаем предыдущий, если это не первый ряд)
        if (!addNewRow(thisImgSize, currentY, imagesOfThisSize, !rows.isEmpty()))
        {
            // Хых, а в атлас-то не влезает по вертикали. Всё фигня, давай по новой!
            rows.clear();
            packIt(expandBy+1); // на следующую попытку возьмём атлас побольше
            return;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

// Возвращает true при успехе (даже когда приходится добавить ещё ряд), а false - когда некуда разместить
bool GreedyPacker::addNewRow(int imgSize, int &currentY, QList<ImageData> &thisImages, bool continueRow)
{
    if (currentY + imgSize>atlasSizeInPX)
        return false; // не влезем в атлас по вертикали

    ImageRow *currentRow;
    // Добавляем новый ряд? Или продолжим старый?
    if (!continueRow)
        currentRow = new ImageRow();
    else
        currentRow = &rows.last();

    currentRow->beginY = currentY;
    if (currentRow->imgMaxSize<imgSize)
        currentRow->imgMaxSize = imgSize;

    // Шерстим изображения этого размера
    QMutableListIterator<ImageData> it(thisImages);

    // Начинаем набивать ряд
    while(it.hasNext())
    {
        // Следующая картинка уже не влезет по горизонтали?
        if (currentRow->currentX + imgSize > atlasSizeInPX)
        {
            // В этот ряд картинка не впихнулась - значит он готов
            rows.append(*currentRow);

            // Всё, теперь она уходит на следующий ряд
            currentY += currentRow->imgMaxSize;

            // Вызовем функцию для заполнению следующего ряда
            return addNewRow(imgSize, currentY, thisImages, false);
        }
        else
        {
            currentRow->images.append(it.next()); // Добавим картинку в ряд
            it.remove();  // Удалим из списка ещё не размещённых

            // Запомним внутри картинки, где её поставили
            currentRow->images.last().beginX = currentRow->currentX;
            currentRow->images.last().beginY = currentRow->beginY;

            currentRow->currentX += imgSize;
        }
    }

    // Ряд успешно закончился, ура
    rows.append(*currentRow);
    return true;
}

QImage GreedyPacker::renderResult(QStringList &atlasText)
{
    // Создаём новую картинку нужного размера
    QImage result = QImage(QSize(atlasSizeInPX, atlasSizeInPX), QImage::Format_ARGB32_Premultiplied);

    // Начинаем рисовать
    QPainter painter(&result);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(result.rect(), Qt::transparent);

    // Заполняем картинки из объектов рядов
    painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
    for(int i=0; i<rows.size(); i++)
    {
        for(int j=0; j<rows[i].images.size(); j++)
        {
            int x = rows[i].images[j].beginX;
            int y = rows[i].images[j].beginY;
            painter.drawImage(x, y, *rows[i].images[j].img);

            // А это запишем в файл координат
            QString str = rows[i].images[j].imageName
                    + " [" +QString::number(x)
                    + "][" + QString::number(y)
                    + "][" + QString::number(rows[i].images[j].img->height())
                    + "][" + QString::number(rows[i].images[j].img->width()) +"]";
            atlasText.append(str);
        }
    }

    // Для отладки, можем залить прозрачные области маджентой
    //painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
    //painter.fillRect(result.rect(), Qt::magenta);

    painter.end();

    // TODO: Где-то в логике тут затаилась проблема, при которой ряды
    // имеют свойство иногда повторяться и рисоваться поверх себя самих.
    // В целом, пока это не вызывает проблем, но потенциально должно быть
    // поправлено (вероятно, проблема найдётся с исправлением логики размещения
    // по размерам)

    // Быстрый фикс неуникальных строк
    atlasText = atlasText.toSet().toList();     // Удалим дубликаты

    return result;
}

////////////////////////////////////////////////////////////////////////////////

void GreedyPacker::endWithError(QString text, int code)
{
    std::cout << text.toStdString() << std::endl;
    exit(code);
}

////////////////////////////////////////////////////////////////////////////////
