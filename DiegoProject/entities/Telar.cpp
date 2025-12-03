// entities/Telar.cpp
// Implementación completa del Telar con producción y dibujo

#include "Telar.h"
#include "../utils/ResourceManager.h"
#include <QDebug>

Telar::Telar(float x, float y, QObject *parent)
    : QObject(parent),
    posX(x),
    posY(y),
    temperatura(20.0f),
    funcionando(true),
    tiempoSinMantenimiento(0.0f),
    colorAsignado(""),
    produccionActual(0.0f),
    velocidadProduccion(0.5f),
    tamaño(120) {

    qDebug() << "Telar creado en posición (" << x << "," << y << ")";
}

Telar::~Telar() {
    qDebug() << "Telar destruido - Producción total:" << produccionActual << "metros";
}

// ACTUALIZACIÓN Y TEMPERATURA
void Telar::actualizar(float deltaTime) {
    if (funcionando) {
        tiempoSinMantenimiento += deltaTime;

        // Calentar gradualmente
        calentar(2.0f * deltaTime);

        // Producir si no está sobrecalentado
        if (temperatura < 90.0f && !colorAsignado.isEmpty()) {
            produccionActual += velocidadProduccion * deltaTime;
        }

        // Apagar si sobrecalentado
        if (temperatura >= 100.0f) {
            apagar();
            emit sobrecalentado();
        }
    }
}

void Telar::calentar(float incremento) {
    temperatura += incremento;
    if (temperatura > 100.0f) {
        temperatura = 100.0f;
    }
}

void Telar::reparar() {
    temperatura = 20.0f;
    funcionando = true;
    tiempoSinMantenimiento = 0.0f;

    emit reparado();
    qDebug() << "Telar reparado";
}

void Telar::apagar() {
    funcionando = false;
    qDebug() << "Telar apagado por sobrecalentamiento";
}

// PRODUCCIÓN
void Telar::procesarCarrete(const QString& colorCarrete) {
    qDebug() << "🔧 Telar procesando carrete:";
    qDebug() << "   Color recibido:" << colorCarrete;
    qDebug() << "   Color asignado:" << colorAsignado;
    qDebug() << "   Funcionando:" << funcionando;
    qDebug() << "   Temperatura antes:" << temperatura;

    if (colorCarrete == colorAsignado) {
        // REINICIAR TEMPERATURA A 20°C
        temperatura = 20.0f;
        tiempoSinMantenimiento = 0.0f;

        // Si estaba apagado, encenderlo
        if (!funcionando) {
            funcionando = true;
        }

        // Producir 10 metros de tela (CAMBIADO: 5 → 10)
        produccionActual += 10.0f;

        qDebug() << " Telar procesado exitosamente:";
        qDebug() << "   Temperatura después:" << temperatura;
        qDebug() << "   Producción acumulada:" << produccionActual << "m";

        emit produccionCompletada(10.0f);
    } else {
        qDebug() << " Color no coincide, no se procesa";
    }
}

bool Telar::puedeProducir() const {
    return funcionando && temperatura < 90.0f;
}

// DIBUJO
void Telar::dibujar(QPainter& painter) {
    // Seleccionar sprite según temperatura
    QString spriteKey;

    if (!funcionando || temperatura >= 100.0f) {
        spriteKey = "telar_averiado";
    } else if (temperatura >= 50.0f) {  // A partir de 50% usa telar_caliente
        spriteKey = "telar_caliente";
    } else {
        spriteKey = "telar_normal";
    }

    // Intentar dibujar sprite
    QPixmap sprite = ResourceManager::getInstance().getSprite(spriteKey);

    if (!sprite.isNull()) {
        painter.drawPixmap(posX, posY, sprite.scaled(tamaño, tamaño,
                                                     Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        // Fallback: rectángulo con color
        QColor colorBase = funcionando ? QColor(150, 150, 150) : QColor(200, 50, 50);
        painter.fillRect(posX, posY, tamaño, tamaño, colorBase);
        painter.setPen(QPen(Qt::black, 2));
        painter.drawRect(posX, posY, tamaño, tamaño);
    }

    // Barra de temperatura CON COLOR SEGÚN colorAsignado
    int barX = posX + 10;
    int barY = posY + tamaño + 5;
    int barAncho = tamaño - 20;
    int barAlto = 8;

    // Fondo de la barra
    painter.fillRect(barX, barY, barAncho, barAlto, QColor(50, 50, 50));

    // Determinar color de la barra según el color asignado del telar
    QColor colorBarra;
    if (colorAsignado == "rojo") {
        colorBarra = QColor(220, 20, 60);  // Rojo
    } else if (colorAsignado == "azul") {
        colorBarra = QColor(30, 144, 255); // Azul
    } else if (colorAsignado == "verde") {
        colorBarra = QColor(34, 139, 34);  // Verde
    } else {
        colorBarra = Qt::gray;  // Por defecto
    }

    // Barra de temperatura con el color del telar
    int anchoTemp = static_cast<int>((temperatura / 100.0f) * barAncho);
    painter.fillRect(barX, barY, anchoTemp, barAlto, colorBarra);

    // Borde
    painter.setPen(QPen(Qt::white, 1));
    painter.drawRect(barX, barY, barAncho, barAlto);
}
