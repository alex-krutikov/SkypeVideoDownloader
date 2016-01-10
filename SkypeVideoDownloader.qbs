import qbs

Application {
    Depends { name: "cpp" }
    Depends { name: "Qt.widgets" }
    Depends { name: "Qt.sql" }

    cpp.cxxPrecompiledHeader: "pch.h"

    files: [
        "pch.h",
        "main.cpp",
        "mainwindow.ui",
    ]
}

