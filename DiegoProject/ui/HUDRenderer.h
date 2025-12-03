// ui/HUDRenderer.h
// Renderizador de HUD (interfaz) para el NivelFabrica
// Maneja el dibujo de estadísticas, barras de presión, indicadores

#ifndef HUDRENDERER_H
#define HUDRENDERER_H

#include <QPainter>
#include <QString>
#include <QRect>
#include <QColor>

class HUDRenderer {
public:
    HUDRenderer();
    ~HUDRenderer();

    // Dibujar componentes del HUD
    void dibujarEstadisticas(QPainter& painter, int metros, int objetivo,
                             int vidas, float presion, float tiempo);

    void dibujarCorazonesVidas(QPainter& painter, int vidas, int xInicio);

    void dibujarBarraPresion(QPainter& painter, float presion,
                             float tiempoSobrecalentada);

    void dibujarIndicadorCarrete(QPainter& painter, const QColor& colorCarrete);

    void dibujarInstrucciones(QPainter& painter);

    void dibujarBotonPausa(QPainter& painter, const QRect& boton);

    // Advertencias y notificaciones
    void dibujarAdvertencia(QPainter& painter, const QString& mensaje,
                            int x, int y);

private:
    // Configuración de colores
    QColor colorFondoHUD;
    QColor colorTexto;
    QColor colorAdvertencia;

    // Posiciones predefinidas
    static const int MARGEN = 10;
    static const int ANCHO_HUD = 250;
    static const int ALTO_HUD = 120;
};

#endif // HUDRENDERER_H
