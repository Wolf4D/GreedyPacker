///////////////////////////////////////////////////////////////////////////////
//
// GreedyPacker. Класс реализует логику формирования атласа.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef GREEDYPACKER_H
#define GREEDYPACKER_H

#include <QList>
#include <QImage>
#include <QMultiMap>

///////////////////////////////////////

// Вспомогательная структура "данные об изображении"
struct ImageData
{
    QImage * img = NULL; // указатель на изображение
    int sizeInPx = 0;    // размер изображения
    QString imageName;   // имя файла изображения

    int beginX = 0;      // координата x верхнего левого угла
    int beginY = 0;      // координата y верхнего левого угла
};

// Вспомогательная структура "ряд изображений"
struct ImageRow
{
    int beginY = 0;     // координата y верхнего левого угла
    int currentX = 0;   // место по x, куда сейчас размещаем картинку
    int imgMaxSize = 0; // размер самой большой картинки в ряду

    QList<ImageData> images;   // перечень изображений в ряду
};

///////////////////////////////////////

// Класс упаковщика, реализующего жадный алгоритм
class GreedyPacker
{
public:
    GreedyPacker() {}

    // Передача текстуры в упаковщик
    void addImage(QImage * img, QString imgName);

    // Запуск процедуры упаковки. Параметр - служебный, нужен для перезапуска
    // процесса упаковки, если с первого раза текстуры не уместились в атлас
    void packIt(int expandBy=1);

    // Отрисовка результата в картинку с формированием координатных данных
    QImage renderResult(QStringList &atlasText);

    // Завершить работу упаковщика (да и всей программы) с ошибкой
    void endWithError(QString text, int code);

private:

    // Список рядов атласа
    QList<ImageRow> rows;

    // Картинки, отсортированные по своим размерам. Ключ - размер
    QMultiMap<int, ImageData> images;

    // Текущий размер атласа
    int atlasSizeInPX = 64;

    // Функция добавления нового ряда в атлас.
    // Возвращает true при завершении (даже если для этого приходится добавить несколько новых рядов),
    // false - если в атласе кончилось место.
    bool addNewRow(int imgSize, int &currentY, QList<ImageData> &thisImages, bool continueRow = true);
    // imgSize - размер текущих изображений
    // currentY - координата y, с которой начинается отрисовка ряда
    // thisImage - ссылка на массив изображений вот такого вот размера
    // continueRow - флаг, регулирующий, продолжить ли предыдущий ряд (по возможности) или начать новый
};

#endif // GREEDYPACKER_H

////////////////////////////////////////////////////////////////////////////////