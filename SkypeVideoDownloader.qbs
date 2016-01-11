import qbs

Application {
    Depends { name: "cpp" }
    Depends { name: "Qt.widgets" }
    Depends { name: "Qt.sql" }

    cpp.cxxPrecompiledHeader: "pch.h"

    files: [
        "pch.h",
        "mainwindow.h",
        "model.h",
        "mainwindow.cpp",
        "model.cpp",
        "main.cpp",
        "mainwindow.ui",
    ]
}

