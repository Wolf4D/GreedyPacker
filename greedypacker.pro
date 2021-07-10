###############################################################################

# Упаковщик текстурных атласов GreedyPacker.
# Задача упаковщика - получив на входе список текстур, скомпоновать их в единую
# текстуру-атлас, плюс создать файл с координатной разметкой (где какая
# суб-текстура размещается).
# Использует для компоновки атласа жадные алгоритмы, отчего так и назван.
#
# Разрабатывался для проекта k-mod.

# СБОРКА:
# Может быть одинаково собран компиляторами VS и MinGW. Требует поддержки C++11.
# Для сборки требуется Qt 5 и собранная библиотека SOIL2. 
# Укажите переменные SOIL2_INCL_DIR и SOIL2_LIB_DIR перед началом.

# Автор: Иван Клёнов aka Wolf4D (e-mail: Wolf4D@list.ru)
# Распространяется по лицензии MIT.
# Все права защищены. 2021 г.

###############################################################################

# Проверка переменных
SOIL2_INCL_DIR = $$(SOIL2_INCL_DIR)
SOIL2_LIB_DIR = $$(SOIL2_LIB_DIR)

isEmpty(SOIL2_INCL_DIR) {
        error("Need path to SOIL2 library includes. Please set the SOIL2_INCL_DIR variable in Qt Creator build enviroment!") }

isEmpty(SOIL2_LIB_DIR) {
        error("Need path to SOIL2 library lib-file. Please set the SOIL2_LIB_DIR variable in Qt Creator build enviroment!") }

####################################

TARGET = GreedyPacker
QT += core gui

TEMPLATE = app

SOURCES += main.cpp\
    greedypacker.cpp

HEADERS  += \
    greedypacker.h

####################################

# Подключим SOIL2
LIBS += $$SOIL2_LIB_DIR/soil2.lib
PRE_TARGETDEPS += $$SOIL2_LIB_DIR/soil2.lib

INCLUDEPATH += $$SOIL2_INCL_DIR
DEPENDPATH += $$SOIL2_INCL_DIR

# Нужна библиотека OpenGL32. Традиционно уже есть в составе WinSDK, поставляемом
# в составе много чего (в т.ч. и Visual Studio), и обыкновенно уже включена
# в окружение сборки, искать ничего не надо.
LIBS += -lOpenGL32
###############################################################################
