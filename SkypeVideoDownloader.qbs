import qbs

Application {
    Depends { name: "cpp" }
    Depends { name: "Qt.widgets" }
    Depends { name: "Qt.sql" }
    Depends { name: "Qt.network" }


    files: [
        "mainwindow.h",
        "model.h",
        "mainwindow.cpp",
        "model.cpp",
        "main.cpp",
        "mainwindow.ui",
    ]
}

