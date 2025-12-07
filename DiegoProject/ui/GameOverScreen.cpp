// ui/GameOverScreen.cpp
// Implementación de la pantalla de Game Over

#include "GameOverScreen.h"
#include <QFont>

GameOverScreen::GameOverScreen()
    : colorFondo(0, 0, 0, 200),
    colorTitulo(Qt::red),
    colorTexto(Qt::white),
    colorBoton(100, 100, 100),
    colorBotonPeligro(150, 50, 50) {
    // Las posiciones de los botones se calculan dinámicamente en dibujar()
}

GameOverScreen::~GameOverScreen() {
}

void GameOverScreen::dibujar(QPainter& painter, int ancho, int alto,
                             const QString& razon, int metros, int objetivo,
                             float tiempoJugado) {
    // Fondo oscuro semitransparente
    painter.fillRect(0, 0, ancho, alto, colorFondo);

    // Título "GAME OVER"
    painter.setPen(colorTitulo);
    painter.setFont(QFont("Arial", 48, QFont::Bold));
    painter.drawText(QRect(0, 100, ancho, 60), Qt::AlignCenter, "GAME OVER");

    // Razón del Game Over
    painter.setFont(QFont("Arial", 24));
    painter.drawText(QRect(0, 180, ancho, 40), Qt::AlignCenter, razon);

    // Estadísticas
    painter.setPen(colorTexto);
    painter.setFont(QFont("Arial", 18));
    QString stats = QString("Metros producidos: %1/%2\n"
                            "Tiempo jugado: %3s")
                        .arg(metros)
                        .arg(objetivo)
                        .arg(static_cast<int>(tiempoJugado));
    painter.drawText(QRect(0, 240, ancho, 80), Qt::AlignCenter, stats);

    // BOTONES CENTRADOS DINÁMICAMENTE
    int anchoBoton = 200;
    int altoBoton = 50;
    int centroX = ancho / 2 - anchoBoton / 2;  // Centrar horizontalmente
    int espaciadoVertical = 60;  // Espacio entre botones
    int yInicial = 340;  // Y inicial para el primer botón

    // Actualizar posiciones de botones centrados
    botonReintentar = QRect(centroX, yInicial, anchoBoton, altoBoton);
    botonVolverMenu = QRect(centroX, yInicial + espaciadoVertical, anchoBoton, altoBoton);
    botonSalir = QRect(centroX, yInicial + espaciadoVertical * 2, anchoBoton, altoBoton);

    // Dibujar botones
    painter.fillRect(botonReintentar, colorBoton);
    painter.fillRect(botonVolverMenu, colorBoton);
    painter.fillRect(botonSalir, colorBotonPeligro);

    // Texto de botones
    painter.setPen(colorTexto);
    painter.setFont(QFont("Arial", 16, QFont::Bold));
    painter.drawText(botonReintentar, Qt::AlignCenter, "Reintentar");
    painter.drawText(botonVolverMenu, Qt::AlignCenter, "Menú Principal");
    painter.drawText(botonSalir, Qt::AlignCenter, "Salir");

    // Bordes de botones
    painter.setPen(QPen(Qt::white, 2));
    painter.drawRect(botonReintentar);
    painter.drawRect(botonVolverMenu);
    painter.drawRect(botonSalir);
}

GameOverScreen::BotonGameOver GameOverScreen::detectarClic(const QPoint& pos) const {
    if (botonReintentar.contains(pos)) {
        return REINTENTAR;
    } else if (botonVolverMenu.contains(pos)) {
        return VOLVER_MENU;
    } else if (botonSalir.contains(pos)) {
        return SALIR;
    }
    return NINGUNO;
}
