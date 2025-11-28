// main.cpp
// Punto de entrada principal del juego "Revolución Industrial"
// Autores: Jesús Alberto Córdoba Delgado - Diego Andrés Páez Mendoza

#include <QApplication>
#include "ui/MainMenu.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Configuración general de la aplicación
    app.setApplicationName("Revolución Industrial");
    app.setApplicationVersion("1.0");

    // Crear y mostrar el menú principal
    MainMenu menuPrincipal;
    menuPrincipal.show();

    return app.exec();
}
