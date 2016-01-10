import qbs

Application {
    Depends { name: "Qt.widgets" }
    Depends { name: "Qt.sql" }

    files: [
        "main.cpp",
        "mainwindow.ui",
    ]
}

