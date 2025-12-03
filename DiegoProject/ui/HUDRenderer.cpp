// ui/HUDRenderer.cpp
// Implementación del renderizador de HUD

#include "HUDRenderer.h"
#include <QFont>
#include <QPainterPath>

HUDRenderer::HUDRenderer()
    : colorFondoHUD(0, 0, 0, 180),
    colorTexto(Qt::white),
    colorAdvertencia(Qt::red) {
}

HUDRenderer::~HUDRenderer() {
}

void HUDRenderer::dibujarEstadisticas(QPainter& painter, int metros, int objetivo,
                                      int, float presion, float tiempoRestante) {
    // Fondo del HUD
    painter.fillRect(MARGEN, MARGEN, ANCHO_HUD, ALTO_HUD, colorFondoHUD);

    painter.setPen(colorTexto);
    painter.setFont(QFont("Arial", 12, QFont::Bold));

    QString info = QString("LA FÁBRICA TEXTIL\n"
                           "Metros: %1/%2\n"
                           "Presión: %3%\n"
                           "Tiempo: %4s")
                       .arg(metros)
                       .arg(objetivo)
                       .arg(static_cast<int>(presion))
                       .arg(static_cast<int>(tiempoRestante));

    painter.drawText(MARGEN + 10, MARGEN + 15, info);
}

void HUDRenderer::dibujarCorazonesVidas(QPainter& painter, int vidas, int xInicio) {
    painter.save();

    if (vidas < 0) vidas = 0;
    if (vidas > 5) vidas = 5;

    int y = MARGEN + 5;
    int size = 20;
    int espaciado = 30;

    for (int i = 0; i < vidas; i++) {
        int x = xInicio + (i * espaciado);

        // Crear path del corazón
        QPainterPath corazon;

        // Punto inferior (punta del corazón)
        float centroX = x + size / 2.0f;
        float centroY = y + size;

        corazon.moveTo(centroX, centroY);

        // Lado derecho
        corazon.cubicTo(
            centroX + size * 0.5f, centroY - size * 0.3f,  // Control 1
            centroX + size * 0.5f, centroY - size * 0.8f,  // Control 2
            centroX, centroY - size * 0.5f                  // Punto superior derecho
            );

        // Lado izquierdo
        corazon.cubicTo(
            centroX - size * 0.5f, centroY - size * 0.8f,  // Control 1
            centroX - size * 0.5f, centroY - size * 0.3f,  // Control 2
            centroX, centroY                                // Punto inferior
            );

        // Dibujar corazón
        painter.setPen(QPen(QColor(139, 0, 0), 2));
        painter.setBrush(QColor(220, 20, 60));
        painter.drawPath(corazon);
    }

    painter.restore();
}

void HUDRenderer::dibujarBarraPresion(QPainter& painter, float presion,
                                      float tiempoEnMaximo) {
    int barX = 120;  // Abajo de la caldera
    int barY = 310;
    int barAncho = 20;
    int barAlto = 100;

    // Fondo de la barra
    painter.fillRect(barX, barY, barAncho, barAlto, QColor(40, 40, 40));

    // Nivel de presión
    int nivel = static_cast<int>((presion / 100.0f) * barAlto);

    QColor colorPresion;
    if (presion < 30) colorPresion = Qt::green;
    else if (presion < 70) colorPresion = Qt::yellow;
    else colorPresion = Qt::red;

    painter.fillRect(barX, barY + barAlto - nivel, barAncho, nivel, colorPresion);

    // Borde
    painter.setPen(QPen(Qt::white, 2));
    painter.drawRect(barX, barY, barAncho, barAlto);

    // Marcadores
    painter.setFont(QFont("Arial", 8));
    painter.drawText(barX + 35, barY + 10, "100");
    painter.drawText(barX + 35, barY + barAlto / 3, "70");
    painter.drawText(barX + 35, barY + 2 * barAlto / 3, "30");
    painter.drawText(barX + 35, barY + barAlto, "0");

    // Advertencia si está sobrecalentada
    if (presion >= 100.0f && tiempoEnMaximo > 0.0f) {
        painter.setPen(colorAdvertencia);
        painter.setFont(QFont("Arial", 12, QFont::Bold));
        QString advertencia = QString("¡SOBRECALENTADA! %1s")
                                  .arg(static_cast<int>(5 - tiempoEnMaximo));
        painter.drawText(barX - 50, barY - 15, 150, 30,
                         Qt::AlignCenter, advertencia);
    }
}

void HUDRenderer::dibujarIndicadorCarrete(QPainter& painter, const QColor& colorCarrete) {
    painter.fillRect(MARGEN, 140, 150, 40, QColor(0, 100, 0, 180));
    painter.setPen(colorTexto);
    painter.setFont(QFont("Arial", 12, QFont::Bold));

    // Dibujar círculo con el color
    painter.setBrush(colorCarrete);
    painter.drawEllipse(MARGEN + 10, 150, 20, 20);

    painter.drawText(MARGEN + 40, 165, "Carrete");
}

void HUDRenderer::dibujarInstrucciones(QPainter& painter) {
    painter.setPen(QColor(255, 255, 200));
    painter.setFont(QFont("Arial", 10));
    painter.drawText(10, 730, "WASD: Mover | CLIC: Recoger/Asignar carrete | ESC: Pausa");
}

void HUDRenderer::dibujarBotonPausa(QPainter& painter, const QRect& boton) {
    painter.fillRect(boton, QColor(100, 100, 100, 150));
    painter.setPen(colorTexto);
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.drawText(boton, Qt::AlignCenter, "PAUSA");
}

void HUDRenderer::dibujarAdvertencia(QPainter& painter, const QString& mensaje,
                                     int x, int y) {
    painter.setPen(colorAdvertencia);
    painter.setFont(QFont("Arial", 14, QFont::Bold));
    painter.drawText(x, y, mensaje);
}
