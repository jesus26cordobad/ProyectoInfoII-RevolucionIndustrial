// entities/VagonTren.cpp

#include "VagonTren.h"
#include <QDebug>

VagonTren::VagonTren(float y, float velocidadInicial, QObject *parent)
    : QObject(parent),
    posX(POS_INICIAL_X),
    posY(y),
    velocidad(velocidadInicial),
    carbonRecibido(0),
    carbonRequerido(CARBON_REQUERIDO_BASE),
    terminado(false) {
    qDebug() << "Vagon creado en posicion X:" << posX << "Y:" << posY << "Velocidad:" << velocidad;
}

VagonTren::~VagonTren() {
}

void VagonTren::cargarSprite(const QPixmap& sprite) {
    this->sprite = sprite;
}

void VagonTren::actualizar(float deltaTime) {
    if (terminado) return;

    // Mover el tren hacia la derecha
    posX += velocidad * deltaTime;

    // Verificar si salio de la pantalla
    if (posX > POS_SALIDA_X) {
        terminado = true;
        bool completado = estaCompleto();
        emit trenSalio(completado);

        if (completado) {
            qDebug() << "Tren salio COMPLETO con" << carbonRecibido << "de" << carbonRequerido;
        } else {
            qDebug() << "Tren salio INCOMPLETO con" << carbonRecibido << "de" << carbonRequerido;
        }
    }
}

void VagonTren::recibirCarbon() {
    if (carbonRecibido < carbonRequerido) {
        carbonRecibido++;
        emit carbonRecibidoCambiado(carbonRecibido);
        qDebug() << "*** CARBON RECIBIDO! Total:" << carbonRecibido << "/" << carbonRequerido << " ***";

        if (carbonRecibido == carbonRequerido) {
            qDebug() << "¡VAGÓN LLENO!";
            emit vagonLleno();
        }
    }
}

QRectF VagonTren::getAreaRecepcion() const {

    // Centrar horizontalmente en el medio del tren
    float centroTrenX = posX + (ANCHO_SPRITE / 2);
    float areaX = centroTrenX - (ANCHO_AREA_RECEPCION / 2);


    float areaY = posY + 250;

    return QRectF(areaX, areaY, ANCHO_AREA_RECEPCION, ALTO_AREA_RECEPCION);
}

bool VagonTren::puedeRecibirCarbon(float x, float y) const {
    if (terminado) return false;

    QRectF area = getAreaRecepcion();
    bool contiene = area.contains(x, y);

    if (contiene) {
        qDebug() << "!!! CARBON EN AREA DE RECEPCION !!! Pos:(" << x << "," << y << ")";
    }

    return contiene;
}

void VagonTren::dibujar(QPainter& painter) {
    if (sprite.isNull()) return;

    painter.save();

    // Dibujar el sprite del tren
    painter.drawPixmap(posX, posY, ANCHO_SPRITE, ALTO_SPRITE, sprite);

    // Dibujar indicadores solo si no ha terminado
    if (!terminado) {
        QRectF area = getAreaRecepcion();

        // Área de llenado en verde (muestra dónde debe caer el carbón)
        //painter.setPen(QPen(QColor(0, 255, 0), 3));
        //painter.setBrush(QColor(0, 255, 0, 40));
        //painter.drawRect(area);

        // Texto del área
        //painter.setPen(Qt::white);
        //painter.setFont(QFont("Arial", 16, QFont::Bold));
        //painter.drawText(area, Qt::AlignCenter, "ÁREA DE\nLLENADO");

        // Barra de progreso - Debajo del área de recepción
        float centroTrenX = posX + (ANCHO_SPRITE / 2);
        float anchoBarra = 300.0f;
        float altoBarra = 30.0f;
        float barraX = centroTrenX - (anchoBarra / 2);

        // Barra justo debajo del área verde
        float barraY = 450;

        // Fondo de la barra (gris oscuro)
        painter.fillRect(barraX, barraY, anchoBarra, altoBarra, QColor(50, 50, 50, 220));

        // Borde de la barra
        painter.setPen(QPen(Qt::white, 3));
        painter.drawRect(barraX, barraY, anchoBarra, altoBarra);

        // Progreso de la barra
        float progreso = static_cast<float>(carbonRecibido) / carbonRequerido;
        QColor colorBarra = progreso >= 1.0f ? QColor(0, 255, 0) : QColor(255, 200, 0);
        painter.fillRect(barraX + 3, barraY + 3, (anchoBarra - 6) * progreso, altoBarra - 6, colorBarra);

        // Texto del progreso
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 16, QFont::Bold));
        QString texto = QString("%1/%2").arg(carbonRecibido).arg(carbonRequerido);
        painter.drawText(barraX, barraY, anchoBarra, altoBarra,
                         Qt::AlignCenter, texto);
    }

    painter.restore();
}
