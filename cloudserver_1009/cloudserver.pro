QT += gui
QT += network
#QT += widgets
CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        basic_c_fun/basic_surf_objs.cpp \
        basic_c_fun/v3d_message.cpp \
        main.cpp \
        manage.cpp \
        messageserver.cpp \
        messagesocket.cpp \
        neuron_editing/apo_xforms.cpp \
        neuron_editing/global_feature_compute.cpp \
        neuron_editing/neuron_format_converter.cpp \
        neuron_editing/neuron_sim_scores.cpp \
        neuron_editing/neuron_xforms.cpp \
        neuron_editing/v_neuronswc.cpp \
        receive_file.cpp \
        send_file.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    basic_c_fun/basic_surf_objs.h \
    basic_c_fun/v3d_message.h \
    basic_c_fun/v_neuronswc.h \
    cropimage.h \
    customdebug.h \
    manage.h \
    managesocket.h \
    messageserver.h \
    messagesocket.h \
    neuron_editing/apo_xforms.h \
    neuron_editing/global_feature_compute.h \
    neuron_editing/neuron_format_converter.h \
    neuron_editing/neuron_sim_scores.h \
    neuron_editing/neuron_xforms.h \
    neuron_editing/v_neuronswc.h \
    publicEnum.h \
    receive_file.h \
    send_file.h \
    basic_c_fun/v3d_basicdatatype.h


