/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../client/MainWindow.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_MainWindow_t {
    uint offsetsAndSizes[50];
    char stringdata0[11];
    char stringdata1[14];
    char stringdata2[1];
    char stringdata3[5];
    char stringdata4[19];
    char stringdata5[22];
    char stringdata6[15];
    char stringdata7[4];
    char stringdata8[13];
    char stringdata9[4];
    char stringdata10[14];
    char stringdata11[3];
    char stringdata12[16];
    char stringdata13[17];
    char stringdata14[5];
    char stringdata15[15];
    char stringdata16[9];
    char stringdata17[11];
    char stringdata18[12];
    char stringdata19[14];
    char stringdata20[12];
    char stringdata21[13];
    char stringdata22[15];
    char stringdata23[16];
    char stringdata24[13];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_MainWindow_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
        QT_MOC_LITERAL(0, 10),  // "MainWindow"
        QT_MOC_LITERAL(11, 13),  // "onTrackerLine"
        QT_MOC_LITERAL(25, 0),  // ""
        QT_MOC_LITERAL(26, 4),  // "line"
        QT_MOC_LITERAL(31, 18),  // "onTrackerConnected"
        QT_MOC_LITERAL(50, 21),  // "onTrackerDisconnected"
        QT_MOC_LITERAL(72, 14),  // "onTrackerError"
        QT_MOC_LITERAL(87, 3),  // "msg"
        QT_MOC_LITERAL(91, 12),  // "onJobUpdated"
        QT_MOC_LITERAL(104, 3),  // "idx"
        QT_MOC_LITERAL(108, 13),  // "onJobFinished"
        QT_MOC_LITERAL(122, 2),  // "ok"
        QT_MOC_LITERAL(125, 15),  // "onGroupSelected"
        QT_MOC_LITERAL(141, 16),  // "QListWidgetItem*"
        QT_MOC_LITERAL(158, 4),  // "item"
        QT_MOC_LITERAL(163, 14),  // "onFileSelected"
        QT_MOC_LITERAL(178, 8),  // "doUpload"
        QT_MOC_LITERAL(187, 10),  // "doDownload"
        QT_MOC_LITERAL(198, 11),  // "doStopShare"
        QT_MOC_LITERAL(210, 13),  // "doCreateGroup"
        QT_MOC_LITERAL(224, 11),  // "doJoinGroup"
        QT_MOC_LITERAL(236, 12),  // "doLeaveGroup"
        QT_MOC_LITERAL(249, 14),  // "doListRequests"
        QT_MOC_LITERAL(264, 15),  // "doAcceptRequest"
        QT_MOC_LITERAL(280, 12)   // "refreshFiles"
    },
    "MainWindow",
    "onTrackerLine",
    "",
    "line",
    "onTrackerConnected",
    "onTrackerDisconnected",
    "onTrackerError",
    "msg",
    "onJobUpdated",
    "idx",
    "onJobFinished",
    "ok",
    "onGroupSelected",
    "QListWidgetItem*",
    "item",
    "onFileSelected",
    "doUpload",
    "doDownload",
    "doStopShare",
    "doCreateGroup",
    "doJoinGroup",
    "doLeaveGroup",
    "doListRequests",
    "doAcceptRequest",
    "refreshFiles"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_MainWindow[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  116,    2, 0x08,    1 /* Private */,
       4,    0,  119,    2, 0x08,    3 /* Private */,
       5,    0,  120,    2, 0x08,    4 /* Private */,
       6,    1,  121,    2, 0x08,    5 /* Private */,
       8,    1,  124,    2, 0x08,    7 /* Private */,
      10,    2,  127,    2, 0x08,    9 /* Private */,
      12,    1,  132,    2, 0x08,   12 /* Private */,
      15,    0,  135,    2, 0x08,   14 /* Private */,
      16,    0,  136,    2, 0x08,   15 /* Private */,
      17,    0,  137,    2, 0x08,   16 /* Private */,
      18,    0,  138,    2, 0x08,   17 /* Private */,
      19,    0,  139,    2, 0x08,   18 /* Private */,
      20,    0,  140,    2, 0x08,   19 /* Private */,
      21,    0,  141,    2, 0x08,   20 /* Private */,
      22,    0,  142,    2, 0x08,   21 /* Private */,
      23,    0,  143,    2, 0x08,   22 /* Private */,
      24,    0,  144,    2, 0x08,   23 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,    9,   11,
    QMetaType::Void, 0x80000000 | 13,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.offsetsAndSizes,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_MainWindow_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MainWindow, std::true_type>,
        // method 'onTrackerLine'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onTrackerConnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onTrackerDisconnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onTrackerError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onJobUpdated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onJobFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'onGroupSelected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QListWidgetItem *, std::false_type>,
        // method 'onFileSelected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'doUpload'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'doDownload'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'doStopShare'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'doCreateGroup'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'doJoinGroup'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'doLeaveGroup'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'doListRequests'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'doAcceptRequest'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'refreshFiles'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onTrackerLine((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->onTrackerConnected(); break;
        case 2: _t->onTrackerDisconnected(); break;
        case 3: _t->onTrackerError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->onJobUpdated((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->onJobFinished((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 6: _t->onGroupSelected((*reinterpret_cast< std::add_pointer_t<QListWidgetItem*>>(_a[1]))); break;
        case 7: _t->onFileSelected(); break;
        case 8: _t->doUpload(); break;
        case 9: _t->doDownload(); break;
        case 10: _t->doStopShare(); break;
        case 11: _t->doCreateGroup(); break;
        case 12: _t->doJoinGroup(); break;
        case 13: _t->doLeaveGroup(); break;
        case 14: _t->doListRequests(); break;
        case 15: _t->doAcceptRequest(); break;
        case 16: _t->refreshFiles(); break;
        default: ;
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 17)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 17;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
