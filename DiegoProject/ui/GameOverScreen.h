// ui/GameOverScreen.h
// Pantalla de Game Over con botones interactivos

#ifndef GAMEOVERSCREEN_H
#define GAMEOVERSCREEN_H

#include <QPainter>
#include <QString>
#include <QRect>
#include <QPoint>

class GameOverScreen {
public:
    enum BotonGameOver {
        NINGUNO,
        VOLVER_MENU,
        SALIR,
        REINTENTAR
    };

    GameOverScreen();
    ~GameOverScreen();

    // Dibujar pantalla completa
    void dibujar(QPainter& painter, int ancho, int alto,
                 const QString& razon, int metros, int objetivo,
                 float tiempoJugado);

    // Detectar clic en botones
    BotonGameOver detectarClic(const QPoint& pos) const;

    // Getters de rectángulos de botones
    QRect getBotonVolverMenu() const { return botonVolverMenu; }
    QRect getBotonSalir() const { return botonSalir; }
    QRect getBotonReintentar() const { return botonReintentar; }

private:
    QRect botonVolverMenu;
    QRect botonSalir;
    QRect botonReintentar;

    // Colores
    QColor colorFondo;
    QColor colorTitulo;
    QColor colorTexto;
    QColor colorBoton;
    QColor colorBotonPeligro;
};

#endif // GAMEOVERSCREEN_H
