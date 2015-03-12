#-------------------------------------------------
#
# Project created by QtCreator 2012-04-27T08:56:08
#
#-------------------------------------------------

QT       += core gui sql

TARGET = scoresanalysis
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    manage_student.cpp \
    daostudent.cpp \
    daogroup.cpp \
    manage_course.cpp \
    type_score.cpp \
    type_integrate_score.cpp \
    analysis_group_score.cpp \
    analysis_score.cpp \
    daocourse.cpp \
    daotranscript.cpp \
    daoaddminusitem.cpp \
    login.cpp \
    pieview.cpp \
    daochart.cpp \
    diagramview.cpp \
    managewidget.cpp \
    about.cpp

HEADERS  += mainwindow.h \
    connectdb.h \
    manage_student.h \
    daostudent.h \
    daogroup.h \
    manage_course.h \
    type_score.h \
    type_integrate_score.h \
    analysis_group_score.h \
    analysis_score.h \
    daocourse.h \
    daotranscript.h \
    daoaddminusitem.h \
    login.h \
    pieview.h \
    daochart.h \
    diagramview.h \
    managewidget.h \
    about.h

FORMS    += mainwindow.ui \
    manage_student.ui \
    manage_course.ui \
    type_score.ui \
    type_integrate_score.ui \
    analysis_group_score.ui \
    analysis_score.ui \
    login.ui \
    about.ui

OTHER_FILES +=

RESOURCES += \
    icons.qrc
