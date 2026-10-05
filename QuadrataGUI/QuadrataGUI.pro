QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET   = QuadrataGUI
TEMPLATE = app

SOURCES += \
    main.cpp \
    MainWindow.cpp \
    SplashPage.cpp \
    LoginPage.cpp \
    AdminDashboard.cpp \
    CustomerDashboard.cpp \
    BookingDialog.cpp

HEADERS += \
    MainWindow.h \
    SplashPage.h \
    LoginPage.h \
    AdminDashboard.h \
    CustomerDashboard.h \
    BookingDialog.h \
    JsonUtils.h

# Uncomment after placing logo.png in this folder:
RESOURCES += resources.qrc
