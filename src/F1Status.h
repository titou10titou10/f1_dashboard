#ifndef F1STATUS_H
#define F1STATUS_H

#include <QMainWindow>
#include <QLabel>

#include "UdpListener.h"

QT_BEGIN_NAMESPACE
namespace Ui { class F1Status; }
QT_END_NAMESPACE

class Tracker;

class F1Status : public QMainWindow {
    Q_OBJECT

public:
    F1Status(QWidget *parent = nullptr);
    ~F1Status();

private:
    Ui::F1Status *ui;
    F1Listener *_listener;
    Tracker *_tracker;

    void setColoredLabel(QLabel* item, quint8 value);
    QString formatTimeMs(float value);
    QString formatTimeS(uint16_t value);
    QString setPSI(float value);
    QString truncate2(float value);
    QString truncate2plus(float value);
    void setRed(QLabel* label);
    void setGreen(QLabel* label);
    void setOrange(QLabel* label);

private slots:
    void telemetryChanged(const PacketHeader &header, const CarTelemetryData &data) ;
    void lapChanged(const PacketHeader &header, const LapData &data);
    void sessionChanged(const PacketHeader &header, const PacketSessionData &data);
    void setupChanged(const PacketHeader &header, const CarSetupData &data);
    void statusChanged(const PacketHeader &header, const CarStatusData &data);
    void participantChanged(const PacketHeader &header, const ParticipantData &data);
    void motionChanged(const PacketHeader &header, const CarMotionData &data);
    void eventChanged(const PacketHeader &header, const PacketEventData &data);

};
#endif // F1STATUS_H
