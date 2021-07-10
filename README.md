# Greedy Packer
Greedy Packer - это простой универсальный упаковщик текстурных атласов для игр. Его задача - получив на входе список текстур, скомпоновать их в единую текстуру-атлас, сопровождая её файлом с координатной разметкой (указанием, где какая суб-текстура размещается).

Использует для компоновки атласа "жадный" алгоритм, отчего так и назван. Разрабатывался для проекта **FPS Creator K-MOD**. 

Является аналогом Nvidia Atlas Creation Tool. Отличается от других упаковщиков полностью открытым исходным кодом. Не слишком умён, но своё дело делает хорошо. 

## Возможности
- собирает атлас с рациональным заполнением свободного пространства.
- умеет читать файлы форматов PNG, TGA, DDS, JPEG, GIF, BMP и некоторых других.
- сохраняет текстурные атласы в формате DDS.
- поддерживает альфа-канал.
- выполнен в виде консольной утилиты, так что может быть интегрирован в любой workflow.
- высокое быстродействие.

## Как собрать?
Собранная версия упаковщика (x64) может быть загружена отсюда: https://github.com/Wolf4D/GreedyPacker/releases/

Сборка осуществляется с использованием qMake. 

Для сборки требуется Qt 5 и **собранная библиотека SOIL2**. 

Проект может быть одинаково собран с компиляторами VS и MinGW. Требует поддержки C++11. В папке test_data лежат файлы, на которых удобно проверить работоспособность результата.

**Укажите переменные среды SOIL2_INCL_DIR и SOIL2_LIB_DIR перед началом сборки.**

## Запуск
Запуск осуществляется из командной строки, с аргументами: `
<БазоваяПапка> <ИмяВыходногоАтласа> <Текстура1> <Текстура2> ...
`

То есть, если игра расположена в папке **C:\Game,** а текстуры внутри лежат в **Texturebank** в подпапках **A** и **B**, то создать атлас с именем **NewAtlas** будет так:`
GreedyPacker.exe C:\Game\Texturebank NewAtlas A\texture1.dds A\texture2.dds B\texture3.dds
`

В папке **C:\Game\Texturebank** будет при этом создан файл  **NewAtlas.dds** с текстурой атласа, а также файл разметки **NewAtlas.atl** с координатами текстур в формате: `имя_и_путь_текстуры [X][Y][ширина][высота]`, где:
- **X** - горизонтальная координата верхнего левого угла текстуры внутри атласа (в пикселях);
- **Y** - вертикальная координата верхнего левого угла текстуры внутри атласа (в пикселях);
- **ширина** и **высота** - ширина и высота текстуры (в пикселях).

## TODO:
- отсутствует генерация MIP-карт.
- логика упаковки рядов требует доработки, в некоторых местах алгоритм работает неэффективно, из-за чего на промежуточных этапах возникают дублирующие варианты.

## Права
Автор: **Иван Клёнов** *aka Wolf4D* (e-mail: Wolf4D@list.ru)

Распространяется по лицензии MIT.

Все права защищены. 2021 г.
